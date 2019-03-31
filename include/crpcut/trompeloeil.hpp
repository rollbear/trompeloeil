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


#ifndef TROMPELOEIL_CRPCUT_HPP_
#define TROMPELOEIL_CRPCUT_HPP_

#ifndef CRPCUT_HERE
#error "<crpcut.hpp> must be included before <crpcut/trompeloeil.hpp>"
#endif

#include "../trompeloeil.hpp"

namespace trompeloeil
{
  template <>
  inline void reporter<specialized>::send(
    severity,
    char const *file,
    unsigned long line,
    const char* msg)
  {
    std::ostringstream os;
    os << file << ':' << line;
    auto loc = os.str();
    auto location = line == 0U
                    ? ::crpcut::crpcut_test_monitor::current_test()->get_location()
                    : ::crpcut::datatypes::fixed_string::make(loc.c_str(), loc.length());
    ::crpcut::comm::report(::crpcut::comm::exit_fail,
                           std::ostringstream(msg),
                           location);
  }
}


#endif //TROMPELOEIL_CRPCUT_HPP_
