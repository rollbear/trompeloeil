#ifdef __cpp_impl_coroutine

#define TROMPELOEIL_EXPERIMENTAL_COROUTINES
#include <trompeloeil.hpp>

#if defined(CATCH2_VERSION) && CATCH2_VERSION == 3
#include <catch2/catch_test_macros.hpp>
#else
#include <catch2/catch.hpp>
#endif
#include "micro_coro.hpp"

namespace {
  using iptr = std::unique_ptr<int>;

  struct co_mock {
    MAKE_MOCK0 (intret, coro::task<int>());
    MAKE_MOCK0 (voidret, coro::task<void>());
    MAKE_MOCK1 (unique, coro::task<iptr>(iptr));
  };
}
TEST_CASE("co await value")
{
  co_mock m;
  REQUIRE_CALL(m, intret()).CO_RETURN(3);

  int x = 0;
   std::invoke([&]()->coro::task<void>{ x = co_await m.intret();});

  REQUIRE(x == 3);
}

TEST_CASE("co await void")
{
  co_mock m;
  REQUIRE_CALL(m, voidret()).CO_RETURN();

  int x = 0;
  std::invoke([&]()->coro::task<void>{ co_await m.voidret(); x = 3;});

  REQUIRE(x == 3);
}

TEST_CASE("co await throws")
{
  co_mock m;
  REQUIRE_CALL(m, intret()).CO_RETURN(false ? 0 : throw "foo");

  int x = 0;
  std::invoke([&]()->coro::task<void>{
    auto p = m.intret();
    REQUIRE_THROWS(co_await p);
    x = 1;
  });
  REQUIRE(x == 1);
}

TEST_CASE("unique_ptr in co_return")
{
  using trompeloeil::_;
  co_mock m;
  REQUIRE_CALL(m, unique(_)).CO_RETURN(std::move(_1));
  iptr x;
  std::invoke([&]()->coro::task<void>{x = co_await m.unique(std::make_unique<int>(3));});
  REQUIRE(x);
  REQUIRE(*x == 3);
}

#endif
