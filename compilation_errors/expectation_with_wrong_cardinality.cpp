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

//function.*call

#include <trompeloeil.hpp>

struct MS
{
  MAKE_MOCK1(f, int(int));
};
using trompeloeil::_;
int main()
{
  MS obj;

#if (TROMPELOEIL_CPLUSPLUS == 201103L)

#if (TROMPELOEIL_CXX11_API_VERSION == 1)

  using m_t = NAMED_MOCK_TYPE(obj, f(_,_));
  REQUIRE_CALL(obj, f(_,_))
    .RETURN_TYPE(m_t, _2);

#elif (TROMPELOEIL_CXX11_API_VERSION == 2)

  MAKE_EXPECTATION(obj, f(_,_),
  REQUIRE_CALL(obj, f(_,_))
    .RETURN(_2)
  );

#else /* (TROMPELOEIL_CXX11_API_VERSION == 3) */

  REQUIRE_CALL_V(obj, f(_,_),
    .RETURN(_2));

#endif /* (TROMPELOEIL_CXX11_API_VERSION == 3) */

#else /* (TROMPELOEIL_CPLUSPLUS == 201103L) */

  REQUIRE_CALL(obj, f(_,_))
    .RETURN(_2);

#endif /* !(TROMPELOEIL_CPLUSPLUS == 201103L) */
}
