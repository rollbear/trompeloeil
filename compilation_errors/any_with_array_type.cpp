/*
 * Trompeloeil C++ mocking framework
 *
 * Copyright Björn Fahller 2018
 *
 *  Use, modification and distribution is subject to the
 *  Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt)
 *
 * Project home: https://github.com/rollbear/trompeloeil
 */

//array parameter type decays to pointer type for ANY

#include <trompeloeil.hpp>


struct MS
{
  MAKE_MOCK1(f, void(int [3]));
};

int main()
{
  MS obj;

#if (TROMPELOEIL_CPLUSPLUS == 201103L)

  REQUIRE_CALL_V(obj, f(ANY(int[3])));

#else /* (TROMPELOEIL_CPLUSPLUS == 201103L) */

  REQUIRE_CALL(obj, f(ANY(int[3])));

#endif /* !(TROMPELOEIL_CPLUSPLUS == 201103L) */
}
