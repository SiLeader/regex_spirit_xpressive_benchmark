//
// Created by cerussite on 2/20/20.
//

#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/qi.hpp>
#include <string>

#include "types.hpp"

namespace qi = boost::spirit::qi;

namespace rb {

namespace spirit_internal {

template <class Iterator>
struct Cql : qi::grammar<Iterator, Insert(), qi::ascii::space_type> {
  template <class Data>
  using Rule = qi::rule<Iterator, Data, qi::ascii::space_type>;

  Rule<Id()> id;
  Rule<Table()> table;
  Rule<IdList()> id_list;

  Rule<Number()> number;
  Rule<String()> string_content;
  Rule<String()> string;
  Rule<Value()> value;
  Rule<ValueList()> value_list;

  Rule<Insert()> insert;

  struct Appender {
    void operator()(std::string& lhs, char ch) const {
      lhs += ch;
    }
    void operator()(std::string& lhs, const boost::optional<char>& ch) const {
      if (ch) {
        (*this)(lhs, *ch);
      }
    }

    void operator()(std::string& lhs, const std::string& rhs) const {
      lhs += rhs;
    }
    void operator()(std::string& lhs, const boost::optional<std::string>& ch) const {
      if (ch) {
        (*this)(lhs, *ch);
      }
    }
  };

  Cql() : Cql::base_type(insert) {
    using boost::phoenix::bind;
    using boost::phoenix::push_back;
    using qi::_1;
    using qi::_val;

    id = +((qi::alnum | qi::char_('_'))[_val += _1]);
    id_list = id[push_back(_val, _1)] >> *(',' >> id[push_back(_val, _1)]);
    table = ((id[bind(&Table::keyspace, _val) = _1] >> '.' >> id[bind(&Table::table, _val) = _1]) | id[bind(&Table::table, _val) = _1]);

    number = qi::double_[_val = _1];
    string_content = *(qi::alnum | qi::char_('_') | qi::lit("''"))[bind(Appender(), _val, _1)];
    string = qi::char_('\'')[_val += _1] >> string_content[_val += _1] >> qi::char_('\'')[_val += _1];
    value = number[_val = _1] | string[_val = _1];
    value_list = value[push_back(_val, _1)] >> *(',' >> value[push_back(_val, _1)]);

    insert = qi::no_case["INSERT"] >> qi::no_case["INTO"] >> table[bind(&Insert::table, _val) = _1] >> '(' >> id_list[bind(&Insert::columns, _val) = _1] >> ')' >> qi::no_case["VALUES"] >> '(' >> value_list[bind(&Insert::data, _val) = _1] >> ')';
  }
};

}  // namespace spirit_internal

Insert spirit(const std::string& str) {
  using CqlParser = spirit_internal::Cql<std::string::const_iterator>;

  //std::cout << str << std::endl;
  CqlParser parser;
  auto itr = std::begin(str);

  Insert insert;
  qi::phrase_parse(itr, std::end(str), parser, qi::ascii::space, insert);
  return insert;
}

}  // namespace rb
