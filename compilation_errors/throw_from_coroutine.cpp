/*
 * Trompeloeil C++ mocking framework
 *
 * Copyright Björn Fahller
 *
 *  Use, modification and distribution is subject to the
 *  Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt)
 *
 * Project home: https://github.com/rollbear/trompeloeil
 */

// exception: macOS\|g++-10\|clang++-1[0-3]\|c++1[147]
// pass: Do not use THROW from a coroutine, use CO_THROW

#define TROMPELOEIL_EXPERIMENTAL_COROUTINES
#include <trompeloeil.hpp>

struct task
{
  struct promise_type {
    std::suspend_never initial_suspend() noexcept { return {};}
    std::suspend_always final_suspend() noexcept { return {};}
    void return_value(int);
  };
};

struct MS
{
  MAKE_MOCK0(f, task());
};

int main()
{
  MS obj;

  REQUIRE_CALL(obj, f())
    .THROW(std::runtime_error(""));
}
