/*
* Trompeloeil C++ mocking framework
*
* Copyright (C) Björn Fahller
* Copyright (C) Andrew Paxie
*
*  Use, modification and distribution is subject to the
*  Boost Software License, Version 1.0. (See accompanying
*  file LICENSE_1_0.txt or copy atl
*  http://www.boost.org/LICENSE_1_0.txt)
*
* Project home: https://github.com/rollbear/trompeloeil
*/


#ifndef TROMPELOEIL_RANGE_HPP
#define TROMPELOEIL_RANGE_HPP

#ifndef TROMPELOEIL_MATCHER_HPP
#include "../matcher.hpp"
#endif

namespace trompeloeil {

template <typename Type = trompeloeil::wildcard, typename ... Es>
auto range_is(Es&& ... es)
{
  return trompeloeil::make_matcher<Type>(
      [](const auto& range, const Es& ... elements) -> bool
      {
        using std::begin;
        using std::end;
        auto it = begin(range);
        const auto e = end(range);
        bool all_true = true;
        const auto match = [&](const auto& compare)
        {
          if (it == e) return false;
          const auto& v = *it++;
          return trompeloeil::param_matches(compare, std::ref(v));
        };
        trompeloeil::ignore(std::initializer_list<bool>{
            (all_true = all_true && match(elements))  ...});
        return all_true && it == e;
      },
      [](std::ostream& os, const Es& ... elements)
      {
        os << " range is {";
        const char* sep = "";
        const auto print = [&](const auto& v) {
          os << std::exchange(sep, ", ") << v;
          return 0;
        };
        trompeloeil::ignore(std::initializer_list<int>{
            (print(elements))...
        });
        os << " }";
      },
      es...);
}

template <typename Type = trompeloeil::wildcard, typename C>
auto range_is_all(C&& compare)
{
  return trompeloeil::make_matcher<Type>(
      [](const auto& range, const C& comp) -> bool
      {
        using std::begin;
        using std::end;
        auto it = begin(range);
        const auto e = end(range);
        return std::all_of(it, e,
                           [&](const auto& t){
                             return trompeloeil::param_matches(comp, std::ref(t));
                           });
      },
      [](std::ostream& os, const C& comp)
      {
        os << " range is all";
        trompeloeil::print(os, comp);
      },
      std::forward<C>(compare));
}

template <typename Type = trompeloeil::wildcard, typename C>
auto range_is_none(C&& compare)
{
  return trompeloeil::make_matcher<Type>(
      [](const auto& range, const C& comp) -> bool
      {
        using std::begin;
        using std::end;
        auto it = begin(range);
        const auto e = end(range);
        return std::none_of(it, e,
                            [&](const auto& t){
                              return trompeloeil::param_matches(comp, std::ref(t));
                            });
      },
      [](std::ostream& os, const C& comp)
      {
        os << " range is none";
        trompeloeil::print(os, comp);
      },
      std::forward<C>(compare));
}

template <typename Type = trompeloeil::wildcard, typename ... Es>
auto range_starts_with(Es&& ... es)
{
  return trompeloeil::make_matcher<Type>(
      [](const auto& range, const Es& ... elements) -> bool
      {
        using std::begin;
        using std::end;
        auto it = begin(range);
        const auto e = end(range);
        bool all_true = true;
        const auto match = [&](const auto& compare)
        {
          if (it == e) return false;
          const auto& v = *it++;
          return trompeloeil::param_matches(compare, std::ref(v));
        };
        trompeloeil::ignore(std::initializer_list<bool>{
            (all_true = all_true && match(elements))  ...});
        return all_true;
      },
      [](std::ostream& os, const Es& ... elements)
      {
        os << " range starts with {";
        const char* sep = "";
        const auto print = [&](const auto& v)
        {
          os << std::exchange(sep, ", ") << v;
          return 0;
        };
        trompeloeil::ignore(std::initializer_list<int>{
            (print(elements))...
        });
        os << " }";
      },
      es...);
}


template <typename Type = trompeloeil::wildcard, typename ... Es>
auto range_ends_with(Es&& ... es)
{
  return trompeloeil::make_matcher<Type>(
      [](const auto& range, const Es& ... elements) -> bool
      {
        using std::begin;
        using std::end;
        auto it = begin(range);
        const auto e = end(range);
        const auto num_values = static_cast<std::ptrdiff_t>(sizeof...(Es));
        const auto size = std::distance(it, e);
        if (size < num_values)
        {
          return false;
        }
        std::advance(it, size - num_values);
        bool all_true = true;
        const auto match = [&](const auto& compare)
        {
          if (it == e) return false;
          const auto& v = *it++;
          return trompeloeil::param_matches(compare, std::ref(v));
        };
        trompeloeil::ignore(std::initializer_list<bool>{
            (all_true = all_true && match(elements))  ...});
        return all_true;
      },
      [](std::ostream& os, const Es& ... elements)
      {
        os << " range ends with {";
        const char* sep = "";
        const auto print = [&](const auto& v) {
          os << std::exchange(sep, ", ") << v;
          return 0;
        };
        trompeloeil::ignore(std::initializer_list<int>{
            (print(elements))...
        });
        os << " }";
      },
      es...);
}


}
#endif // TROMPELOEIL_RANGE_HPP
