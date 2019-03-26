/*
 * Trompeloeil C++ mocking framework
 *
 * Copyright Björn Fahller 2014-2019
 *
 *  Use, modification and distribution is subject to the
 *  Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt)
 *
 * Project home: https://github.com/rollbear/trompeloeil
 */


#ifndef TROMPELOEIL_GTEST_HPP_
#define TROMPELOEIL_GTEST_HPP_

#ifndef GTEST_TEST
#error "<gtest.h> must be included before <gtest/trompeloeil.hpp>"
#endif

#include "../trompeloeil.hpp"

namespace trompeloeil
{
  template <>
  inline void reporter<specialized>::send(
    severity s,
    char const *file,
    unsigned long line,
    const char* msg)
  {
    if (s == severity::fatal)
    {
      std::ostringstream os;
      if (line != 0U)
      {
        os << file << ':' << line << '\n';
      }
      throw expectation_violation(os.str() + msg);
    }

    ADD_FAILURE_AT(file, line) << msg;
  }
}


#endif //TROMPELOEIL_GTEST_HPP_
