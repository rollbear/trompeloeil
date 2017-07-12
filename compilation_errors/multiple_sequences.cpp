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

//Multiple IN_SEQUENCE does not make sense. You can list several sequence
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

  using m_t = NAMED_MOCK_TYPE(obj, f());
  REQUIRE_CALL(obj, f())
    .IN_SEQUENCE(seq)
    .RETURN_TYPE(m_t, 0)
    .IN_SEQUENCE(seq);

#else /* (TROMPELOEIL_CPLUSPLUS == 201103L) */

  REQUIRE_CALL(obj, f())
    .IN_SEQUENCE(seq)
    .RETURN(0)
    .IN_SEQUENCE(seq);

#endif /* !(TROMPELOEIL_CPLUSPLUS == 201103L) */
}
