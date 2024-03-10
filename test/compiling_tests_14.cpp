/*
 * Trompeloeil C++ mocking framework
 *
 * Copyright Björn Fahller 2014-2018
 * Copyright Tore Martin Hagen 2019
 *
 *  Use, modification and distribution is subject to the
 *  Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt)
 *
 * Project home: https://github.com/rollbear/trompeloeil
 */

#include "compiling_tests.hpp"
#include "test_reporter.hpp"

#if defined(CATCH2_VERSION) && CATCH2_VERSION == 3
#include <catch2/catch_test_macros.hpp>
#else
#include <catch2/catch.hpp>
#endif

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <map>
#include <memory>
#include <regex>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>


#if TROMPELOEIL_CPLUSPLUS > 201103L

// Exercise C++14 Trompeloeil interface

using namespace std::string_literals;

using trompeloeil::_;

namespace
{
  /*
   * These types are local to this file as their definition depends on
   * macros whose names are particular to each API version.
   */

  template <typename T>
  class tmock
  {
  public:
    MAKE_MOCK1(func, void(int));
    MAKE_MOCK1(tfunc, void(T));

    tmock() : m(NAMED_FORBID_CALL(*this, func(_))) {}

  private:
    std::unique_ptr<trompeloeil::expectation> m;
  };

  // multiple inheritance

  struct combined
    : mock_c
    , tmock<int>
  {
  };

  class self_ref_mock
  {
  public:
    void expect_self()
    {
      exp = NAMED_REQUIRE_CALL(*this, mfunc());
    }
    MAKE_MOCK0(mfunc, void());
    std::unique_ptr<trompeloeil::expectation> exp;
  };

#define MANY_REQS(obj)                          \
               REQUIRE_CALL(obj, f0());         \
               REQUIRE_CALL(obj, f1(0));        \
               REQUIRE_CALL(obj, f2(0,1))

} /* unnamed namespace */

// mock_interface<> tests

TEST_CASE_METHOD(
  Fixture,
  "C++14: mock from interface is callable like any other",
  "[C++14][mock_interface]")
{
  mi imock;
  REQUIRE_CALL(imock, func(3))
    .RETURN(4);
  REQUIRE_CALL(imock, cfunc(3))
    .RETURN(5);
  REQUIRE_CALL(imock, func3(1,2,"three"))
    .RETURN(6);
  const mi& cimock = imock;
  REQUIRE_CALL(cimock, func3(2,3,"four"))
    .RETURN(7);
  REQUIRE(imock.func(3) == 4);
  REQUIRE(imock.cfunc(3) == 5);
  REQUIRE(imock.func3(1,2,"three") == 6);
  REQUIRE(cimock.func3(2,3,"four") == 7);

}

// IN_SEQUENCE tests

TEST_CASE_METHOD(
  Fixture,
  "C++14: follow single sequence gives no reports",
  "[C++14][sequences]")
{
  { // Compile-time tests
    static_assert(std::is_default_constructible<trompeloeil::sequence>::value, "Should be default constructible");
    static_assert(std::is_nothrow_move_constructible<trompeloeil::sequence>::value, "Should be move constructible");
    static_assert(std::is_nothrow_move_assignable<trompeloeil::sequence>::value, "Should be move assignable");
    static_assert(!std::is_copy_constructible<trompeloeil::sequence>::value, "Should NOT be copy constructible");
    static_assert(!std::is_copy_assignable<trompeloeil::sequence>::value, "Should NOT be copy assignable");
  }

  {
    mock_c obj1(1), obj2("apa");

    auto seq = trompeloeil::sequence{}; // Use "almost always auto" style

    REQUIRE_CALL(obj1, count())
      .IN_SEQUENCE(seq)
      .RETURN(1);

    REQUIRE_CALL(obj2, func(_, _))
      .IN_SEQUENCE(seq);

    REQUIRE_CALL(obj2, count())
      .IN_SEQUENCE(seq)
      .RETURN(3);

    std::string str = "apa";
    obj1.count();
    obj2.func(3, str);
    obj2.count();
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: correct sequence on moved seq object is not reported",
  "[C++14][sequences]")
{
  {
    mock_c obj;
    auto seq1 = trompeloeil::sequence{};
    REQUIRE_CALL(obj, count())
      .IN_SEQUENCE(seq1)
      .RETURN(1);

    auto seq2 = std::move(seq1);

    REQUIRE_CALL(obj, func(_,_))
      .IN_SEQUENCE(seq2);

    obj.count();
    std::string foo = "foo";
    obj.func(3, foo);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: incorrect sequence on moved seq object is reported",
  "[C++14][sequences]")
{
  try
  {
    mock_c obj;
    auto seq1 = trompeloeil::sequence{};
    REQUIRE_CALL(obj, count())
      .IN_SEQUENCE(seq1)
      .RETURN(1);

    auto seq2 = std::move(seq1);

    REQUIRE_CALL(obj, func(_,_))
      .IN_SEQUENCE(seq2);

    std::string foo = "foo";
    obj.func(3, foo);
    FAIL("did not report");
  }
  catch (reported)
  {
    auto re = R":(Sequence mismatch.*\"seq2\".*matching.*obj.func\(_,_\).*\n.*has obj.count\(\) at.*first):";
    auto& msg = reports.front().msg;
    INFO("msg=" << msg);
    REQUIRE(std::regex_search(msg, std::regex(re)));
  }
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: join two sequences gives no report",
  "[C++14][sequences]")
{
  {
    mock_c obj1, obj2;

    trompeloeil::sequence seq1, seq2;

    REQUIRE_CALL(obj1, count())
      .IN_SEQUENCE(seq1)
      .RETURN(1);

    REQUIRE_CALL(obj2, func(_, _))
      .IN_SEQUENCE(seq2);

    REQUIRE_CALL(obj2, count())
      .IN_SEQUENCE(seq2, seq1)
      .RETURN(3);

    std::string str = "apa";
    obj2.func(3, str);
    obj1.count();
    obj2.count();
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: violating single sequence reports first violation as fatal",
  "[C++14][sequences]")
{
  try {
    mock_c obj1, obj2;

    trompeloeil::sequence seq;

    REQUIRE_CALL(obj1, count())
      .IN_SEQUENCE(seq)
      .RETURN(1);

    REQUIRE_CALL(obj2, func(_,_))
      .IN_SEQUENCE(seq);

    REQUIRE_CALL(obj2, count())
      .IN_SEQUENCE(seq)
      .RETURN(3);

    std::string str = "apa";
    obj1.count();
    obj2.count();
    obj2.func(3, str);
    FAIL("didn't throw!");
  }
  catch (reported)
  {
    REQUIRE(!reports.empty());
    auto re = R":(Sequence mismatch.*\"seq\".*matching.*obj2.count\(\).*\n.*has obj2\.func\(_,_\) at.*first):";
    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: violating parallel sequences reports first violation as fatal",
  "[C++14][sequences]")
{
  try {
    mock_c obj1, obj2;

    trompeloeil::sequence seq1, seq2;

    REQUIRE_CALL(obj1, count())
      .IN_SEQUENCE(seq1)
      .RETURN(1);

    REQUIRE_CALL(obj2, func(_, _))
      .IN_SEQUENCE(seq2, seq1);

    REQUIRE_CALL(obj1, count())
      .IN_SEQUENCE(seq2)
      .RETURN(3);

    REQUIRE_CALL(obj2, count())
      .IN_SEQUENCE(seq2)
      .RETURN(3);


    std::string str = "apa";
    obj1.count();
    obj2.func(3, str);
    obj2.count();
    FAIL("didn't throw!");
  }
  catch (reported)
  {
    REQUIRE(!reports.empty());
    auto re = R":(Sequence mismatch.*seq2.*of obj2\.count\(\).*\n.*has obj1.count\(\).*first):";
    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: a sequence retires after min calls",
  "[C++14][sequences]")
{
  {
    int count = 0;

    mock_c obj1;
    trompeloeil::sequence seq1;

    REQUIRE_CALL(obj1, count())
      .IN_SEQUENCE(seq1)
      .TIMES(AT_LEAST(3))
      .RETURN(1);
    REQUIRE_CALL(obj1, func(_, _))
      .IN_SEQUENCE(seq1);

    count += obj1.count();
    count += obj1.count();
    count += obj1.count();
    std::string s = "apa";
    obj1.func(count, s);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: calling a sequenced match after seq retires is allowed",
  "[C++14][sequences]")
{
  {
    int count = 0;

    mock_c obj1;
    trompeloeil::sequence seq1;

    REQUIRE_CALL(obj1, count())
      .IN_SEQUENCE(seq1)
      .TIMES(AT_LEAST(3))
      .RETURN(1);
    REQUIRE_CALL(obj1, func(_, _))
      .IN_SEQUENCE(seq1);

    count += obj1.count();
    count += obj1.count();
    count += obj1.count();
    count += obj1.count();
    count += obj1.count();
    std::string s = "apa";
    obj1.func(count, s);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: breaking a sequence before retirement is illegal",
  "[C++14][sequences]")
{
  int count = 0;

  mock_c obj1;
  trompeloeil::sequence seq1;

  REQUIRE_CALL(obj1, count())
    .IN_SEQUENCE(seq1)
    .TIMES(AT_LEAST(3))
    .RETURN(1);
  REQUIRE_CALL(obj1, func(_, _))
    .IN_SEQUENCE(seq1);

  count += obj1.count();
  count += obj1.count();

  try {
    std::string s = "apa";
    obj1.func(count, s);
    FAIL("didn't throw");
  }
  catch (reported)
  {
    REQUIRE(reports.size() == 1U);
    auto re = R":(Sequence mismatch.*seq1.*of obj1\.func\(_, _\).*\n.*has obj1\.count\(\).*first):";
    INFO("report=" << reports.front().msg);
    REQUIRE(std::regex_search(reports.front().msg,  std::regex(re)));
    auto& first = reports.front();
    INFO(first.file << ':' << first.line << "\n" << first.msg);
  }
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: sequences impose order between multiple matching expectations",
  "[C++14][sequences]")
{
  mock_c obj;

  trompeloeil::sequence seq;

  REQUIRE_CALL(obj, getter(ANY(int)))
    .RETURN(1)
    .IN_SEQUENCE(seq);

  REQUIRE_CALL(obj, getter(ANY(int)))
    .RETURN(2)
    .IN_SEQUENCE(seq);

  REQUIRE_CALL(obj, getter(ANY(int)))
    .RETURN(3)
    .IN_SEQUENCE(seq);

  std::string s;
  s += std::to_string(obj.getter(1));
  s += std::to_string(obj.getter(1));
  s += std::to_string(obj.getter(1));
  REQUIRE(s == "123");
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: Sequence object destruction with live expectations is reported",
  "[C++14][sequences]")
{
  mock_c obj;

  std::unique_ptr<trompeloeil::expectation> e0;
  std::unique_ptr<trompeloeil::expectation> e1;
  {

    trompeloeil::sequence s;

    e0 = NAMED_REQUIRE_CALL(obj, getter(ANY(int)))
      .IN_SEQUENCE(s)
      .RETURN(0);
    e1 = NAMED_REQUIRE_CALL(obj, foo(_))
      .IN_SEQUENCE(s);
  }

  REQUIRE(!reports.empty());
  auto& msg = reports.front().msg;
  INFO("report=" << msg);
  auto re = R":(Sequence expectations not met at destruction of sequence object "s":
  missing obj\.getter\(ANY\(int\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  missing obj\.foo\(_\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
):";
  REQUIRE(std::regex_search(msg, std::regex(re)));
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: a sequence is completed when no expectations remain on it",
  "[C++14][sequences]")
{
  mock_c obj;
  trompeloeil::sequence seq;
  REQUIRE_CALL(obj, getter(ANY(int)))
    .IN_SEQUENCE(seq)
    .RETURN(0);
  REQUIRE_CALL(obj, foo(_))
    .IN_SEQUENCE(seq)
    .TIMES(2);
  REQUIRE_CALL(obj, getter(ANY(int)))
    .IN_SEQUENCE(seq)
    .RETURN(0);

  REQUIRE(!seq.is_completed());
  obj.getter(3);
  REQUIRE(!seq.is_completed());
  obj.foo("");
  REQUIRE(!seq.is_completed());
  obj.foo("bar");
  REQUIRE(!seq.is_completed());
  obj.getter(0);
  REQUIRE(seq.is_completed());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: Sequence mismatch on a required call after an unsatisfied allowed call reports both",
  "[C++14][sequences]")
{
  mock_c obj;
  trompeloeil::sequence seq;

  ALLOW_CALL(obj, count())
    .IN_SEQUENCE(seq)
    .RETURN(1);
  ALLOW_CALL(obj, getter(1))
    .IN_SEQUENCE(seq)
    .RETURN(1);
  REQUIRE_CALL(obj, func(_, _))
    .IN_SEQUENCE(seq);

  REQUIRE_CALL(obj, getter(3))
    .IN_SEQUENCE(seq)
    .RETURN(1);


  try {
    obj.getter(3);
    FAIL("didn't throw");
  }
  catch (reported)
  {
    REQUIRE(reports.size() == 1U);
    auto re = R":(Sequence mismatch.*seq.*of obj\.getter\(3\).*
.*has obj\.count\(\).*first in line
and has.* obj\.func\(_, _\).* as first required):";
    INFO("report=" << reports.front().msg);
    REQUIRE(std::regex_search(reports.front().msg,  std::regex(re)));
    auto& first = reports.front();
    INFO(first.file << ':' << first.line << "\n" << first.msg);
  }
}
// SIDE_EFFECT and LR_SIDE_EFFECT tests

TEST_CASE_METHOD(
  Fixture,
  "C++14: side effect access copy of local object",
  "[C++14][side effects]")
{
  {
    int n = 1;
    mock_c obj;
    REQUIRE_CALL(obj, getter(ANY(int)))
      .SIDE_EFFECT(global_n = n)
      .RETURN(_1);
    n = 2;
    obj.getter(n);
  }
  REQUIRE(global_n == 1);
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: lr side effect access reference of local object",
  "[C++14][side effects]")
{
  {
    int n = 1;
    mock_c obj;
    REQUIRE_CALL(obj, getter(ANY(int)))
      .LR_SIDE_EFFECT(global_n = n)
      .RETURN(_1);
    n = 2;
    obj.getter(n);
  }
  REQUIRE(reports.empty());
  REQUIRE(global_n == 2);
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: multiple side effects are executed in the order declared",
  "[C++14][side effects]")
{
  std::string s;
  {
    mock_c obj;
    REQUIRE_CALL(obj, getter(ANY(int)))
      .LR_SIDE_EFFECT(s = std::to_string(_1))
      .LR_SIDE_EFFECT(s += "_")
      .LR_SIDE_EFFECT(s += s)
      .RETURN(_1);

    obj.getter(3);
  }

  REQUIRE(reports.empty());
  REQUIRE(s == "3_3_");
}

// RETURN and LR_RETURN tests

TEST_CASE_METHOD(
  Fixture,
  "C++14: RETURN access copy of local object",
  "[C++14][return values]")
{
  {
    int n = 1;
    mock_c obj;
    REQUIRE_CALL(obj, getter(ANY(int)))
      .RETURN(n);
    n = 2;
    auto m = obj.getter(n);
    REQUIRE(m == 1);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: LR_RETURN access the actual local object",
  "[C++14][return values]")
{
  {
    int n = 1;
    mock_c obj;
    REQUIRE_CALL(obj, getter(ANY(int)))
      .LR_RETURN(n);
    n = 2;
    auto m = obj.getter(n);
    REQUIRE(m == 2);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: RETURN a ref to local obj, std::ref(obj) returns object given",
  "[C++14][return values]")
{
  {
    mock_c obj;
    unmovable s;
    REQUIRE_CALL(obj, getter(ANY(unmovable&)))
      .LR_RETURN(std::ref(s));

    REQUIRE(&obj.getter(s) == &s);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: RETURN a ref to local obj, (obj) returns object given",
  "[C++14][return values]")
{
  {
    mock_c obj;
    unmovable s;
    REQUIRE_CALL(obj, getter(ANY(unmovable&)))
      .LR_RETURN((s));

    REQUIRE(&obj.getter(s) == &s);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: RETURN const char* from const char*",
  "[C++14][return]")
{
  mstr m;
  const char* s = "foo";
  REQUIRE_CALL(m, cc_str())
    .RETURN(s);
  REQUIRE(m.cc_str() == "foo"s);
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: RETURN const char* from string literal",
  "[C++14][return]")
{
  mstr m;
  REQUIRE_CALL(m, cc_str())
    .RETURN("foo");
  REQUIRE(m.cc_str() == "foo"s);
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: RETURN const char* from static char array",
  "[C++14][return]")
{
  mstr m;
  REQUIRE_CALL(m, cc_str())
    .RETURN(carr);
  REQUIRE(m.cc_str() == "foo"s);
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: RETURN const char* from static const char array",
  "[C++14][return]")
{
  mstr m;
  REQUIRE_CALL(m, cc_str())
    .RETURN(ccarr);
  REQUIRE(m.cc_str() == "bar"s);
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: RETURN char* from char*",
  "[C++14][return]")
{
  mstr m;
  char* s = carr;
  REQUIRE_CALL(m, cc_str())
    .RETURN(s);
  REQUIRE(m.cc_str() == "foo"s);
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: RETURN char* from char array",
  "[C++14][return]")
{
  mstr m;
  REQUIRE_CALL(m, cc_str())
    .RETURN(carr);
  REQUIRE(m.cc_str() == "foo"s);
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: RETURN string from string literal",
  "[C++14][return]")
{
  mstr m;
  REQUIRE_CALL(m, str())
    .RETURN("foo");
  REQUIRE(m.str() == "foo");
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: RETURN string from static char array",
  "[C++14][return]")
{
  mstr m;
  REQUIRE_CALL(m, str())
  .RETURN(carr);
  REQUIRE(m.str() == "foo");
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: RETURN string from static const char array",
  "[C++14][return]")
{
  mstr m;
  REQUIRE_CALL(m, str())
  .RETURN(ccarr);
  REQUIRE(m.str() == "bar"s);
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: RETURN const string from string literal",
  "[C++14][return]")
{
  mstr m;
  REQUIRE_CALL(m, cstr())
  .RETURN("foo");
  REQUIRE(m.cstr() == "foo");
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: RETURN const string from static char array",
  "[C++14][return]")
{
  mstr m;
  REQUIRE_CALL(m, cstr())
  .RETURN(carr);
  REQUIRE(m.cstr() == "foo");
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: RETURN const string from static const char array",
  "[C++14][return]")
{
  mstr m;
  REQUIRE_CALL(m, cstr())
  .RETURN(ccarr);
  REQUIRE(m.cstr() == "bar"s);
}

// THROW and LR_THROW tests

TEST_CASE_METHOD(
  Fixture,
  "C++14: RETURN ref param returns object given",
  "[C++14][return values]")
{
  {
    mock_c obj;
    unmovable s;
    REQUIRE_CALL(obj, getter(ANY(unmovable&)))
      .RETURN(_1);

    REQUIRE(&obj.getter(s) == &s);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: THROW access copy of local object",
  "[C++14][return values]")
{
  int n = 1;
  mock_c obj;
  REQUIRE_CALL(obj, getter(ANY(int)))
    .THROW(n);
  n = 2;
  try {
    obj.getter(n);
    FAIL("didn't throw");
  }
  catch (int m)
  {
    REQUIRE(m == 1);
  }
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: LR_THROW access actual local object",
  "[C++14][return values]")
{
  int n = 1;
  mock_c obj;
  REQUIRE_CALL(obj, getter(ANY(int)))
    .LR_THROW(n);
  n = 2;
  try {
    obj.getter(n);
    FAIL("didn't throw");
  }
  catch (int m)
  {
    REQUIRE(m == 2);
  }
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: THROW throws after side effect when replacing return for non void functions",
  "[C++14][return values]")
{
  int thrown = 0;
  int global = 0;
  try {
    mock_c obj;
    REQUIRE_CALL(obj, getter(ANY(int)))
      .THROW(8)
      .LR_SIDE_EFFECT(global = _1);
    obj.getter(8);
    FAIL("didn't throw");
  }
  catch (int n)
  {
    thrown = n;
  }
  REQUIRE(thrown == 8);
  REQUIRE(global == 8);
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: THROW throws after side effect in void functions",
  "[C++14][return values]")
{
  int thrown = 0;
  std::string s;
  try {
    mock_c obj;

    REQUIRE_CALL(obj, func(_, _))
      .THROW(8)
      .SIDE_EFFECT(_2 = std::to_string(_1));

    obj.func(8, s);
    FAIL("didn't throw");
  }
  catch (int n)
  {
    thrown = n;
  }
  REQUIRE(thrown == 8);
  REQUIRE(s == "8");
}

// WITH and LR_WITH tests

TEST_CASE_METHOD(
  Fixture,
  "C++14: WITH matches copy of local object",
  "[C++14][matching]")
{
  {
    mock_c obj;
    int n = 1;
    REQUIRE_CALL(obj, getter(ANY(int)))
      .WITH(_1 == n)
      .RETURN(_1);
    n = 2;
    obj.getter(1);

    // Read n.
    CHECK(n == 2);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: LR_WITH access actual local object",
  "[C++14][matching]")
{
  {
    mock_c obj;
    int n = 1;
    REQUIRE_CALL(obj, getter(ANY(int)))
      .LR_WITH(_1 == n)
      .RETURN(_1);
    n = 2;
    obj.getter(2);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: rvalue reference parameter can be compared with nullptr in WITH",
  "[C++14][matching]")
{
  {
    mock_c obj;
    REQUIRE_CALL(obj, ptr(_))
      .WITH(_1 != nullptr)
      .RETURN(std::move(_1));

    auto p = obj.ptr(std::unique_ptr<int>(new int{3}));
    REQUIRE(p);
    REQUIRE(*p == 3);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: rvalue reference parameter can be compared with external value on WITH",
  "[C++14][matching]")
{
  {
    mock_c obj;
    auto pi = new int{3};
    REQUIRE_CALL(obj, ptr(_))
      .WITH(_1.get() == pi)
      .RETURN(std::move(_1));

    auto p = obj.ptr(std::unique_ptr<int>(pi));
    REQUIRE(p);
    REQUIRE(p.get() == pi);
  }
  REQUIRE(reports.empty());
}


TEST_CASE_METHOD(
  Fixture,
  "C++14: mocks can be inherited",
  "[C++14][matching]")
{
  combined obj;
  REQUIRE_CALL(obj, getter(3))
    .RETURN(2);
  auto n = obj.getter(3);
  REQUIRE(n == 2);
}

// tests of direct parameter matching with fixed values and wildcards


TEST_CASE_METHOD(
  Fixture,
  "C++14: An uncomparable but constructible type by reference matches a call",
  "[C++14][matching]")
{
  {
    U u;
    REQUIRE_CALL(u, func_ustr("str"));
    u.func_ustr("str");
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: An uncomparable but constructible type by value matches a call",
  "[C++14][matching]")
{
  {
    U u;
    REQUIRE_CALL(u, func_ustrv("str"));
    u.func_ustrv("str");
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: An uncomparable but constructible type by reference mismatch is reported",
  "[C++14][matching]")
{
  try
  {
    U u;
    REQUIRE_CALL(u, func_ustr("str"));
    u.func_ustr("strr");
    FAIL("didn't report");
  }
  catch (reported)
  {
    REQUIRE(reports.size() == 1U);
    auto& msg = reports.front().msg;
    auto re = R":(No match for call of func_ustr with signature void\(const uncomparable_string&\) with\.
  param  _1 == strr

Tried u\.func_ustr\("str"\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*):";
    INFO("msg=" << msg);
    REQUIRE(std::regex_search(msg, std::regex(re)));
  }
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: An uncomparable but constructible type by value mismatch is reported",
  "[C++14][matching]")
{
  try
  {
    U u;
    REQUIRE_CALL(u, func_ustrv("str"));
    u.func_ustrv("strr");
    FAIL("didn't report");
  }
  catch (reported)
  {
    REQUIRE(reports.size() == 1U);
    auto& msg = reports.front().msg;
    auto re = R":(No match for call of func_ustrv with signature void\(uncomparable_string\) with\.
  param  _1 == strr

Tried u\.func_ustrv\("str"\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*):";
    INFO("msg=" << msg);
    REQUIRE(std::regex_search(msg, std::regex(re)));
  }
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: pointer to function matches wildcard",
  "[C++14][matching]")
{
  {
    U u;
    REQUIRE_CALL(u, func_ptr_f(_));
    u.func_ptr_f(intfunc);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: pointer to member function matches wildcard",
  "[C++14][matching]")
{
  {
    U u;
    REQUIRE_CALL(u, func_mptr_f(_));
    u.func_mptr_f(&U::func_cstr);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: pointer to member data matches wildcard",
  "[C++14][matching]")
{
  {
    U u;
    REQUIRE_CALL(u, func_mptr_d(_));
    u.func_mptr_d(&U::m);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: ostream& matches wildcard",
  "[C++14][matching]")
{
  {
    U u;
    REQUIRE_CALL(u, func_streamref(_));
    u.func_streamref(std::cout);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: uncomparable parameter matches wildcard",
  "[C++14][matching]")
{
  {
    U u;
    REQUIRE_CALL(u, func_u(_));
    u.func_u(uncomparable{});
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: uncomparable parameter matches typed wildcard",
  "[C++14][matching]")
{
  {
    U u;
    REQUIRE_CALL(u, func_u(ANY(uncomparable)));
    u.func_u(uncomparable{});
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: wildcard matches parameter value type",
  "[C++14][matching]")
{
  {
    U u;
    REQUIRE_CALL(u, func_v(_));
    u.func_v(1);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: wildcard matches parameter const value type",
  "[C++14][matching]")
{
  {
    U u;
    REQUIRE_CALL(u, func_cv(_));
    u.func_cv(1);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: wildcard matches unique_ptr<> value type",
  "[C++14][matching]")
{
  {
    U u;
    REQUIRE_CALL(u, func_uniqv(_));
    u.func_uniqv(detail::make_unique<int>(3));
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: wildcard matches shared_ptr<> value type",
  "[C++14][matching]")
{
  {
    U u;
    REQUIRE_CALL(u, func_sharedv(_));
    u.func_sharedv(std::make_shared<int>(3));
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: wildcard matches parameter lvalue reference type",
  "[C++14][matching]")
{
  {
    U u;
    REQUIRE_CALL(u, func_lr(_));
    int v = 1;
    u.func_lr(v);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: wildcard matches parameter const lvalue reference type",
  "[C++14][matching]")
{
  {
    U u;
    REQUIRE_CALL(u, func_clr(_));
    int v = 1;
    u.func_clr(v);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: wildcard matches parameter rvalue reference type",
  "[C++14][matching]")
{
  {
    U u;
    REQUIRE_CALL(u, func_rr(_));
    u.func_rr(1);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: wildcard matches parameter const rvalue reference type",
  "[C++14][matching]")
{
  {
    U u;
    REQUIRE_CALL(u, func_crr(_));
    u.func_crr(1);
  }
  REQUIRE(reports.empty());
}

struct promiscuous
{
  template <typename T>
  promiscuous(T&&) {}
};

struct with_promiscuous
{
  MAKE_MOCK1(func, void(promiscuous));
};

TEST_CASE_METHOD(
  Fixture,
  "C++14: wildcard matches parameter constructible from any type",
  "[C++14][matching]")
{
  {
    with_promiscuous obj;
    REQUIRE_CALL(obj, func(_));
    obj.func(1);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: ANY can match unique_ptr<> by value",
  "[C++14][matching]")
{
  {
    U u;
    REQUIRE_CALL(u, func_uniqv(ANY(std::unique_ptr<int>)));
    u.func_uniqv(detail::make_unique<int>(3));
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: ANY can match shared_ptr<> by value",
  "[C++14][matching]")
{
  {
    U u;
    REQUIRE_CALL(u, func_sharedv(ANY(std::shared_ptr<int>)));
    u.func_sharedv(std::make_shared<int>(3));
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: ANY can select overload on lvalue reference type",
  "[C++14][matching]")
{
  {
    U u;
    REQUIRE_CALL(u, func(ANY(int&)));
    int i = 1;
    u.func(i);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: ANY can select overload on const lvalue reference type",
  "[C++14][matching]")
{
  {
    U u;
    REQUIRE_CALL(u, func(ANY(const int&)));
    const int i = 1;
    u.func(i);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: ANY can select overload on rvalue reference type",
  "[C++14][matching]")
{
  {
    U u;
    REQUIRE_CALL(u, func(ANY(int&&)));
    u.func(1);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: Expectation matches a mocked function with param from template",
  "[C++14][matching][templates]")
{
  {
    tmock<int> m;
    REQUIRE_CALL(m, tfunc(_));
    m.tfunc(3);
  }
  REQUIRE(reports.empty());
}

// tests of overload selection with parameter matching

TEST_CASE_METHOD(
  Fixture,
  "C++14: wildcards matches overload on type and parameter count",
  "[C++14][matching][overloads]")
{
  {
    mock_c obj;
    REQUIRE_CALL(obj, getter(ANY(unmovable&)))
      .RETURN(_1);
    FORBID_CALL(obj, getter(ANY(int)));
    REQUIRE_CALL(obj, getter(_, _))
      .SIDE_EFFECT(_2 = std::to_string(_1));

    unmovable u;
    auto& ur = obj.getter(u);
    REQUIRE(&ur == &u);

    std::string s;
    obj.getter(3, s);

    REQUIRE(s == "3");
  }
  REQUIRE(reports.empty());
}

// tests of parameter matching using duck typed matcher eq

TEST_CASE_METHOD(
  Fixture,
  "C++14: long value matches equal int for duck typed eq",
  "[C++14][matching][matchers][eq]")
{
  {
    U obj;
    REQUIRE_CALL(obj, func_v(trompeloeil::eq(3L)));
    obj.func_v(3);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: long value with mismatches int is reported for duck typed eq",
  "[C++14][matching][matchers][eq]")
{
  try {
    U obj;
    REQUIRE_CALL(obj, func_v(trompeloeil::eq(3L)));
    obj.func_v(0);
    FAIL("didn't report");
  }
  catch (reported)
  {
    REQUIRE(reports.size() == 1U);
    auto& msg = reports.front().msg;
    INFO("report=" << msg);
    auto re = R":(No match for call of func_v with signature void\(int\) with\.
  param  _1 == 0

Tried obj\.func_v\(trompeloeil::eq\(3L\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 == 3):";
    REQUIRE(std::regex_search(msg, std::regex(re)));
  }
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: std::string value matches equal const char* for duck typed eq",
  "[C++14][matching][matchers][eq]")
{
  {
    U obj;
    REQUIRE_CALL(obj, func_cstr(trompeloeil::eq("foo"s)));
    obj.func_cstr("foo");
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: std::string value mismatching const char* is reported for duck typed eq",
  "[C++14][matching][matchers][eq]")
{
  try {
    U obj;
    REQUIRE_CALL(obj, func_cstr(trompeloeil::eq("foo"s)));
    obj.func_cstr("bar");
    FAIL("didn't report");
  }
  catch (reported)
  {
    REQUIRE(reports.size() == 1U);
    auto& msg = reports.front().msg;
    INFO("report=" << msg);
    auto re = R":(No match for call of func_cstr with signature void\(const char\*\) with\.
  param  _1 == bar

Tried obj\.func_cstr\(trompeloeil::eq\(\"foo\"s\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 == foo):";
    REQUIRE(std::regex_search(msg, std::regex(re)));
  }
}

// tests of parameter matching using explicitly typed matcher eq

TEST_CASE_METHOD(
  Fixture,
  "C++14: disambiguated eq<int&> matches equal value",
  "[C++14][matching][matchers][eq]")
{
  {
    U obj;
    REQUIRE_CALL(obj, func(trompeloeil::eq<int&>(3)));
    int i = 3;
    obj.func(i);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: disambiguated eq<int&> reports mismatching value",
  "[C++14][matching][matchers][eq]")
{
  try {
    U obj;
    REQUIRE_CALL(obj, func(trompeloeil::eq<int&>(3)));
    int i = 0;
    obj.func(i);
    FAIL("didn't report");
  }
  catch (reported)
  {
    REQUIRE(reports.size() == 1U);
    auto& msg = reports.front().msg;
    INFO("report=" << msg);
    auto re = R":(No match for call of func with signature void\(int&\) with\.
  param  _1 == 0

Tried obj\.func\(trompeloeil::eq<int&>\(3\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 == 3):";
    REQUIRE(std::regex_search(msg, std::regex(re)));
  }
}

// tests of parameter matching using duck typed matcher ne

TEST_CASE_METHOD(
  Fixture,
  "C++14: nullptr mismatching equal function for duck typed ne",
  "[C++14][matching][matchers][ne]")
{
  {
    U obj;
    REQUIRE_CALL(obj, func_f(trompeloeil::ne(nullptr)));
    obj.func_f([](){});
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: long value mismatching equal int for duck typed ne",
  "[C++14][matching][matchers][ne]")
{
  {
    U obj;
    REQUIRE_CALL(obj, func_v(trompeloeil::ne(3L)));
    obj.func_v(0);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: long value with matching int is reported for duck typed ne",
  "[C++14][matching][matchers][ne]")
{
  try {
    U obj;
    REQUIRE_CALL(obj, func_v(trompeloeil::ne(3L)));
    obj.func_v(3);
    FAIL("didn't report");
  }
  catch (reported)
  {
    REQUIRE(reports.size() == 1U);
    auto& msg = reports.front().msg;
    INFO("report=" << msg);
    auto re = R":(No match for call of func_v with signature void\(int\) with\.
  param  _1 == 3

Tried obj\.func_v\(trompeloeil::ne\(3L\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 != 3):";
    REQUIRE(std::regex_search(msg, std::regex(re)));
  }
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: std::string value mismatches inequal const char* for duck typed ne",
  "[C++14][matching][matchers][ne]")
{
  {
    U obj;
    REQUIRE_CALL(obj, func_cstr(trompeloeil::ne("foo"s)));
    obj.func_cstr("bar");
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: std::string value matching const char* is reported for duck typed ne",
  "[C++14][matching][matchers][ne]")
{
  try {
    U obj;
    REQUIRE_CALL(obj, func_cstr(trompeloeil::ne("foo"s)));
    obj.func_cstr("foo");
    FAIL("didn't report");
  }
  catch (reported)
  {
    REQUIRE(reports.size() == 1U);
    auto& msg = reports.front().msg;
    INFO("report=" << msg);
    auto re = R":(No match for call of func_cstr with signature void\(const char\*\) with\.
  param  _1 == foo

Tried obj\.func_cstr\(trompeloeil::ne\(\"foo\"s\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 != foo):";
    REQUIRE(std::regex_search(msg, std::regex(re)));
  }
}

// tests of parameter matching using typed matcher ne

TEST_CASE_METHOD(
  Fixture,
  "C++14: a non equal value matches ne",
  "[C++14][matching][matchers][ne]")
{
  {
    mock_c obj;
    REQUIRE_CALL(obj, foo(trompeloeil::ne<std::string>("bar")));
    obj.foo("baz");
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: an equal value fails ne with report",
  "[C++14][matching][matchers][ne]")
{
  try {
    mock_c obj;
    REQUIRE_CALL(obj, foo(trompeloeil::ne<std::string>("bar")));
    obj.foo("bar");
    FAIL("didn't report");
  }
  catch (reported)
  {
    REQUIRE(reports.size() == 1U);
    INFO("report=" << reports.front().msg);
    auto re = R":(No match for call of foo with signature void\(std::string\) with\.
  param  _1 == bar

Tried obj\.foo\(trompeloeil::ne<std::string>\("bar"\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 != bar):";
    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: non-nullptr call matches ne(nullptr)",
  "[C++14][matching][matchers][ne]")
{
  {
    C_foo1 obj;
    REQUIRE_CALL(obj, foo(trompeloeil::ne(nullptr)));
    int n;
    obj.foo(&n);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: nullptr call with ne(nullptr) is reported",
  "[C++14][matching][matchers][ne]")
{
  try {
    C_foo1 obj;
    REQUIRE_CALL(obj, foo(trompeloeil::ne(nullptr)));
    obj.foo(nullptr);
    FAIL("didn't report");
  }
  catch (reported)
  {
    REQUIRE(reports.size() == 1U);
    INFO("report=" << reports.front().msg);
    auto re = R":(No match for call of foo with signature void\(int\*\) with\.
  param  _1 == .*

Tried obj\.foo\(trompeloeil::ne\(nullptr\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 != .*):";

    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: overloaded non-nullptr call disambiguated with ne<type>(nullptr) is matched",
  "[C++14][matching][matchers][ne]")
{
  {
    C_foo2 obj;
    REQUIRE_CALL(obj, foo(trompeloeil::ne<int*>(nullptr)));
    int i;
    obj.foo(&i);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: overloaded nullptr call disambiguated with ne<type>(nullptr) is reported",
  "[C++14][matching][matchers][ne]")
{
  try {
    C_foo2 obj;
    REQUIRE_CALL(obj, foo(trompeloeil::ne<int*>(nullptr)));
    int* i_null = nullptr;
    obj.foo(i_null);
    FAIL("didn't report");
  }
  catch (reported)
  {
    REQUIRE(reports.size() == 1U);
    INFO("report=" << reports.front().msg);
    auto re = R":(No match for call of foo with signature void\(int\*\) with\.
  param  _1 == nullptr

Tried obj\.foo\(trompeloeil::ne<int\*>\(nullptr\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 != .*):";

    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}

//

TEST_CASE_METHOD(
  Fixture,
  "C++14: overloaded nullptr call disambiguated with eq<type>(nullptr) is matched",
  "[C++14][matching][matchers][ne]")
{
  {
    C_foo2 obj;
    REQUIRE_CALL(obj, foo(trompeloeil::eq<int*>(nullptr)));
    int *i_null = nullptr;
    obj.foo(i_null);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: overloaded non-nullptr call disambiguated with eq<type>(nullptr) is reported",
  "[C++14][matching][matchers][ne]")
{
  try {
    C_foo2 obj;
    REQUIRE_CALL(obj, foo(trompeloeil::eq<int*>(nullptr)));
    int i;
    obj.foo(&i);
    FAIL("didn't report");
  }
  catch (reported)
  {
    REQUIRE(reports.size() == 1U);
    INFO("report=" << reports.front().msg);
    auto re = R":(No match for call of foo with signature void\(int\*\) with\.
  param  _1 == .*

Tried obj\.foo\(trompeloeil::eq<int\*>\(nullptr\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 == nullptr):";

    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: pointer to member call with ne(nullptr) matched",
  "[C++14][matching][matchers][ne]")
{
  {
    C_foo3 obj;
    REQUIRE_CALL(obj, foo(trompeloeil::ne(nullptr)));
    obj.foo(&C_foo3::m);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: pointer to member ptr call with ne(nullptr) is reported",
  "[C++14][matching][matchers][ne]")
{
  try {
    C_foo3 obj;
    REQUIRE_CALL(obj, foo(trompeloeil::ne(nullptr)));
    obj.foo(nullptr);
    FAIL("didn't report");
  }
  catch (reported)
  {
    REQUIRE(reports.size() == 1U);
    INFO("report=" << reports.front().msg);
    auto re = R":(No match for call of foo with signature void\(int C_foo3::\*\) with\.
  param  _1 == .*

Tried obj\.foo\(trompeloeil::ne\(nullptr\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 != .*):";

    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}

//

TEST_CASE_METHOD(
  Fixture,
  "C++14: pointer to member ptr call with eq(nullptr) matched",
  "[C++14][matching][matchers][ne]")
{
  {
    C_foo3 obj;
    REQUIRE_CALL(obj, foo(trompeloeil::eq(nullptr)));
    obj.foo(nullptr);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: pointer to member ptr call with eq(nullptr) is reported",
  "[C++14][matching][matchers][ne]")
{
  try {
    C_foo3 obj;
    REQUIRE_CALL(obj, foo(trompeloeil::eq(nullptr)));
    obj.foo(&C_foo3::m);
    FAIL("didn't report");
  }
  catch (reported)
  {
    REQUIRE(reports.size() == 1U);
    INFO("report=" << reports.front().msg);
    auto re = R":(No match for call of foo with signature void\(int C_foo3::\*\) with\.
  param  _1 == .*

Tried obj\.foo\(trompeloeil::eq\(nullptr\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 == nullptr):";

    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: pointer to function call with ne(nullptr) matched",
  "[C++14][matching][matchers][ne]")
{
  {
    C_foo3 obj;
    REQUIRE_CALL(obj, bar(trompeloeil::ne(nullptr)));
    obj.bar(intfunc);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: pointer to function call with ne(nullptr) is reported",
  "[C++14][matching][matchers][ne]")
{
  try {
    C_foo3 obj;
    REQUIRE_CALL(obj, bar(trompeloeil::ne(nullptr)));
    obj.bar(nullptr);
    FAIL("didn't report");
  }
  catch (reported)
  {
    REQUIRE(reports.size() == 1U);
    INFO("report=" << reports.front().msg);
    auto re = R":(No match for call of bar with signature void\(int \(\*\)\(int\)\) with\.
  param  _1 == .*

Tried obj\.bar\(trompeloeil::ne\(nullptr\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 != .*):";

    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}

//

TEST_CASE_METHOD(
  Fixture,
  "C++14: pointer to function call with eq(nullptr) matched",
  "[C++14][matching][matchers][ne]")
{
  {
    C_foo3 obj;
    REQUIRE_CALL(obj, bar(trompeloeil::eq(nullptr)));
    obj.bar(nullptr);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: pointer to function call with eq(nullptr) is reported",
  "[C++14][matching][matchers][ne]")
{
  try {
    C_foo3 obj;
    REQUIRE_CALL(obj, bar(trompeloeil::eq(nullptr)));
    obj.bar(intfunc);
    FAIL("didn't report");
  }
  catch (reported)
  {
    REQUIRE(reports.size() == 1U);
    INFO("report=" << reports.front().msg);
    auto re = R":(No match for call of bar with signature void\(int \(\*\)\(int\)\) with\.
  param  _1 == .*

Tried obj\.bar\(trompeloeil::eq\(nullptr\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 == nullptr):";

    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}

//
// tests of parameter matching using duck typed matcher ge

TEST_CASE_METHOD(
  Fixture,
  "C++14: an equal value matches ge",
  "[C++14][matching][matchers][ge]")
{
  {
    mock_c obj;
    REQUIRE_CALL(obj, getter(trompeloeil::ge(3)))
      .RETURN(0);
    obj.getter(3);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: a greater value matches ge",
  "[C++14][matching][matchers][ge]")
{
  {
    mock_c obj;
    REQUIRE_CALL(obj, getter(trompeloeil::ge(3)))
      .RETURN(0);
    obj.getter(4);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: a lesser value is reported by ge",
  "[C++14][matching][matchers][ge]")
{
  try {
    mock_c obj;
    REQUIRE_CALL(obj, getter(trompeloeil::ge(3)))
      .RETURN(0);
    obj.getter(2);
    FAIL("didn't report");
  }
  catch (reported)
  {
    REQUIRE(reports.size() == 1U);
    auto re = R":(No match for call of getter with signature int\(int\) with.
  param  _1 == 2

Tried obj\.getter\(trompeloeil::ge\(3\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 >= 3):";
    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}

// tests of parameter matching using typed matcher ge

TEST_CASE_METHOD(
  Fixture,
  "C++14: an equal value matches disambiguated ge<int&>",
  "[C++14][matching][matchers][ge]")
{
  {
    U obj;
    REQUIRE_CALL(obj, func(trompeloeil::ge<int&>(3)));
    int i = 3;
    obj.func(i);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: a greater value matches disambiguated ge<int&>",
  "[C++14][matching][matchers][ge]")
{
  {
    U obj;
    REQUIRE_CALL(obj, func(trompeloeil::ge<int&>(3)));
    int i = 4;
    obj.func(i);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: a lesser value is reported by disambiguated ge<int&>",
  "[C++14][matching][matchers][ge]")
{
  try {
    U obj;
    REQUIRE_CALL(obj, func(trompeloeil::ge<int&>(3)));
    int i = 2;
    obj.func(i);
    FAIL("didn't report");
  }
  catch (reported)
  {
    REQUIRE(reports.size() == 1U);
    auto re = R":(No match for call of func with signature void\(int&\) with.
  param  _1 == 2

Tried obj\.func\(trompeloeil::ge<int&>\(3\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 >= 3):";
    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}

// tests of parameter matching using duck typed matcher gt

TEST_CASE_METHOD(
  Fixture,
  "C++14: an equal value is reported by gt",
  "[C++14][matching][matchers][gt]")
{
  try {
    mock_c obj;
    REQUIRE_CALL(obj, getter(trompeloeil::gt(3)))
      .RETURN(0);
    obj.getter(3);
    FAIL("didn't report");
  }
  catch (reported)
  {
    REQUIRE(reports.size() == 1U);
    auto re = R":(No match for call of getter with signature int\(int\) with\.
  param  _1 == 3

Tried obj\.getter\(trompeloeil::gt\(3\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 > 3):";
    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: a greater value matches gt",
  "[C++14][matching][matchers][gt]")
{
  {
    mock_c obj;
    REQUIRE_CALL(obj, getter(trompeloeil::gt(3)))
      .RETURN(0);
    obj.getter(4);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: a lesser value is reported by gt",
  "[C++14][matching][matchers][gt]")
{
  try {
    mock_c obj;
    REQUIRE_CALL(obj, getter(trompeloeil::gt(3)))
      .RETURN(0);
    obj.getter(2);
    FAIL("didn't report");
  }
  catch (reported)
  {
    REQUIRE(reports.size() == 1U);
    auto re = R":(No match for call of getter with signature int\(int\) with\.
  param  _1 == 2

Tried obj.getter\(trompeloeil::gt\(3\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 > 3):";
    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}

// tests of parameter matching using typed matcher gt

TEST_CASE_METHOD(
  Fixture,
  "C++14: an equal value is reported by disambiguated gt<int&>",
  "[C++14][matching][matchers][gt]")
{
  try {
    U obj;
    REQUIRE_CALL(obj, func(trompeloeil::gt<int&>(3)));
    int i = 3;
    obj.func(i);
    FAIL("didn't report");
  }
  catch (reported)
  {
    REQUIRE(reports.size() == 1U);
    auto re = R":(No match for call of func with signature void\(int&\) with\.
  param  _1 == 3

Tried obj\.func\(trompeloeil::gt<int&>\(3\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 > 3):";
    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: a greater value matches disambiguated gt<int&>",
  "[C++14][matching][matchers][gt]")
{
  {
    U obj;
    REQUIRE_CALL(obj, func(trompeloeil::gt<int&>(3)));
    int i = 4;
    obj.func(i);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: a lesser value is reported by disambiguated gt<int&>",
  "[C++14][matching][matchers][gt]")
{
  try {
    U obj;
    REQUIRE_CALL(obj, func(trompeloeil::gt<int&>(3)));
    int i = 2;
    obj.func(i);
    FAIL("didn't report");
  }
  catch (reported)
  {
    REQUIRE(reports.size() == 1U);
    auto re = R":(No match for call of func with signature void\(int&\) with\.
  param  _1 == 2

Tried obj.func\(trompeloeil::gt<int&>\(3\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 > 3):";
    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}

// tests of parameter matching using duck typed matcher lt

TEST_CASE_METHOD(
  Fixture,
  "C++14: an equal value is reported by lt",
  "[C++14][matching][matchers][lt]")
{
  try {
    mock_c obj;
    REQUIRE_CALL(obj, getter(trompeloeil::lt(3)))
      .RETURN(0);
    obj.getter(3);
    FAIL("didn't report");
  }
  catch (reported)
  {
    REQUIRE(reports.size() == 1U);
    auto re = R":(No match for call of getter with signature int\(int\) with\.
  param  _1 == 3

Tried obj\.getter\(trompeloeil::lt\(3\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 < 3):";
    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: a greater value is reported by lt",
  "[C++14][matching][matchers][lt]")
{
  try {
    mock_c obj;
    REQUIRE_CALL(obj, getter(trompeloeil::lt(3)))
      .RETURN(0);
    obj.getter(4);
    FAIL("didn't report");
  }
  catch (reported)
  {
    REQUIRE(reports.size() == 1U);
    auto re = R":(No match for call of getter with signature int\(int\) with\.
  param  _1 == 4

Tried obj\.getter\(trompeloeil::lt\(3\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 < 3):";
    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: a lesser value matches lt",
  "[C++14][matching][matchers][lt]")
{
  {
    mock_c obj;
    REQUIRE_CALL(obj, getter(trompeloeil::lt(3)))
      .RETURN(0);
    obj.getter(2);
  }
  REQUIRE(reports.empty());
}

// tests of parameter matching using typed matcher lt

TEST_CASE_METHOD(
  Fixture,
  "C++14: an equal value is reported by disambiguated lt<int&>",
  "[C++14][matching][matchers][lt]")
{
  try {
    U obj;
    REQUIRE_CALL(obj, func(trompeloeil::lt<int&>(3)));
    int i = 3;
    obj.func(i);
    FAIL("didn't report");
  }
  catch (reported)
  {
    REQUIRE(reports.size() == 1U);
    auto re = R":(No match for call of func with signature void\(int&\) with\.
  param  _1 == 3

Tried obj\.func\(trompeloeil::lt<int&>\(3\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 < 3):";
    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: a greater value is reported by disambiguated lt<int&>",
  "[C++14][matching][matchers][lt]")
{
  try {
    U obj;
    REQUIRE_CALL(obj, func(trompeloeil::lt<int&>(3)));
    int i = 4;
    obj.func(i);
    FAIL("didn't report");
  }
  catch (reported)
  {
    REQUIRE(reports.size() == 1U);
    auto re = R":(No match for call of func with signature void\(int&\) with\.
  param  _1 == 4

Tried obj\.func\(trompeloeil::lt<int&>\(3\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 < 3):";
    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: a lesser value matches disambiguated lt<int&>",
  "[C++14][matching][matchers][lt]")
{
  {
    U obj;
    REQUIRE_CALL(obj, func(trompeloeil::lt<int&>(3)));
    int i = 2;
    obj.func(i);
  }
  REQUIRE(reports.empty());
}

// tests of parameter matching using duck typed matcher le

TEST_CASE_METHOD(
  Fixture,
  "C++14: an equal value matches le",
  "[C++14][matching][matchers][le]")
{
  {
    mock_c obj;
    REQUIRE_CALL(obj, getter(trompeloeil::le(3)))
      .RETURN(0);
    obj.getter(3);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: a greater value is reported by le",
  "[C++14][matching][matchers][le]")
{
  try {
    mock_c obj;
    REQUIRE_CALL(obj, getter(trompeloeil::le(3)))
      .RETURN(0);
    obj.getter(4);
    FAIL("didn't report");
  }
  catch (reported)
  {
    REQUIRE(reports.size() == 1U);
    auto re = R":(No match for call of getter with signature int\(int\) with\.
  param  _1 == 4

Tried obj\.getter\(trompeloeil::le\(3\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 <= 3):";
    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: a lesser value matches le",
  "[C++14][matching][matchers][le]")
{
  {
    mock_c obj;
    REQUIRE_CALL(obj, getter(trompeloeil::le(3)))
      .RETURN(0);
    obj.getter(2);
  }
  REQUIRE(reports.empty());
}

// tests of parameter matching using typed matcher le

TEST_CASE_METHOD(
  Fixture,
  "C++14: an equal value matches disambiguated le<int&>",
  "[C++14][matching][matchers][le]")
{
  {
    U obj;
    REQUIRE_CALL(obj, func(trompeloeil::le<int&>(3)));
    int i = 3;
    obj.func(i);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: a greater value is reported by disambiguated le<int&>",
  "[C++14][matching][matchers][le]")
{
  try {
    U obj;
    REQUIRE_CALL(obj, func(trompeloeil::le<int&>(3)));
    int i = 4;
    obj.func(i);
    FAIL("didn't report");
  }
  catch (reported)
  {
    REQUIRE(reports.size() == 1U);
    auto re = R":(No match for call of func with signature void\(int&\) with\.
  param  _1 == 4

Tried obj\.func\(trompeloeil::le<int&>\(3\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 <= 3):";
    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: a lesser value matches disambiguated le<int&>",
  "[C++14][matching][matchers][le]")
{
  {
    U obj;
    REQUIRE_CALL(obj, func(trompeloeil::le<int&>(3)));
    int i = 2;
    obj.func(i);
  }
  REQUIRE(reports.empty());
}

// tests of parameter matching using typed matcher re

#if TROMPELOEIL_TEST_REGEX_FAILURES

TEST_CASE_METHOD(
  Fixture,
  "C++14: call to const c-string function matching regex is not reported",
  "[C++14][matching][matchers][re]")
{
  {
    mock_str obj;
    REQUIRE_CALL(obj, c_c_str(trompeloeil::re("mid")));
    char str[] = "pre mid post";
    obj.c_c_str(str);
  }
  REQUIRE(reports.empty());
}

#endif /* TROMPELOEIL_TEST_REGEX_FAILURES */

TEST_CASE_METHOD(
  Fixture,
  "C++14: call to const c-string function with nullptr to regex is reported",
  "[C++14][matching][matchers][re]")
{
  mock_str obj;
  REQUIRE_CALL(obj, c_c_str(trompeloeil::re("mid")));
  try
  {
    obj.c_c_str(nullptr);
    FAIL("did not throw");
  }
  catch (reported)
  {
    REQUIRE(reports.size() == 1U);
    auto& msg = reports.front().msg;
    INFO("msg=" << msg);
    auto re = R":(No match for call of c_c_str with signature void\(char const\*\) with.
  param  _1 == nullptr

Tried obj.c_c_str\(trompeloeil::re\("mid"\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 matching regular expression /mid/):";
    REQUIRE(std::regex_search(msg, std::regex(re)));
  }
}

#if TROMPELOEIL_TEST_REGEX_FAILURES

TEST_CASE_METHOD(
  Fixture,
  "C++14: call to const c-string function with non-matching string to regex is reported",
  "[C++14][matching][matchers][re]")
{
  mock_str obj;
  REQUIRE_CALL(obj, c_c_str(trompeloeil::re("mid")));
  try
  {
    char str[] = "abcde";
    obj.c_c_str(str);
    FAIL("did not throw");
  }
  catch (reported)
  {
    REQUIRE(reports.size() == 1U);
    auto& msg = reports.front().msg;
    INFO("msg=" << msg);
    auto re = R":(No match for call of c_c_str with signature void\(char const\*\) with.
  param  _1 == abcde

Tried obj.c_c_str\(trompeloeil::re\("mid"\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 matching regular expression /mid/):";
    REQUIRE(std::regex_search(msg, std::regex(re)));
  }
}

#endif /* TROMPELOEIL_TEST_REGEX_FAILURES */

//

#if TROMPELOEIL_TEST_REGEX_FAILURES

TEST_CASE_METHOD(
  Fixture,
  "C++14: call to non-const c-string function matching regex is not reported",
  "[C++14][matching][matchers][re]")
{
  {
    mock_str obj;
    REQUIRE_CALL(obj, c_str(trompeloeil::re("mid")));
    char str[] = "pre mid post";
    obj.c_str(str);
  }
  REQUIRE(reports.empty());
}

#endif /* TROMPELOEIL_TEST_REGEX_FAILURES */

TEST_CASE_METHOD(
  Fixture,
  "C++14: call to non-const c-string function with nullptr to regex is reported",
  "[C++14][matching][matchers][re]")
{
  mock_str obj;
  REQUIRE_CALL(obj, c_str(trompeloeil::re("mid")));
  try
  {
    obj.c_str(nullptr);
    FAIL("did not throw");
  }
  catch (reported)
  {
    REQUIRE(reports.size() == 1U);
    auto& msg = reports.front().msg;
    INFO("msg=" << msg);
    auto re = R":(No match for call of c_str with signature void\(char\*\) with.
  param  _1 == nullptr

Tried obj.c_str\(trompeloeil::re\("mid"\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 matching regular expression /mid/):";
    REQUIRE(std::regex_search(msg, std::regex(re)));
  }
}

#if TROMPELOEIL_TEST_REGEX_FAILURES

TEST_CASE_METHOD(
  Fixture,
  "C++14: call to non-const c-string function with non-matching string to regex is reported",
  "[C++14][matching][matchers][re]")
{
  mock_str obj;
  REQUIRE_CALL(obj, c_str(trompeloeil::re("mid")));
  try
  {
    char str[] = "abcde";
    obj.c_str(str);
    FAIL("did not throw");
  }
  catch (reported)
  {
    REQUIRE(reports.size() == 1U);
    auto& msg = reports.front().msg;
    INFO("msg=" << msg);
    auto re = R":(No match for call of c_str with signature void\(char\*\) with.
  param  _1 == abcde

Tried obj.c_str\(trompeloeil::re\("mid"\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 matching regular expression /mid/):";
    REQUIRE(std::regex_search(msg, std::regex(re)));
  }
}

#endif /* TROMPELOEIL_TEST_REGEX_FAILURES */

//@@

#if TROMPELOEIL_TEST_REGEX_FAILURES

TEST_CASE_METHOD(
  Fixture,
  "C++14: call to const strref function matching regex is not reported",
  "[C++14][matching][matchers][re]")
{
  {
    mock_str obj;
    REQUIRE_CALL(obj, strcref(trompeloeil::re("mid")));
    obj.strcref(std::string("pre mid post"));
  }
  REQUIRE(reports.empty());
}

#endif /* TROMPELOEIL_TEST_REGEX_FAILURES */

#if TROMPELOEIL_TEST_REGEX_FAILURES

TEST_CASE_METHOD(
  Fixture,
  "C++14: call to const strref function with non-matching string to regex is reported",
  "[C++14][matching][matchers][re]")
{
  mock_str obj;
  REQUIRE_CALL(obj, strcref(trompeloeil::re("mid")));
  try
  {
    obj.strcref(std::string("abcde"));
    FAIL("did not throw");
  }
  catch (reported)
  {
    REQUIRE(reports.size() == 1U);
    auto& msg = reports.front().msg;
    INFO("msg=" << msg);
    auto re = R":(No match for call of strcref with signature void\(std::string const&\) with\.
  param  _1 == abcde

Tried obj\.strcref\(trompeloeil::re\("mid"\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 matching regular expression /mid/):";
    REQUIRE(std::regex_search(msg, std::regex(re)));
  }
}

#endif /* TROMPELOEIL_TEST_REGEX_FAILURES */

//

#if TROMPELOEIL_TEST_REGEX_FAILURES

TEST_CASE_METHOD(
  Fixture,
  "C++14: call to non-const strref function matching regex is not reported",
  "[C++14][matching][matchers][re]")
{
  {
    mock_str obj;
    REQUIRE_CALL(obj, strref(trompeloeil::re("mid")));
    std::string str = "pre mid post";
    obj.strref(str);
  }
  REQUIRE(reports.empty());
}

#endif /* TROMPELOEIL_TEST_REGEX_FAILURES */

#if TROMPELOEIL_TEST_REGEX_FAILURES

TEST_CASE_METHOD(
  Fixture,
  "C++14: call to non-const strref function with non-matching string to regex is reported",
  "[C++14][matching][matchers][re]")
{
  mock_str obj;
  REQUIRE_CALL(obj, strref(trompeloeil::re("mid")));
  try
  {
    std::string str = "abcde";
    obj.strref(str);
    FAIL("did not throw");
  }
  catch (reported)
  {
    REQUIRE(reports.size() == 1U);
    auto& msg = reports.front().msg;
    INFO("msg=" << msg);
    auto re = R":(No match for call of strref with signature void\(std::string&\) with.
  param  _1 == abcde

Tried obj.strref\(trompeloeil::re\("mid"\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 matching regular expression /mid/):";
    REQUIRE(std::regex_search(msg, std::regex(re)));
  }
}

#endif /* TROMPELOEIL_TEST_REGEX_FAILURES */

//

#if TROMPELOEIL_TEST_REGEX_FAILURES

TEST_CASE_METHOD(
  Fixture,
  "C++14: call to non-const strrref function matching regex is not reported",
  "[C++14][matching][matchers][re]")
{
  {
    mock_str obj;
    REQUIRE_CALL(obj, strrref(trompeloeil::re("mid")));
    std::string str = "pre mid post";
    obj.strrref(std::move(str));
  }
  REQUIRE(reports.empty());
}

#endif /* TROMPELOEIL_TEST_REGEX_FAILURES */

#if TROMPELOEIL_TEST_REGEX_FAILURES

TEST_CASE_METHOD(
  Fixture,
  "C++14: call to non-const strrref function with non-matching string to regex is reported",
  "[C++14][matching][matchers][re]")
{
  mock_str obj;
  REQUIRE_CALL(obj, strrref(trompeloeil::re("mid")));
  try
  {
    std::string str = "abcde";
    obj.strrref(std::move(str));
    FAIL("did not throw");
  }
  catch (reported)
  {
    REQUIRE(reports.size() == 1U);
    auto& msg = reports.front().msg;
    INFO("msg=" << msg);
    auto re = R":(No match for call of strrref with signature void\(std::string&&\) with.
  param  _1 == abcde

Tried obj.strrref\(trompeloeil::re\("mid"\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 matching regular expression /mid/):";
    REQUIRE(std::regex_search(msg, std::regex(re)));
  }
}

#endif /* TROMPELOEIL_TEST_REGEX_FAILURES */

//

#if TROMPELOEIL_TEST_REGEX_FAILURES

TEST_CASE_METHOD(
  Fixture,
  "C++14: call to str val function matching regex is not reported",
  "[C++14][matching][matchers][re]")
{
  {
    mock_str obj;
    REQUIRE_CALL(obj, str(trompeloeil::re("mid")));
    std::string str = "pre mid post";
    obj.str(std::move(str));
  }
  REQUIRE(reports.empty());
}

#endif /* TROMPELOEIL_TEST_REGEX_FAILURES */

#if TROMPELOEIL_TEST_REGEX_FAILURES

TEST_CASE_METHOD(
  Fixture,
  "C++14: call to str val function with non-matching string to regex is reported",
  "[C++14][matching][matchers][re]")
{
  mock_str obj;
  REQUIRE_CALL(obj, str(trompeloeil::re("mid")));
  try
  {
    std::string str = "abcde";
    obj.str(std::move(str));
    FAIL("did not throw");
  }
  catch (reported)
  {
    REQUIRE(reports.size() == 1U);
    auto& msg = reports.front().msg;
    INFO("msg=" << msg);
    auto re = R":(No match for call of str with signature void\(std::string\) with.
  param  _1 == abcde

Tried obj.str\(trompeloeil::re\("mid"\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 matching regular expression /mid/):";
    REQUIRE(std::regex_search(msg, std::regex(re)));
  }
}

#endif /* TROMPELOEIL_TEST_REGEX_FAILURES */

#if TROMPELOEIL_TEST_REGEX_FAILURES

TEST_CASE_METHOD(
  Fixture,
  "C++14: call to mismatching regex of typed overload is reported",
  "[C++14][matching][matchers][re]")
{
  mock_str obj;
  REQUIRE_CALL(obj, overload(trompeloeil::re<char const*>("mid")));
  try
  {
    obj.overload("abcde");
    FAIL("did not throw");
  }
  catch (reported)
  {
    REQUIRE(reports.size() == 1U);
    auto& msg = reports.front().msg;
    INFO("msg=" << msg);
    auto re = R":(No match for call of overload with signature void\(char const\*\) with.
  param  _1 == abcde

Tried obj.overload\(trompeloeil::re<char const\*>\("mid"\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 matching regular expression /mid/):";
    REQUIRE(std::regex_search(msg, std::regex(re)));
  }
}

#endif /* TROMPELOEIL_TEST_REGEX_FAILURES */

#if TROMPELOEIL_TEST_REGEX_FAILURES

TEST_CASE_METHOD(
  Fixture,
  "C++14: call to matching regex of typed overload is not reported",
  "[C++14][matching][matchers][re]")
{
  {
    mock_str obj;
    REQUIRE_CALL(obj, overload(trompeloeil::re<std::string const&>("mid")));
    std::string str = "pre mid post";
    obj.overload(str);
  }
  REQUIRE(reports.empty());
}

#endif /* TROMPELOEIL_TEST_REGEX_FAILURES */

//

#if TROMPELOEIL_TEST_REGEX_FAILURES

TEST_CASE_METHOD(
  Fixture,
  "C++14: case insensitive regex matches case mismatch without explicit type",
  "[C++14][matching][matchers][re]")
{
  {
    mock_str obj;
    REQUIRE_CALL(obj, str(trompeloeil::re("MiXeD", std::regex_constants::icase)));
    std::string str = "mIXEd";
    obj.str(str);
  }
  REQUIRE(reports.empty());
}

#endif /* TROMPELOEIL_TEST_REGEX_FAILURES */

#if TROMPELOEIL_TEST_REGEX_FAILURES

TEST_CASE_METHOD(
  Fixture,
  "C++14: case insensitive regex matches case mismatch overload",
  "[C++14][matching][matchers][re]")
{
  {
    mock_str obj;
    REQUIRE_CALL(obj, overload(trompeloeil::re<std::string const&>("MiXeD", std::regex_constants::icase)));
    std::string str = "mIXEd";
    obj.overload(str);
  }
  REQUIRE(reports.empty());
}

#endif /* TROMPELOEIL_TEST_REGEX_FAILURES */

#if TROMPELOEIL_TEST_REGEX_FAILURES

TEST_CASE_METHOD(
  Fixture,
  "C++14: not_eol regex mismatching $ without explicit type is reported",
  "[C++14][matching][matchers][re]")
{
  try {
    mock_str obj;
    REQUIRE_CALL(obj, str(trompeloeil::re("end$", std::regex_constants::match_not_eol)));
    std::string str = "begin end";
    obj.str(str);
  }
  catch (reported)
  {
    REQUIRE(reports.size() == 1U);
    auto& msg = reports.front().msg;
    INFO("msg=" << msg);
    auto re = R":(No match for call of str with signature void\(std::string\) with.
  param  _1 == begin end

Tried obj.str\(trompeloeil::re\("end\$", std::regex_constants::match_not_eol\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 matching regular expression /end\$/):";
    REQUIRE(std::regex_search(msg, std::regex(re)));
  }
}

#endif /* TROMPELOEIL_TEST_REGEX_FAILURES */

#if TROMPELOEIL_TEST_REGEX_BOL_EOL_FAILURES

TEST_CASE_METHOD(
  Fixture,
  "C++14: not_eol regex mismatching $ overload is reported",
  "[C++14][matching][matchers][re]")
{
  try {
    mock_str obj;
    REQUIRE_CALL(obj, overload(trompeloeil::re<std::string const&>("end$", std::regex_constants::match_not_eol)));
    std::string str = "begin end";
    obj.overload(str);
    FAIL("did not throw");
  }
  catch (reported)
  {
    REQUIRE(reports.size() == 1U);
    auto& msg = reports.front().msg;
    INFO("msg=" << msg);
    auto re = R":(No match for call of overload with signature void\(std::string const&\) with.
  param  _1 == begin end

Tried obj.overload\(trompeloeil::re<std::string const&>\("end\$", std::regex_constants::match_not_eol\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 matching regular expression /end\$/):";
    REQUIRE(std::regex_search(msg, std::regex(re)));
  }
}

#endif /* TROMPELOEIL_TEST_REGEX_BOL_EOL_FAILURES */

// tests of parameter matching using not_matcher

TEST_CASE_METHOD(
  Fixture,
  "C++14: ! to duck typed equal matches inequal string",
  "[C++14][matching][matchers][eq][neg]")
{
  {
    mock_str obj;
    REQUIRE_CALL(obj, str(!trompeloeil::eq("foo")));
    obj.str("bar");
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: ! to duck typed equal reports equal string",
  "[C++14][matching][matchers][eq][neg]")
{
  try {
    mock_str obj;
    REQUIRE_CALL(obj, str(!trompeloeil::eq("foo")));
    obj.str("foo");
    FAIL("did not throw");
  }
  catch (reported)
  {
    REQUIRE(reports.size() == 1U);
    auto& msg = reports.front().msg;
    INFO("msg=" << msg);
    auto re= R":(No match for call of str with signature void\(std::string\) with.
  param  _1 == foo

Tried obj\.str\(!trompeloeil::eq\("foo"\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected not _1 == foo):";
    REQUIRE(std::regex_search(msg, std::regex(re)));
  }
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: ! to disambiguated equal matches inequal string",
  "[C++14][matching][matchers][eq][neg]")
{
  {
    mock_str obj;
    REQUIRE_CALL(obj, overload(!trompeloeil::eq<std::string>("foo")));
    obj.overload("bar"s);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: ! to disambiguated equal reports equal string",
  "[C++14][matching][matchers][eq][neg]")
{
  try {
    mock_str obj;
    REQUIRE_CALL(obj, overload(!trompeloeil::eq<std::string>("foo")));
    obj.overload("foo"s);
    FAIL("did not throw");
  }
  catch (reported)
  {
    REQUIRE(reports.size() == 1U);
    auto& msg = reports.front().msg;
    INFO("msg=" << msg);
    auto re= R":(No match for call of overload with signature void\(std::string const&\) with.
  param  _1 == foo

Tried obj\.overload\(!trompeloeil::eq<std::string>\("foo"\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected not _1 == foo):";
    REQUIRE(std::regex_search(msg, std::regex(re)));
  }
}

// tests of parameter matching using ptr deref matcher

TEST_CASE_METHOD(
  Fixture,
  "C++14: ptr to disambiguated equal const value matches deref",
  "[C++14][matching][matchers][eq]")
{
  {
    C_ptr obj;
    REQUIRE_CALL(obj, coverload(*trompeloeil::eq<const int&>(3)));
    const int n = 3;
    obj.coverload(&n);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: ptr to equal value matches deref",
  "[C++14][matching][matchers][eq]")
{
  {
    C_ptr obj;
    REQUIRE_CALL(obj, ptr(*trompeloeil::eq(3)));
    int n = 3;
    obj.ptr(&n);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: ptr to equal nullptr matches deref",
  "[C++14][matching][matchers][eq]")
{
  {
    C_ptr obj;
    REQUIRE_CALL(obj, pp(*trompeloeil::eq(nullptr)));
    int* p = nullptr;
    obj.pp(&p);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: ptr to overloaded ptr matches equal deref",
  "[C++14][matching][matchers][eq]")
{
  {
    C_ptr obj;
    REQUIRE_CALL(obj, overloaded(*trompeloeil::eq(nullptr)));
    int* p = nullptr;
    obj.overloaded(&p);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: ptr to overloaded string matches equal deref to string literal",
  "[C++14][matching][matchers][eq]")
{
  {
    C_ptr obj;
    REQUIRE_CALL(obj, overloaded(*trompeloeil::eq<std::string&>("apa"s)));
    std::string s{"apa"};
    obj.overloaded(&s);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: nullptr when equal ptr deref expected is reported",
  "[C++14][matching][matchers][eq]")
{
  try
  {
    C_ptr obj;
    REQUIRE_CALL(obj, ptr(*trompeloeil::eq(3)));
    obj.ptr(nullptr);
    FAIL("didn't throw");
  }
  catch (reported)
  {
    REQUIRE(!reports.empty());
    auto re = R":(No match for call of ptr with signature void\(int\*\) with\.
  param  _1 == nullptr

Tried obj\.ptr\(\*trompeloeil::eq\(3\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected \*_1 == 3):";

    INFO("msg=" << reports.front().msg);
    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: non-nullptr when equal nullptr ptr deref expected is reported",
  "[C++14][matching][matchers][eq]")
{
  try
  {
    C_ptr obj;
    REQUIRE_CALL(obj, pp(*trompeloeil::eq(nullptr)));
    int i = 3;
    auto pi = &i;
    obj.pp(&pi);
    FAIL("didn't throw");
  }
  catch (reported)
  {
    REQUIRE(!reports.empty());
    auto re = R":(No match for call of pp with signature void\(int\*\*\) with\.
  param  _1 == .*

Tried obj\.pp\(\*trompeloeil::eq\(nullptr\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected \*_1 == nullptr):";

    INFO("msg=" << reports.front().msg);
    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: ptr to different value when equal ptr deref expected is reported",
  "[C++14][matching][matchers][eq]")
{
  try
  {
    C_ptr obj;
    REQUIRE_CALL(obj, ptr(*trompeloeil::eq(3)));
    int n = 2;
    obj.ptr(&n);
    FAIL("didn't throw");
  }
  catch (reported)
  {
    REQUIRE(!reports.empty());
    auto re = R":(No match for call of ptr with signature void\(int\*\) with\.
  param  _1 == .*

Tried obj\.ptr\(\*trompeloeil::eq\(3\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected \*_1 == 3):";
    INFO(reports.front().msg);
    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}

TEST_CASE_METHOD(Fixture, "ptr to equal value of different size matches deref", "[matching][matchers][eq]")
{
  {
    C_ptr obj;
    REQUIRE_CALL(obj, ptr(*trompeloeil::eq(3L)));
    int n = 3;
    obj.ptr(&n);
  }
  REQUIRE(reports.empty());
}

////

TEST_CASE_METHOD(Fixture, "unique_ptr value to equal value matches deref", "[matching][matchers][eq]")
{
  {
    C_ptr obj;
    REQUIRE_CALL(obj, uptr(*trompeloeil::eq(3)));
    obj.uptr(detail::make_unique<int>(3));
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(Fixture, "unique_ptr<>() value when equal ptr deref expected is reported", "[matching][matchers][eq]")
{
  try
  {
    C_ptr obj;
    REQUIRE_CALL(obj, uptr(*trompeloeil::eq(3)));
    obj.uptr(nullptr);
    FAIL("didn't throw");
  }
  catch (reported)
  {
    REQUIRE(!reports.empty());
    auto re = R":(No match for call of uptr with signature void\(std::unique_ptr<int>\) with\.
  param  _1 == nullptr

Tried obj\.uptr\(\*trompeloeil::eq\(3\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected \*_1 == 3):";
    INFO("msg=" << reports.front().msg);
    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}

TEST_CASE_METHOD(Fixture, "unique ptr value to different value when equal ptr deref expected is reported", "[matching][matchers][eq]")
{
  try
  {
    C_ptr obj;
    REQUIRE_CALL(obj, uptr(*trompeloeil::eq(3)));
    obj.uptr(detail::make_unique<int>(2));
    FAIL("didn't throw");
  }
  catch (reported)
  {
    REQUIRE(!reports.empty());
    auto re = R":(No match for call of uptr with signature void\(std::unique_ptr<int>\) with\.
  param  _1 == .*

Tried obj\.uptr\(\*trompeloeil::eq\(3\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected \*_1 == 3):";
    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}

TEST_CASE_METHOD(Fixture, "unique value ptr to equal value of different size matches deref", "[matching][matchers][eq]")
{
  {
    C_ptr obj;
    REQUIRE_CALL(obj, uptr(*trompeloeil::eq(3L)));
    obj.uptr(detail::make_unique<int>(3));
  }
  REQUIRE(reports.empty());
}

//////

TEST_CASE_METHOD(Fixture, "unique_ptr rvalue ref to equal value matches deref", "[matching][matchers][eq]")
{
  {
    C_ptr obj;
    REQUIRE_CALL(obj, uptrrr(*trompeloeil::eq(3)));
    obj.uptrrr(detail::make_unique<int>(3));
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(Fixture, "unique_ptr<>() rvalue ref when equal ptr deref expected is reported", "[matching][matchers][eq]")
{
  try
  {
    C_ptr obj;
    REQUIRE_CALL(obj, uptrrr(*trompeloeil::eq(3)));
    obj.uptrrr(nullptr);
    FAIL("didn't throw");
  }
  catch (reported)
  {
    REQUIRE(!reports.empty());
    auto re = R":(No match for call of uptrrr with signature void\(std::unique_ptr<int>&&\) with\.
  param  _1 == nullptr

Tried obj\.uptrrr\(\*trompeloeil::eq\(3\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected \*_1 == 3):";
    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}

TEST_CASE_METHOD(Fixture, "unique ptr rvalue ref to different value when equal ptr deref expected is reported", "[matching][matchers][eq]")
{
  try
  {
    C_ptr obj;
    REQUIRE_CALL(obj, uptrrr(*trompeloeil::eq(3)));
    obj.uptrrr(detail::make_unique<int>(2));
    FAIL("didn't throw");
  }
  catch (reported)
  {
    REQUIRE(!reports.empty());
    auto re = R":(No match for call of uptrrr with signature void\(std::unique_ptr<int>&&\) with\.
  param  _1 == .*

Tried obj\.uptrrr\(\*trompeloeil::eq\(3\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected \*_1 == 3):";
    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}

TEST_CASE_METHOD(Fixture, "unique ptr rvalue ref to equal value of different size matches deref", "[matching][matchers][eq]")
{
  {
    C_ptr obj;
    REQUIRE_CALL(obj, uptrrr(*trompeloeil::eq(3L)));
    obj.uptrrr(detail::make_unique<int>(3));
  }
  REQUIRE(reports.empty());
}

////

TEST_CASE_METHOD(Fixture, "unique_ptr const lvalue ref to equal value matches deref", "[matching][matchers][eq]")
{
  {
    C_ptr obj;
    REQUIRE_CALL(obj, uptrcr(*trompeloeil::eq(3)));
    obj.uptrcr(detail::make_unique<int>(3));
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(Fixture, "unique_ptr<>() const lvalue ref when equal ptr deref expected is reported", "[matching][matchers][eq]")
{
  try
  {
    C_ptr obj;
    REQUIRE_CALL(obj, uptrcr(*trompeloeil::eq(3)));
    obj.uptrcr(nullptr);
    FAIL("didn't throw");
  }
  catch (reported)
  {
    REQUIRE(!reports.empty());
    auto re = R":(No match for call of uptrcr with signature void\(std::unique_ptr<int> const&\) with\.
  param  _1 == nullptr

Tried obj\.uptrcr\(\*trompeloeil::eq\(3\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected \*_1 == 3):";
    INFO(reports.front().msg);
    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}

TEST_CASE_METHOD(Fixture, "unique ptr const lvalue ref to different value when equal ptr deref expected is reported", "[matching][matchers][eq]")
{
  try
  {
    C_ptr obj;
    REQUIRE_CALL(obj, uptrcr(*trompeloeil::eq(3)));
    obj.uptrcr(detail::make_unique<int>(2));
    FAIL("didn't throw");
  }
  catch (reported)
  {
    REQUIRE(!reports.empty());
    auto re = R":(No match for call of uptrcr with signature void\(std::unique_ptr<int> const&\) with\.
  param  _1 == .*

Tried obj\.uptrcr\(\*trompeloeil::eq\(3\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected \*_1 == 3):";

    INFO(reports.front().msg);
    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}

TEST_CASE_METHOD(Fixture, "unique ptr const lvalue ref to equal value of different size matches deref", "[matching][matchers][eq]")
{
  {
    C_ptr obj;
    REQUIRE_CALL(obj, uptrcr(*trompeloeil::eq(3L)));
    obj.uptrcr(detail::make_unique<int>(3));
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(Fixture, "missing call to unique ptr const lvalue ref to equal value is reported", "[matching][matchers][eq]")
{
  {
    C_ptr obj;
    REQUIRE_CALL(obj, uptrcr(*trompeloeil::eq(3)));
  }
  REQUIRE(!reports.empty());
  auto re = R":(Unfulfilled expectation:
Expected obj\.uptrcr\(\*trompeloeil::eq\(3\)\) to be called once, actually never called
  param \*_1 == 3):";

  INFO(reports.front().msg);
  REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));

//  REQUIRE(reports.front().msg == "");
}

#if TROMPELOEIL_TEST_REGEX_FAILURES

TEST_CASE_METHOD(
  Fixture,
  "C++14: call to string* function matching regex is not reported",
  "[C++14][matching][matchers][re]")
{
  {
    C_ptr obj;
    REQUIRE_CALL(obj, strptr(*trompeloeil::re("end$")));
    std::string s = "begin end";
    obj.strptr(&s);
  }
  REQUIRE(reports.empty());
}

#endif /* TROMPELOEIL_TEST_REGEX_FAILURES */

#if TROMPELOEIL_TEST_REGEX_FAILURES

TEST_CASE_METHOD(
  Fixture,
  "C++14: call to string* function not matching regex is reported",
  "[C++14][matching][matchers][re]")
{
  try {
    C_ptr obj;
    REQUIRE_CALL(obj, strptr(*trompeloeil::re("end$")));
    std::string s = "begin end;";
    obj.strptr(&s);
  }
  catch (reported)
  {
    REQUIRE(reports.size() == 1U);
    INFO("report=" << reports.front().msg);
    auto re = R":(No match for call of strptr with signature void\(std::string\*\) with\.
  param  _1 == .*

Tried obj\.strptr\(\*trompeloeil::re\("end\$"\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected \*_1 matching regular expression /end\$/):";
    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}

#endif /* TROMPELOEIL_TEST_REGEX_FAILURES */

// tests of parameter matching using custom typed matcher

TEST_CASE_METHOD(
  Fixture,
  "C++14: custom matcher of first element is not reported",
  "[C++14][matching][matchers][custom]")
{
  {
    mock_c obj;
    REQUIRE_CALL(obj, getter(any_of({ 1, 5, 77 })))
      .RETURN(0);
    obj.getter(1);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: custom matcher of last element is not reported",
  "[C++14][matching][matchers][custom]")
{
  {
    mock_c obj;
    REQUIRE_CALL(obj, getter(any_of({ 1, 5, 77 })))
      .RETURN(0);
    obj.getter(77);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: custom matcher of mid element is not reported",
  "[C++14][matching][matchers][custom]")
{
  {
    mock_c obj;
    REQUIRE_CALL(obj, getter(any_of({ 1, 5, 77 })))
      .RETURN(0);
    obj.getter(5);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: custom matcher of unlisted element is reported",
  "[C++14][matching][matchers][custom]")
{
  try {
    mock_c obj;
    REQUIRE_CALL(obj, getter(any_of({ 1,5,77 })))
      .RETURN(0);
    obj.getter(4);
    FAIL("didn't report");
  }
  catch (reported)
  {
    REQUIRE(reports.size() == 1U);
    INFO("report=" << reports.front().msg);
    auto re = R":(No match for call of getter with signature int\(int\) with\.
  param  _1 == 4

Tried obj\.getter\(any_of\(\{ 1,5,77 \}\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 matching any_of\(\{ 1, 5, 77 \}\)):";
    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: custom matcher can math pointer via *deref",
  "[C++14][matching][matchers][custom]")
{
  {
    C_ptr obj;
    REQUIRE_CALL(obj, ptr(*any_of({1,5,7})));
    int n = 5;
    obj.ptr(&n);
  }
  REQUIRE(reports.empty());
}

// tests of parameter matching using custom duck-typed matcher

TEST_CASE_METHOD(
  Fixture,
  "C++14: a non empty string gives no report",
  "[C++14][matching][matchers][custom]")
{
  {
    mock_c obj;
    REQUIRE_CALL(obj, foo(not_empty{}));
    obj.foo("bar");
  }
  REQUIRE(reports.empty());
}

#if TROMPELOEIL_TEST_REGEX_FAILURES

TEST_CASE_METHOD(
  Fixture,
  "C++14: an empty string is reported",
  "[C++14][matching][matchers][custom]")
{
  try {
    mock_c obj;
    REQUIRE_CALL(obj, foo(not_empty{}));
    obj.foo("");
    FAIL("didn't report");
  }
  catch (reported)
  {
    REQUIRE(reports.size() == 1U);
    auto re = R":(No match for call of foo with signature void\(std::string\) with\.
  param  _1 ==.

Tried obj\.foo\(not_empty\{\}\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 is not empty):";
    auto& msg = reports.front().msg;
    INFO("msg=" << msg);
    REQUIRE(std::regex_search(msg, std::regex(re)));
  }
}

#endif /* TROMPELOEIL_TEST_REGEX_FAILURES */

#if !(TROMPELOEIL_CLANG &&                                                           \
     (TROMPELOEIL_CLANG_VERSION >= 100000 && TROMPELOEIL_CLANG_VERSION < 120000) &&  \
     (TROMPELOEIL_CPLUSPLUS > 201703L && TROMPELOEIL_CPLUSPLUS < 202100L))

/*
 * Compile for all configurations except
 * Clang and (10.0.0 <= version < 12.0.0) and C++20 mode and
 * libstdc++v3 released with GCC version 10 (not checked).
 *
 * This configuration has a defect such that
 * operator<=>() for std::string, generates a
 * -Wzero-as-null-pointer-constant warning with the note
 * "while rewriting comparison as call to 'operator<=>'".
 * See: https://bugs.llvm.org/show_bug.cgi?id=44325 .
 */

auto is_clamped_lambda_normal =
  [](auto x, auto min, auto max) -> decltype(x >= min && x <= max)
  {
    return x >= min && x <= max;
  };

template <typename kind = trompeloeil::wildcard, typename T>
auto is_clamped_normal(T min, T max)
{
  using trompeloeil::make_matcher;
  return make_matcher<kind>(
    is_clamped_lambda_normal,

    [](std::ostream& os, auto amin, auto amax) {
      os << " in range [";
      ::trompeloeil::print(os, amin);
      os << ", ";
      ::trompeloeil::print(os, amax);
      os << "]";
    },

    min,
    max);
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: a normal custom duck typed make_matcher-matcher that fails is reported",
  "[C++14][matching][matchers][custom]")
{
  try {
    mock_c obj;
    REQUIRE_CALL(obj, foo(is_clamped_normal("b", "d")));
    obj.foo(std::string("a"));
    FAIL("didn't report");
  }
  catch(reported)
  {
    REQUIRE(reports.size() == 1U);
    auto& msg = reports.front().msg;
    INFO(msg);
    auto re = R":(No match for call of foo with signature void\(std::string\) with\.
  param  _1 == a

Tried obj\.foo\(is_clamped_normal\("b", "d"\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 in range \[b, d\]):";
    REQUIRE(std::regex_search(msg, std::regex(re)));
  }
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: a normal custom duck typed make_matcher-matcher that succeeds is not reported",
  "[C++14][matching][matchers][custom]")
{
  mock_c obj;
  REQUIRE_CALL(obj, foo(is_clamped_normal("b", "d")));
  obj.foo(std::string("c"));
}

#endif /* !(Clang and (10.0.0 <= version < 12.0.0) and C++20 mode) */

/*
 * Define is_clamped_lambda in terms of an expression
 * that never uses operator<=>() for std::string, which otherwise
 * may generate a -Wzero-as-null-pointer-constant warning.
 *
 * This is compiled for all configurations >= C++14.
 */

auto is_clamped_lambda =
  [](auto x, auto min, auto max)
  -> decltype(x.compare(min) >= 0 && x.compare(max) <= 0)
  {
    return x.compare(min) >= 0 && x.compare(max) <= 0;
  };

template <typename kind = trompeloeil::wildcard, typename T>
auto is_clamped(T min, T max)
{
  using trompeloeil::make_matcher;
  return make_matcher<kind>(
    is_clamped_lambda,

    [](std::ostream& os, auto amin, auto amax) {
      os << " in range [";
      ::trompeloeil::print(os, amin);
      os << ", ";
      ::trompeloeil::print(os, amax);
      os << "]";
    },

    min,
    max);
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: a custom duck typed make_matcher-matcher that fails is reported",
  "[C++14][matching][matchers][custom]")
{
  try {
    mock_c obj;
    REQUIRE_CALL(obj, foo(is_clamped("b", "d")));
    obj.foo(std::string("a"));
    FAIL("didn't report");
  }
  catch(reported)
  {
    REQUIRE(reports.size() == 1U);
    auto& msg = reports.front().msg;
    INFO(msg);
    auto re = R":(No match for call of foo with signature void\(std::string\) with\.
  param  _1 == a

Tried obj\.foo\(is_clamped\("b", "d"\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 in range \[b, d\]):";
    REQUIRE(std::regex_search(msg, std::regex(re)));
  }
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: a custom duck typed make_matcher-matcher that succeeds is not reported",
  "[C++14][matching][matchers][custom]")
{
  mock_c obj;
  REQUIRE_CALL(obj, foo(is_clamped("b", "d")));
  obj.foo(std::string("c"));
}

// tests of parameter values ostream insertion

TEST_CASE_METHOD(
  Fixture,
  "C++14: unknown type is printed as hex dump",
  "[C++14][streaming]")
{
  std::ostringstream os;
  trompeloeil::print(os, unknown{});
  REQUIRE(os.str() == "4-byte object={ 0x01 0x02 0x12 0x13 }");
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: print after unknown has flags back to default",
  "[C++14][streaming]")
{
  std::ostringstream os;
  trompeloeil::print(os, unknown{});
  int16_t u = 10000;
  os << u;
  REQUIRE(os.str() == "4-byte object={ 0x01 0x02 0x12 0x13 }10000");
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: previous formatting is ignored before hexdump and then reset",
  "[C++14][streaming]")
{
  std::ostringstream os;
  os << std::oct << std::setfill('_') << std::setw(4) << std::left;
  trompeloeil::print(os, unknown{});
  os << 8;
  REQUIRE(os.str() == "4-byte object={ 0x01 0x02 0x12 0x13 }10__");
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: call predefined ostream operator with defaulted flags and then reset",
  "[C++14][streaming]")
{
  std::ostringstream os;
  os << std::oct << std::setfill('_') << std::setw(4) << std::left;
  trompeloeil::print(os, 25);
  os << 8;
  REQUIRE(os.str() == "2510__");
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: large unknown is multi row hex dump",
  "[C++14][streaming]")
{
  struct big {
    char c[64] = { 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                   0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
                   0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
                   0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
                   0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
                   0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
                   0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
                   0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f };
  };
  std::ostringstream os;
  trompeloeil::print(os, big{});
  char str[] = R"__(64-byte object={
 0x10 0x11 0x12 0x13 0x14 0x15 0x16 0x17 0x18 0x19 0x1a 0x1b 0x1c 0x1d 0x1e 0x1f
 0x20 0x21 0x22 0x23 0x24 0x25 0x26 0x27 0x28 0x29 0x2a 0x2b 0x2c 0x2d 0x2e 0x2f
 0x30 0x31 0x32 0x33 0x34 0x35 0x36 0x37 0x38 0x39 0x3a 0x3b 0x3c 0x3d 0x3e 0x3f
 0x40 0x41 0x42 0x43 0x44 0x45 0x46 0x47 0x48 0x49 0x4a 0x4b 0x4c 0x4d 0x4e 0x4f
 })__";
  auto s = os.str();
  REQUIRE(os.str() == str);
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: unknown object is one line if 8 bytes",
  "[C++14][streaming]")
{
  struct big {
    char c[8] = { 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17 };
  };

  std::ostringstream os;
  trompeloeil::print(os, big{});
  REQUIRE(os.str() == "8-byte object={ 0x10 0x11 0x12 0x13 0x14 0x15 0x16 0x17 }");
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: A std::pair<> is printed as { val1, val2 }",
  "[C++14][streaming]")
{
  std::ostringstream os;
  trompeloeil::print(os, std::make_pair(3, std::string("hello")));
  REQUIRE(os.str() == "{ 3, hello }");
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: A std::tuple<> is printed as { val1, val2, val3, }",
  "[C++14][streaming]")
{
  std::ostringstream os;
  trompeloeil::print(os, std::make_tuple(3, "hello", std::string("world")));
  REQUIRE(os.str() == "{ 3, hello, world }");
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: A C-array is printed as { val1, val2, val3 }",
  "[C++14][streaming]")
{
  std::ostringstream os;
  std::tuple<int, std::string> v[] {
    std::make_tuple(1, "one"),
    std::make_tuple(2, "two"),
    std::make_tuple(3, "three")
  };
  static_assert(!trompeloeil::is_output_streamable<decltype(v)>(),"");
  static_assert(trompeloeil::is_collection<decltype(v)>(), "");
  trompeloeil::print(os, v);
  REQUIRE(os.str() == "{ { 1, one }, { 2, two }, { 3, three } }");
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: A std::map<> is printed as { { key1, val1 }, { key2, val2 } }",
  "[C++14][streaming]")
{
  std::ostringstream os;
  std::map<std::string, int> m{ {"one", 1}, {"two", 2 }, {"three", 3 } };
  trompeloeil::print(os, m);
  REQUIRE(os.str() == "{ { one, 1 }, { three, 3 }, { two, 2 } }");
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: A tuple with pairs and maps is printed element wise",
  "[C++14][streaming]")
{
    std::ostringstream os;
    auto v = std::make_tuple(std::make_pair(3, std::string("hello")),
                             std::map<int, std::string>{{1, "one"},{2, "two"},{3, "three"}});
    trompeloeil::print(os, v);
    REQUIRE(os.str() == "{ { 3, hello }, { { 1, one }, { 2, two }, { 3, three } } }");
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: failure on parameter of user type is printed with custom print func",
  "[C++14][streaming]")
{
  TestOutputMock m;
  try
  {
    m.func(nn::TestOutput{ 3 });
    FAIL("didn't throw");
  }
  catch (reported)
  {
    REQUIRE(!reports.empty());
    auto re = R":(No match for call of func with signature void\(nn::TestOutput\) with\.
  param  _1 == trompeloeil::print\(nn::TestOutput\{3\}\)):";
    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}


// tests on scoping (lifetime) of expectations

TEST_CASE_METHOD(
  Fixture,
  "C++14: require calls are matched in reversed order of creation",
  "[C++14][scoping]")
{
  {
    mock_c obj;

    ALLOW_CALL(obj, getter(_, _))
      .SIDE_EFFECT(_2 = std::to_string(_1));

    REQUIRE_CALL(obj, getter(3, _))
      .SIDE_EFFECT(_2 = "III")
      .TIMES(2);

    std::string s;
    obj.getter(2, s);

    REQUIRE(s == "2");

    obj.getter(3, s);

    REQUIRE(s == "III");

    obj.getter(1, s);

    REQUIRE(s == "1");

    obj.getter(3, s);

    REQUIRE(s == "III");

    obj.getter(3, s);

    REQUIRE(s == "3");
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: require calls are removed when they go out of scope",
  "[C++14][scoping]")
{
  {
    mock_c obj;
    std::string s;

    ALLOW_CALL(obj, getter(_, _))
      .SIDE_EFFECT(_2 = std::to_string(_1));

    {
      REQUIRE_CALL(obj, getter(3, _))
        .SIDE_EFFECT(_2 = "III")
        .TIMES(1, 8);

      obj.getter(3, s);

      REQUIRE(s == "III");

      obj.getter(2, s);

      REQUIRE(s == "2");

      obj.getter(3, s);

      REQUIRE(s == "III");
    }

    obj.getter(3, s);

    REQUIRE(s == "3");
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: a pending unsatisfied require call is reported at end of scope",
  "[C++14][scoping]")
{
  mock_c obj;
  {
    REQUIRE_CALL(obj, foo("bar"));
  }
  REQUIRE(reports.size() == 1U);

  auto re = R":(Unfulfilled expectation:
Expected obj\.foo\("bar"\) to be called once, actually never called
  param  _1 == bar):";
  INFO(reports.front().msg);
  REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: Unfulfilled expectation with ANY is reported with expected values all parameters",
  "[C++14][scoping][wildcard]")
{
 {
    mock_c obj;
    REQUIRE_CALL(obj, func(3, ANY(std::string&)));
    std::string s = "foo";
  }

  REQUIRE(reports.size() == 1U);
  auto re = R":(Unfulfilled expectation:
Expected obj\.func\(3, ANY\(std::string&\)\) to be called once, actually never called
  param  _1 == 3
  param  _2 matching ANY\(std::string&\)):";
  auto& msg = reports.front().msg;
  INFO("msg=" << msg);
  REQUIRE(std::regex_search(msg, std::regex(re)));
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: Unfulfilled expectation with _ is reported with expected values all parameters",
  "[C++14][scoping][wildcard]")
{
 {
    mock_c obj;
    REQUIRE_CALL(obj, func(3, _));
    std::string s = "foo";
  }

  REQUIRE(reports.size() == 1U);
  auto re = R":(Unfulfilled expectation:
Expected obj\.func\(3, _\) to be called once, actually never called
  param  _1 == 3
  param  _2 matching _):";
  auto& msg = reports.front().msg;
  INFO("msg=" << msg);
  REQUIRE(std::regex_search(msg, std::regex(re)));
}

// test of multiplicity retiring expectations, fulfilled or not

TEST_CASE_METHOD(
  Fixture,
  "C++14: FORBID_CALL is always both satisfied and saturated",
  "[C++14][multiplicity]")
{
  {
    mock_c obj;
    auto e = NAMED_FORBID_CALL(obj, count());
    REQUIRE(e->is_satisfied());
    REQUIRE(e->is_saturated());
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: ALLOW_ALL is always satisfied and never saturated",
  "[C++14][multiplicity]")
{
  {
    mock_c obj;
    auto e = NAMED_ALLOW_CALL(obj, count())
      .RETURN(1);
    REQUIRE(e->is_satisfied());
    REQUIRE(!e->is_saturated());
    obj.count();
    REQUIRE(e->is_satisfied());
    REQUIRE(!e->is_saturated());
    obj.count();
    REQUIRE(e->is_satisfied());
    REQUIRE(!e->is_saturated());
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: .TIMES is satisfied when min calls is reached, and not saturated until max calls is reached",
  "[C++14][multiplicity]")
{
 {
    mock_c obj;
    auto e = NAMED_REQUIRE_CALL(obj, count())
      .TIMES(1,3)
      .RETURN(1);
    REQUIRE(!e->is_satisfied());
    REQUIRE(!e->is_saturated());
    obj.count();
    REQUIRE(e->is_satisfied());
    REQUIRE(!e->is_saturated());
    obj.count();
    REQUIRE(e->is_satisfied());
    REQUIRE(!e->is_saturated());
    obj.count();
    REQUIRE(e->is_satisfied());
    REQUIRE(e->is_saturated());
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: unsatisfied expectation when mock dies is reported",
  "[C++14][scoping][multiplicity]")
{
  auto m = detail::make_unique<mock_c>();
  REQUIRE_CALL(*m, count())
    .RETURN(1);
  m.reset();
  REQUIRE(reports.size() == 1U);
  INFO(reports.front().msg);
  auto re = R":(Pending expectation on destroyed mock object:
Expected .*count\(\) to be called once, actually never called):";
  REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: multiple unsatisfied expectation when mock dies are reported in mock definition order",
  "[C++14][scoping][multiplicity]")
{
  auto m = detail::make_unique<mock_c>();
  REQUIRE_CALL(*m, count())
    .RETURN(1);
  REQUIRE_CALL(*m, getter(1))
    .RETURN(1);
  m.reset();
  REQUIRE(reports.size() == 2U);
  INFO(reports[0].msg);
  INFO(reports[1].msg);
  auto re_count = R":(Pending expectation on destroyed mock object:
Expected .*count\(\) to be called once, actually never called):";
  auto re_getter = R":(Pending expectation on destroyed mock object:
Expected .*getter\(1\) to be called once, actually never called):";
  REQUIRE(std::regex_search(reports[0].msg, std::regex(re_getter)));
  REQUIRE(std::regex_search(reports[1].msg, std::regex(re_count)));
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: active allow call when mock dies is not reported",
  "[C++14][scoping][multiplicity]")
{
  {
    auto m = detail::make_unique<mock_c>();
    ALLOW_CALL(*m, count())
      .RETURN(1);
    m.reset();
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: active forbid call when mock dies is not reported",
  "[C++14][scoping][multiplicity]")
{
  {
    auto m = detail::make_unique<mock_c>();
    FORBID_CALL(*m, count());
    m.reset();
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: saturated expectation when mock dies is not reported",
  "[C++14][scoping]")
{
  {
    auto m = detail::make_unique<mock_c>();
    REQUIRE_CALL(*m, count())
      .RETURN(1);
    m->count();
    m.reset();
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: no calls reported as never called",
  "[C++14][scoping][multiplicity]")
{
  mock_c obj;
  {
    REQUIRE_CALL(obj, count())
      .RETURN(1);
  }
  REQUIRE(reports.size() == 1U);
  REQUIRE(std::regex_search(reports.front().msg, std::regex("actually never called")));
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: undersatisfied with one call reported as once",
  "[C++14][scoping][multiplicity]")
{
  mock_c obj;
  {
    REQUIRE_CALL(obj, count())
      .RETURN(1)
      .TIMES(2);
    obj.count();
  }
  REQUIRE(reports.size() == 1U);
  REQUIRE(std::regex_search(reports.front().msg, std::regex("actually called once")));
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: undersatisfied with two call reported as count",
  "[C++14][scoping][multiplicity]")
{
  mock_c obj;
  {
    REQUIRE_CALL(obj, count())
      .RETURN(1)
      .TIMES(3);
    obj.count();
    obj.count();
  }
  REQUIRE(reports.size() == 1U);
  REQUIRE(std::regex_search(reports.front().msg, std::regex("actually called 2 times")));
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: no calls when one required reported as expected once",
  "[C++14][scoping][multiplicity]")
{
  mock_c obj;
  {
    REQUIRE_CALL(obj, count())
      .RETURN(1);
  }
  REQUIRE(reports.size() == 1U);
  REQUIRE(std::regex_search(reports.front().msg, std::regex("to be called once")));
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: no calls when two required reported as expected 2 times",
  "[C++14][scoping][multiplicity]")
{
  mock_c obj;
  {
    REQUIRE_CALL(obj, count())
      .TIMES(2)
      .RETURN(1);
  }
  REQUIRE(reports.size() == 1U);
  REQUIRE(std::regex_search(reports.front().msg, std::regex("to be called 2 times")));
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: TIMES works for templated mock classes",
  "[C++14][multiplicity][templates]")
{
  try
  {
    tmock<int> m;
    m.func(3);
    FAIL("didn't throw");
  }
  catch (reported)
  {
    REQUIRE(reports.size() == 1U);
    INFO(reports.front().msg);
    REQUIRE(std::regex_search(reports.front().msg,
                              std::regex("Match of forbidden call")));

  }
}

// test of destruction, or lack of, for deathwatched objects

TEST_CASE_METHOD(
  Fixture,
  "C++14: an unexpected destruction of monitored object is reported",
  "[C++14][deathwatched]")
{
  {
    trompeloeil::deathwatched<mock_c> obj;
  }
  REQUIRE(reports.size() == 1U);
  REQUIRE(std::regex_search(reports.front().msg, std::regex("Unexpected destruction of.*@")));
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: an expected destruction of monitored object is not reported",
  "[C++14][deathwatched]")
{
  {
    auto obj = new trompeloeil::deathwatched<mock_c>;
    REQUIRE_DESTRUCTION(*obj);
    delete obj;
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: a copy of a deathwatched object with expectation is not expected to die",
  "[C++14][deathwatched]")
{
  auto orig = new trompeloeil::deathwatched<none>;
  REQUIRE_DESTRUCTION(*orig);
  auto copy = new trompeloeil::deathwatched<none>(*orig);

  delete orig;
  REQUIRE(reports.empty());

  delete copy;
  REQUIRE(reports.size() == 1U);
  REQUIRE(std::regex_search(reports.front().msg, std::regex("Unexpected destruction of .*@")));
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: a deathwatched object move constructed from original with expectation is not expected to die and the original still is",
  "[C++14][deathwatched]")
{
  auto orig = new trompeloeil::deathwatched<none>;
  REQUIRE_DESTRUCTION(*orig);
  auto copy = new trompeloeil::deathwatched<none>(std::move(*orig));

  delete orig;
  REQUIRE(reports.empty());

  delete copy;
  REQUIRE(reports.size() == 1U);
  REQUIRE(std::regex_search(reports.front().msg, std::regex("Unexpected destruction of .*@")));
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: object alive when destruction expectation goes out of scope is reported",
  "[C++14][deathwatched]")
{
  trompeloeil::deathwatched<mock_c> obj;
  {
    std::unique_ptr<trompeloeil::expectation> p = NAMED_REQUIRE_DESTRUCTION(obj);
  }
  REQUIRE(reports.size() == 1U);
  REQUIRE(std::regex_search(reports.front().msg, std::regex("Object obj is still alive")));
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: require destruction is neither satisfied nor saturated while object is alive",
  "[C++14][deathwatched]")
{
  {
    using monitor = std::unique_ptr<trompeloeil::lifetime_monitor>;

    auto obj = new trompeloeil::deathwatched<mock_c>();
    monitor p = NAMED_REQUIRE_DESTRUCTION(*obj);
    REQUIRE(!p->is_saturated());
    REQUIRE(!p->is_satisfied());
    delete obj;
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: require destruction is both satisfied and saturated when object is destroyed",
  "[C++14][deathwatched]")
{
  auto obj = new trompeloeil::deathwatched<mock_c>();
  auto p = NAMED_REQUIRE_DESTRUCTION(*obj);
  delete obj;
  REQUIRE(p->is_saturated());
  REQUIRE(p->is_satisfied());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: require destruction succeeds also without deathwatch",
  "[C++14][deathwatched]")
{
  {
    auto obj = new trompeloeil::deathwatched<mock_c>;
    REQUIRE_DESTRUCTION(*obj);
    delete obj;
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: a deathwatched objects constructor passes params to mock",
  "[C++14][deathwatched]")
{
  auto obj = new trompeloeil::deathwatched<mock_c>{ "apa" };
  REQUIRE(obj->p_ == std::string("apa"));
  REQUIRE_DESTRUCTION(*obj);
  delete obj;
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: require destruction fulfilled in sequence is not reported",
  "[C++14][deathwatched][sequences]")
{
  auto obj = new trompeloeil::deathwatched<mock_c>;
  trompeloeil::sequence s;
  REQUIRE_CALL(*obj, foo("foo"))
    .IN_SEQUENCE(s);
  REQUIRE_DESTRUCTION(*obj)
    .IN_SEQUENCE(s);
  obj->foo("foo");
  delete obj;
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
    Fixture,
    "C++14: require destruction fulfilled in sequence with another object is not reported",
    "[C++14][deathwatched][sequences]")
{
  auto obj = new trompeloeil::deathwatched<mock_c>;
  mock_c obj2;
  trompeloeil::sequence s;
  REQUIRE_DESTRUCTION(*obj)
    .IN_SEQUENCE(s);
  REQUIRE_CALL(obj2, foo("foo"))
    .IN_SEQUENCE(s);
  delete obj;
  obj2.foo("foo");
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: named require destruction fulfilled in sequence is not reported",
  "[C++14][deathwatched][sequences]")
{
  auto obj = new trompeloeil::deathwatched<mock_c>;
  trompeloeil::sequence s;
  REQUIRE_CALL(*obj, foo("foo"))
    .IN_SEQUENCE(s);
  auto d = NAMED_REQUIRE_DESTRUCTION(*obj)
    .IN_SEQUENCE(s);
  obj->foo("foo");
  delete obj;
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: require destruction fulfilled out of sequence is reported",
  "[C++14][deathwatched][sequences]")
{
  auto obj = new trompeloeil::deathwatched<mock_c>;
  trompeloeil::sequence s;
  REQUIRE_CALL(*obj, foo("foo"))
    .IN_SEQUENCE(s);
  REQUIRE_DESTRUCTION(*obj)
    .IN_SEQUENCE(s);
  delete obj;
  REQUIRE(!reports.empty());
  auto& msg = reports.front().msg;
  auto re = R":(Sequence mismatch for sequence "s".*destructor for \*obj at [A-Za-z0-9_ ./:\]*:[0-9]*.*\n.*foo"):";
  INFO("msg=" << msg);
  REQUIRE(std::regex_search(msg, std::regex(re)));
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: sequence mismatch with require destruction first is reported",
  "[C++14][deathwatched][sequences]")
{
  std::unique_ptr<trompeloeil::deathwatched<mock_c>> obj;
  try {
    obj.reset(new trompeloeil::deathwatched<mock_c>);
    trompeloeil::sequence s;
    REQUIRE_DESTRUCTION(*obj)
      .IN_SEQUENCE(s);
    REQUIRE_CALL(*obj, foo("foo"))
      .IN_SEQUENCE(s);
    obj->foo("foo");
    FAIL("didn't throw");
  }
  catch (reported&)
  {
    REQUIRE(!reports.empty());
    auto& msg = reports.front().msg;
    auto re = R":(Sequence mismatch for sequence "s".*\*obj.foo\("foo"\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*\nSequence.* REQUIRE_DESTRUCTION\(\*obj\)):";
    INFO("msg=" << msg);
    REQUIRE(std::regex_search(msg, std::regex(re)));
  }
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: named require destruction fulfilled out of sequence is reported",
  "[C++14][deathwatched][sequences]")
{
  auto obj = new trompeloeil::deathwatched<mock_c>;
  trompeloeil::sequence s;
  REQUIRE_CALL(*obj, foo("foo"))
    .IN_SEQUENCE(s);
  auto d = NAMED_REQUIRE_DESTRUCTION(*obj)
    .IN_SEQUENCE(s);
  delete obj;
  REQUIRE(!reports.empty());
  auto& msg = reports.front().msg;
  auto re = R":(Sequence mismatch for sequence "s".*destructor for \*obj at [A-Za-z0-9_ ./:\]*:[0-9]*.*\n.*foo"):";
  INFO("msg=" << msg);
  REQUIRE(std::regex_search(msg, std::regex(re)));
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: sequence mismatch with named require destruction first is reported",
  "[C++14][deathwatched][sequences]")
{
  std::unique_ptr<trompeloeil::deathwatched<mock_c>> obj;
  try {
    obj.reset(new trompeloeil::deathwatched<mock_c>);
    trompeloeil::sequence s;
    auto d = NAMED_REQUIRE_DESTRUCTION(*obj)
      .IN_SEQUENCE(s);
    REQUIRE_CALL(*obj, foo("foo"))
      .IN_SEQUENCE(s);
    obj->foo("foo");
    FAIL("didn't throw");
  }
  catch (reported&)
  {
    REQUIRE(!reports.empty());
    auto& msg = reports.front().msg;
    auto re = R":(Sequence mismatch for sequence "s".*\*obj.foo\("foo"\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*\nSequence.* NAMED_REQUIRE_DESTRUCTION\(\*obj\)):";
    INFO("msg=" << msg);
    REQUIRE(std::regex_search(msg, std::regex(re)));
  }
}

// tests of calls that do not match any valid expectations

TEST_CASE_METHOD(
  Fixture,
  "C++14: unmatched call is reported",
  "[C++14][mismatches]")
{
  try {
    mock_c obj;
    obj.getter(7);
    FAIL("didn't throw!");
  }
  catch (reported)
  {
    REQUIRE(reports.size() == 1U);
    auto re = R":(No match for call of getter with signature int\(int\) with\.
  param  _1 == 7):";
    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: match of saturated call is reported",
  "[C++14][mismatches]")
{
  int count = 0;
  try {
    mock_c obj;
    ALLOW_CALL(obj, getter(ANY(int)))
      .WITH(_1 != 3)
      .RETURN(1);

    REQUIRE_CALL(obj, getter(3))
      .TIMES(3)
      .RETURN(1);

    count += obj.getter(4); // 1
    count += obj.getter(2); // 2
    count += obj.getter(3); // 3 -> 1
    count += obj.getter(4); // 4
    count += obj.getter(2); // 5
    count += obj.getter(3); // 6 -> 2
    count += obj.getter(5); // 7
    count += obj.getter(3); // 8 -> 3
    count += obj.getter(8); // 9
    count += obj.getter(3); // boom!
    FAIL("didn't report");
  }
  catch (reported)
  {
    REQUIRE(count == 9);
    REQUIRE(reports.size() == 1U);
    auto re =
           R":(No match for call of getter with signature int\(int\) with\.
  param  _1 == 3

Matches saturated call requirement
  obj\.getter\(3\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*):";
    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: a matching call that throws is saturated",
  "[C++14][mismatches]")
{
  int count = 0;
  try {
    mock_c obj;
    ALLOW_CALL(obj, getter(ANY(int)))
      .WITH(_1 != 3)
      .RETURN(1);

    REQUIRE_CALL(obj, getter(3))
      .TIMES(3)
      .SIDE_EFFECT(throw 0)
      .RETURN(1);

    count += obj.getter(4); // 1
    count += obj.getter(2); // 2
    try {
      count += obj.getter(3); // 2 -> 1
      FAIL("didn't throw");
    }
    catch (int) {}
    count += obj.getter(4); // 3
    count += obj.getter(2); // 4
    try {
      count += obj.getter(3); // 4 -> 2
      FAIL("didn't throw");
    }
    catch (int) {}
    count += obj.getter(5); // 5
    try {
      count += obj.getter(3); // 5 -> 3
      FAIL("didn't throw");
    }
    catch (int) {}
    count += obj.getter(8); // 6
    count += obj.getter(3); // boom!
    FAIL("didn't report");
  }
  catch (reported)
  {
    REQUIRE(count == 6);
    REQUIRE(reports.size() == 1U);
    auto re =
           R":(No match for call of getter with signature int\(int\) with\.
  param  _1 == 3

Matches saturated call requirement
  obj\.getter\(3\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*):";
    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: unmatched call with mismatching requirements is reported",
  "[C++14][mismatches]")
{
  try {
    mock_c obj;
    REQUIRE_CALL(obj, getter(4))
      .RETURN(0);
    REQUIRE_CALL(obj, getter(5))
      .RETURN(0);
    obj.getter(3);
  }
  catch (reported)
  {
    REQUIRE(reports.size() == 1U);
    auto re =
           R":(No match for call of getter with signature int\(int\) with\.
  param  _1 == 3

Tried obj\.getter\(5\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 == 5

Tried obj\.getter\(4\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 == 4):";
    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: unmatched with wildcard reports failed WITH clauses",
  "[C++14][mismatches]")
{
  try {
    mock_c obj;
    REQUIRE_CALL(obj, getter(ANY(int)))
      .WITH(_1 < 3)
      .WITH(_1 > 5)
      .RETURN(0);
    obj.getter(4);
  }
  catch (reported)
  {
    REQUIRE(reports.size() == 1U);
    auto re =
           R":(No match for call of getter with signature int\(int\) with\.
  param  _1 == 4

Tried obj\.getter\(ANY\(int\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Failed WITH\(_1 < 3\)
  Failed WITH\(_1 > 5\)):";
    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: unmatched with wildcard reports only failed WITH clauses",
  "[C++14][mismatches]")
{
  try {
    mock_c obj;
    REQUIRE_CALL(obj, getter(ANY(int)))
      .WITH(_1 < 3)
      .WITH(_1 > 3)
      .RETURN(0);
    obj.getter(4);
  }
  catch (reported)
  {
    REQUIRE(reports.size() == 1U);
    auto re =
           R":(No match for call of getter with signature int\(int\) with\.
  param  _1 == 4

Tried obj\.getter\(ANY\(int\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Failed WITH\(_1 < 3\)):";
    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: match of forbidden call is reported",
  "[C++14][mismatches]")
{
  try {
    mock_c obj;
    ALLOW_CALL(obj, getter(ANY(int)))
      .RETURN(0);
    FORBID_CALL(obj, getter(3));

    obj.getter(4);
    obj.getter(2);
    obj.getter(3);
    FAIL("didn't report");
  }
  catch (reported)
  {
    REQUIRE(reports.size() == 1U);
    INFO("report=" << reports.front().msg);
    auto re = R":(Match of forbidden call of obj\.getter\(3\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  param  _1 == 3):";
    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: Mismatched call to a mocked function with param from template is reported",
  "[C++14][mismatches][templates]")
{
  try {
    tmock<int> obj;
    REQUIRE_CALL(obj, tfunc(3));
    obj.tfunc(2);
    FAIL("didn't report");
  }
  catch (reported)
  {
    REQUIRE(reports.size() == 1U);
    INFO("report=" << reports.front().msg);
    auto re = R":(No match for call of tfunc with signature void\(T\) with\.
  param  _1 == 2

Tried obj\.tfunc\(3\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 == 3):";
    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}

// tests of parameter passing to expectations

TEST_CASE_METHOD(
  Fixture,
  "C++14: parameters are passed in correct order when matching",
  "[C++14][parameters]")
{
  T obj;
  REQUIRE_CALL(obj, concats(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15))
    .RETURN("");
  auto s = obj.concats(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
  REQUIRE(s == "");
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: parameters are passed in correct order to WITH",
  "[C++14][parameters]")
{
  T obj;
  REQUIRE_CALL(obj, concats(_, _, _, _, _, _, _, _, _, _, _, _, _, _, _))
    .WITH(_1  ==  1 &&  _2 ==  2 &&  _3 ==  3 &&  _4 ==  4 &&
          _5  ==  5 &&  _6 ==  6 &&  _7 ==  7 &&  _8 ==  8 &&
          _9  ==  9 && _10 == 10 && _11 == 11 && _12 == 12 &&
          _13 == 13 && _14 == 14 && _15 == 15)
    .RETURN("");
  auto s = obj.concats(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
  REQUIRE(s == "");
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: parameters are passed in correct order to LR_SIDE_EFFECT",
  "[C++14][parameters]")
{
  T obj;
  int n = 0;
  REQUIRE_CALL(obj, concats(_, _, _, _, _, _, _, _, _, _, _, _, _, _, _))
    .LR_SIDE_EFFECT(n = _1 + _2 - _3 + _4 - _5 + _6 - _7 + _8 - _9 + _10 - _11 + _12 - _13 + _14 - _15)
    .RETURN("");
  auto s = obj.concats(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
  REQUIRE(n == -6);
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: parameters are passed in correct order to RETURN",
  "[C++14][parameters]")
{
  T obj;
  REQUIRE_CALL(obj, concats(_, _, _, _, _, _, _, _, _, _, _, _, _, _, _))
    .RETURN(std::to_string(_1) +
            std::to_string(_2) +
            std::to_string(_3) +
            std::to_string(_4) +
            std::to_string(_5) +
            std::to_string(_6) +
            std::to_string(_7) +
            std::to_string(_8) +
            std::to_string(_9) +
            std::to_string(_10) +
            std::to_string(_11) +
            std::to_string(_12) +
            std::to_string(_13) +
            std::to_string(_14) +
            std::to_string(_15));
  auto s = obj.concats(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
  REQUIRE(s == "123456789101112131415");
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: parameters are passed in correct order to THROW",
  "[C++14][parameters]")
{
  T obj;

  REQUIRE_CALL(obj, concats(_, _, _, _, _, _, _, _, _, _, _, _, _, _, _))
    .THROW(_1 + _2 - _3 + _4 - _5 + _6 - _7 + _8 - _9 + _10 - _11 + _12 - _13 + _14 - _15);
  try {
    obj.concats(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
    FAIL("didn't throw");
  }
  catch (int n)
  {
    REQUIRE(n == -6);
  }
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: shared ptr by value in expectation is copied",
  "[C++14][parameters]")
{
  T obj;
  auto s = std::make_shared<int>(3);
  {
    REQUIRE_CALL(obj, ptr(s));
    REQUIRE(s.use_count() == 2U);
    obj.ptr(s);
  }
  REQUIRE(reports.empty());
  REQUIRE(s.use_count() == 1U);
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: shared ptr by std ref in expectation is not copied",
  "[C++14][parameters]")
{
  T obj;
  auto s = std::make_shared<int>(3);
  {
    REQUIRE_CALL(obj, ptr(std::ref(s)));
    REQUIRE(s.use_count() == 1U);
    obj.ptr(s);
  }
  REQUIRE(reports.empty());
  REQUIRE(s.use_count() == 1U);
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: unique ptr by value is matched with raw ptr in WITH",
  "[C++14][parameters]")
{
  T obj;
  auto s = std::unique_ptr<int>(new int(3));
  {
    auto sr = s.get();
    REQUIRE_CALL(obj, ptr(ANY(std::unique_ptr<int>)))
      .WITH(_1.get() == sr);
    obj.ptr(std::move(s));
    REQUIRE_FALSE(s);
  }
  REQUIRE(reports.empty());
}

// tests of tracing of matched calls

TEST_CASE_METHOD(
  Fixture,
  "C++14: matching calls are traced",
  "[C++14][tracing]")
{
  std::ostringstream os;
  trompeloeil::stream_tracer logger(os);
  mock_c obj1;
  mock_c obj2;
  REQUIRE_CALL(obj1, getter(_, _));
  REQUIRE_CALL(obj2, foo("bar"))
    .THROW(std::logic_error("nonono"));
  REQUIRE_CALL(obj1, getter(ANY(int)))
    .RETURN(_1 + 1);
  REQUIRE_CALL(obj2, foo("baz"))
    .THROW(3);
  std::string s = "foo";
  obj1.getter(3, s);
  try {
    obj2.foo("bar");
  }
  catch (std::logic_error&) { /* ignore, it's meant to happen */}
  obj1.getter(4);
  try {
    obj2.foo("baz");
  }
  catch (int) { /* ignore, it's meant to happen */}
  auto re =
         R":([A-Za-z0-9_ ./:\]*:[0-9]*.*
obj1\.getter\(_, _\) with.
  param  _1 == 3
  param  _2 == foo

[A-Za-z0-9_ ./:\]*:[0-9]*.*
obj2\.foo\("bar"\) with\.
  param  _1 == bar
threw exception: what\(\) = nonono

[A-Za-z0-9_ ./:\]*:[0-9]*.*
obj1\.getter\(ANY\(int\)\) with\.
  param  _1 == 4
 -> 5

[A-Za-z0-9_ ./:\]*:[0-9]*.*
obj2\.foo\("baz"\) with\.
  param  _1 == baz
threw unknown exception
):";
  INFO(os.str());
  REQUIRE(std::regex_search(os.str(), std::regex(re)));
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: tracing is only active when tracer obj is alive",
  "[C++14][tracing]")
{
  std::ostringstream os;
  mock_c obj1;
  mock_c obj2;
  ALLOW_CALL(obj1, getter(_, _));
  REQUIRE_CALL(obj2, foo("bar"));
  std::string s = "foo";
  obj1.getter(3, s);
  {
    trompeloeil::stream_tracer logger(os);
    obj2.foo("bar");
  }
  obj1.getter(4, s);
  auto re =
         R":([A-Za-z0-9_ ./:\]*:[0-9]*.*
obj2\.foo\("bar"\) with\.
  param  _1 == bar
):";
  REQUIRE(std::regex_search(os.str(), std::regex(re)));
}

TEST_CASE(
  "C++14: all overridden short mocks can be expected and called",
  "[C++14][signatures][override]")
{
  all_mock_if mock;
  REQUIRE_CALL(mock, f0());
  REQUIRE_CALL(mock, f1(0));
  REQUIRE_CALL(mock, f2(0,1));
  REQUIRE_CALL(mock, f3(0,1,2));
  REQUIRE_CALL(mock, f4(0,1,2,3));
  REQUIRE_CALL(mock, f5(0,1,2,3,4));
  REQUIRE_CALL(mock, f6(0,1,2,3,4,5));
  REQUIRE_CALL(mock, f7(0,1,2,3,4,5,6));
  REQUIRE_CALL(mock, f8(0,1,2,3,4,5,6,7));
  REQUIRE_CALL(mock, f9(0,1,2,3,4,5,6,7,8));
  REQUIRE_CALL(mock, f10(0,1,2,3,4,5,6,7,8,9));
  REQUIRE_CALL(mock, f11(0,1,2,3,4,5,6,7,8,9,10));
  REQUIRE_CALL(mock, f12(0,1,2,3,4,5,6,7,8,9,10,11));
  REQUIRE_CALL(mock, f13(0,1,2,3,4,5,6,7,8,9,10,11,12));
  REQUIRE_CALL(mock, f14(0,1,2,3,4,5,6,7,8,9,10,11,12,13));
  REQUIRE_CALL(mock, f15(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14));
  REQUIRE_CALL(mock, cf0());
  REQUIRE_CALL(mock, cf1(0));
  REQUIRE_CALL(mock, cf2(0,1));
  REQUIRE_CALL(mock, cf3(0,1,2));
  REQUIRE_CALL(mock, cf4(0,1,2,3));
  REQUIRE_CALL(mock, cf5(0,1,2,3,4));
  REQUIRE_CALL(mock, cf6(0,1,2,3,4,5));
  REQUIRE_CALL(mock, cf7(0,1,2,3,4,5,6));
  REQUIRE_CALL(mock, cf8(0,1,2,3,4,5,6,7));
  REQUIRE_CALL(mock, cf9(0,1,2,3,4,5,6,7,8));
  REQUIRE_CALL(mock, cf10(0,1,2,3,4,5,6,7,8,9));
  REQUIRE_CALL(mock, cf11(0,1,2,3,4,5,6,7,8,9,10));
  REQUIRE_CALL(mock, cf12(0,1,2,3,4,5,6,7,8,9,10,11));
  REQUIRE_CALL(mock, cf13(0,1,2,3,4,5,6,7,8,9,10,11,12));
  REQUIRE_CALL(mock, cf14(0,1,2,3,4,5,6,7,8,9,10,11,12,13));
  REQUIRE_CALL(mock, cf15(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14));

  mock.f0();
  mock.f1(0);
  mock.f2(0,1);
  mock.f3(0,1,2);
  mock.f4(0,1,2,3);
  mock.f5(0,1,2,3,4);
  mock.f6(0,1,2,3,4,5);
  mock.f7(0,1,2,3,4,5,6);
  mock.f8(0,1,2,3,4,5,6,7);
  mock.f9(0,1,2,3,4,5,6,7,8);
  mock.f10(0,1,2,3,4,5,6,7,8,9);
  mock.f11(0,1,2,3,4,5,6,7,8,9,10);
  mock.f12(0,1,2,3,4,5,6,7,8,9,10,11);
  mock.f13(0,1,2,3,4,5,6,7,8,9,10,11,12);
  mock.f14(0,1,2,3,4,5,6,7,8,9,10,11,12,13);
  mock.f15(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14);
  mock.cf0();
  mock.cf1(0);
  mock.cf2(0,1);
  mock.cf3(0,1,2);
  mock.cf4(0,1,2,3);
  mock.cf5(0,1,2,3,4);
  mock.cf6(0,1,2,3,4,5);
  mock.cf7(0,1,2,3,4,5,6);
  mock.cf8(0,1,2,3,4,5,6,7);
  mock.cf9(0,1,2,3,4,5,6,7,8);
  mock.cf10(0,1,2,3,4,5,6,7,8,9);
  mock.cf11(0,1,2,3,4,5,6,7,8,9,10);
  mock.cf12(0,1,2,3,4,5,6,7,8,9,10,11);
  mock.cf13(0,1,2,3,4,5,6,7,8,9,10,11,12);
  mock.cf14(0,1,2,3,4,5,6,7,8,9,10,11,12,13);
  mock.cf15(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14);
}

TEST_CASE(
  "C++14: all non-overridden short mocks can be expected and called",
  "[C++14][signatures]")
{
  all_mock mock;
  REQUIRE_CALL(mock, f0());
  REQUIRE_CALL(mock, f1(0));
  REQUIRE_CALL(mock, f2(0,1));
  REQUIRE_CALL(mock, f3(0,1,2));
  REQUIRE_CALL(mock, f4(0,1,2,3));
  REQUIRE_CALL(mock, f5(0,1,2,3,4));
  REQUIRE_CALL(mock, f6(0,1,2,3,4,5));
  REQUIRE_CALL(mock, f7(0,1,2,3,4,5,6));
  REQUIRE_CALL(mock, f8(0,1,2,3,4,5,6,7));
  REQUIRE_CALL(mock, f9(0,1,2,3,4,5,6,7,8));
  REQUIRE_CALL(mock, f10(0,1,2,3,4,5,6,7,8,9));
  REQUIRE_CALL(mock, f11(0,1,2,3,4,5,6,7,8,9,10));
  REQUIRE_CALL(mock, f12(0,1,2,3,4,5,6,7,8,9,10,11));
  REQUIRE_CALL(mock, f13(0,1,2,3,4,5,6,7,8,9,10,11,12));
  REQUIRE_CALL(mock, f14(0,1,2,3,4,5,6,7,8,9,10,11,12,13));
  REQUIRE_CALL(mock, f15(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14));
  REQUIRE_CALL(mock, cf0());
  REQUIRE_CALL(mock, cf1(0));
  REQUIRE_CALL(mock, cf2(0,1));
  REQUIRE_CALL(mock, cf3(0,1,2));
  REQUIRE_CALL(mock, cf4(0,1,2,3));
  REQUIRE_CALL(mock, cf5(0,1,2,3,4));
  REQUIRE_CALL(mock, cf6(0,1,2,3,4,5));
  REQUIRE_CALL(mock, cf7(0,1,2,3,4,5,6));
  REQUIRE_CALL(mock, cf8(0,1,2,3,4,5,6,7));
  REQUIRE_CALL(mock, cf9(0,1,2,3,4,5,6,7,8));
  REQUIRE_CALL(mock, cf10(0,1,2,3,4,5,6,7,8,9));
  REQUIRE_CALL(mock, cf11(0,1,2,3,4,5,6,7,8,9,10));
  REQUIRE_CALL(mock, cf12(0,1,2,3,4,5,6,7,8,9,10,11));
  REQUIRE_CALL(mock, cf13(0,1,2,3,4,5,6,7,8,9,10,11,12));
  REQUIRE_CALL(mock, cf14(0,1,2,3,4,5,6,7,8,9,10,11,12,13));
  REQUIRE_CALL(mock, cf15(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14));

  mock.f0();
  mock.f1(0);
  mock.f2(0,1);
  mock.f3(0,1,2);
  mock.f4(0,1,2,3);
  mock.f5(0,1,2,3,4);
  mock.f6(0,1,2,3,4,5);
  mock.f7(0,1,2,3,4,5,6);
  mock.f8(0,1,2,3,4,5,6,7);
  mock.f9(0,1,2,3,4,5,6,7,8);
  mock.f10(0,1,2,3,4,5,6,7,8,9);
  mock.f11(0,1,2,3,4,5,6,7,8,9,10);
  mock.f12(0,1,2,3,4,5,6,7,8,9,10,11);
  mock.f13(0,1,2,3,4,5,6,7,8,9,10,11,12);
  mock.f14(0,1,2,3,4,5,6,7,8,9,10,11,12,13);
  mock.f15(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14);
  mock.cf0();
  mock.cf1(0);
  mock.cf2(0,1);
  mock.cf3(0,1,2);
  mock.cf4(0,1,2,3);
  mock.cf5(0,1,2,3,4);
  mock.cf6(0,1,2,3,4,5);
  mock.cf7(0,1,2,3,4,5,6);
  mock.cf8(0,1,2,3,4,5,6,7);
  mock.cf9(0,1,2,3,4,5,6,7,8);
  mock.cf10(0,1,2,3,4,5,6,7,8,9);
  mock.cf11(0,1,2,3,4,5,6,7,8,9,10);
  mock.cf12(0,1,2,3,4,5,6,7,8,9,10,11);
  mock.cf13(0,1,2,3,4,5,6,7,8,9,10,11,12);
  mock.cf14(0,1,2,3,4,5,6,7,8,9,10,11,12,13);
  mock.cf15(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14);
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: a member function of a mock object can call a mocked function",
  "[C++14]")
{
  {
    self_ref_mock m;
    m.expect_self();
    m.mfunc();
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: expectation on a mock function can call the same mock func recursively as side effect",
  "[C++14]")
{
  {
    self_ref_mock m;
    unsigned mask = 0U;
    REQUIRE_CALL(m, mfunc())
      .LR_SIDE_EFFECT(mask |= 2U);
    REQUIRE_CALL(m, mfunc())
      .LR_SIDE_EFFECT(mask |= 1U)
      .LR_SIDE_EFFECT(m.mfunc());
    m.mfunc();
    REQUIRE(mask == 3U);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: A macro may instantiate many expectations",
  "[C++14]")
{
  all_mock m;
  MANY_REQS(m);
  m.f0();
  m.f1(0);
  m.f2(0,1);
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: REQUIRE_CALL generate OK-report when satisfied",
  "[C++14][matching]")
{
  {
    mock_c obj;
    REQUIRE_CALL(obj, foo("bar"));
    obj.foo("bar");
  }
  REQUIRE(okReports.size() == 1U);
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: REQUIRE_CALL doesn't generate OK-report when not satisfied",
  "[C++14][matching]")
{
  {
    mock_c obj;
    REQUIRE_CALL(obj, foo("bar"));
  }
  REQUIRE(okReports.empty());
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: ALLOW_CALL generate OK-report when satisfied",
  "[C++14][matching]")
{
  {
    mock_c obj;
    ALLOW_CALL(obj, foo("bar"));
    obj.foo("bar");
  }
  REQUIRE(okReports.size() == 1U);
}

TEST_CASE_METHOD(
  Fixture,
  "C++14: ALLOW_CALL doesn't generate OK-report when not satisfied",
  "[C++14][matching]")
{
  {
    mock_c obj;
    REQUIRE_CALL(obj, foo("bar"));
  }
  REQUIRE(okReports.empty());
}

#endif /* TROMPELOEIL_CPLUSPLUS > 201103L */
