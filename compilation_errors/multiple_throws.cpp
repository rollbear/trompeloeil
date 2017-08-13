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

//Multiple THROW does not make sense

#include <trompeloeil.hpp>

struct MS
{
  MAKE_MOCK0(f, int());
};

int main()
{
  MS obj;

#if (TROMPELOEIL_CPLUSPLUS == 201103L)

#if (TROMPELOEIL_CXX11_API_VERSION == 1)

  using m_t = NAMED_MOCK_TYPE(obj, f());
  REQUIRE_CALL(obj, f())
    .THROW_TYPE(m_t, 3)
    .THROW_TYPE(m_t, 'a');

#else /* (TROMPELOEIL_CXX11_API_VERSION == 1) */

  MAKE_EXPECTATION(obj, f(),
  REQUIRE_CALL(obj, f())
    .THROW(3)
    .THROW('a')
  );

#endif /* !(TROMPELOEIL_CXX11_API_VERSION == 1) */

#else /* (TROMPELOEIL_CPLUSPLUS == 201103L) */

  REQUIRE_CALL(obj, f())
    .THROW(3)
    .THROW('a');

#endif /* !(TROMPELOEIL_CPLUSPLUS == 201103L) */

}
