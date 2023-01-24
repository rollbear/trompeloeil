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

//value from a typed matcher

#include <trompeloeil.hpp>

struct S
{
  MAKE_MOCK0(f, void*());
};

int main()
{
  S s;
#if (TROMPELOEIL_CPLUSPLUS == 201103L)
  REQUIRE_CALL_V(s, f(),
    .RETURN(ANY(std::nullptr_t)));
#else
  REQUIRE_CALL(s, f()).RETURN(ANY(std::nullptr_t));
#endif
}
