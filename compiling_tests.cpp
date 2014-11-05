#include "trompeloeil.hpp"
#include <crpcut.hpp>

#include <vector>
#include <string>

class reported {};

struct report
{
  trompeloeil::severity s;
  std::string           loc;
  std::string           msg;
};
std::vector<report> reports;

void send_report(trompeloeil::severity s, const std::string& loc, const std::string msg)
{
#if 0
      auto location = loc.empty()
        ? ::crpcut::crpcut_test_monitor::current_test()->get_location()
        : ::crpcut::datatypes::fixed_string{loc.c_str(), loc.length()};
      ::crpcut::comm::report(::crpcut::comm::exit_fail,
                             std::ostringstream(msg),
                             location);
#else
  reports.push_back(report{s, loc, msg});
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
  unmovable() {};
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
  C(const char*) {}
};

class mock_c : public trompeloeil::mocked_class<C>
{
 public:
  mock_c() : mocked_class() {}
  mock_c(int i) : mocked_class(i) {}
  mock_c(const char* p) : mocked_class(p) {}
  MOCK(count,  ());
  MOCK(func,   (int, std::string&));
  MOCK(getter, (unmovable&));
  MOCK(getter, (int));
  MOCK(getter, (int, std::string&));
  MOCK(ptr,    (std::unique_ptr<int>&&));
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
      ASSERT_TRUE(reports.front().msg =~ crpcut::regex("Sequence mismatch.*seq.*first"));
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
      ASSERT_TRUE(reports.front().msg =~ crpcut::regex("Sequence mismatch.*seq2.*first"));
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
      ASSERT_TRUE(reports.front().msg =~ crpcut::regex("Sequence mismatch.*seq.*first"));
      INFO << reports.front().loc << "\n" << reports.front().msg;
    }
  }
}

TESTSUITE(return_values)
{
  TEST(return_by_reference_returns_object_given)
  {
    {
      mock_c obj;
      unmovable s;
      REQUIRE_CALL(obj, getter(ANY(unmovable&)))
        .RETURN(std::ref(s));

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
}

TESTSUITE(matching)
{
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
      FORBID_CALL(obj, getter(ANY(int)))
        .RETURN(_1);
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
}

TESTSUITE(destruction)
{
  TEST(an_unexpected_destruction_of_monitored_object_is_reported)
  {
    {
      mock_c obj;
      DEATHWATCH(obj);
    }
    ASSERT_TRUE(reports.size() == 1U);
    ASSERT_TRUE(reports.front().msg =~ crpcut::regex("Unexpected destruction of.*@"));
  }

  TEST(an_expected_destruction_of_monitored_object_is_not_reported)
  {
    {
      auto obj = new mock_c;
      DEATHWATCH(*obj);
      REQUIRE_DESTRUCTION(*obj);
      delete obj;
    }
    ASSERT_TRUE(reports.empty());
  }

  TEST(object_alive_when_destruction_expectation_goes_out_of_scope_is_reported)
  {
    mock_c obj;
    DEATHWATCH(obj);
    {
      auto p = NAMED_REQUIRE_DESTRUCTION(obj);
    }
    ASSERT_TRUE(reports.size() == 1U);
    ASSERT_TRUE(reports.front().msg =~ crpcut::regex("Object obj is still alive"));
  }

  TEST(require_destruction_succeeds_also_without_deathwatch)
  {
    {
      auto obj = new mock_c;
      REQUIRE_DESTRUCTION(*obj);
      delete obj;
    }
    ASSERT_TRUE(reports.empty());
  }

  TEST(non_watched_object_still_alive_after_require_destruction_is_reported)
  {
    {
      mock_c obj;
      {
        REQUIRE_DESTRUCTION(obj);
      }
    }
    ASSERT_TRUE(reports.size() > 0U);
    ASSERT_TRUE(reports.front().msg =~ crpcut::regex("Object obj is still alive"));
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
      auto re = R"(No match for call of getter(int) with\.
  param  _1 = 7)";
      ASSERT_TRUE(reports.front().msg =~ crpcut::regex(re, crpcut::regex::m));
    }
  }

  TEST(match_of_saturated_call_is_reported)
  {
    unsigned count = 0;
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
      ASSERT_TRUE(count == 9U);
      ASSERT_TRUE(reports.size() == 1U);
      auto re =
        R"_(No match for call of getter(int) with\.
  param  _1 = 3

Matches saturated call requirement
  obj.getter(3) at [a-z_.]*:[0-9]*)_";
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
        R"_(No match for call of getter(int) with\.
  param  _1 = 3

Tried obj.getter(5) at [a-z_.]*:[0-9]*
  Expected  _1 = 5

Tried obj.getter(4) at [a-z_.]*:[0-9]*
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
        R"_(No match for call of getter(int) with\.
  param  _1 = 4

Tried obj.getter(::trompeloeil::typed_wildcard<int>()) at [a-z_.]*:[0-9]*
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
        R"_(No match for call of getter(int) with\.
  param  _1 = 4

Tried obj.getter(::trompeloeil::typed_wildcard<int>()) at [a-z_.]*:[0-9]*
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

      FORBID_CALL(obj, getter(3))
        .RETURN(0);

      obj.getter(4);
      obj.getter(2);
      obj.getter(3);
      FAIL << "didn't report";
    }
    catch (reported)
    {
      ASSERT_TRUE(reports.size() == 1U);
      auto re = R"_(Match of forbidden call of obj.getter(3) at [a-z_.]*:[0-9]*
  param  _1 = 3)_";
      ASSERT_TRUE(reports.front().msg =~ crpcut::regex(re, crpcut::regex::m));
    }
  }

}
int main(int argc, char *argv[])
{
  trompeloeil::set_reporter(send_report);
  return crpcut::run(argc, argv);
}

