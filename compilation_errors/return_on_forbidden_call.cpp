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

// pass: RETURN for forbidden call does not make sense

#include <trompeloeil.hpp>

struct MS
{
  MAKE_MOCK0(f, int());
};

int main()
{
  MS obj;

#if (TROMPELOEIL_CPLUSPLUS == 201103L)

  FORBID_CALL_V(obj, f(),
    .RETURN(0));

#else /* !(TROMPELOEIL_CPLUSPLUS == 201103L) */

  FORBID_CALL(obj, f())
    .RETURN(0);

#endif /* !(TROMPELOEIL_CPLUSPLUS == 201103L) */
}
