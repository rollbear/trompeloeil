/*
 * Trompeloeil C++ mocking framework
 *
 * Copyright Björn Fahller 2014-2018
 * Copyright (C) 2017 Andrew Paxie
 *
 *  Use, modification and distribution is subject to the
 *  Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt)
 *
 * Project home: https://github.com/rollbear/trompeloeil
 */

#define TROMPELOEIL_SANITY_CHECKS

#include <trompeloeil.hpp>
#if not defined(CATCH2_VERSION) || CATCH2_VERSION == 2
#ifndef CATCH2_MAIN
#define CATCH_CONFIG_MAIN
#endif
#include <catch2/catch.hpp>
#else
#if CATCH2_VERSION == 3
#include <catch2/catch_test_macros.hpp>
#endif
#endif

#include <algorithm>
#include <cstddef>
#include <memory>
#include <regex>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include "compiling_tests.hpp"

std::vector<report> reports;
std::vector<std::string> okReports;

int
intfunc(int i)
{
  return i;
}

int global_n = 0;

char carr[4] = "foo";
