//
// Created by cerussite on 2/20/20.
//

#include <boost/xpressive/xpressive.hpp>
#include <iostream>

#include "types.hpp"
#include "utils_for_regex.hpp"

namespace rb {

Insert xpressive(const std::string& query) {
  using namespace boost::xpressive;
  using namespace std::literals::string_literals;

  static const sregex ID_FRAGMENT = _w | _d | '_';
  static const sregex ID = +ID_FRAGMENT;
  static const sregex ID_LIST = *(ID_FRAGMENT | ',' | _s);
  static const sregex KS_TABLE = (ID >> *_s >> '.' >> *_s >> ID) | ID;

  static const sregex RAW_DATA_LIST = *(_d | _w | _s | '\'' | '_' | ',');

  static const sregex INSERT = icase("INSERT") >> +_s >> icase("INTO") >> +_s >> (s1 = KS_TABLE) >> *_s >> '(' >> (s2 = ID_LIST) >> ')' >> *_s >> icase("VALUES") >> *_s >> '(' >> (s3 = RAW_DATA_LIST) >> ')';

  Insert insert = {};

  smatch sm;
  if (regex_match(query, sm, INSERT)) {
    insert.table = rb::utils::CreateTable(sm[1]);
    insert.columns = rb::utils::SplitAndTrim(sm[2], boost::is_any_of(","));
    insert.data = rb::utils::CreateValues(sm[3]);
  }

  return insert;
}

}  // namespace rb
