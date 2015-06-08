/*
 * Trompeloeil C++ mocking framework
 *
 * Copyright Björn Fahller 2014,2015
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
#include <crpcut.hpp>

#include <vector>
#include <string>
#include <algorithm>
class reported {};

struct report
{
  trompeloeil::severity s;
  const char* file;
  unsigned line;
  std::string           msg;
};
static std::vector<report> reports;

static void send_report(trompeloeil::severity s, const char* file, unsigned line, const std::string msg)
{
#if 0
  std::ostringstream os;
  os << file << ':' << line;
  auto loc = os.str();
  auto location = line == 0U
    ? ::crpcut::crpcut_test_monitor::current_test()->get_location()
    : ::crpcut::datatypes::fixed_string::make(loc.c_str(), loc.length());
  ::crpcut::comm::report(::crpcut::comm::exit_fail,
                         std::ostringstream(msg),
                         location);
#else
  reports.push_back(report{s, file, line, msg});
  if (s == trompeloeil::severity::fatal && !std::uncaught_exception())
  {
    throw reported{};
  }
#endif
}

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
  C(const char* s) : p_{s} {}
  const char *p_ = nullptr;
};

class mock_c : public C
{
 public:
  mock_c() {}
  mock_c(int i) : C(i) {}
  mock_c(const char* p) : C(p) {}
  MAKE_MOCK1(ptr,  std::unique_ptr<int>(std::unique_ptr<int>&&));
  MAKE_MOCK0(count,  int());
  MAKE_MOCK1(foo, void(std::string));
  MAKE_MOCK2(func,   void(int, std::string&));
  MAKE_MOCK1(getter, unmovable&(unmovable&));
  MAKE_MOCK1(getter, int(int));
  MAKE_MOCK2(getter, void(int, std::string&));
  using C::p_;
};


using trompeloeil::_;

TESTSUITE(sequences)
{

  TEST(follow_single_sequence_gives_no_reports)
  {
    {
      mock_c obj1(1), obj2("apa");

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
      obj2.func(3, str);
      obj2.count();
    }
    ASSERT_TRUE(reports.empty());
  }

  TEST(join_two_sequences_gives_no_report)
  {
    {
      mock_c obj1, obj2;

      trompeloeil::sequence seq1, seq2;

      REQUIRE_CALL(obj1, count())
        .IN_SEQUENCE(seq1)
        .RETURN(1);

      REQUIRE_CALL(obj2, func(_,_))
        .IN_SEQUENCE(seq2);

      REQUIRE_CALL(obj2, count())
        .IN_SEQUENCE(seq2, seq1)
        .RETURN(3);

      std::string str = "apa";
      obj2.func(3, str);
      obj1.count();
      obj2.count();
    }
    ASSERT_TRUE(reports.empty());
  }

  TEST(violating_single_sequence_reports_first_violation_as_fatal)
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
      FAIL << "didn't throw!";
    }
    catch (reported)
    {
      ASSERT_TRUE(!reports.empty());
      ASSERT_TRUE(reports.front().msg =~ crpcut::regex("Sequence mismatch.*\"seq\".*matching.*obj2.count().*has obj2.func(_,_) at.*.*first"));
    }
  }

  TEST(violating_parallel_sequences_reports_first_violation_as_fatal)
  {
    try {
      mock_c obj1, obj2;

      trompeloeil::sequence seq1, seq2;

      REQUIRE_CALL(obj1, count())
        .IN_SEQUENCE(seq1)
        .RETURN(1);

      REQUIRE_CALL(obj2, func(_,_))
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
      FAIL << "didn't throw!";
    }
    catch (reported)
    {
      ASSERT_TRUE(!reports.empty());
      ASSERT_TRUE(reports.front().msg =~ crpcut::regex("Sequence mismatch.*seq2.*of obj2.count().*has obj1.count().*first"));
    }
  }

  TEST(a_sequence_retires_after_min_calls)
  {
    {
      int count = 0;

      mock_c obj1;
      trompeloeil::sequence seq1;

      REQUIRE_CALL(obj1, count())
        .IN_SEQUENCE(seq1)
        .TIMES(AT_LEAST(3))
        .RETURN(1);
      REQUIRE_CALL(obj1, func(_,_))
        .IN_SEQUENCE(seq1);

      count+= obj1.count();
      count+= obj1.count();
      count+= obj1.count();
      std::string s = "apa";
      obj1.func(3, s);
    }
    ASSERT_TRUE(reports.empty());
  }

  TEST(calling_a_sequenced_match_after_seq_retires_is_allowed)
  {
    {
      int count = 0;

      mock_c obj1;
      trompeloeil::sequence seq1;

      REQUIRE_CALL(obj1, count())
        .IN_SEQUENCE(seq1)
        .TIMES(AT_LEAST(3))
        .RETURN(1);
      REQUIRE_CALL(obj1, func(_,_))
        .IN_SEQUENCE(seq1);

      count+= obj1.count();
      count+= obj1.count();
      count+= obj1.count();
      count+= obj1.count();
      count+= obj1.count();
      std::string s = "apa";
      obj1.func(3, s);
    }
    ASSERT_TRUE(reports.empty());
  }

  TEST(breaking_a_sequence_before_retirement_is_illegal)
  {
    int count = 0;

    mock_c obj1;
    trompeloeil::sequence seq1;

    REQUIRE_CALL(obj1, count())
      .IN_SEQUENCE(seq1)
      .TIMES(AT_LEAST(3))
      .RETURN(1);
    REQUIRE_CALL(obj1, func(_,_))
      .IN_SEQUENCE(seq1);

    count+= obj1.count();
    count+= obj1.count();

    try {
      std::string s = "apa";
      obj1.func(3, s);
      FAIL << "didn't throw";
    }
    catch (reported)
    {
      ASSERT_TRUE(reports.size() == 1U);
      ASSERT_TRUE(reports.front().msg =~ crpcut::regex("Sequence mismatch.*seq1.*of obj1.func(_,_).*has obj1.count().*first"));
      auto& first = reports.front();
      INFO << first.file << ':' <<  first.line << "\n" << first.msg;
    }
  }

  TEST(sequences_impose_order_between_multiple_matching_expectations)
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
    s+= std::to_string(obj.getter(1));
    s+= std::to_string(obj.getter(1));
    s+= std::to_string(obj.getter(1));
    ASSERT_TRUE(s == "123");
  }
}

TESTSUITE(side_effects)
{
  static int global_n = 0;
  TEST(side_effect_access_copy_of_local_object)
  {
    int n = 1;
    mock_c obj;
    REQUIRE_CALL(obj, getter(ANY(int)))
      .SIDE_EFFECT(global_n = n)
      .RETURN(_1);
    n = 2;
    obj.getter(n);
    ASSERT_TRUE(global_n == 1);
  }
  TEST(lr_side_effect_access_reference_of_local_object)
  {
    int n = 1;
    mock_c obj;
    REQUIRE_CALL(obj, getter(ANY(int)))
      .LR_SIDE_EFFECT(global_n = n)
      .RETURN(_1);
    n = 2;
    obj.getter(n);
    ASSERT_TRUE(global_n == 2);
  }
  TEST(multiple_side_effects_are_executed_in_the_order_declared)
  {
    std::string s;

    mock_c obj;

    REQUIRE_CALL(obj, getter(ANY(int)))
      .LR_SIDE_EFFECT(s = std::to_string(_1))
      .LR_SIDE_EFFECT(s += "_")
      .LR_SIDE_EFFECT(s += s)
      .RETURN(_1);

    obj.getter(3);

    ASSERT_TRUE(s == "3_3_");
  }
}
TESTSUITE(return_values)
{
  TEST(return_access_copy_of_local_object)
  {
    int n = 1;
    mock_c obj;
    REQUIRE_CALL(obj, getter(ANY(int)))
      .RETURN(n);
    n = 2;
    auto m = obj.getter(n);
    ASSERT_TRUE(m == 1);
  }
  TEST(lr_return_access_copy_of_local_object)
  {
    int n = 1;
    mock_c obj;
    REQUIRE_CALL(obj, getter(ANY(int)))
      .LR_RETURN(n);
    n = 2;
    auto m = obj.getter(n);
    ASSERT_TRUE(m == 2);
  }
  TEST(return_by_reference_returns_object_given)
  {
    {
      mock_c obj;
      unmovable s;
      REQUIRE_CALL(obj, getter(ANY(unmovable&)))
        .LR_RETURN(std::ref(s));

      ASSERT_TRUE(&obj.getter(s) == &s);
    }
    ASSERT_TRUE(reports.empty());
  }

  TEST(return_param_by_reference_returns_object_given)
  {
    {
      mock_c obj;
      unmovable s;
      REQUIRE_CALL(obj, getter(ANY(unmovable&)))
        .RETURN(std::ref(_1));

      ASSERT_TRUE(&obj.getter(s) == &s);
    }
    ASSERT_TRUE(reports.empty());
  }
  TEST(throw_access_copy_of_local_object)
  {
    int n = 1;
    mock_c obj;
    REQUIRE_CALL(obj, getter(ANY(int)))
      .THROW(n);
    n = 2;
    ASSERT_THROW(obj.getter(n), int, [](int m){return m == 1;});
  }
  TEST(lr_throw_access_copy_of_local_object)
  {
    int n = 1;
    mock_c obj;
    REQUIRE_CALL(obj, getter(ANY(int)))
      .LR_THROW(n);
    n = 2;
    ASSERT_THROW(obj.getter(n), int, [](int m){return m == 2;});
  }

  TEST(THROW_throws_after_side_effect_when_replacing_return_for_non_void_functions)
  {
    int thrown = 0;
    int global = 0;
    try {
      mock_c obj;
      REQUIRE_CALL(obj, getter(ANY(int)))
        .THROW(8)
        .LR_SIDE_EFFECT(global = _1);
      obj.getter(8);
      FAIL << "didn't throw";
    }
    catch (int n)
    {
      thrown = n;
    }
    ASSERT_TRUE(thrown == 8);
    ASSERT_TRUE(global == 8);
  }

  TEST(THROW_throws_after_side_effect_in_void_functions)
  {
    int thrown = 0;
    std::string s;
    try {
      mock_c obj;

      REQUIRE_CALL(obj, func(_,_))
        .SIDE_EFFECT(_2 = std::to_string(_1))
        .THROW(8);

      obj.func(8, s);
      FAIL << "didn't throw";
    }
    catch (int n)
    {
      thrown = n;
    }
    ASSERT_TRUE(thrown == 8);
    ASSERT_TRUE(s == "8");
  }
}

TESTSUITE(matching)
{
  TEST(with_matches_copy_of_local_object)
  {
    mock_c obj;
    int n = 1;
    REQUIRE_CALL(obj, getter(ANY(int)))
      .WITH(_1 == n)
      .RETURN(_1);
    n = 2;
    obj.getter(1);
  }
  TEST(lr_with_matches_reference_to_local_object)
  {
    mock_c obj;
    int n = 1;
    REQUIRE_CALL(obj, getter(ANY(int)))
      .LR_WITH(_1 == n)
      .RETURN(_1);
    n = 2;
    obj.getter(2);
  }
  TEST(rvalue_reference_parameter_can_be_compared_with_nullptr)
  {
    {
      mock_c obj;
      REQUIRE_CALL(obj, ptr(_))
        .WITH(_1 != nullptr)
        .RETURN(std::move(_1));

      auto p = obj.ptr(std::unique_ptr<int>(new int{3}));
      ASSERT_TRUE(p);
      ASSERT_TRUE(*p == 3);
    }
    ASSERT_TRUE(reports.empty());
  }

  TEST(rvalue_reference_parameter_can_be_compared_with_external_value)
  {
    {
      mock_c obj;
      auto pi = new int{3};
      REQUIRE_CALL(obj, ptr(_))
        .WITH(_1.get() == pi)
        .RETURN(std::move(_1));

      auto p = obj.ptr(std::unique_ptr<int>(pi));
      ASSERT_TRUE(p);
      ASSERT_TRUE(p.get() == pi);
    }
    ASSERT_TRUE(reports.empty());
  }

  class U
  {
  public:
    MAKE_MOCK1(func_u, void(const uncomparable&));
    MAKE_MOCK1(func_v, void(int));
    MAKE_MOCK1(func_cv, void(const int));
    MAKE_MOCK1(func_lr, void(int&));
    MAKE_MOCK1(func_clr, void(const int&));
    MAKE_MOCK1(func_rr, void(int&&));
    MAKE_MOCK1(func_crr, void(const int&&));
    MAKE_MOCK1(func, void(int&));
    MAKE_MOCK1(func, void(const int&));
    MAKE_MOCK1(func, void(int&&));
  };

  TEST(uncomparable_parameter_matches_wildcard)
  {
    U u;
    REQUIRE_CALL(u, func_u(_));
    u.func_u(uncomparable{});
  }

  TEST(uncomparable_parameter_matches_typed_wildcard)
  {
    U u;
    REQUIRE_CALL(u, func_u(ANY(uncomparable)));
    u.func_u(uncomparable{});
  }

  TEST(wildcard_matches_parameter_value_type)
  {
    U u;
    REQUIRE_CALL(u, func_v(_));
    u.func_v(1);
  }

  TEST(wildcard_matches_parameter_const_value_type)
  {
    U u;
    REQUIRE_CALL(u, func_cv(_));
    u.func_cv(1);
  }

  TEST(wildcard_matches_parameter_lvalue_reference_type)
  {
    U u;
    REQUIRE_CALL(u, func_lr(_));
    int v = 1;
    u.func_lr(v);
  }

  TEST(wildcard_matches_parameter_const_lvalue_reference_type)
  {
    U u;
    REQUIRE_CALL(u, func_clr(_));
    int v = 1;
    u.func_clr(v);
  }

  TEST(wildcard_matches_parameter_rvalue_reference_type)
  {
    U u;
    REQUIRE_CALL(u, func_rr(_));
    u.func_rr(1);
  }

  TEST(wildcard_matches_parameter_const_rvalue_reference_type)
  {
    U u;
    REQUIRE_CALL(u, func_crr(_));
    u.func_crr(1);
  }

  TEST(ANY_can_select_overload_on_lvalue_reference_type)
  {
    U u;
    REQUIRE_CALL(u, func(ANY(int&)));
    int i = 1;
    u.func(i);
  }
  TEST(ANY_can_select_overload_on_const_lvalue_reference_type)
  {
    U u;
    REQUIRE_CALL(u, func(ANY(const int&)));
    const int i = 1;
    u.func(i);
  }
  TEST(ANY_can_select_overload_on_rvalue_reference_type)
  {
    U u;
    REQUIRE_CALL(u, func(ANY(int&&)));
    u.func(1);
  }

  TESTSUITE(matchers)
  {
    TESTSUITE(ne)
    {
      TEST(a_non_equal_value_is_matched)
      {
        mock_c obj;
        REQUIRE_CALL(obj, foo(trompeloeil::ne<std::string>("bar")));
        obj.foo("baz");
      }

      TEST(an_equal_value_fails_with_report)
      {
        try {
          mock_c obj;
          REQUIRE_CALL(obj, foo(trompeloeil::ne<std::string>("bar")));
          obj.foo("bar");
          FAIL << "din't report";
        }
        catch(reported)
        {
          ASSERT_TRUE(reports.size() == 1U);
          auto re = R"_(No match for call of foo with signature void(std::string) with.
  param  _1 = bar

Tried obj.foo(trompeloeil::ne<std::string>("bar")) at [a-z_./]*:[0-9]*
  Expected  _1 != bar)_";
          ASSERT_TRUE(reports.front().msg =~ crpcut::regex(re, crpcut::regex::m));
        }
      }
    }


    TESTSUITE(ge)
    {
      TEST(an_equal_value_is_matched)
      {
        mock_c obj;
        REQUIRE_CALL(obj, getter(trompeloeil::ge(3)))
          .RETURN(0);
        obj.getter(3);
      }

      TEST(a_greater_value_is_matched)
      {
        mock_c obj;
        REQUIRE_CALL(obj, getter(trompeloeil::ge(3)))
          .RETURN(0);
        obj.getter(4);
      }

      TEST(a_lesser_value_is_reported)
      {
        try {
          mock_c obj;
          REQUIRE_CALL(obj, getter(trompeloeil::ge(3)))
            .RETURN(0);
          obj.getter(2);
          FAIL << "din't report";
        }
        catch(reported)
        {
          ASSERT_TRUE(reports.size() == 1U);
          auto re = R"_(No match for call of getter with signature int(int) with.
  param  _1 = 2

Tried obj.getter(trompeloeil::ge(3)) at [a-z_./]*:[0-9]*
  Expected  _1 >= 3)_";
          ASSERT_TRUE(reports.front().msg =~ crpcut::regex(re, crpcut::regex::m));
        }
      }
    }

    TESTSUITE(gt)
    {
      TEST(an_equal_value_is_reported)
      {
        try {
          mock_c obj;
          REQUIRE_CALL(obj, getter(trompeloeil::gt(3)))
            .RETURN(0);
          obj.getter(3);
          FAIL << "din't report";
        }
        catch(reported)
        {
          ASSERT_TRUE(reports.size() == 1U);
          auto re = R"_(No match for call of getter with signature int(int) with.
  param  _1 = 3

Tried obj.getter(trompeloeil::gt(3)) at [a-z_./]*:[0-9]*
  Expected  _1 > 3)_";
          ASSERT_TRUE(reports.front().msg =~ crpcut::regex(re, crpcut::regex::m));
        }
      }

      TEST(a_greater_value_is_matched)
      {
        mock_c obj;
        REQUIRE_CALL(obj, getter(trompeloeil::gt(3)))
          .RETURN(0);
        obj.getter(4);
      }

      TEST(a_lesser_value_is_reported)
      {
        try {
          mock_c obj;
          REQUIRE_CALL(obj, getter(trompeloeil::gt(3)))
            .RETURN(0);
          obj.getter(2);
          FAIL << "din't report";
        }
        catch(reported)
        {
          ASSERT_TRUE(reports.size() == 1U);
          auto re = R"_(No match for call of getter with signature int(int) with.
  param  _1 = 2

Tried obj.getter(trompeloeil::gt(3)) at [a-z_./]*:[0-9]*
  Expected  _1 > 3)_";
          ASSERT_TRUE(reports.front().msg =~ crpcut::regex(re, crpcut::regex::m));
        }
      }
    }

    TESTSUITE(lt)
    {
      TEST(an_equal_value_is_reported)
      {
        try {
          mock_c obj;
          REQUIRE_CALL(obj, getter(trompeloeil::lt(3)))
            .RETURN(0);
          obj.getter(3);
          FAIL << "din't report";
        }
        catch(reported)
        {
          ASSERT_TRUE(reports.size() == 1U);
          auto re = R"_(No match for call of getter with signature int(int) with.
  param  _1 = 3

Tried obj.getter(trompeloeil::lt(3)) at [a-z_./]*:[0-9]*
  Expected  _1 < 3)_";
          ASSERT_TRUE(reports.front().msg =~ crpcut::regex(re, crpcut::regex::m));
        }
      }

      TEST(a_greater_value_is_reported)
      {
        try {
          mock_c obj;
          REQUIRE_CALL(obj, getter(trompeloeil::lt(3)))
            .RETURN(0);
          obj.getter(4);
          FAIL << "din't report";
        }
        catch(reported)
        {
          ASSERT_TRUE(reports.size() == 1U);
          auto re = R"_(No match for call of getter with signature int(int) with.
  param  _1 = 4

Tried obj.getter(trompeloeil::lt(3)) at [a-z_./]*:[0-9]*
  Expected  _1 < 3)_";
          ASSERT_TRUE(reports.front().msg =~ crpcut::regex(re, crpcut::regex::m));
        }
      }

      TEST(a_lesser_value_is_matched)
      {
        mock_c obj;
        REQUIRE_CALL(obj, getter(trompeloeil::lt(3)))
          .RETURN(0);
        obj.getter(2);
      }
    }

    TESTSUITE(le)
    {
      TEST(an_equal_value_is_matched)
      {
        mock_c obj;
        REQUIRE_CALL(obj, getter(trompeloeil::le(3)))
          .RETURN(0);
        obj.getter(3);
      }

      TEST(a_greater_value_is_reported)
      {
        try {
          mock_c obj;
          REQUIRE_CALL(obj, getter(trompeloeil::le(3)))
            .RETURN(0);
          obj.getter(4);
          FAIL << "din't report";
        }
        catch(reported)
        {
          ASSERT_TRUE(reports.size() == 1U);
          auto re = R"_(No match for call of getter with signature int(int) with.
  param  _1 = 4

Tried obj.getter(trompeloeil::le(3)) at [a-z_./]*:[0-9]*
  Expected  _1 <= 3)_";
          ASSERT_TRUE(reports.front().msg =~ crpcut::regex(re, crpcut::regex::m));
        }
      }

      TEST(a_lesser_value_is_matched)
      {
        mock_c obj;
        REQUIRE_CALL(obj, getter(trompeloeil::le(3)))
          .RETURN(0);
        obj.getter(2);
      }
    }
    TESTSUITE(custom)
    {
      template <typename T>
      class any_of_t : public trompeloeil::matcher
      {
      public:
        any_of_t(std::initializer_list<T> elements) : alternatives(std::begin(elements), std::end(elements)) {}
        operator T() const;
        bool matches(T const& t) const
        {
          return std::any_of(std::begin(alternatives), std::end(alternatives),
                             [&](T val) { return t == val; });
        }
        friend std::ostream& operator<<(std::ostream& os, any_of_t<T> const& t)
        {
          os << " matching any_of({";
          char const* prefix=" ";
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
      TEST(custom_matcher_of_first_element_is_not_reported)
      {
        mock_c obj;
        REQUIRE_CALL(obj, getter(any_of({1, 5, 77})))
          .RETURN(0);
        obj.getter(1);
      }
      TEST(custom_matcher_of_last_element_is_not_reported)
      {
        mock_c obj;
        REQUIRE_CALL(obj, getter(any_of({1, 5, 77})))
          .RETURN(0);
        obj.getter(77);
      }
      TEST(custom_matcher_of_mid_element_is_not_reported)
      {
        mock_c obj;
        REQUIRE_CALL(obj, getter(any_of({1, 5, 77})))
          .RETURN(0);
        obj.getter(5);
      }
      TEST(custom_matcher_of_unlisted_element_is_reported)
      {
        try {
          mock_c obj;
          REQUIRE_CALL(obj, getter(any_of({1,5,77})))
            .RETURN(0);
          obj.getter(4);
          FAIL << "din't report";
        }
        catch(reported)
        {
          ASSERT_TRUE(reports.size() == 1U);
          auto re = R"_(No match for call of getter with signature int(int) with.
  param  _1 = 4

Tried obj.getter(any_of({1,5,77})) at [a-z_./]*:[0-9]*
  Expected  _1 matching any_of({ 1, 5, 77 })_";
          ASSERT_TRUE(reports.front().msg =~ crpcut::regex(re, crpcut::regex::m));
        }
      }

      class not_empty : public trompeloeil::matcher
      {
      public:
        template <typename T>
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

      TEST(a_non_empty_string_gives_no_report)
      {
        mock_c obj;
        REQUIRE_CALL(obj, foo(not_empty{}));
        obj.foo("bar");
      }

      TEST(an_empty_string_is_reported)
      {
        try {
          mock_c obj;
          REQUIRE_CALL(obj, foo(not_empty{}));
          obj.foo("");
          FAIL << "din't report";
        }
        catch(reported)
        {
          ASSERT_TRUE(reports.size() == 1U);
          auto re = R"_(No match for call of foo with signature void(std::string) with.
  param  _1 = 

Tried obj.foo(not_empty{}) at [a-z_./]*:[0-9]*
  Expected  _1 is not empty)_";
          ASSERT_TRUE(reports.front().msg =~ crpcut::regex(re, crpcut::regex::m));
        }
      }
    }
  }
}

TESTSUITE(streaming)
{
  struct unknown {
    const char values[4] = { 0x10, 0x11, 0x12, 0x13 };
  };

  TEST(unknown_type_is_printed_as_hex_dump)
  {
    std::ostringstream os;
    trompeloeil::print(os, unknown{});
    ASSERT_TRUE(os.str() == "4-byte object={ 0x10 0x11 0x12 0x13 }");
  }

  TEST(print_after_unknown_has_flags_back_to_default)
  {
    std::ostringstream os;
    trompeloeil::print(os, unknown{});
    int16_t u=10000;
    os << u;
    ASSERT_TRUE(os.str() == "4-byte object={ 0x10 0x11 0x12 0x13 }10000");
  }
  TEST(previous_formatting_is_ignored_before_hexdump_and_then_reset)
  {
    std::ostringstream os;
    os << std::oct << std::setfill('_') << std::setw(4) << std::left;
    trompeloeil::print(os, unknown{});
    os << 8;
    ASSERT_TRUE(os.str() == "4-byte object={ 0x10 0x11 0x12 0x13 }10__");
  }
  TEST(call_predefined_ostream_operator_with_defaulted_flags_and_then_reset)
  {
    std::ostringstream os;
    os << std::oct << std::setfill('_') << std::setw(4) << std::left;
    trompeloeil::print(os, 25);
    os << 8;
    ASSERT_TRUE(os.str() == "2510__");
  }
  TEST(large_unknown_is_multi_row_hex_dump)
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
    ASSERT_TRUE(os.str() == str);
  }
  TEST(unknown_object_is_one_line_if_8_bytes)
  {
    struct big {
      char c[8] = { 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17 };
    };
    std::ostringstream os;
    trompeloeil::print(os, big{});
    ASSERT_TRUE(os.str() == "8-byte object={ 0x10 0x11 0x12 0x13 0x14 0x15 0x16 0x17 }");
  }
}

TESTSUITE(overloads)
{
  TEST(wildcards_matches_overload_on_type_and_parameter_count)
  {
    {
      mock_c obj;

      REQUIRE_CALL(obj, getter(ANY(unmovable&)))
        .RETURN(std::ref(_1));
      FORBID_CALL(obj, getter(ANY(int)));
      REQUIRE_CALL(obj, getter(_,_))
        .SIDE_EFFECT(_2 = std::to_string(_1));

      unmovable u;
      auto& ur = obj.getter(u);
      ASSERT_TRUE(&ur == &u);

      std::string s;
      obj.getter(3, s);

      ASSERT_TRUE(s == "3");
    }
    ASSERT_TRUE(reports.empty());
  }
}

TESTSUITE(scoping)
{
  TEST(require_calls_are_matched_in_reversed_order_of_creation)
  {
    {
      mock_c obj;

      ALLOW_CALL(obj, getter(_,_))
        .SIDE_EFFECT(_2 = std::to_string(_1));

      REQUIRE_CALL(obj, getter(3, _))
        .SIDE_EFFECT(_2 = "III")
        .TIMES(2);

      std::string s;
      obj.getter(2, s);

      ASSERT_TRUE(s == "2");

      obj.getter(3, s);

      ASSERT_TRUE(s == "III");

      obj.getter(1, s);

      ASSERT_TRUE(s == "1");

      obj.getter(3, s);

      ASSERT_TRUE(s == "III");

      obj.getter(3, s);

      ASSERT_TRUE(s == "3");
    }
    ASSERT_TRUE(reports.empty());
  }

  TEST(require_calls_are_removed_when_they_go_out_of_scope)
  {
    {
      mock_c obj;
      std::string s;

      ALLOW_CALL(obj, getter(_,_))
        .SIDE_EFFECT(_2 = std::to_string(_1));

      {
        REQUIRE_CALL(obj, getter(3, _))
          .SIDE_EFFECT(_2 = "III")
          .TIMES(1, 8);

        obj.getter(3, s);

        ASSERT_TRUE(s == "III");

        obj.getter(2, s);

        ASSERT_TRUE(s == "2");

        obj.getter(3, s);

        ASSERT_TRUE(s == "III");
      }

      obj.getter(3, s);

      ASSERT_TRUE(s == "3");
    }
    ASSERT_TRUE(reports.empty());
  }

  TEST(an_unsatisfied_require_call_is_reported_at_end_of_scope)
  {
    mock_c obj;
    {
      REQUIRE_CALL(obj, foo("bar"));
    }
    ASSERT_TRUE(reports.size() == 1U);

    auto re = R"_(Unfulfilled expectation:
Expected obj.foo("bar") to be called once, actually never called
  param  _1 = bar)_";
    ASSERT_TRUE(reports.front().msg =~ crpcut::regex(re, crpcut::regex::m));
  }
  TESTSUITE(multiplicity)
  {
    TEST(no_calls_reported_as_never_called)
    {
      mock_c obj;
      {
        REQUIRE_CALL(obj, count())
          .RETURN(1);
      }
      ASSERT_TRUE(reports.size() == 1U);
      ASSERT_TRUE(reports.front().msg =~ crpcut::regex("actually never called"));
    }

    TEST(undersatisfied_with_one_call_reported_as_once)
    {
      mock_c obj;
      {
        REQUIRE_CALL(obj, count())
          .RETURN(1)
          .TIMES(2);
        obj.count();
      }
      ASSERT_TRUE(reports.size() == 1U);
      ASSERT_TRUE(reports.front().msg =~ crpcut::regex("actually called once"));
    }

    TEST(undersatisfied_with_two_call_reported_as_count)
    {
      mock_c obj;
      {
        REQUIRE_CALL(obj, count())
          .RETURN(1)
          .TIMES(3);
        obj.count();
        obj.count();
      }
      ASSERT_TRUE(reports.size() == 1U);
      ASSERT_TRUE(reports.front().msg =~ crpcut::regex("actually called 2 times"));
    }

    TEST(no_calls_when_one_required_reported_as_expected_once)
    {
      mock_c obj;
      {
        REQUIRE_CALL(obj, count())
          .RETURN(1);
      }
      ASSERT_TRUE(reports.size() == 1U);
      ASSERT_TRUE(reports.front().msg =~ crpcut::regex("to be called once"));
    }

    TEST(no_calls_when_two_required_reported_as_expected_2_times)
    {
      mock_c obj;
      {
        REQUIRE_CALL(obj, count())
          .TIMES(2)
          .RETURN(1);
      }
      ASSERT_TRUE(reports.size() == 1U);
      ASSERT_TRUE(reports.front().msg =~ crpcut::regex("to be called 2 times"));
    }
  }

}

TESTSUITE(destruction)
{
  TEST(an_unexpected_destruction_of_monitored_object_is_reported)
  {
    {
      trompeloeil::deathwatched<mock_c> obj;
    }
    ASSERT_TRUE(reports.size() == 1U);
    ASSERT_TRUE(reports.front().msg =~ crpcut::regex("Unexpected destruction of.*@"));
  }

  TEST(an_expected_destruction_of_monitored_object_is_not_reported)
  {
    {
      auto obj = new trompeloeil::deathwatched<mock_c>;
      REQUIRE_DESTRUCTION(*obj);
      delete obj;
    }
    ASSERT_TRUE(reports.empty());
  }

  class none
  {
  public:
    virtual ~none() = default;
  };
  TEST(a_copy_of_a_deathwatched_object_with_expectation_is_not_expected_to_die)
  {
    auto orig = new trompeloeil::deathwatched<none>;
    REQUIRE_DESTRUCTION(*orig);
    auto copy = new trompeloeil::deathwatched<none>(*orig);

    delete orig;
    ASSERT_TRUE(reports.empty());

    delete copy;
    ASSERT_TRUE(reports.size() == 1U);
    ASSERT_TRUE(reports.front().msg =~ crpcut::regex("Unexpected destruction of .*@"));
  }

  TEST(a_deathwatched_object_move_constructed_from_original_with_expectation_is_not_expected_to_die_and_the_original_still_is)
  {
    auto orig = new trompeloeil::deathwatched<none>;
    REQUIRE_DESTRUCTION(*orig);
    auto copy = new trompeloeil::deathwatched<none>(std::move(*orig));

    delete orig;
    ASSERT_TRUE(reports.empty());

    delete copy;
    ASSERT_TRUE(reports.size() == 1U);
    ASSERT_TRUE(reports.front().msg =~ crpcut::regex("Unexpected destruction of .*@"));
  }
  
  TEST(object_alive_when_destruction_expectation_goes_out_of_scope_is_reported)
  {
    trompeloeil::deathwatched<mock_c> obj;
    {
      auto p = NAMED_REQUIRE_DESTRUCTION(obj);
    }
    ASSERT_TRUE(reports.size() == 1U);
    ASSERT_TRUE(reports.front().msg =~ crpcut::regex("Object obj is still alive"));
  }

  TEST(require_destruction_succeeds_also_without_deathwatch)
  {
    {
      auto obj = new trompeloeil::deathwatched<mock_c>;
      REQUIRE_DESTRUCTION(*obj);
      delete obj;
    }
    ASSERT_TRUE(reports.empty());
  }

  TEST(a_deathwatched_objects_constructor_passes_params_to_mock)
  {
    auto obj = new trompeloeil::deathwatched<mock_c>{"apa"};
    ASSERT_TRUE(obj->p_ == std::string("apa"));
    REQUIRE_DESTRUCTION(*obj);
    delete obj;
  }
}

TESTSUITE(mismatches)
{
  TEST(unmatched_call_is_reported)
  {
    try {
      mock_c obj;
      obj.getter(7);
      FAIL << "didn't throw!";
    }
    catch (reported)
    {
      ASSERT_TRUE(reports.size() == 1U);
      auto re = R"(No match for call of getter with signature int(int) with\.
  param  _1 = 7)";
      ASSERT_TRUE(reports.front().msg =~ crpcut::regex(re, crpcut::regex::m));
    }
  }

  TEST(match_of_saturated_call_is_reported)
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
      FAIL << "didn't report";
    }
    catch (reported)
    {
      ASSERT_TRUE(count == 9);
      ASSERT_TRUE(reports.size() == 1U);
      auto re =
        R"_(No match for call of getter with signature int(int) with\.
  param  _1 = 3

Matches saturated call requirement
  obj.getter(3) at [a-z_./]*:[0-9]*)_";
      ASSERT_TRUE(reports.front().msg =~ crpcut::regex(re, crpcut::regex::m));
    }
  }

  TEST(a_matching_call_that_throws_is_saturated)
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
        FAIL << "didn't throw";
      } catch (int) {}
      count += obj.getter(4); // 3
      count += obj.getter(2); // 4
      try {
        count += obj.getter(3); // 4 -> 2
        FAIL << "didn't throw";
      } catch (int) {}
      count += obj.getter(5); // 5
      try {
        count += obj.getter(3); // 5 -> 3
        FAIL << "didn't throw";
      } catch(int) {}
      count += obj.getter(8); // 6
      count += obj.getter(3); // boom!
      FAIL << "didn't report";
    }
    catch (reported)
    {
      ASSERT_TRUE(count == 6);
      ASSERT_TRUE(reports.size() == 1U);
      auto re =
        R"_(No match for call of getter with signature int(int) with\.
  param  _1 = 3

Matches saturated call requirement
  obj.getter(3) at [a-z_./]*:[0-9]*)_";
      ASSERT_TRUE(reports.front().msg =~ crpcut::regex(re, crpcut::regex::m));
    }
  }

  TEST(unmatched_call_with_mismatching_requirements_is_reported)
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
      ASSERT_TRUE(reports.size() == 1U);
      auto re =
        R"_(No match for call of getter with signature int(int) with\.
  param  _1 = 3

Tried obj.getter(5) at [a-z_./]*:[0-9]*
  Expected  _1 = 5

Tried obj.getter(4) at [a-z_./]*:[0-9]*
  Expected  _1 = 4)_";
      ASSERT_TRUE(reports.front().msg =~ crpcut::regex(re, crpcut::regex::m));
    }
  }

  TEST(unmatched_with_wildcard_reports_failed_WITH_clauses)
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
      ASSERT_TRUE(reports.size() == 1U);
      auto re =
        R"_(No match for call of getter with signature int(int) with\.
  param  _1 = 4

Tried obj.getter(ANY(int)) at [a-z_./]*:[0-9]*
  Failed WITH(_1 < 3)
  Failed WITH(_1 > 5))_";
      ASSERT_TRUE(reports.front().msg =~ crpcut::regex(re, crpcut::regex::m));
    }
  }

  TEST(unmatched_with_wildcard_reports_only_failed_WITH_clauses)
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
      ASSERT_TRUE(reports.size() == 1U);
      auto re =
        R"_(No match for call of getter with signature int(int) with\.
  param  _1 = 4

Tried obj.getter(ANY(int)) at [a-z_./]*:[0-9]*
  Failed WITH(_1 < 3))_";
      ASSERT_TRUE(reports.front().msg =~ crpcut::regex(re, crpcut::regex::m));
    }
  }

  TEST(match_of_forbidden_call_is_reported)
  {
    try {
      mock_c obj;
      ALLOW_CALL(obj, getter(ANY(int)))
        .RETURN(0);

      FORBID_CALL(obj, getter(3));

      obj.getter(4);
      obj.getter(2);
      obj.getter(3);
      FAIL << "didn't report";
    }
    catch (reported)
    {
      ASSERT_TRUE(reports.size() == 1U);
      auto re = R"_(Match of forbidden call of obj.getter(3) at [a-z_./]*:[0-9]*
  param  _1 = 3)_";
      ASSERT_TRUE(reports.front().msg =~ crpcut::regex(re, crpcut::regex::m));
    }
  }
}

TESTSUITE(parameters)
{
  class T
  {
  public:
    MAKE_MOCK15(concats, std::string(int,int,int,int,
                                     int,int,int,int,
                                     int,int,int,int,
                                     int,int,int));
    MAKE_MOCK1(ptr, void(std::shared_ptr<int>));
    MAKE_MOCK1(ptr, void(std::unique_ptr<int>));
  };

  TEST(parameters_are_passed_in_correct_order_when_matching)
  {
    T obj;
    REQUIRE_CALL(obj, concats(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15))
      .RETURN("");
    auto s = obj.concats(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15);
    ASSERT_TRUE(s == "");
  }

  TEST(parameters_are_passed_in_correct_order_to_WITH)
  {
    T obj;
    REQUIRE_CALL(obj, concats(_,_,_,_,_,_,_,_,_,_,_,_,_,_,_))
      .WITH(_1 == 1 && _2 == 2 && _3 == 3 && _4 == 4 &&
            _5 == 5 && _6 == 6 && _7 == 7 && _8 == 8 &&
            _9 == 9 && _10 == 10 && _11 == 11 && _12 == 12 &&
            _13 == 13 && _14 == 14 && _15 == 15)
      .RETURN("");
    auto s = obj.concats(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15);
    ASSERT_TRUE(s == "");
  }
  TEST(parametess_are_passed_in_correct_order_to_SIDE_EFFECT)
  {
    T obj;
    int n = 0;
    REQUIRE_CALL(obj, concats(_,_,_,_,_,_,_,_,_,_,_,_,_,_,_))
      .LR_SIDE_EFFECT(n = _1 + _2 - _3 + _4 - _5 + _6 - _7 + _8 - _9 + _10 - _11 + _12 - _13 + _14 - _15)
      .RETURN("");
    auto s = obj.concats(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15);
    ASSERT_TRUE(n == -6);
  }
  TEST(parameters_are_passed_in_correct_order_to_RETURN)
  {
    T obj;
    REQUIRE_CALL(obj, concats(_,_,_,_,_,_,_,_,_,_,_,_,_,_,_))
      .RETURN(std::to_string(_1)+
              std::to_string(_2)+
              std::to_string(_3)+
              std::to_string(_4)+
              std::to_string(_5)+
              std::to_string(_6)+
              std::to_string(_7)+
              std::to_string(_8)+
              std::to_string(_9)+
              std::to_string(_10)+
              std::to_string(_11)+
              std::to_string(_12)+
              std::to_string(_13)+
              std::to_string(_14)+
              std::to_string(_15));
    auto s = obj.concats(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15);
    ASSERT_TRUE(s == "123456789101112131415");
  }
  TEST(parametess_are_passed_in_correct_order_to_THROW)
  {
    T obj;

    REQUIRE_CALL(obj, concats(_,_,_,_,_,_,_,_,_,_,_,_,_,_,_))
      .THROW(_1 + _2 - _3 + _4 - _5 + _6 - _7 + _8 - _9 + _10 - _11 + _12 - _13 + _14 - _15);
    ASSERT_THROW(obj.concats(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15),
                 int,
                 [](int n) { return n == -6; });
  }

  TEST(shared_ptr_by_value_in_expectation_is_copied)
  {
    T obj;
    auto s = std::make_shared<int>(3);
    {
      REQUIRE_CALL(obj, ptr(s));
      ASSERT_TRUE(s.use_count() == 2U);
      obj.ptr(s);
    }
    ASSERT_TRUE(s.use_count() == 1U);
  }

  TEST(shared_ptr_by_std_ref_in_expectation_is_not_copied)
  {
    T obj;
    auto s = std::make_shared<int>(3);
    {
      REQUIRE_CALL(obj, ptr(std::ref(s)));
      ASSERT_TRUE(s.use_count() == 1U);
      obj.ptr(s);
    }
    ASSERT_TRUE(s.use_count() == 1U);
  }

  TEST(unique_ptr_by_value_is_matched_with_raw_ptr_in_WITH)
  {
    T obj;
    auto s = std::unique_ptr<int>(new int(3));
    {
      auto sr = s.get();
      REQUIRE_CALL(obj, ptr(ANY(std::unique_ptr<int>)))
        .WITH(_1.get() == sr);
      obj.ptr(std::move(s));
      ASSERT_FALSE(s);
    }
  }
}

TESTSUITE(tracing)
{
  TEST(matching_calls_are_traced)
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
"^[a-z_./]*:[0-9]*\n"
"obj1.getter(_, _) with.\n"
"  param  _1 = 3\n"
"  param  _2 = foo\n"
"\n"
"[a-z_./]*:[0-9]*\n"
"obj2.foo(\"bar\") with.\n"
"  param  _1 = bar\n$";
    ASSERT_TRUE(os.str() =~ crpcut::regex(re, crpcut::regex::m));
  }

  TEST(tracing_is_only_active_when_tracer_obj_is_alive)
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
"^[a-z_./]*:[0-9]*\n"
"obj2.foo(\"bar\") with.\n"
"  param  _1 = bar\n$";
    ASSERT_TRUE(os.str() =~ crpcut::regex(re, crpcut::regex::m));
  }
}
int main(int argc, char *argv[])
{
  trompeloeil::set_reporter(send_report);
  return crpcut::run(argc, argv);
}

