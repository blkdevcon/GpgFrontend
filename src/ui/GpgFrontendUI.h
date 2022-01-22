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

#ifndef GPGFRONTEND_GPGFRONTENDUI_H
#define GPGFRONTEND_GPGFRONTENDUI_H

#include <QtCore>
#include <QtNetwork>
#include <QtPrintSupport>
#include <QtWidgets>
#include <optional>

#include "GpgFrontend.h"
#include "gpg/GpgConstants.h"
#include "gpg/GpgModel.h"

#undef LIBCONFIGXX_STATIC
#define LIBCONFIGXX_STATIC
#include <qt-aes/qaesencryption.h>

#include <libconfig.h++>

#ifdef SMTP_SUPPORT
#include <SmtpMime>
#endif

#define VMIME_STATIC
#undef VMIME_HAVE_MLANG
#include <vmime/vmime.hpp>

/**
 * Resources File(s) Path Vars
 */
#if defined(MACOS) && defined(RELEASE)
#define RESOURCE_DIR(appDir) (appDir + "/../Resources/")
#define RESOURCE_DIR_BOOST_PATH(appDir) (appDir / ".." / "Resources")
#elif defined(LINUX) && defined(RELEASE)
#define RESOURCE_DIR(appDir) (appDir + "/../share/")
#define RESOURCE_DIR_BOOST_PATH(appDir) (appDir / ".." / "share")
#else
#define RESOURCE_DIR(appDir) (appDir)
#define RESOURCE_DIR_BOOST_PATH(appDir) (appDir)
#endif

#endif  // GPGFRONTEND_GPGFRONTENDUI_H
