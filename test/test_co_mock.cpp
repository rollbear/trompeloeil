/*
 * Trompeloeil C++ mocking framework
 *
 * Copyright (C) Björn Fahller
 *
 *  Use, modification and distribution is subject to the
 *  Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy atl
 *  http://www.boost.org/LICENSE_1_0.txt)
 *
 * Project home: https://github.com/rollbear/trompeloeil
 */

#ifdef __cpp_impl_coroutine

#include <trompeloeil/mock.hpp>
#include <trompeloeil/coro.hpp>

#if defined(CATCH2_VERSION) && CATCH2_VERSION == 3
#include <catch2/catch_test_macros.hpp>
#else
#include <catch2/catch.hpp>
#endif
#include "micro_coro.hpp"
#include "test_reporter.hpp"
#include <optional>

#include <version>
#ifdef __cpp_lib_generator
#include <generator>
#endif

using trompeloeil::_;

namespace {
  using iptr = std::unique_ptr<int>;

  struct co_mock {
    MAKE_MOCK0 (intret, coro::task<int>());
    MAKE_MOCK0 (voidret, coro::task<void>());
    MAKE_MOCK1 (unique, coro::task<iptr>(iptr));
    MAKE_MOCK0 (gen, coro::generator<int>());

#ifdef __cpp_lib_generator
    MAKE_MOCK0 (stdgen, std::generator<int>());
#endif // __cpp_lib_generator
  };
}

TEST_CASE_METHOD(
    Fixture,
    "A CO_RETURNed value is obtained from co_await",
    "[coro]")
{
  co_mock m;
  REQUIRE_CALL(m, intret()).CO_RETURN(3);

  int x = 0;
   std::invoke([&]()->coro::task<void>{ x = co_await m.intret();});

  REQUIRE(x == 3);

  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
    Fixture,
    "The coroutine can be executed after the expectation has been retired",
    "[coro]")
{
  co_mock m;
  std::optional<coro::task<int>> t;
  {
    REQUIRE_CALL(m, intret()).CO_RETURN(3);
    t.emplace(m.intret());
  }

  int x = 0;
  std::invoke([&]()->coro::task<void>{ x = co_await *t;});

  REQUIRE(x == 3);
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
    Fixture,
    "A void co_routine is CO_RETURNed",
    "[coro]")
{
  co_mock m;
  REQUIRE_CALL(m, voidret()).CO_RETURN();

  int x = 0;
  std::invoke([&]()->coro::task<void>{ co_await m.voidret(); x = 3;});

  REQUIRE(x == 3);
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
    Fixture,
    "If the CO_RETURN expression throws, the exception is thrown from co_await",
    "[coro]")
{
  co_mock m;
  std::optional<coro::task<int>> t;
  {
    REQUIRE_CALL(m, intret()).CO_RETURN(false ? 0 : throw "foo");
    t.emplace(m.intret());
  }
  int x = 0;
  std::invoke([&]()->coro::task<void>{
    REQUIRE_THROWS(co_await *t);
    x = 1;
  });
  REQUIRE(x == 1);
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
    Fixture,
    "Exception from CO_THROW is thrown from co_await",
    "[coro]")
{
  co_mock m;
  REQUIRE_CALL(m, intret()).CO_THROW("foo");
  auto p = m.intret();
  int x = 0;
  std::invoke([&]()->coro::task<void>{
    REQUIRE_THROWS(co_await p);
    x = 1;
  });
  REQUIRE(x == 1);
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
    Fixture,
    "A move-only type can be CO_RETURNed from the argument",
    "[coro]")
{
  co_mock m;
  REQUIRE_CALL(m, unique(_)).CO_RETURN(std::move(_1));
  auto p = m.unique(std::make_unique<int>(3));
  iptr x;
  std::invoke([&]() -> coro::task<void> { x = co_await p; });
  REQUIRE(x);
  REQUIRE(*x == 3);
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
    Fixture,
    "SIDE_EFFECT runs on the call to co-routine function, not when the coro runs",
    "[coro]")
{
  co_mock m;
  int x = 0;
  int y = 0;
  REQUIRE_CALL(m, intret()).LR_SIDE_EFFECT(x=1).CO_RETURN(5);
  auto p = m.intret();
  REQUIRE(x == 1);
  REQUIRE(y == 0);
  x = 0;
  std::invoke([&]() -> coro::task<void> { y = co_await p;});
  REQUIRE(x == 0);
  REQUIRE(y == 5);
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
    Fixture,
    "CO_YIELDed values are co_await:ed in order with CO_RETURN last",
    "[coro]")
{
  co_mock m;
  REQUIRE_CALL(m, intret()).CO_YIELD(1).CO_YIELD(2).CO_RETURN(0);
  auto p = m.intret();
  int v = 0;
  std::invoke([&]() -> coro::task<void> { v = co_await p;});
  REQUIRE(v == 1);
  std::invoke([&]() -> coro::task<void> { v = co_await p;});
  REQUIRE(v == 2);
  std::invoke([&]() -> coro::task<void> { v = co_await p;});
  REQUIRE(v == 0);
  REQUIRE(reports.empty());
}

TEST_CASE_METHOD(
    Fixture,
    "Empty CO_RETURN can end a generator with yield and return_void",
    "[coro]")
{
    co_mock m;
    REQUIRE_CALL(m, gen())
      .CO_YIELD(5)
      .CO_YIELD(8)
      .CO_YIELD(3)
      .CO_RETURN();
    auto p = m.gen();
    int v = 0;
    std::invoke([&]() -> coro::task<void> { v = co_await p;});
    REQUIRE(v == 5);
    std::invoke([&]() -> coro::task<void> { v = co_await p;});
    REQUIRE(v == 8);
    std::invoke([&]() -> coro::task<void> { v = co_await p;});
    REQUIRE(v == 3);
    REQUIRE(reports.empty());
}

struct co_member_fixture : Fixture
{
  int member_variable = 1;
};

TEST_CASE_METHOD(
    co_member_fixture,
    "MEM_CO_RETURN can access member variables",
    "[coro]")
{
  co_mock m;
  REQUIRE_CALL(m, intret())
    .MEM_CO_RETURN(member_variable++);
  auto p = m.intret();
  int v = 0;
  std::invoke([&]() -> coro::task<void> { v = co_await p;});
  REQUIRE(v == 1);
  REQUIRE(this->member_variable == 2);
}

TEST_CASE_METHOD(
    co_member_fixture,
    "MEM_CO_YIELD can access member variables",
    "[coro]")
{
  co_mock m;
  REQUIRE_CALL(m, intret())
    .MEM_CO_YIELD(member_variable++)
    .CO_RETURN(0);
  auto p = m.intret();
  int v = 0;
  std::invoke([&]() -> coro::task<void> { v = co_await p;});
  REQUIRE(v == 1);
  REQUIRE(this->member_variable == 2);
  std::invoke([&]() -> coro::task<void> { v = co_await p;});
  REQUIRE(v == 0);
}

TEST_CASE_METHOD(
    co_member_fixture,
    "MEM_CO_THROW can access member variables",
    "[coro]")
{
  co_mock m;
  REQUIRE_CALL(m, intret())
      .MEM_CO_THROW(member_variable++);
  auto p = m.intret();
  std::invoke([&]() -> coro::task<void> { REQUIRE_THROWS(co_await p); });
  REQUIRE(member_variable == 2);
}

#ifdef __cpp_lib_generator
TEST_CASE_METHOD(
  Fixture,
  "CO_YIELD with std::generator",
  "[coro]")
{
  co_mock m;
  REQUIRE_CALL(m, stdgen())
      .CO_YIELD(5)
      .CO_YIELD(8)
      .CO_YIELD(3)
      .CO_YIELD(0)
      .CO_RETURN();

  auto gen = m.stdgen();

  SECTION("as iterator")
  {
    auto it = std::ranges::begin(gen);
    REQUIRE(*it == 5);
    ++it;
    REQUIRE(it != std::ranges::end(gen));
    REQUIRE(*it == 8);
    ++it;
    REQUIRE(it != std::ranges::end(gen));
    REQUIRE(*it == 3);
    ++it;
    REQUIRE(it != std::ranges::end(gen));
    REQUIRE(*it == 0);
    ++it;
    REQUIRE(it == std::ranges::end(gen));
  }

  SECTION("as range")
  {
    REQUIRE(std::ranges::equal(gen, std::array{5, 8, 3, 0}));
  }

  REQUIRE(reports.empty());
}
#endif // __cpp_lib_generator
#endif
