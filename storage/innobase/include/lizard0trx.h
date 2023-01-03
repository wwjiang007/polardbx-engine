/* Copyright (c) 2018, 2021, Alibaba and/or its affiliates. All rights reserved.
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License, version 2.0,
   as published by the Free Software Foundation.
   This program is also distributed with certain software (including
   but not limited to OpenSSL) that is licensed under separate terms,
   as designated in a particular file or component or in included license
   documentation.  The authors of MySQL/PolarDB-X Engine hereby grant you an
   additional permission to link the program and your derivative works with the
   separately licensed software that they have included with
   MySQL/PolarDB-X Engine.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License, version 2.0, for more details.
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA */


/** @file include/lizard0trx.h
  Lizard transaction structure.

 Created 2020-08-27 by Jianwei.zhao
 *******************************************************/

#ifndef lizard0trx_h
#define lizard0trx_h

#include <map>

#include "trx0types.h"

struct trx_t;

typedef std::unordered_map<trx_id_t, trx_t *, std::hash<trx_id_t>,
                           std::equal_to<trx_id_t>,
                           ut_allocator<std::pair<const trx_id_t, trx_t *>>>
    TrxIdHash;

typedef std::pair<trx_id_t, trx_t *> TrxPair;

namespace lizard {

extern void copy_to(TrxIdHash &h, TrxIdSet &s);

/** Alloc & release cleanout cursors. */
extern void alloc_cleanout_cursors(trx_t *trx);
extern void release_cleanout_cursors(trx_t *trx);

extern void cleanout_rows_at_commit(trx_t *trx);

}  // namespace lizard

#endif
