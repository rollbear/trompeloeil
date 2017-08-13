/*
 * Trompeloeil C++ mocking framework
 *
 * Copyright Björn Fahller 2016,2017
 *
 *  Use, modification and distribution is subject to the
 *  Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt)
 *
 * Project home: https://github.com/rollbear/trompeloeil
 */

//illegal argument

#include <trompeloeil.hpp>

struct MS
{
  MAKE_MOCK1(f, void(int));
};

void func(const int&);

int main()
{
  MS obj;

#if (TROMPELOEIL_CPLUSPLUS == 201103L)

#if (TROMPELOEIL_CXX11_API_VERSION == 1)

  using m_t = NAMED_MOCK_TYPE(obj, f(ANY(int)));
  REQUIRE_CALL(obj, f(ANY(int)))
    .SIDE_EFFECT_TYPE(m_t, *_2 = 1);

#else /* (TROMPELOEIL_CXX11_API_VERSION == 1) */

  MAKE_EXPECTATION(obj, f(ANY(int)),
  REQUIRE_CALL(obj, f(ANY(int)))
    .SIDE_EFFECT(*_2 = 1)
  );

#endif /* !(TROMPELOEIL_CXX11_API_VERSION == 1) */

#else /* (TROMPELOEIL_CPLUSPLUS == 201103L) */

  REQUIRE_CALL(obj, f(ANY(int)))
    .SIDE_EFFECT(*_2 = 1);

#endif /* !(TROMPELOEIL_CPLUSPLUS == 201103L) */
}
