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

// exception: clang++-3
// pass: value from a duck typed matcher

#include <trompeloeil.hpp>

struct S
{
  MAKE_MOCK0(f, bool());
};

int main()
{
  S s;
#if (TROMPELOEIL_CPLUSPLUS == 201103L)
  REQUIRE_CALL_V(s, f(),
    .RETURN(trompeloeil::eq(3)));
#else
  REQUIRE_CALL(s, f()).RETURN(trompeloeil::eq(3));
#endif
}
