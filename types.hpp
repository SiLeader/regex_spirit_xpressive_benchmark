//
// Created by cerussite on 2/20/20.
//

#pragma once

#include <boost/optional.hpp>
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
struct KeyValue {
  Id key;
  Value value;
};
using KeyValueList = std::vector<KeyValue>;

struct Insert {
  Table table;
  IdList columns;
  ValueList data;
};

struct Select {
  Table table;
  IdList columns;
  boost::optional<KeyValue> where;
};

struct Update {
  Table table;
  KeyValueList kvs;
  KeyValue where;
};

using Query = boost::variant<Select, Update, Insert>;

struct QueryPrint {
  void operator()(const Insert& insert) const {
    assert(insert.columns.size() == insert.data.size());

    std::cout << "insert" << std::endl;
    std::cout << "  table: " << insert.table.keyspace << "." << insert.table.table << std::endl;
    std::cout << "  data:" << std::endl;
    for (std::size_t i = 0; i < insert.columns.size(); ++i) {
      std::cout << "    " << insert.columns[i] << "=" << insert.data[i] << std::endl;
    }
  }

  void operator()(const Select& select) const {
    std::cout << "select" << std::endl;
    std::cout << "  table: " << select.table.keyspace << "." << select.table.table << std::endl;
    std::cout << "  columns: ";
    if (select.columns.empty()) {
      std::cout << "*" << std::endl;
    } else {
      std::cout << "\n";
      for (const auto& column : select.columns) {
        std::cout << "    " << column << std::endl;
      }
    }
    std::cout << "  where: ";
    if (select.where) {
      std::cout << select.where->key << "=" << select.where->value << std::endl;
    } else {
      std::cout << "null" << std::endl;
    }
  }

  void operator()(const Update& update) const {
    std::cout << "update" << std::endl;
    std::cout << "  table: " << update.table.keyspace << "." << update.table.table << std::endl;
    std::cout << "  data: " << std::endl;
    for (const auto& kv : update.kvs) {
      std::cout << "    " << kv.key << "=" << kv.value << std::endl;
    }

    std::cout << "  where: " << update.where.key << "=" << update.where.value << std::endl;
  }
};

}  // namespace rb
