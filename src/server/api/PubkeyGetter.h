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

#ifndef GPGFRONTEND_ZH_CN_TS_PUBKEYGETTER_H
#define GPGFRONTEND_ZH_CN_TS_PUBKEYGETTER_H

#include "GpgFrontend.h"
#include "gpg/GpgContext.h"
#include "server/BaseAPI.h"

class ComUtils;

/**
 * Get and Import Pubkey from server
 */
class PubkeyGetter : public BaseAPI {
 public:
  PubkeyGetter(GpgFrontend::GpgContext *ctx, const QVector<QString> &fprs);

 private:
  GpgFrontend::GpgContext *mCtx;

  const QVector<QString> &mFprs;

 protected:
  void construct_json() final;

  void deal_reply() final;
};

#endif  // GPGFRONTEND_ZH_CN_TS_PUBKEYGETTER_H
