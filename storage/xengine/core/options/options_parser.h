/*
 * Portions Copyright (c) 2020, Alibaba Group Holding Limited
 */
// Copyright (c) 2011-present, Facebook, Inc.  All rights reserved.
// This source code is licensed under the BSD-style license found in the
// LICENSE file in the root directory of this source tree. An additional grant
// of patent rights can be found in the PATENTS file in the same directory.

#pragma once

#include <map>
#include <string>
#include <vector>

#include "options/options_helper.h"
#include "options/options_sanity_check.h"
#include "table/block_based_table_factory.h"
#include "xengine/env.h"
#include "xengine/options.h"

namespace xengine {
namespace common {

#ifndef ROCKSDB_LITE

#define ROCKSDB_OPTION_FILE_MAJOR 1
#define ROCKSDB_OPTION_FILE_MINOR 1

enum OptionSection : char {
  kOptionSectionVersion = 0,
  kOptionSectionDBOptions,
  kOptionSectionCFOptions,
  kOptionSectionTableOptions,
  kOptionSectionUnknown
};

static const std::string opt_section_titles[] = {
    "Version", "DBOptions", "CFOptions", "TableOptions/", "Unknown"};

Status PersistRocksDBOptions(const DBOptions& db_opt,
                             const std::vector<std::string>& cf_names,
                             const std::vector<ColumnFamilyOptions>& cf_opts,
                             const std::string& file_name, util::Env* env);

class RocksDBOptionsParser {
 public:
  explicit RocksDBOptionsParser();
  ~RocksDBOptionsParser() {}
  void Reset();

  common::Status Parse(const std::string& file_name, util::Env* env);
  static std::string TrimAndRemoveComment(const std::string& line,
                                          const bool trim_only = false);

  const DBOptions* db_opt() const { return &db_opt_; }
  const std::unordered_map<std::string, std::string>* db_opt_map() const {
    return &db_opt_map_;
  }
  const std::vector<ColumnFamilyOptions>* cf_opts() const { return &cf_opts_; }
  const std::vector<std::string>* cf_names() const { return &cf_names_; }
  const std::vector<std::unordered_map<std::string, std::string>>* cf_opt_maps()
      const {
    return &cf_opt_maps_;
  }

  const ColumnFamilyOptions* GetCFOptions(const std::string& name) {
    return GetCFOptionsImpl(name);
  }
  size_t NumColumnFamilies() { return cf_opts_.size(); }

  static common::Status VerifyRocksDBOptionsFromFile(
      const DBOptions& db_opt, const std::vector<std::string>& cf_names,
      const std::vector<ColumnFamilyOptions>& cf_opts,
      const std::string& file_name, util::Env* env,
      OptionsSanityCheckLevel sanity_check_level = kSanityLevelExactMatch);

  static common::Status VerifyDBOptions(
      const DBOptions& base_opt, const DBOptions& new_opt,
      const std::unordered_map<std::string, std::string>* new_opt_map = nullptr,
      OptionsSanityCheckLevel sanity_check_level = kSanityLevelExactMatch);

  static common::Status VerifyCFOptions(
      const ColumnFamilyOptions& base_opt, const ColumnFamilyOptions& new_opt,
      const std::unordered_map<std::string, std::string>* new_opt_map = nullptr,
      OptionsSanityCheckLevel sanity_check_level = kSanityLevelExactMatch);

  static common::Status VerifyTableFactory(
      const table::TableFactory* base_tf, const table::TableFactory* file_tf,
      OptionsSanityCheckLevel sanity_check_level = kSanityLevelExactMatch);

  static common::Status VerifyBlockBasedTableFactory(
      const table::BlockBasedTableFactory* base_tf,
      const table::BlockBasedTableFactory* file_tf,
      OptionsSanityCheckLevel sanity_check_level);

  static common::Status ExtraParserCheck(
      const RocksDBOptionsParser& input_parser);

 protected:
  bool IsSection(const std::string& line);
  common::Status ParseSection(OptionSection* section, std::string* title,
                              std::string* argument, const std::string& line,
                              const int line_num);

  common::Status CheckSection(const OptionSection section,
                              const std::string& section_arg,
                              const int line_num);

  common::Status ParseStatement(std::string* name, std::string* value,
                                const std::string& line, const int line_num);

  common::Status EndSection(
      const OptionSection section, const std::string& title,
      const std::string& section_arg,
      const std::unordered_map<std::string, std::string>& opt_map);

  common::Status ValidityCheck();

  common::Status InvalidArgument(const int line_num,
                                 const std::string& message);

  common::Status ParseVersionNumber(const std::string& ver_name,
                                    const std::string& ver_string,
                                    const int max_count, int* version);

  ColumnFamilyOptions* GetCFOptionsImpl(const std::string& name) {
    assert(cf_names_.size() == cf_opts_.size());
    for (size_t i = 0; i < cf_names_.size(); ++i) {
      if (cf_names_[i] == name) {
        return &cf_opts_[i];
      }
    }
    return nullptr;
  }

 private:
  DBOptions db_opt_;
  std::unordered_map<std::string, std::string> db_opt_map_;
  std::vector<std::string> cf_names_;
  std::vector<ColumnFamilyOptions> cf_opts_;
  std::vector<std::unordered_map<std::string, std::string>> cf_opt_maps_;
  bool has_version_section_;
  bool has_db_options_;
  bool has_default_cf_options_;
  int db_version[3];
  int opt_file_version[3];
};

#endif  // !ROCKSDB_LITE

}  // namespace common
}  // namespace xengine
