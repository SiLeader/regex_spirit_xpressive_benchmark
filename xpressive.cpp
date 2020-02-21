//
// Created by cerussite on 2/20/20.
//

#include <boost/xpressive/xpressive.hpp>
#include <iostream>

#include "types.hpp"
#include "utils_for_regex.hpp"

namespace rb {

Query xpressive(const std::string& str) {
  using namespace boost::xpressive;
  using namespace std::literals::string_literals;

  static const sregex ID_FRAGMENT = _w | _d | '_';
  static const sregex ID = +ID_FRAGMENT;
  static const sregex ID_LIST = *(ID_FRAGMENT | ',' | _s);
  static const sregex KS_TABLE = (ID >> *_s >> '.' >> *_s >> ID) | ID;

  static const sregex RAW_DATA_FRAGMENT = _d | _w | _s | '\'' | '_' | ',';
  static const sregex RAW_DATA_LIST = *RAW_DATA_FRAGMENT;

  static const sregex KV_FRAGMENT = RAW_DATA_FRAGMENT | '=';

  static const sregex INSERT = icase("INSERT") >> +_s >> icase("INTO") >> +_s >> (s1 = KS_TABLE) >> *_s >> '(' >> (s2 = ID_LIST) >> ')' >> *_s >> icase("VALUES") >> *_s >> '(' >> (s3 = RAW_DATA_LIST) >> ')';
  static const sregex UPDATE = icase("UPDATE") >> +_s >> (s1 = KS_TABLE) >> +_s >> icase("SET") >> +_s >> (s2 = +KV_FRAGMENT) >> +_s >> icase("WHERE") >> +_s >> (s3 = +KV_FRAGMENT);
  static const sregex SELECT = icase("SELECT") >> +_s >> (s1 = (+(ID_FRAGMENT | _s | ',') | '*')) >> +_s >> icase("FROM") >> +_s >> (s2 = KS_TABLE) >> !(+_s >> icase("WHERE") >> +_s >> (s3 = +KV_FRAGMENT));

  Query query;

  smatch sm;
  if (regex_match(str, sm, INSERT)) {
    Insert insert = {};
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
    if (sm.size() >= 3) {
      select.where = rb::utils::CreateKeyValue(sm[3]);
    }

    query = select;
  }

  return query;
}

}  // namespace rb
