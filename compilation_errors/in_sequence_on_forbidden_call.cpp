/*
 * Trompeloeil C++ mocking framework
 *
 * Copyright Björn Fahller 2015,2017
 *
 *  Use, modification and distribution is subject to the
 *  Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt)
 *
 * Project home: https://github.com/rollbear/trompeloeil
 */

//IN_SEQUENCE for forbidden call does not make sense

#include <trompeloeil.hpp>

struct MS
{
  MAKE_MOCK0(f, void());
};

int main()
{
  MS obj;
  trompeloeil::sequence s;

#if (TROMPELOEIL_CPLUSPLUS == 201103L)

#if (TROMPELOEIL_CXX11_API_VERSION == 3)

  REQUIRE_CALL_V(obj, f(),
    .TIMES(0)
    .IN_SEQUENCE(s));

#else /* (TROMPELOEIL_CXX11_API_VERSION == 3) */

  REQUIRE_CALL(obj, f())
    .TIMES(0)
    .IN_SEQUENCE(s);

#endif /* !(TROMPELOEIL_CXX11_API_VERSION == 3) */

#else /* (TROMPELOEIL_CPLUSPLUS == 201103L) */

  REQUIRE_CALL(obj, f())
    .TIMES(0)
    .IN_SEQUENCE(s);

#endif /* !(TROMPELOEIL_CPLUSPLUS == 201103L) */
}
