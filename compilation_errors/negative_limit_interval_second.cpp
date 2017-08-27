/*
 * Trompeloeil C++ mocking framework
 *
 * Copyright Björn Fahller 2014,2017
 *
 *  Use, modification and distribution is subject to the
 *  Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt)
 *
 * Project home: https://github.com/rollbear/trompeloeil
 */

//In TIMES the first value must not exceed the second
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

#if (TROMPELOEIL_CXX11_API_VERSION == 1)

  using m_t = NAMED_MOCK_TYPE(obj, f());
  REQUIRE_CALL(obj, f())
    .RETURN_TYPE(m_t, 0)
    .TIMES(3,2);

#elif (TROMPELOEIL_CXX11_API_VERSION == 2)

  MAKE_EXPECTATION(obj, f(),
  REQUIRE_CALL(obj, f())
    .RETURN(0)
    .TIMES(3,2)
  );

#else /* (TROMPELOEIL_CXX11_API_VERSION == 3) */

  REQUIRE_CALL_V(obj, f(),
    .RETURN(0)
    .TIMES(3,2));

#endif /* !(TROMPELOEIL_CXX11_API_VERSION == 3) */

#else /* (TROMPELOEIL_CPLUSPLUS == 201103L) */

  REQUIRE_CALL(obj, f())
    .RETURN(0)
    .TIMES(3,2);

#endif /* !(TROMPELOEIL_CPLUSPLUS == 201103L) */
}
