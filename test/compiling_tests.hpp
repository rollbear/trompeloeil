/*
 * Trompeloeil C++ mocking framework
 *
 * Copyright Björn Fahller 2014-2018
 * Copyright (C) 2017, 2018 Andrew Paxie
 *
 *  Use, modification and distribution is subject to the
 *  Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt)
 *
 * Project home: https://github.com/rollbear/trompeloeil
 */

#ifndef COMPILING_TESTS_HPP_

#define TROMPELOEIL_SANITY_CHECKS
#include <trompeloeil.hpp>

#include <algorithm>
#include <cstddef>
#include <memory>
#include <regex>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>


#if defined(_MSC_VER)

#define TROMPELOEIL_TEST_REGEX_FAILURES 1
#define TROMPELOEIL_TEST_OVERLOAD_FAILURES 1
#define TROMPELOEIL_TEST_NEG_MATCHER_FAILURES 1
#define TROMPELOEIL_TEST_RVALUE_REFERENCE_FAILURES 1

#else /* defined(_MSC_VER) */

// Detect if using libstdc++.
#if defined(__GLIBCXX__)
// Using libstdc++.
#define TROMPELOEIL_USING_LIBSTDCPP 1
#else
#define TROMPELOEIL_USING_LIBSTDCPP 0
#endif

/*
 * The implementation of <regex> is not complete for libstdc++ in GCC 4.8.x.
 * For this reason most tests with reporting will be disabled
 * if compiling with libstdc++.
 */
#if !defined(TROMPELOEIL_TEST_REGEX_FAILURES)

#if TROMPELOEIL_USING_LIBSTDCPP && \
    TROMPELOEIL_GCC && TROMPELOEIL_GCC_VERSION < 40900
#define TROMPELOEIL_TEST_REGEX_FAILURES 0
#else
#define TROMPELOEIL_TEST_REGEX_FAILURES 1
#endif

#endif /* !defined(TROMPELOEIL_TEST_REGEX_FAILURES) */

/*
 * GCC 4.8 has issues with overloading that affects wildcard
 * and duck_typed_matcher.
 */
#if !defined(TROMPELOEIL_TEST_OVERLOAD_FAILURES)

#if TROMPELOEIL_GCC && TROMPELOEIL_GCC_VERSION < 40900
#define TROMPELOEIL_TEST_OVERLOAD_FAILURES 0
#else
#define TROMPELOEIL_TEST_OVERLOAD_FAILURES 1
#endif

#endif /* !defined(TROMPELOEIL_TEST_OVERLOAD_FAILURES) */

/*
 * GCC 4.8 has issues with overloading that affects neg_matcher.
 */
#if !defined(TROMPELOEIL_TEST_NEG_MATCHER_FAILURES)

#if TROMPELOEIL_GCC && TROMPELOEIL_GCC_VERSION < 40900
#define TROMPELOEIL_TEST_NEG_MATCHER_FAILURES 0
#else
#define TROMPELOEIL_TEST_NEG_MATCHER_FAILURES 1
#endif

#endif /* !defined(TROMPELOEIL_TEST_NEG_MATCHER_FAILURES) */

/*
 * GCC 4.8 has issues with user-defined conversions to rvalue references
 * that affects duck_typed_matcher and wildcard.
 */
#if !defined(TROMPELOEIL_TEST_RVALUE_REFERENCE_FAILURES)

#if TROMPELOEIL_GCC && TROMPELOEIL_GCC_VERSION < 40900
#define TROMPELOEIL_TEST_RVALUE_REFERENCE_FAILURES 0
#else
#define TROMPELOEIL_TEST_RVALUE_REFERENCE_FAILURES 1
#endif

#endif /* !defined(TROMPELOEIL_TEST_RVALUE_REFERENCE_FAILURES) */

#endif /* !defined(_MSC_VER) */

namespace detail
{
  /*
   * Use compiler-version independent make_unique.
   */
  using ::trompeloeil::detail::make_unique;

  // std::uncaught_exception() is deprecated in C++17.
  inline
  bool
  there_are_uncaught_exceptions()
  {
    /*
     * GCC 5.x supports specifying -std=c++17 but libstdc++-v3 for
     * GCC 5.x doesn't declare std::uncaught_exceptions().
     * Rather than detect what version of C++ Standard Library
     * is in use, we equate the compiler version with the library version.
     *
     * Some day this test will based on __cpp_lib_uncaught_exceptions.
     */
#   if (TROMPELOEIL_CPLUSPLUS > 201402L) && \
       ((!TROMPELOEIL_GCC) || \
        (TROMPELOEIL_GCC && TROMPELOEIL_GCC_VERSION >= 60000))

    return std::uncaught_exceptions() > 0;

#   else

    return std::uncaught_exception();

#   endif
  }

} /* namespace detail */

class reported {};

struct report
{
  trompeloeil::severity s;
  const char           *file;
  unsigned long         line;
  std::string           msg;
};

extern std::vector<report> reports;

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
      if (s == severity::fatal && !::detail::there_are_uncaught_exceptions())
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

struct interface
{
  virtual ~interface() = default;
  virtual int func(int) = 0;
  virtual int cfunc(int) const = 0;
  virtual int func3(int, int, std::string) = 0;
  virtual int func3(int, int, std::string) const = 0;
};

struct mi : trompeloeil::mock_interface<interface>
{
  using mock_interface::mock_interface;
  IMPLEMENT_MOCK1(func);
  IMPLEMENT_CONST_MOCK1(cfunc);
  IMPLEMENT_MOCK3(func3);
  IMPLEMENT_CONST_MOCK3(func3);
};

struct uncomparable { };

struct uncomparable_string {
  uncomparable_string(const char* p) : s(p) {}
  bool operator==(const uncomparable_string& rh) const noexcept
  {
    return s == rh.s;
  }
  bool operator==(const char*) const = delete;
  friend
  std::ostream& operator<<(std::ostream& os, const uncomparable_string& u)
  {
    return os << u.s;
  }
  std::string s;
};

struct null_comparable {
  void* p;
  bool operator==(std::nullptr_t) const { return !p; }
  friend std::ostream& operator<<(std::ostream& os, const null_comparable&)
  {
    return os << "null_comparable";
  }
};

struct pseudo_null_comparable {
  void operator==(std::nullptr_t) const {} // looking at you, boost::variant<>!
  friend
  std::ostream& operator<<(std::ostream& os, const pseudo_null_comparable&)
  {
    return os << "pseudo_null_comparable";
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

int intfunc(int i);

extern int global_n;

struct mstr
{
  MAKE_MOCK0(cc_str, const char*());
  MAKE_MOCK0(c_str, char*());
  MAKE_MOCK0(str, std::string());
  MAKE_MOCK0(cstr, const std::string());
};

extern char carr[4]; // silence clang++ warning

const char ccarr[] = "bar";

class U
{
public:
  using mptr_f = void (U::*)(const char*);
  using mptr_d = int U::*;
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
  MAKE_MOCK1(func_ptr_f, void(int (*)(int)));
  MAKE_MOCK1(func_mptr_f, void(mptr_f));
  MAKE_MOCK1(func_mptr_d, void(mptr_d));
  MAKE_MOCK1(func_ustr, void(const uncomparable_string&));
  MAKE_MOCK1(func_ustrv, void(uncomparable_string));
  MAKE_MOCK1(func_f, void(std::function<void()>));
  int m;
};

struct C_foo1
{
  MAKE_MOCK1(foo, void(int*));
};

struct C_foo2
{
  MAKE_MOCK1(foo, void(int*));
  MAKE_MOCK1(foo, void(char*));
};

struct C_foo3
{
  int m;
  MAKE_MOCK1(foo, void(int C_foo3::*));
  MAKE_MOCK1(bar, void(int (*)(int)));
};

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
inline
auto
any_of(std::initializer_list<T> elements)
-> decltype(any_of_t<T>(elements))
{
  return any_of_t<T>(elements);
}

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

struct unknown {
  const char values[4] = { 0x10, 0x11, 0x12, 0x13 };
};

namespace nn
{
  struct TestOutput;
  inline void print(std::ostream&, const TestOutput&);

  struct TestOutput
  {
    int n;
  };

  void
  print(std::ostream& os, const TestOutput& p)
  {
    os << "nn::print(TestOutput{" << p.n << "}";
  }

} // namespace nn

namespace trompeloeil
{
  template <>
  inline void print(std::ostream& os, const nn::TestOutput& p)
  {
    os << "trompeloeil::print(nn::TestOutput{" << p.n << "})";
  }

} // namespace trompeloeil

class TestOutputMock
{
public:
  MAKE_MOCK1(func, void(nn::TestOutput));
};

class none
{
public:
  none() {}
  none(none const&) {}
  virtual ~none() {}
};

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

#endif /* !COMPILING_TESTS_HPP_ */
