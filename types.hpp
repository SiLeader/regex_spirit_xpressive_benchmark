//
// Created by cerussite on 2/20/20.
//

#pragma once

#include <boost/variant.hpp>
#include <string>
#include <vector>

namespace rb {

using Id = std::string;
struct Table {
  Id keyspace;
  Id table;
};
using IdList = std::vector<Id>;
using Number = double;
using String = std::string;
using Value = boost::variant<Number, String>;
using ValueList = std::vector<Value>;

struct Insert {
  Table table;
  IdList columns;
  ValueList data;
};

}  // namespace rb
