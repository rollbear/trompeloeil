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
// pass: CO_RETURN when return type is not a coroutine

#define TROMPELOEIL_EXPERIMENTAL_COROUTINES
#include <trompeloeil.hpp>

struct MS
{
  MAKE_MOCK0(f, int());
};

int main()
{
  MS obj;

  REQUIRE_CALL(obj, f())
    .CO_RETURN(0);
}
