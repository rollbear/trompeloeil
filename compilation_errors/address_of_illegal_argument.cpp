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

// pass: illegal argument

#include <trompeloeil.hpp>

struct MS
{
  MAKE_MOCK1(f, void(int));
};

void func(const void*);

int main()
{
  MS obj;

#if (TROMPELOEIL_CPLUSPLUS == 201103L)

  REQUIRE_CALL_V(obj, f(ANY(int)),
    .SIDE_EFFECT(func(&_2)));

#else /* (TROMPELOEIL_CPLUSPLUS == 201103L) */

  REQUIRE_CALL(obj, f(ANY(int)))
    .SIDE_EFFECT(func(&_2));

#endif /* !(TROMPELOEIL_CPLUSPLUS == 201103L) */
}
