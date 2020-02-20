#include <boost/fusion/container/vector.hpp>
#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "types.hpp"

namespace rb {

Insert xpressive(const std::string& query);
Insert regex(const std::string& query);
Insert spirit(const std::string& str);

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

}  // namespace

int main() {
  std::ifstream fin("test.sql");
  std::vector<std::string> queries;

  while (fin) {
    std::string query;
    std::getline(fin, query);
    if (query.empty()) continue;

    queries.emplace_back(query);
  }

  SpeedTest("Boost.Xpressive", rb::xpressive, queries);
  SpeedTest("std::regex", rb::regex, queries);
  SpeedTest("Boost.Spirit.Qi", rb::spirit, queries);

  return 0;
}
