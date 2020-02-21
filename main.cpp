#include <boost/algorithm/string.hpp>
#include <boost/fusion/container/vector.hpp>
#include <chrono>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "types.hpp"

namespace rb {

Query xpressive(const std::string& str);
Query regex(const std::string& str);
Query spirit(const std::string& str);

}  // namespace rb

namespace {

template <class Callback>
void SpeedTest(const std::string& name, Callback callback, const std::vector<std::string>& queries) {
  auto begin = std::chrono::system_clock::now();

  for (const auto& query : queries) {
    callback(query);
  }

  auto end = std::chrono::system_clock::now();

  std::cout << name << ": elapsed: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms" << std::endl;
}

template <class Callback>
void Print(const std::string& name, Callback callback, const std::vector<std::string>& queries) {
  rb::QueryPrint qp;

  std::cout << name << std::endl;
  for (const auto& query : queries) {
    std::cout << query << std::endl;
    boost::apply_visitor(qp, callback(query));
  }

  std::cout << std::endl;
}

std::vector<std::string> CreateQuery(int select_count, int insert_count, int update_count) {
  static std::mt19937 mt(std::random_device{}());

  static const auto generate_random_string = [] {
    static constexpr char kChars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    static constexpr std::size_t kLength = 32;
    static std::uniform_int_distribution<> rand(0, sizeof(kChars) / sizeof(kChars[0]) - 2);

    std::string str = "'";
    str.reserve(kLength);

    for (std::size_t i = 0; i < kLength; ++i) {
      str += kChars[rand(mt)];
    }
    str += "'";
    return str;
  };

  static const auto generate_string_set = [](std::size_t length) {
    std::vector<std::string> set;
    set.reserve(length);

    for (std::size_t i = 0; i < length; ++i) {
      set.emplace_back(generate_random_string());
    }

    return set;
  };

  std::vector<std::string> queries;
  queries.reserve(select_count + insert_count + update_count);

  for (int i = 0; i < select_count; ++i) {
    std::stringstream ss;
    ss << "SELECT * FROM ycsb.usertable WHERE y_id = " << generate_random_string() << std::flush;
    queries.emplace_back(ss.str());
  }

  for (int i = 0; i < insert_count; ++i) {
    std::stringstream ss;
    ss << "INSERT INTO ycsb.usertable(y_id, field0, field1, field2, field3, field4, field5, field6, field7, field8, field9) VALUES (";
    ss << boost::algorithm::join(generate_string_set(11), ", ");
    ss << ")" << std::flush;
    queries.emplace_back(ss.str());
  }

  for (int i = 0; i < update_count; ++i) {
    std::stringstream ss;

    auto sset = generate_string_set(10);
    ss << "UPDATE ycsb.usertable SET ";
    for (std::size_t ii = 0; ii < sset.size(); ++ii) {
      sset[ii] = "field" + std::to_string(ii) + " = " + sset[ii];
    }
    ss << boost::algorithm::join(sset, ", ");
    ss << " WHERE y_id = " << generate_random_string() << std::flush;
    queries.emplace_back(ss.str());
  }

  std::shuffle(std::begin(queries), std::end(queries), mt);

  return queries;
}

}  // namespace

//#define PRINT 1

int main() {
#if defined(PRINT)
  static constexpr int kSelectCount = 1;
  static constexpr int kInsertCount = 1;
  static constexpr int kUpdateCount = 1;
  auto queries = CreateQuery(kSelectCount, kInsertCount, kUpdateCount);
  Print("Boost.Xpressive", rb::xpressive, queries);
  Print("std::regex", rb::regex, queries);
  Print("Boost.Spirit.Qi", rb::spirit, queries);
#else
  static constexpr int kSelectCount = 10000;
  static constexpr int kInsertCount = 10000;
  static constexpr int kUpdateCount = 10000;
  auto queries = CreateQuery(kSelectCount, kInsertCount, kUpdateCount);
  SpeedTest("Boost.Xpressive", rb::xpressive, queries);
  SpeedTest("std::regex", rb::regex, queries);
  SpeedTest("Boost.Spirit.Qi", rb::spirit, queries);
#endif

  return 0;
}
