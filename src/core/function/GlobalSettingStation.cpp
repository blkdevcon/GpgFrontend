/**
 * Copyright (C) 2021 Saturneric
 *
 * This file is part of GpgFrontend.
 *
 * GpgFrontend is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GpgFrontend is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GpgFrontend. If not, see <https://www.gnu.org/licenses/>.
 *
 * The initial version of the source code is inherited from
 * the gpg4usb project, which is under GPL-3.0-or-later.
 *
 * All the source code of GpgFrontend was modified and released by
 * Saturneric<eric@bktus.com> starting on May 12, 2021.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "GlobalSettingStation.h"

#include <openssl/bio.h>
#include <openssl/pem.h>

#include <vmime/security/cert/openssl/X509Certificate_OpenSSL.hpp>
#include <vmime/vmime.hpp>

#include "core/function/FileOperator.h"

void GpgFrontend::GlobalSettingStation::SyncSettings() noexcept {
  using namespace libconfig;
  try {
    ui_cfg_.writeFile(ui_config_path_.u8string().c_str());
    LOG(INFO) << _("Updated ui configuration successfully written to")
              << ui_config_path_;

  } catch (const FileIOException &fioex) {
    LOG(ERROR) << _("I/O error while writing ui configuration file")
               << ui_config_path_;
  }
}

GpgFrontend::GlobalSettingStation::GlobalSettingStation(int channel) noexcept
    : SingletonFunctionObject<GlobalSettingStation>(channel) {
  using namespace std::filesystem;
  using namespace libconfig;

  el::Loggers::addFlag(el::LoggingFlag::AutoSpacing);

  LOG(INFO) << _("App Path") << app_path_;
  LOG(INFO) << _("App Configure Path") << app_configure_path_;
  LOG(INFO) << _("App Data Path") << app_data_path_;
  LOG(INFO) << _("App Log Path") << app_log_path_;
  LOG(INFO) << _("App Locale Path") << app_locale_path_;

  if (!is_directory(app_configure_path_)) create_directory(app_configure_path_);

  if (!is_directory(app_data_path_)) create_directory(app_data_path_);

  if (!is_directory(app_log_path_)) create_directory(app_log_path_);

  if (!is_directory(ui_config_dir_path_)) create_directory(ui_config_dir_path_);

  if (!exists(ui_config_path_)) {
    try {
      this->ui_cfg_.writeFile(ui_config_path_.u8string().c_str());
      LOG(INFO) << _("UserInterface configuration successfully written to")
                << ui_config_path_;

    } catch (const FileIOException &fioex) {
      LOG(ERROR)
          << _("I/O error while writing UserInterface configuration file")
          << ui_config_path_;
    }
  } else {
    try {
      this->ui_cfg_.readFile(ui_config_path_.u8string().c_str());
      LOG(INFO) << _("UserInterface configuration successfully read from")
                << ui_config_path_;
    } catch (const FileIOException &fioex) {
      LOG(ERROR) << _("I/O error while reading UserInterface configure file");
    } catch (const ParseException &pex) {
      LOG(ERROR) << _("Parse error at ") << pex.getFile() << ":"
                 << pex.getLine() << " - " << pex.getError();
    }
  }
}

void GpgFrontend::GlobalSettingStation::AddRootCert(
    const std::filesystem::path &path) {
  std::string out_buffer;
  if (!FileOperator::ReadFileStd(path, out_buffer)) {
    LOG(ERROR) << _("Failed to read root certificate file") << path;
    return;
  }

  auto mem_bio = std::shared_ptr<BIO>(
      BIO_new_mem_buf(out_buffer.data(), static_cast<int>(out_buffer.size())),
      [](BIO *_p) { BIO_free(_p); });

  auto x509 = std::shared_ptr<X509>(
      PEM_read_bio_X509(mem_bio.get(), nullptr, nullptr, nullptr),
      [](X509 *_p) { X509_free(_p); });

  if (!x509) return;

  root_certs_.push_back(x509);
}

vmime::shared_ptr<vmime::security::cert::defaultCertificateVerifier>
GpgFrontend::GlobalSettingStation::GetCertVerifier() const {
  auto p_cv =
      vmime::make_shared<vmime::security::cert::defaultCertificateVerifier>();

  std::vector<vmime::shared_ptr<vmime::security::cert::X509Certificate>>
      _root_certs;
  for (const auto &cert : root_certs_) {
    _root_certs.push_back(
        std::make_shared<vmime::security::cert::X509Certificate_OpenSSL>(
            cert.get()));
  }
  return p_cv;
}

const std::vector<std::shared_ptr<X509>>
    &GpgFrontend::GlobalSettingStation::GetRootCerts() {
  return root_certs_;
}

void GpgFrontend::GlobalSettingStation::init_app_secure_key() {}

GpgFrontend::GlobalSettingStation::~GlobalSettingStation() noexcept = default;
