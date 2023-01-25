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

// exception: c++11
// pass: RETURN non-reference from function returning reference

#include <trompeloeil.hpp>

struct MS
{
  MAKE_MOCK0(f, char&());
};

int main()
{
  MS obj;

#if (TROMPELOEIL_CPLUSPLUS == 201103L)

  REQUIRE_CALL_V(obj, f(),
    .RETURN('a'));

#else /* (TROMPELOEIL_CPLUSPLUS == 201103L) */

  REQUIRE_CALL(obj, f())
    .RETURN('a');

#endif /* !(TROMPELOEIL_CPLUSPLUS == 201103L) */
}
