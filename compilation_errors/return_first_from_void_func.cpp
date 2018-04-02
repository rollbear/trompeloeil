/*
 * Trompeloeil C++ mocking framework
 *
 * Copyright Björn Fahller 2014,2015,2017
 *
 *  Use, modification and distribution is subject to the
 *  Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt)
 *
 * Project home: https://github.com/rollbear/trompeloeil
 */

//RETURN does not make sense for void-function

#include <trompeloeil.hpp>


struct MS
{
  MAKE_MOCK0(f, void());
};

int main()
{
  int n;
  MS obj;

#if (TROMPELOEIL_CPLUSPLUS == 201103L)

  REQUIRE_CALL_V(obj, f(),
    .RETURN(1)
    .LR_SIDE_EFFECT(n = 0));

#else /* (TROMPELOEIL_CPLUSPLUS == 201103L) */

  REQUIRE_CALL(obj, f())
    .RETURN(1)
    .LR_SIDE_EFFECT(n = 0);

#endif /* !(TROMPELOEIL_CPLUSPLUS == 201103L) */
}
