/*
* Trompeloeil C++ mocking framework
*
* Copyright Björn Fahller 2014-2016
*
*  Use, modification and distribution is subject to the
*  Boost Software License, Version 1.0. (See accompanying
*  file LICENSE_1_0.txt or copy at
*  http://www.boost.org/LICENSE_1_0.txt)
*
* Project home: https://github.com/rollbear/trompeloeil
*/

#define TROMPELOEIL_SANITY_CHECKS
#include "trompeloeil.hpp"
#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <vector>
#include <string>
#include <algorithm>
#include <regex>

using namespace std::string_literals;

class reported {};

struct report
{
  trompeloeil::severity s;
  const char           *file;
  unsigned long         line;
  std::string           msg;
};

static std::vector<report> reports;

namespace trompeloeil
{
  template <>
  struct reporter<specialized>
  {
    static void send(severity s,
                     char const* file,
                     unsigned long line,
                     char const* msg)
    {
      reports.push_back(report{s, file, line, msg});
      if (s == severity::fatal && !std::uncaught_exception())
      {
        throw reported{};
      }
    }
  };
}
struct Fixture
{
  Fixture() {
    reports.clear();
  }
};
struct uncopyable
{
  uncopyable() {}
  uncopyable(uncopyable&&) = default;
  uncopyable(const uncopyable&) = delete;
  uncopyable& operator=(const uncopyable&) = delete;
  bool operator==(const uncopyable& p) const { return &p == this; }
  bool operator!=(const uncopyable& p) const { return &p == this; }
  friend std::ostream& operator<<(std::ostream& os, const uncopyable& obj)
  {
    return os << "uncopyable@" << &obj;
  }
};

struct unmovable
{
  unmovable() {}
  unmovable(unmovable&&) = delete;
  unmovable(const unmovable&) = delete;
  unmovable& operator=(unmovable&&) = delete;
  unmovable& operator=(const unmovable&) = delete;
  bool operator==(const unmovable& p) const { return &p == this; }
  bool operator!=(const unmovable& p) const { return &p != this; }
  friend std::ostream& operator<<(std::ostream& os, const unmovable& obj)
  {
    return os << "unmovable@" << &obj;
  }
};

struct uncomparable { };

class C
{
public:
  C() {}
  C(int) {}
  virtual ~C() = default;
  virtual int count() = 0;
  virtual void func(int, std::string& s) = 0;
  virtual unmovable& getter(unmovable&) = 0;
  virtual int getter(int) = 0;
  virtual void getter(int, std::string&) = 0;
  virtual std::unique_ptr<int> ptr(std::unique_ptr<int>&&) = 0;
protected:
  C(const char* s) : p_{ s } {}
  const char *p_ = nullptr;
};

class mock_c : public C
{
public:
  mock_c() {}
  mock_c(int i) : C(i) {}
  mock_c(const char* p) : C(p) {}
  MAKE_MOCK1(ptr, std::unique_ptr<int>(std::unique_ptr<int>&&), override);
  MAKE_MOCK0(count, int(), override final);
  MAKE_MOCK1(foo, void(std::string));
  MAKE_MOCK2(func, void(int, std::string&), override);
  MAKE_MOCK1(getter, unmovable&(unmovable&), override);
  MAKE_MOCK1(getter, int(int), override);
  MAKE_MOCK2(getter, void(int, std::string&), override);
  using C::p_;
};


using trompeloeil::_;

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

// IN_SEQUENCE tests

TEST_CASE_METHOD(Fixture, "follow single sequence gives no reports", "[sequences]")
{
  {
    mock_c obj1(1), obj2("apa");

    trompeloeil::sequence seq;

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

TEST_CASE_METHOD(Fixture, "join two sequences gives no report", "[sequences]")
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

TEST_CASE_METHOD(Fixture, "violating single sequence reports first violation as fatal", "[sequences]")
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
    auto re = R":(Sequence mismatch.*\"seq\".*matching.*obj2.count\(\).*has obj2\.func\(_,_\) at.*first):";
    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}

TEST_CASE_METHOD(Fixture, "violating parallel sequences reports first violation as fatal", "[sequences]")
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
    auto re = R":(Sequence mismatch.*seq2.*of obj2\.count\(\).*has obj1.count\(\).*first):";
    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}

TEST_CASE_METHOD(Fixture, "a sequence retires after min calls", "[sequences]")
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
    obj1.func(3, s);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(Fixture, "calling a sequenced match after seq retires is allowed", "[sequences]")
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
    obj1.func(3, s);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(Fixture, "breaking a sequence before retirement is illegal", "[sequences]")
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
    obj1.func(3, s);
    FAIL("didn't throw");
  }
  catch (reported)
  {
    REQUIRE(reports.size() == 1U);
    auto re = R":(Sequence mismatch.*seq1.*of obj1\.func\(_, _\).*has obj1\.count\(\).*first):";
    INFO("report=" << reports.front().msg);
    REQUIRE(std::regex_search(reports.front().msg,  std::regex(re)));
    auto& first = reports.front();
    INFO(first.file << ':' << first.line << "\n" << first.msg);
  }
}

TEST_CASE_METHOD(Fixture, "sequences impose order between multiple matching expectations", "[sequences]")
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

TEST_CASE_METHOD(Fixture, "Sequence object destruction with live expectations is reported", "[sequences]")
{
  mock_c obj;

  std::unique_ptr<trompeloeil::expectation> e[2];
  {

    trompeloeil::sequence s;

    e[0] = NAMED_REQUIRE_CALL(obj, getter(ANY(int)))
      .IN_SEQUENCE(s)
      .RETURN(0);
    e[1] = NAMED_REQUIRE_CALL(obj, foo(_))
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

// SIDE_EFFECT and LR_SIDE_EFFECT tests

static int global_n = 0;
TEST_CASE_METHOD(Fixture, "side effect access copy of local object", "[side effects]")
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

TEST_CASE_METHOD(Fixture, "lr side effect access reference of local object", "[side effects]")
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

TEST_CASE_METHOD(Fixture, "multiple side effects are executed in the order declared", "[side effects]")
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

TEST_CASE_METHOD(Fixture, "RETURN access copy of local object", "[return values]")
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

TEST_CASE_METHOD(Fixture, "LR_RETURN access the actual local object", "[return values]")
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

TEST_CASE_METHOD(Fixture, "RETURN a ref to local obj, std::ref(obj) returns object given", "[return values]")
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

TEST_CASE_METHOD(Fixture, "RETURN a ref to local obj, (obj) returns object given", "[return values]")
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


struct mstr
{
  MAKE_MOCK0(cc_str, const char*());
  MAKE_MOCK0(c_str, char*());
  MAKE_MOCK0(str, std::string());
  MAKE_MOCK0(cstr, const std::string());
};

extern char carr[]; // silence clang++ warning
char carr[]= "foo";
const char ccarr[] = "bar";

TEST_CASE_METHOD(Fixture, "RETURN const char* from const char*", "[return]")
{
  mstr m;
  const char* s = "foo";
  REQUIRE_CALL(m, cc_str())
    .RETURN(s);
  REQUIRE(m.cc_str() == "foo"s);
}

TEST_CASE_METHOD(Fixture, "RETURN const char* from string literal", "[return]")
{
  mstr m;
  REQUIRE_CALL(m, cc_str())
  .RETURN("foo");
  REQUIRE(m.cc_str() == "foo"s);
}

TEST_CASE_METHOD(Fixture, "RETURN const char* from static char array", "[return]")
{
  mstr m;
  REQUIRE_CALL(m, cc_str())
  .RETURN(carr);
  REQUIRE(m.cc_str() == "foo"s);
}

TEST_CASE_METHOD(Fixture, "RETURN const char* from static const char array", "[return]")
{
  mstr m;
  REQUIRE_CALL(m, cc_str())
  .RETURN(ccarr);
  REQUIRE(m.cc_str() == "bar"s);
}

TEST_CASE_METHOD(Fixture, "RETURN char* from char*", "[return]")
{
  mstr m;
  char* s = carr;
  REQUIRE_CALL(m, cc_str())
  .RETURN(s);
  REQUIRE(m.cc_str() == "foo"s);
}

TEST_CASE_METHOD(Fixture, "RETURN char* from char array", "[return]")
{
  mstr m;
  REQUIRE_CALL(m, cc_str())
  .RETURN(carr);
  REQUIRE(m.cc_str() == "foo"s);
}


TEST_CASE_METHOD(Fixture, "RETURN string from string literal", "[return]")
{
  mstr m;
  REQUIRE_CALL(m, str())
  .RETURN("foo");
  REQUIRE(m.str() == "foo");
}

TEST_CASE_METHOD(Fixture, "RETURN string from static char array", "[return]")
{
  mstr m;
  REQUIRE_CALL(m, str())
  .RETURN(carr);
  REQUIRE(m.str() == "foo");
}

TEST_CASE_METHOD(Fixture, "RETURN string from static const char array", "[return]")
{
  mstr m;
  REQUIRE_CALL(m, str())
  .RETURN(ccarr);
  REQUIRE(m.str() == "bar"s);
}

TEST_CASE_METHOD(Fixture, "RETURN const string from string literal", "[return]")
{
  mstr m;
  REQUIRE_CALL(m, cstr())
  .RETURN("foo");
  REQUIRE(m.cstr() == "foo");
}

TEST_CASE_METHOD(Fixture, "RETURN const string from static char array", "[return]")
{
  mstr m;
  REQUIRE_CALL(m, cstr())
  .RETURN(carr);
  REQUIRE(m.cstr() == "foo");
}

TEST_CASE_METHOD(Fixture, "RETURN const string from static const char array", "[return]")
{
  mstr m;
  REQUIRE_CALL(m, cstr())
  .RETURN(ccarr);
  REQUIRE(m.cstr() == "bar"s);
}

// THROW and LR_THROW tests

TEST_CASE_METHOD(Fixture, "RETURN ref param returns object given", "[return values]")
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

TEST_CASE_METHOD(Fixture, "THROW access copy of local object", "[return values]")
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

TEST_CASE_METHOD(Fixture, "LR_THROW access actual local object", "[return values]")
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

TEST_CASE_METHOD(Fixture, "THROW throws after side effect when replacing return for non void functions", "[return values]")
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

TEST_CASE_METHOD(Fixture, "THROW throws after side effect in void functions", "[return values]")
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

TEST_CASE_METHOD(Fixture, "WITH matches copy of local object", "[matching]")
{
  {
    mock_c obj;
    int n = 1;
    REQUIRE_CALL(obj, getter(ANY(int)))
      .WITH(_1 == n)
      .RETURN(_1);
    n = 2;
    obj.getter(1);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(Fixture, "LR_WITH access actual local object", "[matching]")
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

TEST_CASE_METHOD(Fixture, "rvalue reference parameter can be compared with nullptr in WITH", "[matching]")
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

TEST_CASE_METHOD(Fixture, "rvalue reference parameter can be compared with external value on WITH", "[matching]")
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

class U
{
public:
  MAKE_MOCK1(func_streamref, void(std::ostream&));
  MAKE_MOCK1(func_u, void(const uncomparable&));
  MAKE_MOCK1(func_v, void(int));
  MAKE_MOCK1(func_cv, void(const int));
  MAKE_MOCK1(func_lr, void(int&));
  MAKE_MOCK1(func_clr, void(const int&));
  MAKE_MOCK1(func_rr, void(int&&));
  MAKE_MOCK1(func_crr, void(const int&&));
  MAKE_MOCK1(func_uniqv, void(std::unique_ptr<int>));
  MAKE_MOCK1(func_sharedv, void(std::shared_ptr<int>));
  MAKE_MOCK1(func, void(int&));
  MAKE_MOCK1(func, void(const int&));
  MAKE_MOCK1(func, void(int&&));
  MAKE_MOCK1(func_cstr, void(const char*));
};

// tests of direct parameter matching with fixed values and wildcards

TEST_CASE_METHOD(Fixture, "ostream& matches wildcard", "[matching]")
{
  {
    U u;
    REQUIRE_CALL(u, func_streamref(_));
    u.func_streamref(std::cout);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(Fixture, "uncomparable parameter matches wildcard", "[matching]")
{
  {
    U u;
    REQUIRE_CALL(u, func_u(_));
    u.func_u(uncomparable{});
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(Fixture, "uncomparable parameter matches typed wildcard", "[matching]")
{
  {
    U u;
    REQUIRE_CALL(u, func_u(ANY(uncomparable)));
    u.func_u(uncomparable{});
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(Fixture, "wildcard matches parameter value type", "[matching]")
{
  {
    U u;
    REQUIRE_CALL(u, func_v(_));
    u.func_v(1);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(Fixture, "wildcard matches parameter const value type", "[matching]")
{
  {
    U u;
    REQUIRE_CALL(u, func_cv(_));
    u.func_cv(1);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(Fixture, "wildcard matches unique_ptr<> value type", "[matching]")
{
  {
    U u;
    REQUIRE_CALL(u, func_uniqv(_));
    u.func_uniqv(std::make_unique<int>(3));
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(Fixture, "wildcard matches shared_ptr<> value type", "[matching]")
{
  {
    U u;
    REQUIRE_CALL(u, func_sharedv(_));
    u.func_sharedv(std::make_shared<int>(3));
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(Fixture, "wildcard matches parameter lvalue reference type", "[matching]")
{
  {
    U u;
    REQUIRE_CALL(u, func_lr(_));
    int v = 1;
    u.func_lr(v);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(Fixture, "wildcard matches parameter const lvalue reference type", "[matching]")
{
  {
    U u;
    REQUIRE_CALL(u, func_clr(_));
    int v = 1;
    u.func_clr(v);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(Fixture, "wildcard matches parameter rvalue reference type", "[matching]")
{
  {
    U u;
    REQUIRE_CALL(u, func_rr(_));
    u.func_rr(1);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(Fixture, "wildcard matches parameter const rvalue reference type", "[matching]")
{
  {
    U u;
    REQUIRE_CALL(u, func_crr(_));
    u.func_crr(1);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(Fixture, "ANY can match unique_ptr<> by value", "[matching]")
{
  {
    U u;
    REQUIRE_CALL(u, func_uniqv(ANY(std::unique_ptr<int>)));
    u.func_uniqv(std::make_unique<int>(3));
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(Fixture, "ANY can match shared_ptr<> by value", "[matching]")
{
  {
    U u;
    REQUIRE_CALL(u, func_sharedv(ANY(std::shared_ptr<int>)));
    u.func_sharedv(std::make_shared<int>(3));
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(Fixture, "ANY can select overload on lvalue reference type", "[matching]")
{
  {
    U u;
    REQUIRE_CALL(u, func(ANY(int&)));
    int i = 1;
    u.func(i);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(Fixture, "ANY can select overload on const lvalue reference type", "[matching]")
{
  {
    U u;
    REQUIRE_CALL(u, func(ANY(const int&)));
    const int i = 1;
    u.func(i);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(Fixture, "ANY can select overload on rvalue reference type", "[matching]")
{
  {
    U u;
    REQUIRE_CALL(u, func(ANY(int&&)));
    u.func(1);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(Fixture, "Expectation matches a mocked function with param from template", "[matching][templates]")
{
  {
    tmock<int> m;
    REQUIRE_CALL(m, tfunc(_));
    m.tfunc(3);
  }
  REQUIRE(reports.empty());
}

// tests of overload selection with parameter matching

TEST_CASE_METHOD(Fixture, "wildcards matches overload on type and parameter count", "[matching][overloads]")
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

TEST_CASE_METHOD(Fixture, "long value matches equal int for duck typed eq", "[matching][matchers][eq]")
{
  {
    U obj;
    REQUIRE_CALL(obj, func_v(trompeloeil::eq(3L)));
    obj.func_v(3);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(Fixture, "long value with mismatches int is reported for duck typed eq", "[matching][matchers][eq]")
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
  param  _1 = 0

Tried obj\.func_v\(trompeloeil::eq\(3L\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 == 3):";
    REQUIRE(std::regex_search(msg, std::regex(re)));
  }
}

TEST_CASE_METHOD(Fixture, "std::string value matches equal const char* for duck typed eq", "[matching][matchers][eq]")
{
  {
    U obj;
    REQUIRE_CALL(obj, func_cstr(trompeloeil::eq("foo"s)));
    obj.func_cstr("foo");
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(Fixture, "std::string value mismatching const char* is reported for duck typed eq", "[matching][matchers][eq]")
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
  param  _1 = bar

Tried obj\.func_cstr\(trompeloeil::eq\(\"foo\"s\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 == foo):";
    REQUIRE(std::regex_search(msg, std::regex(re)));
  }
}

// tests of parameter matching using explicitly typed matcher eq

TEST_CASE_METHOD(Fixture, "disambiguated eq<int&> matches equal value", "[matching][matchers][eq]")
{
  {
    U obj;
    REQUIRE_CALL(obj, func(trompeloeil::eq<int&>(3)));
    int i = 3;
    obj.func(i);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(Fixture, "disambiguated eq<int&> reports mismatching value", "[matching][matchers][eq]")
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
  param  _1 = 0

Tried obj\.func\(trompeloeil::eq<int&>\(3\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 == 3):";
    REQUIRE(std::regex_search(msg, std::regex(re)));
  }
}


// tests of parameter matching using duck typed matcher ne

TEST_CASE_METHOD(Fixture, "long value mismatching equal int for duck typed ne", "[matching][matchers][ne]")
{
  {
    U obj;
    REQUIRE_CALL(obj, func_v(trompeloeil::ne(3L)));
    obj.func_v(0);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(Fixture, "long value with matching int is reported for duck typed ne", "[matching][matchers][ne]")
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
  param  _1 = 3

Tried obj\.func_v\(trompeloeil::ne\(3L\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 != 3):";
    REQUIRE(std::regex_search(msg, std::regex(re)));
  }
}

TEST_CASE_METHOD(Fixture, "std::string value mismatches inequal const char* for duck typed ne", "[matching][matchers][ne]")
{
  {
    U obj;
    REQUIRE_CALL(obj, func_cstr(trompeloeil::ne("foo"s)));
    obj.func_cstr("bar");
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(Fixture, "std::string value matching const char* is reported for duck typed ne", "[matching][matchers][ne]")
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
  param  _1 = foo

Tried obj\.func_cstr\(trompeloeil::ne\(\"foo\"s\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 != foo):";
    REQUIRE(std::regex_search(msg, std::regex(re)));
  }
}

// tests of parameter matching using typed matcher ne

TEST_CASE_METHOD(Fixture, "a non equal value matches ne", "[matching][matchers][ne]")
{
  {
    mock_c obj;
    REQUIRE_CALL(obj, foo(trompeloeil::ne<std::string>("bar")));
    obj.foo("baz");
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(Fixture, "an equal value fails ne with report", "[matching][matchers][ne]")
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
  param  _1 = bar

Tried obj\.foo\(trompeloeil::ne<std::string>\("bar"\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 != bar):";
    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}

struct C_foo1
{
  MAKE_MOCK1(foo, void(int*));
};


TEST_CASE_METHOD(Fixture, "non-nullptr call matches ne(nullptr)", "[matching][matchers][ne]")
{
  {
    C_foo1 obj;
    REQUIRE_CALL(obj, foo(trompeloeil::ne(nullptr)));
    int n;
    obj.foo(&n);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(Fixture, "nullptr call with ne(nullptr) is reported", "[matching][matchers][ne]")
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
  param  _1 = .*

Tried obj\.foo\(trompeloeil::ne\(nullptr\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 != .*):";

    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}


struct C_foo2
{
  MAKE_MOCK1(foo, void(int*));
  MAKE_MOCK1(foo, void(char*));
};

TEST_CASE_METHOD(Fixture, "overloaded non-nullptr call disambiguated with ne<type>(nullptr) is matched", "[matching][matchers][ne]")
{
  {
    C_foo2 obj;
    REQUIRE_CALL(obj, foo(trompeloeil::ne<int*>(nullptr)));
    int i;
    obj.foo(&i);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(Fixture, "overloaded nullptr call disambiguated with ne<type>(nullptr) is reported", "[matching][matchers][ne]")
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
  param  _1 = nullptr

Tried obj\.foo\(trompeloeil::ne<int\*>\(nullptr\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 != .*):";

    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}
//

TEST_CASE_METHOD(Fixture, "overloaded nullptr call disambiguated with eq<type>(nullptr) is matched", "[matching][matchers][ne]")
{
  {
    C_foo2 obj;
    REQUIRE_CALL(obj, foo(trompeloeil::eq<int*>(nullptr)));
    int *i_null = nullptr;
    obj.foo(i_null);
  }
  REQUIRE(reports.empty());
}
TEST_CASE_METHOD(Fixture, "overloaded non-nullptr call disambiguated with eq<type>(nullptr) is reported", "[matching][matchers][ne]")
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
  param  _1 = .*

Tried obj\.foo\(trompeloeil::eq<int\*>\(nullptr\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 == nullptr):";

    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}

//

struct C_foo3
{
  int m;
  MAKE_MOCK1(foo, void(int C_foo3::*));
};


TEST_CASE_METHOD(Fixture, "pointer to member call with ne(nullptr) matched", "[matching][matchers][ne]")
{
  {
    C_foo3 obj;
    REQUIRE_CALL(obj, foo(trompeloeil::ne(nullptr)));
    obj.foo(&C_foo3::m);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(Fixture, "pointer to member ptr call with ne(nullptr) is reported", "[matching][matchers][ne]")
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
  param  _1 = .*

Tried obj\.foo\(trompeloeil::ne\(nullptr\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 != .*):";

    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}

//
TEST_CASE_METHOD(Fixture, "pointer to member ptr call with eq(nullptr) matched", "[matching][matchers][ne]")
{
  {
    C_foo3 obj;
    REQUIRE_CALL(obj, foo(trompeloeil::eq(nullptr)));
    obj.foo(nullptr);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(Fixture, "pointer to member ptr call with eq(nullptr) is reported", "[matching][matchers][ne]")
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
  param  _1 = .*

Tried obj\.foo\(trompeloeil::eq\(nullptr\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 == nullptr):";

    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}
//
// tests of parameter matching using duck typed matcher ge

TEST_CASE_METHOD(Fixture, "an equal value matches ge", "[matching][matchers][ge]")
{
  {
    mock_c obj;
    REQUIRE_CALL(obj, getter(trompeloeil::ge(3)))
      .RETURN(0);
    obj.getter(3);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(Fixture, "a greater value matches ge", "[matching][matchers][ge]")
{
  {
    mock_c obj;
    REQUIRE_CALL(obj, getter(trompeloeil::ge(3)))
      .RETURN(0);
    obj.getter(4);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(Fixture, "a lesser value is reported by ge", "[matching][matchers][ge]")
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
  param  _1 = 2

Tried obj\.getter\(trompeloeil::ge\(3\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 >= 3):";
    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}

// tests of parameter matching using typed matcher ge

TEST_CASE_METHOD(Fixture, "an equal value matches disambiguated ge<int&>", "[matching][matchers][ge]")
{
  {
    U obj;
    REQUIRE_CALL(obj, func(trompeloeil::ge<int&>(3)));
    int i = 3;
    obj.func(i);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(Fixture, "a greater value matches disambiguated ge<int&>", "[matching][matchers][ge]")
{
  {
    U obj;
    REQUIRE_CALL(obj, func(trompeloeil::ge<int&>(3)));
    int i = 4;
    obj.func(i);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(Fixture, "a lesser value is reported by disambiguated ge<int&>", "[matching][matchers][ge]")
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
  param  _1 = 2

Tried obj\.func\(trompeloeil::ge<int&>\(3\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 >= 3):";
    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}

// tests of parameter matching using duck typed matcher gt

TEST_CASE_METHOD(Fixture, "an equal value is reported by gt", "[matching][matchers][gt]")
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
  param  _1 = 3

Tried obj\.getter\(trompeloeil::gt\(3\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 > 3):";
    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}

TEST_CASE_METHOD(Fixture, "a greater value matches gt", "[matching][matchers][gt]")
{
  {
    mock_c obj;
    REQUIRE_CALL(obj, getter(trompeloeil::gt(3)))
      .RETURN(0);
    obj.getter(4);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(Fixture, "a lesser value is reported by gt", "[matching][matchers][gt]")
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
  param  _1 = 2

Tried obj.getter\(trompeloeil::gt\(3\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 > 3):";
    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}

// tests of parameter matching using typed matcher gt

TEST_CASE_METHOD(Fixture, "an equal value is reported by disambiguated gt<int&>", "[matching][matchers][gt]")
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
  param  _1 = 3

Tried obj\.func\(trompeloeil::gt<int&>\(3\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 > 3):";
    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}

TEST_CASE_METHOD(Fixture, "a greater value matches disambiguated gt<int&>", "[matching][matchers][gt]")
{
  {
    U obj;
    REQUIRE_CALL(obj, func(trompeloeil::gt<int&>(3)));
    int i = 4;
    obj.func(i);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(Fixture, "a lesser value is reported by disambiguated gt<int&>", "[matching][matchers][gt]")
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
  param  _1 = 2

Tried obj.func\(trompeloeil::gt<int&>\(3\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 > 3):";
    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}

// tests of parameter matching using duck typed matcher lt

TEST_CASE_METHOD(Fixture, "an equal value is reported by lt", "[matching][matchers][lt]")
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
  param  _1 = 3

Tried obj\.getter\(trompeloeil::lt\(3\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 < 3):";
    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}

TEST_CASE_METHOD(Fixture, "a greater value is reported by lt", "[matching][matchers][lt]")
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
  param  _1 = 4

Tried obj\.getter\(trompeloeil::lt\(3\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 < 3):";
    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}

TEST_CASE_METHOD(Fixture, "a lesser value matches lt", "[matching][matchers][lt]")
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

TEST_CASE_METHOD(Fixture, "an equal value is reported by disambiguated lt<int&>", "[matching][matchers][lt]")
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
  param  _1 = 3

Tried obj\.func\(trompeloeil::lt<int&>\(3\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 < 3):";
    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}

TEST_CASE_METHOD(Fixture, "a greater value is reported by disambiguated lt<int&>", "[matching][matchers][lt]")
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
  param  _1 = 4

Tried obj\.func\(trompeloeil::lt<int&>\(3\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 < 3):";
    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}

TEST_CASE_METHOD(Fixture, "a lesser value matches disambiguated lt<int&>", "[matching][matchers][lt]")
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

TEST_CASE_METHOD(Fixture, "an equal value matches le", "[matching][matchers][le]")
{
  {
    mock_c obj;
    REQUIRE_CALL(obj, getter(trompeloeil::le(3)))
      .RETURN(0);
    obj.getter(3);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(Fixture, "a greater value is reported by le", "[matching][matchers][le]")
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
  param  _1 = 4

Tried obj\.getter\(trompeloeil::le\(3\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 <= 3):";
    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}

TEST_CASE_METHOD(Fixture, "a lesser value matches le", "[matching][matchers][le]")
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

TEST_CASE_METHOD(Fixture, "an equal value matches disambiguated le<int&>", "[matching][matchers][le]")
{
  {
    U obj;
    REQUIRE_CALL(obj, func(trompeloeil::le<int&>(3)));
    int i = 3;
    obj.func(i);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(Fixture, "a greater value is reported by disambiguated le<int&>", "[matching][matchers][le]")
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
  param  _1 = 4

Tried obj\.func\(trompeloeil::le<int&>\(3\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 <= 3):";
    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}

TEST_CASE_METHOD(Fixture, "a lesser value matches disambiguated le<int&>", "[matching][matchers][le]")
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

class mock_str
{
public:
  MAKE_MOCK1(c_c_str, void(char const*));
  MAKE_MOCK1(c_str, void(char*));
  MAKE_MOCK1(strcref, void(std::string const&));
  MAKE_MOCK1(strref, void(std::string&));
  MAKE_MOCK1(strrref, void(std::string&&));
  MAKE_MOCK1(str, void(std::string));
  MAKE_MOCK1(overload, void(char const*));
  MAKE_MOCK1(overload, void(std::string const&));
};

TEST_CASE_METHOD(Fixture, "call to const c-string function matching regex is not reported", "[matching][matchers][re]")
{
  {
    mock_str obj;
    REQUIRE_CALL(obj, c_c_str(trompeloeil::re("mid")));
    char str[] = "pre mid post";
    obj.c_c_str(str);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(Fixture, "call to const c-string function with nullptr to regex is reported", "[matching][matchers][re]")
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
  param  _1 = nullptr

Tried obj.c_c_str\(trompeloeil::re\("mid"\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 matching regular expression /mid/):";
    REQUIRE(std::regex_search(msg, std::regex(re)));
  }
}

TEST_CASE_METHOD(Fixture, "call to const c-string function with non-matching string to regex is reported", "[matching][matchers][re]")
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
  param  _1 = abcde

Tried obj.c_c_str\(trompeloeil::re\("mid"\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 matching regular expression /mid/):";
    REQUIRE(std::regex_search(msg, std::regex(re)));
  }
}

//

TEST_CASE_METHOD(Fixture, "call to non-const c-string function matching regex is not reported", "[matching][matchers][re]")
{
  {
    mock_str obj;
    REQUIRE_CALL(obj, c_str(trompeloeil::re("mid")));
    char str[] = "pre mid post";
    obj.c_str(str);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(Fixture, "call to non-const c-string function with nullptr to regex is reported", "[matching][matchers][re]")
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
  param  _1 = nullptr

Tried obj.c_str\(trompeloeil::re\("mid"\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 matching regular expression /mid/):";
    REQUIRE(std::regex_search(msg, std::regex(re)));
  }
}

TEST_CASE_METHOD(Fixture, "call to non-const c-string function with non-matching string to regex is reported", "[matching][matchers][re]")
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
  param  _1 = abcde

Tried obj.c_str\(trompeloeil::re\("mid"\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 matching regular expression /mid/):";
    REQUIRE(std::regex_search(msg, std::regex(re)));
  }
}

//@@

TEST_CASE_METHOD(Fixture, "call to const strref function matching regex is not reported", "[matching][matchers][re]")
{
  {
    mock_str obj;
    REQUIRE_CALL(obj, strcref(trompeloeil::re("mid")));
    obj.strcref(std::string("pre mid post"));
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(Fixture, "call to const strref function with non-matching string to regex is reported", "[matching][matchers][re]")
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
  param  _1 = abcde

Tried obj\.strcref\(trompeloeil::re\("mid"\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 matching regular expression /mid/):";
    REQUIRE(std::regex_search(msg, std::regex(re)));
  }
}

//

TEST_CASE_METHOD(Fixture, "call to non-const strref function matching regex is not reported", "[matching][matchers][re]")
{
  {
    mock_str obj;
    REQUIRE_CALL(obj, strref(trompeloeil::re("mid")));
    std::string str = "pre mid post";
    obj.strref(str);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(Fixture, "call to non-const strref function with non-matching string to regex is reported", "[matching][matchers][re]")
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
  param  _1 = abcde

Tried obj.strref\(trompeloeil::re\("mid"\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 matching regular expression /mid/):";
    REQUIRE(std::regex_search(msg, std::regex(re)));
  }
}
//

TEST_CASE_METHOD(Fixture, "call to non-const strrref function matching regex is not reported", "[matching][matchers][re]")
{
  {
    mock_str obj;
    REQUIRE_CALL(obj, strrref(trompeloeil::re("mid")));
    std::string str = "pre mid post";
    obj.strrref(std::move(str));
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(Fixture, "call to non-const strrref function with non-matching string to regex is reported", "[matching][matchers][re]")
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
  param  _1 = abcde

Tried obj.strrref\(trompeloeil::re\("mid"\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 matching regular expression /mid/):";
    REQUIRE(std::regex_search(msg, std::regex(re)));
  }
}
//

TEST_CASE_METHOD(Fixture, "call to str val function matching regex is not reported", "[matching][matchers][re]")
{
  {
    mock_str obj;
    REQUIRE_CALL(obj, str(trompeloeil::re("mid")));
    std::string str = "pre mid post";
    obj.str(std::move(str));
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(Fixture, "call to str val function with non-matching string to regex is reported", "[matching][matchers][re]")
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
  param  _1 = abcde

Tried obj.str\(trompeloeil::re\("mid"\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 matching regular expression /mid/):";
    REQUIRE(std::regex_search(msg, std::regex(re)));
  }
}

TEST_CASE_METHOD(Fixture, "call to mismatching regex of typed overload is reported", "[matching][matchers][re]")
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
  param  _1 = abcde

Tried obj.overload\(trompeloeil::re<char const\*>\("mid"\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 matching regular expression /mid/):";
    REQUIRE(std::regex_search(msg, std::regex(re)));
  }
}

TEST_CASE_METHOD(Fixture, "call to matching regex of typed overload is not reported", "[matching][matchers][re]")
{
  {
    mock_str obj;
    REQUIRE_CALL(obj, overload(trompeloeil::re<std::string const&>("mid")));
    std::string str = "pre mid post";
    obj.overload(str);
  }
  REQUIRE(reports.empty());
}

//

TEST_CASE_METHOD(Fixture, "case insensitive regex matches case mismatch without explicit type", "[matching][matchers][re]")
{
  {
    mock_str obj;
    REQUIRE_CALL(obj, str(trompeloeil::re("MiXeD", std::regex_constants::icase)));
    std::string str = "mIXEd";
    obj.str(str);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(Fixture, "case insensitive regex matches case mismatch overload", "[matching][matchers][re]")
{
  {
    mock_str obj;
    REQUIRE_CALL(obj, overload(trompeloeil::re<std::string const&>("MiXeD", std::regex_constants::icase)));
    std::string str = "mIXEd";
    obj.overload(str);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(Fixture, "not_eol regex mismatching $ without explicit type is reported", "[matching][matchers][re]")
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
  param  _1 = begin end

Tried obj.str\(trompeloeil::re\("end\$", std::regex_constants::match_not_eol\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 matching regular expression /end\$/):";
    REQUIRE(std::regex_search(msg, std::regex(re)));
  }
}

TEST_CASE_METHOD(Fixture, "not_eol regex mismatching $ overload is reported", "[matching][matchers][re]")
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
  param  _1 = begin end

Tried obj.overload\(trompeloeil::re<std::string const&>\("end\$", std::regex_constants::match_not_eol\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 matching regular expression /end\$/):";
    REQUIRE(std::regex_search(msg, std::regex(re)));
  }
}

// tests of parameter matching using ptr deref matcher

class C_ptr
{
public:
  MAKE_MOCK1(ptr, void(int*));
  MAKE_MOCK1(uptr, void(std::unique_ptr<int>));
  MAKE_MOCK1(uptrrr, void(std::unique_ptr<int>&&));
  MAKE_MOCK1(uptrcr, void(std::unique_ptr<int> const&));
  MAKE_MOCK1(strptr, void(std::string*));
  MAKE_MOCK1(pp, void(int**));
  MAKE_MOCK1(overloaded, void(int**));
  MAKE_MOCK1(overloaded, void(std::string*));
  MAKE_MOCK1(coverload, void(int*));
  MAKE_MOCK1(coverload, void(const int*));
};


TEST_CASE_METHOD(Fixture, "ptr to disambiguated equal const value matches deref", "[matching][matchers][eq]")
{
  {
    C_ptr obj;
    REQUIRE_CALL(obj, coverload(*trompeloeil::eq<const int&>(3)));
    const int n = 3;
    obj.coverload(&n);
  }
  REQUIRE(reports.empty());
}


TEST_CASE_METHOD(Fixture, "ptr to equal value matches deref", "[matching][matchers][eq]")
{
  {
    C_ptr obj;
    REQUIRE_CALL(obj, ptr(*trompeloeil::eq(3)));
    int n = 3;
    obj.ptr(&n);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(Fixture, "ptr to equal nullptr matrches deref", "[matching][matchers][eq]")
{
  {
    C_ptr obj;
    REQUIRE_CALL(obj, pp(*trompeloeil::eq(nullptr)));
    int* p = nullptr;
    obj.pp(&p);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(Fixture, "ptr to overloaded ptr matches equal deref","[matching][matchers][eq]")
{
  {
    C_ptr obj;
    REQUIRE_CALL(obj, overloaded(*trompeloeil::eq(nullptr)));
    int* p = nullptr;
    obj.overloaded(&p);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(Fixture, "ptr to overloaded string matches equal deref to string literal", "[matching][matchers][eq]")
{
  {
    C_ptr obj;
    REQUIRE_CALL(obj, overloaded(*trompeloeil::eq<std::string&>("apa"s)));
    std::string s{"apa"};
    obj.overloaded(&s);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(Fixture, "nullptr when equal ptr deref expected is reported", "[matching][matchers][eq]")
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
  param  _1 = nullptr

Tried obj\.ptr\(\*trompeloeil::eq\(3\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected \*_1 == 3):";

    INFO("msg=" << reports.front().msg);
    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}

TEST_CASE_METHOD(Fixture, "non-nullptr when equal nullptr ptr deref expected is reported", "[matching][matchers][eq]")
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
  param  _1 = .*

Tried obj\.pp\(\*trompeloeil::eq\(nullptr\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected \*_1 == nullptr):";

    INFO("msg=" << reports.front().msg);
    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}

TEST_CASE_METHOD(Fixture, "ptr to different value when equal ptr deref expected is reported", "[matching][matchers][eq]")
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
  param  _1 = .*

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
    obj.uptr(std::make_unique<int>(3));
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
  param  _1 = nullptr

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
    obj.uptr(std::make_unique<int>(2));
    FAIL("didn't throw");
  }
  catch (reported)
  {
    REQUIRE(!reports.empty());
    auto re = R":(No match for call of uptr with signature void\(std::unique_ptr<int>\) with\.
  param  _1 = .*

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
    obj.uptr(std::make_unique<int>(3));
  }
  REQUIRE(reports.empty());
}
//////

TEST_CASE_METHOD(Fixture, "unique_ptr rvalue ref to equal value matches deref", "[matching][matchers][eq]")
{
  {
    C_ptr obj;
    REQUIRE_CALL(obj, uptrrr(*trompeloeil::eq(3)));
    obj.uptrrr(std::make_unique<int>(3));
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
  param  _1 = nullptr

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
    obj.uptrrr(std::make_unique<int>(2));
    FAIL("didn't throw");
  }
  catch (reported)
  {
    REQUIRE(!reports.empty());
    auto re = R":(No match for call of uptrrr with signature void\(std::unique_ptr<int>&&\) with\.
  param  _1 = .*

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
    obj.uptrrr(std::make_unique<int>(3));
  }
  REQUIRE(reports.empty());
}

////
TEST_CASE_METHOD(Fixture, "unique_ptr const lvalue ref to equal value matches deref", "[matching][matchers][eq]")
{
  {
    C_ptr obj;
    REQUIRE_CALL(obj, uptrcr(*trompeloeil::eq(3)));
    obj.uptrcr(std::make_unique<int>(3));
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
  param  _1 = nullptr

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
    obj.uptrcr(std::make_unique<int>(2));
    FAIL("didn't throw");
  }
  catch (reported)
  {
    REQUIRE(!reports.empty());
    auto re = R":(No match for call of uptrcr with signature void\(std::unique_ptr<int> const&\) with\.
  param  _1 = .*

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
    obj.uptrcr(std::make_unique<int>(3));
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


TEST_CASE_METHOD(Fixture, "call to string* function matching regex is not reported", "[matching][matchers][re]")
{
  {
    C_ptr obj;
    REQUIRE_CALL(obj, strptr(*trompeloeil::re("end$")));
    std::string s = "begin end";
    obj.strptr(&s);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(Fixture, "call to string* function not matching regex is reported", "[matching][matchers][re]")
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
  param  _1 = .*

Tried obj\.strptr\(\*trompeloeil::re\("end\$"\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected \*_1 matching regular expression /end\$/):";
    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}

// tests of parameter matching using custom typed matcher

template <typename T>
class any_of_t : public trompeloeil::typed_matcher<T>
{
public:
  any_of_t(std::initializer_list<T> elements) : alternatives(std::begin(elements), std::end(elements)) {}
  bool matches(T const& t) const
  {
    return std::any_of(std::begin(alternatives), std::end(alternatives),
                       [&](T val) { return t == val; });
  }
  friend std::ostream& operator<<(std::ostream& os, any_of_t<T> const& t)
  {
    os << " matching any_of({";
    char const* prefix = " ";
    for (auto& n : t.alternatives)
    {
      os << prefix << n;
      prefix = ", ";
    }
    return os << " })";
  }
private:
  std::vector<T> alternatives;
};

template <typename T>
auto any_of(std::initializer_list<T> elements)
{
  return any_of_t<T>(elements);
}

TEST_CASE_METHOD(Fixture, "custom matcher of first element is not reported", "[matching][matchers][custom]")
{
  {
    mock_c obj;
    REQUIRE_CALL(obj, getter(any_of({ 1, 5, 77 })))
      .RETURN(0);
    obj.getter(1);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(Fixture, "custom matcher of last element is not reported", "[matching][matchers][custom]")
{
  {
    mock_c obj;
    REQUIRE_CALL(obj, getter(any_of({ 1, 5, 77 })))
      .RETURN(0);
    obj.getter(77);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(Fixture, "custom matcher of mid element is not reported", "[matching][matchers][custom]")
{
  {
    mock_c obj;
    REQUIRE_CALL(obj, getter(any_of({ 1, 5, 77 })))
      .RETURN(0);
    obj.getter(5);
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(Fixture, "custom matcher of unlisted element is reported", "[matching][matchers][custom]")
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
  param  _1 = 4

Tried obj\.getter\(any_of\(\{ 1,5,77 \}\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 matching any_of\(\{ 1, 5, 77 \}\)):";
    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}

TEST_CASE_METHOD(Fixture, "custom matcher can math pointer via *deref", "[matching][matchers][custom]")
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

template <typename T>
class has_empty
{
  struct no;
  static no func(...);
  template <typename U>
  static auto func(U const* u) -> decltype(u->empty());
public:
  static const bool value = !std::is_same<no, decltype(func(std::declval<T*>()))>::value;
};

class not_empty : public trompeloeil::matcher
{
public:
  template <typename T, typename = typename std::enable_if<has_empty<T>::value>::type>
  operator T() const;
  template <typename T>
  bool matches(T const& t) const
  {
    return !t.empty();
  }
  friend std::ostream& operator<<(std::ostream& os, not_empty const&)
  {
    return os << " is not empty";
  }
};

TEST_CASE_METHOD(Fixture, "a non empty string gives no report", "[matching][matchers][custom]")
{
  {
    mock_c obj;
    REQUIRE_CALL(obj, foo(not_empty{}));
    obj.foo("bar");
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(Fixture, "an empty string is reported", "[matching][matchers][custom]")
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
  param  _1 = 

Tried obj\.foo\(not_empty\{\}\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 is not empty):";
    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}

// tests of parameter values ostream insertion

struct unknown {
  const char values[4] = { 0x10, 0x11, 0x12, 0x13 };
};

TEST_CASE_METHOD(Fixture, "unknown type is printed as hex dump", "[streaming]")
{
  std::ostringstream os;
  trompeloeil::print(os, unknown{});
  REQUIRE(os.str() == "4-byte object={ 0x10 0x11 0x12 0x13 }");
}

TEST_CASE_METHOD(Fixture, "print after unknown has flags back to default", "[streaming]")
{
  std::ostringstream os;
  trompeloeil::print(os, unknown{});
  int16_t u = 10000;
  os << u;
  REQUIRE(os.str() == "4-byte object={ 0x10 0x11 0x12 0x13 }10000");
}

TEST_CASE_METHOD(Fixture, "previous formatting is ignored before hexdump and then reset", "[streaming]")
{
  std::ostringstream os;
  os << std::oct << std::setfill('_') << std::setw(4) << std::left;
  trompeloeil::print(os, unknown{});
  os << 8;
  REQUIRE(os.str() == "4-byte object={ 0x10 0x11 0x12 0x13 }10__");
}

TEST_CASE_METHOD(Fixture, "call predefined ostream operator with defaulted flags and then reset", "[streaming]")
{
  std::ostringstream os;
  os << std::oct << std::setfill('_') << std::setw(4) << std::left;
  trompeloeil::print(os, 25);
  os << 8;
  REQUIRE(os.str() == "2510__");
}

TEST_CASE_METHOD(Fixture, "large unknown is multi row hex dump", "[streaming]")
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

TEST_CASE_METHOD(Fixture, "unknown object is one line if 8 bytes", "[streaming]")
{
  struct big {
    char c[8] = { 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17 };
  };

  std::ostringstream os;
  trompeloeil::print(os, big{});
  REQUIRE(os.str() == "8-byte object={ 0x10 0x11 0x12 0x13 0x14 0x15 0x16 0x17 }");
}

namespace nn
{
struct TestOutput;
void print(std::ostream&, const TestOutput&);

struct TestOutput
{
  int n;
};

void print(std::ostream& os, const TestOutput& p)
{
  os << "nn::print(TestOutput{" << p.n << "}";
}

} // namespace nn

namespace trompeloeil
{
template <>
void print(std::ostream& os, const nn::TestOutput& p)
{
  os << "trompeloeil::print(nn::TestOutput{" << p.n << "})";
}

} // namespace trompeloeil

class TestOutputMock
{
public:
  MAKE_MOCK1(func, void(nn::TestOutput));
};

TEST_CASE_METHOD(Fixture, "failure on parameter of user type is printed with custom print func", "[streaming]")
{
  TestOutputMock m;
  try
  {
    m.func(nn::TestOutput{ 3 });
    FAIL("didn's throw");
  }
  catch (reported)
  {
    REQUIRE(!reports.empty());
    auto re = R":(No match for call of func with signature void\(nn::TestOutput\) with\.
  param  _1 = trompeloeil::print\(nn::TestOutput\{3\}\)):";
    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}
// tests on scoping (lifetime) of expectations

TEST_CASE_METHOD(Fixture, "require calls are matched in reversed order of creation", "[scoping]")
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

TEST_CASE_METHOD(Fixture, "require calls are removed when they go out of scope", "[scoping]")
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

TEST_CASE_METHOD(Fixture, "a pending unsatisfied require call is reported at end of scope", "[scoping]")
{
  mock_c obj;
  {
    REQUIRE_CALL(obj, foo("bar"));
  }
  REQUIRE(reports.size() == 1U);

  auto re = R":(Unfulfilled expectation:
Expected obj\.foo\("bar"\) to be called once, actually never called
  param  _1 = bar):";
  INFO(reports.front().msg);
  REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
}

TEST_CASE_METHOD(Fixture, "Unfulfilled expectation with ANY is reported with expected values all parameters", "[scoping][wildcard]")
{
 {
    mock_c obj;
    REQUIRE_CALL(obj, func(3, ANY(std::string&)));
    std::string s = "foo";
  }

  REQUIRE(reports.size() == 1U);
  auto re = R":(Unfulfilled expectation:
Expected obj\.func\(3, ANY\(std::string&\)\) to be called once, actually never called
  param  _1 = 3
  param  _2 matching ANY\(std::string&\)):";
  auto& msg = reports.front().msg;
  INFO("msg=" << msg);
  REQUIRE(std::regex_search(msg, std::regex(re)));
}

TEST_CASE_METHOD(Fixture, "Unfulfilled expectation with _ is reported with expected values all parameters", "[scoping][wildcard]")
{
 {
    mock_c obj;
    REQUIRE_CALL(obj, func(3, _));
    std::string s = "foo";
  }

  REQUIRE(reports.size() == 1U);
  auto re = R":(Unfulfilled expectation:
Expected obj\.func\(3, _\) to be called once, actually never called
  param  _1 = 3
  param  _2 matching _):";
  auto& msg = reports.front().msg;
  INFO("msg=" << msg);
  REQUIRE(std::regex_search(msg, std::regex(re)));
}

// test of multiplicity retiring expectations, fulfilled or not

TEST_CASE_METHOD(Fixture, "unsatisfied expectation when mock dies is reported", "[scoping][multiplicity]")
{
  auto m = std::make_unique<mock_c>();
  REQUIRE_CALL(*m, count())
    .RETURN(1);
  m.reset();
  REQUIRE(reports.size() == 1U);
  INFO(reports.front().msg);
  auto re = R":(Pending expectation on destroyed mock object:
Expected .*count\(\) to be called once, actually never called):";
  REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
}

TEST_CASE_METHOD(Fixture, "multiple unsatisfied expectation when mock dies are reported in mock definition order", "[scoping][multiplicity]")
{
  auto m = std::make_unique<mock_c>();
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

TEST_CASE_METHOD(Fixture, "active allow call when mock dies is not reported", "[scoping][multiplicity]")
{
  {
    auto m = std::make_unique<mock_c>();
    ALLOW_CALL(*m, count())
      .RETURN(1);
    m.reset();
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(Fixture, "active forbid call when mock dies is not reported", "[scoping][multiplicity]")
{
  {
    auto m = std::make_unique<mock_c>();
    FORBID_CALL(*m, count());
    m.reset();
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(Fixture, "saturated expectation when mock dies is not reported", "[scoping]")
{
  {
    auto m = std::make_unique<mock_c>();
    REQUIRE_CALL(*m, count())
      .RETURN(1);
    m->count();
    m.reset();
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(Fixture, "no calls reported as never called", "[scoping][multiplicity]")
{
  mock_c obj;
  {
    REQUIRE_CALL(obj, count())
      .RETURN(1);
  }
  REQUIRE(reports.size() == 1U);
  REQUIRE(std::regex_search(reports.front().msg, std::regex("actually never called")));
}

TEST_CASE_METHOD(Fixture, "undersatisfied with one call reported as once", "[scoping][multiplicity]")
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

TEST_CASE_METHOD(Fixture, "undersatisfied with two call reported as count", "[scoping][multiplicity]")
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

TEST_CASE_METHOD(Fixture, "no calls when one required reported as expected once", "[scoping][multiplicity]")
{
  mock_c obj;
  {
    REQUIRE_CALL(obj, count())
      .RETURN(1);
  }
  REQUIRE(reports.size() == 1U);
  REQUIRE(std::regex_search(reports.front().msg, std::regex("to be called once")));
}

TEST_CASE_METHOD(Fixture, "no calls when two required reported as expected 2 times", "[scoping][multiplicity]")
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

TEST_CASE_METHOD(Fixture, "TIMES works for templated mock classes", "[multiplicity][templates]")
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

TEST_CASE_METHOD(Fixture, "an unexpected destruction of monitored object is reported", "[deatwatched]")
{
  {
    trompeloeil::deathwatched<mock_c> obj;
  }
  REQUIRE(reports.size() == 1U);
  REQUIRE(std::regex_search(reports.front().msg, std::regex("Unexpected destruction of.*@")));
}

TEST_CASE_METHOD(Fixture, "an expected destruction of monitored object is not reported", "[deatwatched]")
{
  {
    auto obj = new trompeloeil::deathwatched<mock_c>;
    REQUIRE_DESTRUCTION(*obj);
    delete obj;
  }
  REQUIRE(reports.empty());
}

class none
{
public:
  none() {}
  none(none const&) {}
  virtual ~none() {}
};

TEST_CASE_METHOD(Fixture, "a copy of a deathwatched object with expectation is not expected to die", "[deatwatched]")
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


TEST_CASE_METHOD(Fixture, "a deathwatched object move constructed from original with expectation is not expected to die and the original still is", "[deatwatched]")
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

TEST_CASE_METHOD(Fixture, "object alive when destruction expectation goes out of scope is reported", "[deatwatched]")
{
  trompeloeil::deathwatched<mock_c> obj;
  {
    std::unique_ptr<trompeloeil::expectation> p = NAMED_REQUIRE_DESTRUCTION(obj);
  }
  REQUIRE(reports.size() == 1U);
  REQUIRE(std::regex_search(reports.front().msg, std::regex("Object obj is still alive")));
}

TEST_CASE_METHOD(Fixture, "require destruction succeeds also without deathwatch", "[deatwatched]")
{
  {
    auto obj = new trompeloeil::deathwatched<mock_c>;
    REQUIRE_DESTRUCTION(*obj);
    delete obj;
  }
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(Fixture, "a deathwatched objects constructor passes params to mock", "[deatwatched]")
{
  auto obj = new trompeloeil::deathwatched<mock_c>{ "apa" };
  REQUIRE(obj->p_ == std::string("apa"));
  REQUIRE_DESTRUCTION(*obj);
  delete obj;
}

TEST_CASE_METHOD(Fixture, "require destruction fulfilled in sequence is not reported", "[deathwatched],[sequences]")
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

TEST_CASE_METHOD(Fixture, "named require destruction fulfilled in sequence is not reported", "[deathwatched],[sequences]")
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

TEST_CASE_METHOD(Fixture, "require destruction fulfilled out of sequence is reported", "[deathwatched],[sequences]")
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
  auto re = R":(Sequence mismatch for sequence "s".*destructor for \*obj at [A-Za-z0-9_ ./:\]*:[0-9]*.*foo"):";
  INFO("msg=" << msg);
  REQUIRE(std::regex_search(msg, std::regex(re)));
}

TEST_CASE_METHOD(Fixture, "sequence mismatch with require destruction first is reported", "[deathwatched],[sequences]")
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
    auto re = R":(Sequence mismatch for sequence "s".*\*obj.foo\("foo"\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*Sequence.* REQUIRE_DESTRUCTION\(\*obj\)):";
    INFO("msg=" << msg);
    REQUIRE(std::regex_search(msg, std::regex(re)));
  }
}

TEST_CASE_METHOD(Fixture, "named require destruction fulfilled out of sequence is reported", "[deathwatched],[sequences]")
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
  auto re = R":(Sequence mismatch for sequence "s".*destructor for \*obj at [A-Za-z0-9_ ./:\]*:[0-9]*.*foo"):";
  INFO("msg=" << msg);
  REQUIRE(std::regex_search(msg, std::regex(re)));
}

TEST_CASE_METHOD(Fixture, "sequence mismatch with named require destruction first is reported", "[deathwatched],[sequences]")
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
    auto re = R":(Sequence mismatch for sequence "s".*\*obj.foo\("foo"\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*Sequence.* NAMED_REQUIRE_DESTRUCTION\(\*obj\)):";
    INFO("msg=" << msg);
    REQUIRE(std::regex_search(msg, std::regex(re)));
  }
}

// tests of calls that do not match any valid expectations

TEST_CASE_METHOD(Fixture, "unmatched call is reported", "[mismatches]")
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
  param  _1 = 7):";
    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}


TEST_CASE_METHOD(Fixture, "match of saturated call is reported", "[mismatches]")
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
  param  _1 = 3

Matches saturated call requirement
  obj\.getter\(3\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*):";
    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}

TEST_CASE_METHOD(Fixture, "a matching call that throws is saturated", "[mismatches]")
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
  param  _1 = 3

Matches saturated call requirement
  obj\.getter\(3\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*):";
    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}

TEST_CASE_METHOD(Fixture, "unmatched call with mismatching requirements is reported", "[mismatches]")
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
  param  _1 = 3

Tried obj\.getter\(5\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 = 5

Tried obj\.getter\(4\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 = 4):";
    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}

TEST_CASE_METHOD(Fixture, "unmatched with wildcard reports failed WITH clauses", "[mismatches]")
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
  param  _1 = 4

Tried obj\.getter\(ANY\(int\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Failed WITH\(_1 < 3\)
  Failed WITH\(_1 > 5\)):";
    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}

TEST_CASE_METHOD(Fixture, "unmatched with wildcard reports only failed WITH clauses", "[mismatches]")
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
  param  _1 = 4

Tried obj\.getter\(ANY\(int\)\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Failed WITH\(_1 < 3\)):";
    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}

TEST_CASE_METHOD(Fixture, "match of forbidden call is reported", "[mismatches]")
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
  param  _1 = 3):";
    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}

TEST_CASE_METHOD(Fixture, "Mismatched call to a mocked function with param from template is reported", "[mismatches][templates]")
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
  param  _1 = 2

Tried obj\.tfunc\(3\) at [A-Za-z0-9_ ./:\]*:[0-9]*.*
  Expected  _1 = 3):";
    REQUIRE(std::regex_search(reports.front().msg, std::regex(re)));
  }
}

// tests of parameter passing to expectations

class T
{
public:
  MAKE_MOCK15(concats, std::string(int, int, int, int,
                                   int, int, int, int,
                                   int, int, int, int,
                                   int, int, int));
  MAKE_MOCK1(ptr, void(std::shared_ptr<int>));
  MAKE_MOCK1(ptr, void(std::unique_ptr<int>));
};

TEST_CASE_METHOD(Fixture, "parameters are passed in correct order when matching", "[parameters]")
{
  T obj;
  REQUIRE_CALL(obj, concats(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15))
    .RETURN("");
  auto s = obj.concats(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
  REQUIRE(s == "");
}

TEST_CASE_METHOD(Fixture, "parameters are passed in correct order to WITH", "[parameters]")
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

TEST_CASE_METHOD(Fixture, "parameters are passed in correct order to LR_SIDE_EFFECT", "[parameters]")
{
  T obj;
  int n = 0;
  REQUIRE_CALL(obj, concats(_, _, _, _, _, _, _, _, _, _, _, _, _, _, _))
    .LR_SIDE_EFFECT(n = _1 + _2 - _3 + _4 - _5 + _6 - _7 + _8 - _9 + _10 - _11 + _12 - _13 + _14 - _15)
    .RETURN("");
  auto s = obj.concats(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
  REQUIRE(n == -6);
}

TEST_CASE_METHOD(Fixture, "parameters are passed in correct order to RETURN", "[parameters]")
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

TEST_CASE_METHOD(Fixture, "parameters are passed in correct order to THROW", "[parameters]")
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

TEST_CASE_METHOD(Fixture, "shared ptr by value in expectation is copied", "[parameters]")
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

TEST_CASE_METHOD(Fixture, "shared ptr by std ref in expectation is not copied", "[parameters]")
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

TEST_CASE_METHOD(Fixture, "unique ptr by value is matched with raw ptr in WITH", "[parameters]")
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

TEST_CASE_METHOD(Fixture, "matching calls are traced", "[tracing]")
{
  std::ostringstream os;
  trompeloeil::stream_tracer logger(os);
  mock_c obj1;
  mock_c obj2;
  REQUIRE_CALL(obj1, getter(_, _));
  REQUIRE_CALL(obj2, foo("bar"));
  std::string s = "foo";
  obj1.getter(3, s);
  obj2.foo("bar");
  auto re =
         R":([A-Za-z0-9_ ./:\]*:[0-9]*.*
obj1\.getter\(_, _\) with.
  param  _1 = 3
  param  _2 = foo

[A-Za-z0-9_ ./:\]*:[0-9]*.*
obj2\.foo\("bar"\) with\.
  param  _1 = bar
):";
  REQUIRE(std::regex_search(os.str(), std::regex(re)));
}

TEST_CASE_METHOD(Fixture, "tracing is only active when tracer obj is alive", "[tracing]")
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
  param  _1 = bar
):";
  REQUIRE(std::regex_search(os.str(), std::regex(re)));
}

template <int N>
struct I
{
  I(int i_) : i{i_} {}
  operator int() const { return i; }
  int i;
};

struct all_if
{
  virtual ~all_if() = default;
  virtual void f0() = 0;
  virtual void f1(I<1>) = 0;
  virtual void f2(I<1>, I<2>) = 0;
  virtual void f3(I<1>, I<2>, I<3>) = 0;
  virtual void f4(I<1>, I<2>, I<3>, I<4>) = 0;
  virtual void f5(I<1>, I<2>, I<3>, I<4>, I<5>) = 0;
  virtual void f6(I<1>, I<2>, I<3>, I<4>, I<5>, I<6>) = 0;
  virtual void f7(I<1>, I<2>, I<3>, I<4>, I<5>, I<6>, I<7>) = 0;
  virtual void f8(I<1>, I<2>, I<3>, I<4>, I<5>, I<6>, I<7>, I<8>) = 0;
  virtual void f9(I<1>, I<2>, I<3>, I<4>, I<5>, I<6>, I<7>, I<8>,
                  I<9>) = 0;
  virtual void f10(I<1>, I<2>, I<3>, I<4>, I<5>, I<6>, I<7>, I<8>,
                   I<9>,I<10>) = 0;
  virtual void f11(I<1>, I<2>, I<3>, I<4>, I<5>, I<6>, I<7>, I<8>,
                   I<9>,I<10>,I<11>) = 0;
  virtual void f12(I<1>, I<2>, I<3>, I<4>, I<5>, I<6>, I<7>, I<8>,
                   I<9>,I<10>,I<11>,I<12>) = 0;
  virtual void f13(I<1>, I<2>, I<3>, I<4>, I<5>, I<6>, I<7>, I<8>,
                   I<9>,I<10>,I<11>,I<12>,I<13>) = 0;
  virtual void f14(I<1>, I<2>, I<3>, I<4>, I<5>, I<6>, I<7>, I<8>,
                   I<9>,I<10>,I<11>,I<12>,I<13>,I<14>) = 0;
  virtual void f15(I<1>, I<2>, I<3>, I<4>, I<5>, I<6>, I<7>, I<8>,
                   I<9>,I<10>,I<11>,I<12>,I<13>,I<14>,I<15>) = 0;

  virtual void cf0() const = 0;
  virtual void cf1(I<1>) const = 0;
  virtual void cf2(I<1>, I<2>) const = 0;
  virtual void cf3(I<1>, I<2>, I<3>) const = 0;
  virtual void cf4(I<1>, I<2>, I<3>, I<4>) const = 0;
  virtual void cf5(I<1>, I<2>, I<3>, I<4>, I<5>) const = 0;
  virtual void cf6(I<1>, I<2>, I<3>, I<4>, I<5>, I<6>) const = 0;
  virtual void cf7(I<1>, I<2>, I<3>, I<4>, I<5>, I<6>, I<7>) const = 0;
  virtual void cf8(I<1>, I<2>, I<3>, I<4>, I<5>, I<6>, I<7>, I<8>) const = 0;
  virtual void cf9(I<1>, I<2>, I<3>, I<4>, I<5>, I<6>, I<7>, I<8>,
                   I<9>) const = 0;
  virtual void cf10(I<1>, I<2>, I<3>, I<4>, I<5>, I<6>, I<7>, I<8>,
                    I<9>,I<10>) const= 0;
  virtual void cf11(I<1>, I<2>, I<3>, I<4>, I<5>, I<6>, I<7>, I<8>,
                    I<9>,I<10>,I<11>) const = 0;
  virtual void cf12(I<1>, I<2>, I<3>, I<4>, I<5>, I<6>, I<7>, I<8>,
                    I<9>,I<10>,I<11>,I<12>) const = 0;
  virtual void cf13(I<1>, I<2>, I<3>, I<4>, I<5>, I<6>, I<7>, I<8>,
                    I<9>,I<10>,I<11>,I<12>,I<13>) const = 0;
  virtual void cf14(I<1>, I<2>, I<3>, I<4>, I<5>, I<6>, I<7>, I<8>,
                    I<9>,I<10>,I<11>,I<12>,I<13>,I<14>) const = 0;
  virtual void cf15(I<1>, I<2>, I<3>, I<4>, I<5>, I<6>, I<7>, I<8>,
                    I<9>,I<10>,I<11>,I<12>,I<13>,I<14>,I<15>) const = 0;
};

struct all_mock_if : public all_if
{
  MAKE_MOCK0(f0, void(), override);
  MAKE_MOCK1(f1, void(I<1>), override);
  MAKE_MOCK2(f2, void(I<1>, I<2>), override);
  MAKE_MOCK3(f3, void(I<1>, I<2>, I<3>), override);
  MAKE_MOCK4(f4, void(I<1>, I<2>, I<3>, I<4>), override);
  MAKE_MOCK5(f5, void(I<1>, I<2>, I<3>, I<4>, I<5>), override);
  MAKE_MOCK6(f6, void(I<1>, I<2>, I<3>, I<4>, I<5>, I<6>), override);
  MAKE_MOCK7(f7, void(I<1>, I<2>, I<3>, I<4>, I<5>, I<6>, I<7>), override);
  MAKE_MOCK8(f8, void(I<1>, I<2>, I<3>, I<4>, I<5>, I<6>, I<7>, I<8>), override);
  MAKE_MOCK9(f9, void(I<1>, I<2>, I<3>, I<4>, I<5>, I<6>, I<7>, I<8>,
                      I<9>), override);
  MAKE_MOCK10(f10, void(I<1>, I<2>, I<3>, I<4>, I<5>, I<6>, I<7>, I<8>,
                        I<9>,I<10>), override);
  MAKE_MOCK11(f11, void(I<1>, I<2>, I<3>, I<4>, I<5>, I<6>, I<7>, I<8>,
                        I<9>,I<10>,I<11>), override);
  MAKE_MOCK12(f12, void(I<1>, I<2>, I<3>, I<4>, I<5>, I<6>, I<7>, I<8>,
                        I<9>,I<10>,I<11>,I<12>), override);
  MAKE_MOCK13(f13, void(I<1>, I<2>, I<3>, I<4>, I<5>, I<6>, I<7>, I<8>,
                        I<9>,I<10>,I<11>,I<12>,I<13>), override);
  MAKE_MOCK14(f14, void(I<1>, I<2>, I<3>, I<4>, I<5>, I<6>, I<7>, I<8>,
                        I<9>,I<10>,I<11>,I<12>,I<13>,I<14>), override);
  MAKE_MOCK15(f15, void(I<1>, I<2>, I<3>, I<4>, I<5>, I<6>, I<7>, I<8>,
                        I<9>,I<10>,I<11>,I<12>,I<13>,I<14>,I<15>), override);

  MAKE_CONST_MOCK0(cf0, void(), override);
  MAKE_CONST_MOCK1(cf1, void(I<1>), override);
  MAKE_CONST_MOCK2(cf2, void(I<1>, I<2>), override);
  MAKE_CONST_MOCK3(cf3, void(I<1>, I<2>, I<3>), override);
  MAKE_CONST_MOCK4(cf4, void(I<1>, I<2>, I<3>, I<4>), override);
  MAKE_CONST_MOCK5(cf5, void(I<1>, I<2>, I<3>, I<4>, I<5>), override);
  MAKE_CONST_MOCK6(cf6, void(I<1>, I<2>, I<3>, I<4>, I<5>, I<6>), override);
  MAKE_CONST_MOCK7(cf7, void(I<1>, I<2>, I<3>, I<4>, I<5>, I<6>, I<7>), override);
  MAKE_CONST_MOCK8(cf8, void(I<1>, I<2>, I<3>, I<4>, I<5>, I<6>, I<7>, I<8>), override);
  MAKE_CONST_MOCK9(cf9, void(I<1>, I<2>, I<3>, I<4>, I<5>, I<6>, I<7>, I<8>,
                             I<9>), override);
  MAKE_CONST_MOCK10(cf10, void(I<1>, I<2>, I<3>, I<4>, I<5>, I<6>, I<7>, I<8>,
                               I<9>,I<10>), override);
  MAKE_CONST_MOCK11(cf11, void(I<1>, I<2>, I<3>, I<4>, I<5>, I<6>, I<7>, I<8>,
                               I<9>,I<10>,I<11>), override);
  MAKE_CONST_MOCK12(cf12, void(I<1>, I<2>, I<3>, I<4>, I<5>, I<6>, I<7>, I<8>,
                               I<9>,I<10>,I<11>,I<12>), override);
  MAKE_CONST_MOCK13(cf13, void(I<1>, I<2>, I<3>, I<4>, I<5>, I<6>, I<7>, I<8>,
                               I<9>,I<10>,I<11>,I<12>,I<13>), override);
  MAKE_CONST_MOCK14(cf14, void(I<1>, I<2>, I<3>, I<4>, I<5>, I<6>, I<7>, I<8>,
                               I<9>,I<10>,I<11>,I<12>,I<13>,I<14>), override);
  MAKE_CONST_MOCK15(cf15, void(I<1>, I<2>, I<3>, I<4>, I<5>, I<6>, I<7>, I<8>,
                               I<9>,I<10>,I<11>,I<12>,I<13>,I<14>,I<15>), override);

};


struct all_mock
{
  MAKE_MOCK0(f0, void());
  MAKE_MOCK1(f1, void(I<1>));
  MAKE_MOCK2(f2, void(I<1>, I<2>));
  MAKE_MOCK3(f3, void(I<1>, I<2>, I<3>));
  MAKE_MOCK4(f4, void(I<1>, I<2>, I<3>, I<4>));
  MAKE_MOCK5(f5, void(I<1>, I<2>, I<3>, I<4>, I<5>));
  MAKE_MOCK6(f6, void(I<1>, I<2>, I<3>, I<4>, I<5>, I<6>));
  MAKE_MOCK7(f7, void(I<1>, I<2>, I<3>, I<4>, I<5>, I<6>, I<7>));
  MAKE_MOCK8(f8, void(I<1>, I<2>, I<3>, I<4>, I<5>, I<6>, I<7>, I<8>));
  MAKE_MOCK9(f9, void(I<1>, I<2>, I<3>, I<4>, I<5>, I<6>, I<7>, I<8>,
                      I<9>));
  MAKE_MOCK10(f10, void(I<1>, I<2>, I<3>, I<4>, I<5>, I<6>, I<7>, I<8>,
                        I<9>,I<10>));
  MAKE_MOCK11(f11, void(I<1>, I<2>, I<3>, I<4>, I<5>, I<6>, I<7>, I<8>,
                        I<9>,I<10>,I<11>));
  MAKE_MOCK12(f12, void(I<1>, I<2>, I<3>, I<4>, I<5>, I<6>, I<7>, I<8>,
                        I<9>,I<10>,I<11>,I<12>));
  MAKE_MOCK13(f13, void(I<1>, I<2>, I<3>, I<4>, I<5>, I<6>, I<7>, I<8>,
                        I<9>,I<10>,I<11>,I<12>,I<13>));
  MAKE_MOCK14(f14, void(I<1>, I<2>, I<3>, I<4>, I<5>, I<6>, I<7>, I<8>,
                        I<9>,I<10>,I<11>,I<12>,I<13>,I<14>));
  MAKE_MOCK15(f15, void(I<1>, I<2>, I<3>, I<4>, I<5>, I<6>, I<7>, I<8>,
                        I<9>,I<10>,I<11>,I<12>,I<13>,I<14>,I<15>));

  MAKE_CONST_MOCK0(cf0, void());
  MAKE_CONST_MOCK1(cf1, void(I<1>));
  MAKE_CONST_MOCK2(cf2, void(I<1>, I<2>));
  MAKE_CONST_MOCK3(cf3, void(I<1>, I<2>, I<3>));
  MAKE_CONST_MOCK4(cf4, void(I<1>, I<2>, I<3>, I<4>));
  MAKE_CONST_MOCK5(cf5, void(I<1>, I<2>, I<3>, I<4>, I<5>));
  MAKE_CONST_MOCK6(cf6, void(I<1>, I<2>, I<3>, I<4>, I<5>, I<6>));
  MAKE_CONST_MOCK7(cf7, void(I<1>, I<2>, I<3>, I<4>, I<5>, I<6>, I<7>));
  MAKE_CONST_MOCK8(cf8, void(I<1>, I<2>, I<3>, I<4>, I<5>, I<6>, I<7>, I<8>));
  MAKE_CONST_MOCK9(cf9, void(I<1>, I<2>, I<3>, I<4>, I<5>, I<6>, I<7>, I<8>,
                             I<9>));
  MAKE_CONST_MOCK10(cf10, void(I<1>, I<2>, I<3>, I<4>, I<5>, I<6>, I<7>, I<8>,
                               I<9>,I<10>));
  MAKE_CONST_MOCK11(cf11, void(I<1>, I<2>, I<3>, I<4>, I<5>, I<6>, I<7>, I<8>,
                               I<9>,I<10>,I<11>));
  MAKE_CONST_MOCK12(cf12, void(I<1>, I<2>, I<3>, I<4>, I<5>, I<6>, I<7>, I<8>,
                               I<9>,I<10>,I<11>,I<12>));
  MAKE_CONST_MOCK13(cf13, void(I<1>, I<2>, I<3>, I<4>, I<5>, I<6>, I<7>, I<8>,
                               I<9>,I<10>,I<11>,I<12>,I<13>));
  MAKE_CONST_MOCK14(cf14, void(I<1>, I<2>, I<3>, I<4>, I<5>, I<6>, I<7>, I<8>,
                               I<9>,I<10>,I<11>,I<12>,I<13>,I<14>));
  MAKE_CONST_MOCK15(cf15, void(I<1>, I<2>, I<3>, I<4>, I<5>, I<6>, I<7>, I<8>,
                               I<9>,I<10>,I<11>,I<12>,I<13>,I<14>,I<15>));

};


TEST_CASE("all overridden short mocks can be expected and called", "[signatures],[override]")
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

TEST_CASE("all non-overridden short mocks can be expected and called", "[signatures]")
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

