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

//SIDE_EFFECT for forbidden call does not make sense

#include <trompeloeil.hpp>
#include <iostream>

struct MS
{
  MAKE_MOCK0(f, int());
};

int main()
{
  MS obj;

#if (TROMPELOEIL_CPLUSPLUS == 201103L)

  using m_t = NAMED_MOCK_TYPE(obj, f());
  FORBID_CALL(obj, f())
    .SIDE_EFFECT_TYPE(m_t, std::cout << 3);

#else /* (TROMPELOEIL_CPLUSPLUS == 201103L) */

  FORBID_CALL(obj, f())
    .SIDE_EFFECT(std::cout << 3);

#endif /* !(TROMPELOEIL_CPLUSPLUS == 201103L) */
}
