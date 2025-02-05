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

#include "GpgBasicOperator.h"

#include <vector>

#include "GpgKeyGetter.h"

GpgFrontend::GpgBasicOperator::GpgBasicOperator(int channel)
    : SingletonFunctionObject<GpgBasicOperator>(channel) {}

GpgFrontend::GpgError GpgFrontend::GpgBasicOperator::Encrypt(
    KeyListPtr keys, GpgFrontend::BypeArrayRef in_buffer,
    GpgFrontend::ByteArrayPtr& out_buffer, GpgFrontend::GpgEncrResult& result) {
  // gpgme_encrypt_result_t e_result;
  gpgme_key_t recipients[keys->size() + 1];

  int index = 0;
  for (const auto& key : *keys) recipients[index++] = gpgme_key_t(key);

  // Last entry data_in array has to be nullptr
  recipients[keys->size()] = nullptr;

  GpgData data_in(in_buffer.data(), in_buffer.size()), data_out;

  gpgme_error_t err = check_gpg_error(gpgme_op_encrypt(
      ctx_, recipients, GPGME_ENCRYPT_ALWAYS_TRUST, data_in, data_out));

  auto temp_data_out = data_out.Read2Buffer();
  std::swap(temp_data_out, out_buffer);

  auto temp_result = _new_result(gpgme_op_encrypt_result(ctx_));
  std::swap(result, temp_result);

  return err;
}

GpgFrontend::GpgError GpgFrontend::GpgBasicOperator::Decrypt(
    BypeArrayRef in_buffer, GpgFrontend::ByteArrayPtr& out_buffer,
    GpgFrontend::GpgDecrResult& result) {
  gpgme_error_t err;

  GpgData data_in(in_buffer.data(), in_buffer.size()), data_out;
  err = check_gpg_error(gpgme_op_decrypt(ctx_, data_in, data_out));

  auto temp_data_out = data_out.Read2Buffer();
  std::swap(temp_data_out, out_buffer);

  auto temp_result = _new_result(gpgme_op_decrypt_result(ctx_));
  std::swap(result, temp_result);

  return err;
}

GpgFrontend::GpgError GpgFrontend::GpgBasicOperator::Verify(
    BypeArrayRef& in_buffer, ByteArrayPtr& sig_buffer,
    GpgVerifyResult& result) const {
  gpgme_error_t err;

  GpgData data_in(in_buffer.data(), in_buffer.size());
  GpgData data_out;

  if (sig_buffer != nullptr && sig_buffer->size() > 0) {
    GpgData sig_data(sig_buffer->data(), sig_buffer->size());
    err = check_gpg_error(gpgme_op_verify(ctx_, sig_data, data_in, nullptr));
  } else
    err = check_gpg_error(gpgme_op_verify(ctx_, data_in, nullptr, data_out));

  auto temp_result = _new_result(gpgme_op_verify_result(ctx_));
  std::swap(result, temp_result);

  return err;
}

GpgFrontend::GpgError GpgFrontend::GpgBasicOperator::Sign(
    KeyListPtr signers, BypeArrayRef in_buffer, ByteArrayPtr& out_buffer,
    gpgme_sig_mode_t mode, GpgSignResult& result) {
  gpgme_error_t err;

  // Set Singers of this opera
  SetSigners(*signers);

  GpgData data_in(in_buffer.data(), in_buffer.size()), data_out;

  err = check_gpg_error(gpgme_op_sign(ctx_, data_in, data_out, mode));

  auto temp_data_out = data_out.Read2Buffer();
  std::swap(temp_data_out, out_buffer);

  auto temp_result = _new_result(gpgme_op_sign_result(ctx_));

  std::swap(result, temp_result);

  return err;
}

gpgme_error_t GpgFrontend::GpgBasicOperator::DecryptVerify(
    BypeArrayRef in_buffer, ByteArrayPtr& out_buffer,
    GpgDecrResult& decrypt_result, GpgVerifyResult& verify_result) {
  gpgme_error_t err;

  GpgData data_in(in_buffer.data(), in_buffer.size()), data_out;

  err = check_gpg_error(gpgme_op_decrypt_verify(ctx_, data_in, data_out));

  auto temp_data_out = data_out.Read2Buffer();
  std::swap(temp_data_out, out_buffer);

  auto temp_decr_result = _new_result(gpgme_op_decrypt_result(ctx_));
  std::swap(decrypt_result, temp_decr_result);

  auto temp_verify_result = _new_result(gpgme_op_verify_result(ctx_));
  std::swap(verify_result, temp_verify_result);

  return err;
}

gpgme_error_t GpgFrontend::GpgBasicOperator::EncryptSign(
    KeyListPtr keys, KeyListPtr signers, BypeArrayRef in_buffer,
    ByteArrayPtr& out_buffer, GpgEncrResult& encr_result,
    GpgSignResult& sign_result) {
  gpgme_error_t err;
  SetSigners(*signers);

  // gpgme_encrypt_result_t e_result;
  gpgme_key_t recipients[keys->size() + 1];

  // set key for user
  int index = 0;
  for (const auto& key : *keys) recipients[index++] = gpgme_key_t(key);

  // Last entry dataIn array has to be nullptr
  recipients[keys->size()] = nullptr;

  GpgData data_in(in_buffer.data(), in_buffer.size()), data_out;

  // If the last parameter isnt 0, a private copy of data is made
  err = check_gpg_error(gpgme_op_encrypt_sign(
      ctx_, recipients, GPGME_ENCRYPT_ALWAYS_TRUST, data_in, data_out));

  auto temp_data_out = data_out.Read2Buffer();
  std::swap(temp_data_out, out_buffer);

  auto temp_encr_result = _new_result(gpgme_op_encrypt_result(ctx_));
  swap(encr_result, temp_encr_result);
  auto temp_sign_result = _new_result(gpgme_op_sign_result(ctx_));
  swap(sign_result, temp_sign_result);

  return err;
}

void GpgFrontend::GpgBasicOperator::SetSigners(KeyArgsList& signers) {
  gpgme_signers_clear(ctx_);
  for (const GpgKey& key : signers) {
    SPDLOG_DEBUG("key fpr: {}", key.GetFingerprint());
    if (key.IsHasActualSigningCapability()) {
      SPDLOG_DEBUG("signer");
      auto error = gpgme_signers_add(ctx_, gpgme_key_t(key));
      check_gpg_error(error);
    }
  }
  if (signers.size() != gpgme_signers_count(ctx_))
    SPDLOG_DEBUG("not all signers added");
}

std::unique_ptr<GpgFrontend::KeyArgsList>
GpgFrontend::GpgBasicOperator::GetSigners() {
  auto count = gpgme_signers_count(ctx_);
  auto signers = std::make_unique<std::vector<GpgKey>>();
  for (auto i = 0u; i < count; i++) {
    auto key = GpgKey(gpgme_signers_enum(ctx_, i));
    signers->push_back(GpgKey(std::move(key)));
  }
  return signers;
}

gpg_error_t GpgFrontend::GpgBasicOperator::EncryptSymmetric(
    GpgFrontend::ByteArray& in_buffer, GpgFrontend::ByteArrayPtr& out_buffer,
    GpgFrontend::GpgEncrResult& result) {
  // deepcopy from ByteArray to GpgData
  GpgData data_in(in_buffer.data(), in_buffer.size()), data_out;

  gpgme_error_t err = check_gpg_error(gpgme_op_encrypt(
      ctx_, nullptr, GPGME_ENCRYPT_SYMMETRIC, data_in, data_out));

  auto temp_data_out = data_out.Read2Buffer();
  std::swap(temp_data_out, out_buffer);

  // TODO(Saturneric): maybe a bug of gpgme
  if (gpgme_err_code(err) == GPG_ERR_NO_ERROR) {
    auto temp_result = _new_result(gpgme_op_encrypt_result(ctx_));
    std::swap(result, temp_result);
  }

  return err;
}
