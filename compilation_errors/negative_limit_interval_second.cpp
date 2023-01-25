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

// pass: In TIMES the first value must not exceed the second
#include <trompeloeil.hpp>

struct MS
{
  MAKE_MOCK0(f, int());
};

int main()
{
  trompeloeil::sequence seq;
  MS obj;

#if (TROMPELOEIL_CPLUSPLUS == 201103L)

  REQUIRE_CALL_V(obj, f(),
    .RETURN(0)
    .TIMES(3,2));

#else /* (TROMPELOEIL_CPLUSPLUS == 201103L) */

  REQUIRE_CALL(obj, f())
    .RETURN(0)
    .TIMES(3,2);

#endif /* !(TROMPELOEIL_CPLUSPLUS == 201103L) */
}
