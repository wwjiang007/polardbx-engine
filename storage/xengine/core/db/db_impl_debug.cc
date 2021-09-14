// Portions Copyright (c) 2020, Alibaba Group Holding Limited
//  Copyright (c) 2011-present, Facebook, Inc.  All rights reserved.
//  This source code is licensed under the BSD-style license found in the
//  LICENSE file in the root directory of this source tree. An additional grant
//  of patent rights can be found in the PATENTS file in the same directory.
//
// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef NDEBUG

#include "db/db_impl.h"
#include "monitoring/thread_status_updater.h"
#include "table/filter_manager.h"
#include "table/internal_iterator.h"

using namespace xengine;
using namespace common;
using namespace util;
using namespace monitor;
using namespace table;
using namespace storage;

namespace xengine {
namespace db {

uint64_t DBImpl::TEST_GetLevel0TotalSize() {
  InstrumentedMutexLock l(&mutex_);
  return 0;
}

void DBImpl::TEST_HandleWALFull() {
  WriteContext write_context;
  InstrumentedMutexLock l(&mutex_);
  HandleWALFull(&write_context);
}

int64_t DBImpl::TEST_MaxNextLevelOverlappingBytes(
    ColumnFamilyHandle* column_family) {
  return 0;
}

void DBImpl::TEST_GetFilesMetaData(
    ColumnFamilyHandle* column_family,
    std::vector<std::vector<FileMetaData>>* metadata) {
}

uint64_t DBImpl::TEST_Current_Manifest_FileNo() {
  return versions_->manifest_file_number();
}

Status DBImpl::TEST_CompactRange(int level, const Slice* begin,
                                 const Slice* end,
                                 ColumnFamilyHandle* column_family,
                                 bool disallow_trivial_move) {
  Status s = CompactRange(CompactRangeOptions(), column_family, begin, end, TaskType::STREAM_COMPACTION_TASK);
  if (!s.ok()) {
    return s;
  } else {
    TEST_WaitForCompact();
    return s;
  }
}

Status DBImpl::TEST_FlushMemTable(bool wait, ColumnFamilyHandle* cfh) {
  FlushOptions fo;
  fo.wait = wait;
  ColumnFamilyData* cfd;
  if (cfh == nullptr) {
    cfd = default_cf_handle_->cfd();
  } else {
    auto cfhi = reinterpret_cast<ColumnFamilyHandleImpl*>(cfh);
    cfd = cfhi->cfd();
  }
  return FlushMemTable(cfd, fo);
}

Status DBImpl::TEST_WaitForFlushMemTable(ColumnFamilyHandle* column_family) {
  ColumnFamilyData* cfd;
  if (column_family == nullptr) {
    cfd = default_cf_handle_->cfd();
  } else {
    auto cfh = reinterpret_cast<ColumnFamilyHandleImpl*>(column_family);
    cfd = cfh->cfd();
  }
  return WaitForFlushMemTable(cfd);
}

Status DBImpl::TEST_WaitForCompact() {
  // Wait until the compaction completes

  // TODO: a bug here. This function actually does not necessarily
  // wait for compact. It actually waits for scheduled compaction
  // OR flush to finish.

  InstrumentedMutexLock l(&mutex_);
  while ((bg_compaction_scheduled_ || bg_flush_scheduled_) && bg_error_.ok()) {
    bg_cv_.Wait();
  }
  return bg_error_;
}

void DBImpl::TEST_wait_for_filter_build() {
  while (filter_build_quota_.load() <
         mutable_db_options_.filter_building_threads) {
    port::AsmVolatilePause();
  }
}

void DBImpl::TEST_LockMutex() { mutex_.Lock(); }

void DBImpl::TEST_UnlockMutex() { mutex_.Unlock(); }

void* DBImpl::TEST_BeginWrite() {
  auto w = new WriteThread::Writer();
  write_thread_.EnterUnbatched(w, &mutex_);
  return reinterpret_cast<void*>(w);
}

void DBImpl::TEST_EndWrite(void* w) {
  auto writer = reinterpret_cast<WriteThread::Writer*>(w);
  write_thread_.ExitUnbatched(writer);
  delete writer;
}

size_t DBImpl::TEST_LogsToFreeSize() {
  InstrumentedMutexLock l(&mutex_);
  return logs_to_free_.size();
}

uint64_t DBImpl::TEST_LogfileNumber() {
  InstrumentedMutexLock l(&mutex_);
  return logfile_number_;
}

Status DBImpl::TEST_GetAllImmutableCFOptions(
    std::unordered_map<std::string, const ImmutableCFOptions*>* iopts_map) {
  std::vector<std::string> cf_names;
  std::vector<const ImmutableCFOptions*> iopts;
  {
    InstrumentedMutexLock l(&mutex_);
    /*
    for (auto cfd : *versions_->GetColumnFamilySet()) {
      cf_names.push_back(cfd->GetName());
      iopts.push_back(cfd->ioptions());
    }*/
  }
  iopts_map->clear();
  for (size_t i = 0; i < cf_names.size(); ++i) {
    iopts_map->insert({cf_names[i], iopts[i]});
  }

  return Status::OK();
}

uint64_t DBImpl::TEST_FindMinLogContainingOutstandingPrep() {
  return FindMinLogContainingOutstandingPrep();
}

uint64_t DBImpl::TEST_FindMinPrepLogReferencedByMemTable() {
  return FindMinPrepLogReferencedByMemTable();
}

int DBImpl::TEST_create_subtable(const ColumnFamilyDescriptor &cf, int32_t tid, ColumnFamilyHandle *&handle)
{
  int ret = Status::kOk;
  const int64_t TEST_INDEX_ID_BASE = 100;
  common::ColumnFamilyOptions cf_options;
  CreateSubTableArgs args(tid, cf_options, true, tid);

  if (FAILED(CreateColumnFamily(args, &handle).code())) {
    XENGINE_LOG(WARN, "fail to create subtable", K(ret));
  }

  return ret;
}

Status DBImpl::TEST_GetLatestMutableCFOptions(
    ColumnFamilyHandle* column_family, MutableCFOptions* mutable_cf_options) {
  InstrumentedMutexLock l(&mutex_);

  auto cfh = reinterpret_cast<ColumnFamilyHandleImpl*>(column_family);
  *mutable_cf_options = *cfh->cfd()->GetLatestMutableCFOptions();
  return Status::OK();
}

int DBImpl::TEST_BGCompactionsAllowed() const {
  InstrumentedMutexLock l(&mutex_);
  return BGCompactionsAllowed();
}
}
}  // namespace xengine
#endif  // NDEBUG
