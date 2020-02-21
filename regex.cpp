//
// Created by cerussite on 2/20/20.
//

#include <iostream>
#include <regex>
#include <string>

#include "types.hpp"
#include "utils_for_regex.hpp"

namespace rb {

Query regex(const std::string& str) {
  static const std::regex INSERT(R"rrr(INSERT\s+INTO\s+([\w\d\.]+)\s*\(([\w\d,\s]+)\)\s*VALUES\s*\(([\d\w\s'_,]+)\))rrr", std::regex_constants::icase);
  static const std::regex UPDATE(R"rrr(UPDATE\s+([\w\d\.]+)\s+SET\s+([\w\d,'=\s]+)\s+WHERE\s+([\w\d\s=']+))rrr", std::regex_constants::icase);
  static const std::regex SELECT(R"rrr(SELECT\s+([\w\d,\s]+|\*)\s+FROM\s+([\w\d\.]+)(\s+WHERE\s+([\w\d\s=']+))?)rrr", std::regex_constants::icase);

  Query query;

  std::smatch sm;
  if (regex_match(str, sm, INSERT)) {
    Insert insert;
    insert.table = rb::utils::CreateTable(sm[1]);
    insert.columns = rb::utils::SplitAndTrim(sm[2], boost::is_any_of(","));
    insert.data = rb::utils::CreateValues(sm[3]);

    query = insert;

  } else if (regex_match(str, sm, UPDATE)) {
    Update update = {};
    update.table = rb::utils::CreateTable(sm[1]);
    update.kvs = rb::utils::CreateKeyValueList(sm[2]);
    update.where = rb::utils::CreateKeyValue(sm[3]);

    query = update;

  } else if (regex_match(str, sm, SELECT)) {
    Select select = {};

    select.table = rb::utils::CreateTable(sm[2]);
    if (sm[1] != "*") {
      select.columns = rb::utils::SplitAndTrim(sm[1], boost::is_any_of(","));
    }
    if (sm.size() >= 4) {
      select.where = rb::utils::CreateKeyValue(sm[4]);
    }

    query = select;
  }
  return query;
}

}  // namespace rb
