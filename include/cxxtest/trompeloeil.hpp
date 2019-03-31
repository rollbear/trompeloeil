/*
 * Trompeloeil C++ mocking framework
 *
 * Copyright Björn Fahller 2014-2019
 * Copyright (C) 2019 Andrew Paxie
 *
 *  Use, modification and distribution is subject to the
 *  Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt)
 *
 * Project home: https://github.com/rollbear/trompeloeil
 */


#ifndef TROMPELOEIL_CXXTEST_HPP_
#define TROMPELOEIL_CXXTEST_HPP_

#ifndef CXXTEST_FLAGS
#error "<cxxtest/TestSuite.h> must be included before <cxxtest/trompeloeil.hpp>"
#endif

#include "../trompeloeil.hpp"

#include <ostream>
#include <sstream>

namespace trompeloeil
{
  template <>
  inline void reporter<specialized>::send(
    severity s,
    const char* file,
    unsigned long line,
    const char* msg)
  {
    std::ostringstream os;
    if (line) os << file << ':' << line << '\n';
    os << msg;
    auto failure = os.str();
    if (s == severity::fatal)
    {
      // Must not return normally i.e. must throw, abort or terminate.
      TS_FAIL(failure);
    }
    else
    {
      // nonfatal: violation occurred during stack rollback.
      // Must not throw an exception.
      TS_WARN(failure);
    }
  }
} /* namespace trompeloeil */

#endif //TROMPELOEIL_CXXTEST_HPP_
