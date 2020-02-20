//
// Created by cerussite on 2/20/20.
//

#include <iostream>
#include <regex>
#include <string>

#include "types.hpp"
#include "utils_for_regex.hpp"

namespace rb {

Insert regex(const std::string& query) {
  static const std::regex INSERT(R"rrr(INSERT\s+INTO\s+([\w\d\.]+)\s*\(([\w\d,\s]+)\)\s*VALUES\s*\(([\d\w\s'_,]+)\))rrr", std::regex_constants::icase);

  Insert insert = {};

  std::smatch sm;
  if (regex_match(query, sm, INSERT)) {
    insert.table = rb::utils::CreateTable(sm[1]);
    insert.columns = rb::utils::SplitAndTrim(sm[2], boost::is_any_of(","));
    insert.data = rb::utils::CreateValues(sm[3]);
  }
  return insert;
}

}  // namespace rb
