//
// Created by cerussite on 2/20/20.
//

#pragma once

#include <boost/algorithm/string.hpp>

#include "types.hpp"

namespace rb {
namespace utils {

inline Table CreateTable(const std::string& str) {
  std::vector<std::string> ks;
  boost::algorithm::split(ks, str, boost::is_any_of("."));
  Table table;

  if (ks.size() >= 2) {
    table.keyspace = ks[0];
    table.table = ks[1];
  } else {
    table.table = ks[0];
  }

  return table;
}

template <class Pred>
std::vector<std::string> SplitAndTrim(const std::string& str, Pred&& delim) {
  std::vector<std::string> ks(2);
  boost::algorithm::split(ks, str, std::forward<Pred>(delim));

  std::transform(std::begin(ks), std::end(ks), std::begin(ks), [](const std::string& str) { return boost::algorithm::trim_copy(str); });

  return ks;
}

inline ValueList CreateValues(const std::string& str) {
  auto values_str = SplitAndTrim(str, boost::is_any_of(","));
  ValueList values(values_str.size());

  std::transform(std::begin(values_str), std::end(values_str), std::begin(values), [](const std::string& value_str) -> Value {
    if (value_str[0] == '\'') {
      return value_str;
    }
    return std::stod(value_str);
  });

  return values;
}

}  // namespace utils
}  // namespace rb
