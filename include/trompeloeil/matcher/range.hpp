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

namespace impl {
struct is_checker
{
  template <typename R, typename ... Es>
  bool operator()(const R& range, const Es& ... elements) const
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
  }
};

struct is_printer
{
  template <typename ... Es>
  void operator()(std::ostream& os, const Es& ... elements) const
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
  }
};

}
template <
    typename Type = trompeloeil::wildcard,
    typename ... Es,
    typename R = make_matcher_return<
        Type,
        impl::is_checker,
        impl::is_printer,
        Es...>
    >
R range_is(Es&& ... es)
{
  return trompeloeil::make_matcher<Type>(
      impl::is_checker{},
      impl::is_printer{},
      std::forward<Es>(es)...);
}

namespace impl
{
struct is_all_checker
{
  template <typename R, typename C>
  bool operator()(const R& range, const C& comp) const
  {
    using std::begin;
    using std::end;
    auto it = begin(range);
    const auto e = end(range);
    return std::all_of(it, e,
                       [&](const auto& t){
                         return trompeloeil::param_matches(comp, std::ref(t));
                       });

  }
};

struct is_all_printer
{
  template <typename C>
  void operator()(std::ostream& os, const C& comp) const
  {
    os << " range is all";
    trompeloeil::print(os, comp);
  }
};
}

template <typename Type = trompeloeil::wildcard,
    typename C,
          typename R = make_matcher_return<
          Type,
          impl::is_all_checker,
          impl::is_all_printer,
          C>
          >
R range_is_all(C&& compare)
{
  return trompeloeil::make_matcher<Type>(
      impl::is_all_checker{},
      impl::is_all_printer{},
      std::forward<C>(compare));
}

namespace impl
{
struct is_none_checker
{
  template <typename R, typename C>
  bool operator()(const R& range, const C& comp) const
  {
    using std::begin;
    using std::end;
    auto it = begin(range);
    const auto e = end(range);
    return std::none_of(it, e,
                        [&](const auto& t){
                          return trompeloeil::param_matches(comp, std::ref(t));
                        });
  }
};

struct is_none_printer
{
  template <typename C>
  void operator()(std::ostream& os, const C& comp) const
  {
    os << " range is none";
    trompeloeil::print(os, comp);
  }
};
}

template <
    typename Type = trompeloeil::wildcard,
    typename C,
    typename R = make_matcher_return<
        Type,
        impl::is_none_checker,
        impl::is_none_printer,
        C>>
R range_is_none(C&& compare)
{
  return trompeloeil::make_matcher<Type>(
      impl::is_none_checker{},
      impl::is_none_printer{},
      std::forward<C>(compare));
}

namespace impl
{
struct starts_with_checker
{
  template <typename R, typename ... Es>
  bool operator()(const R& range, const Es& ... elements) const
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
  }
};

struct starts_with_printer
{
  template <typename ... Es>
  void operator()(std::ostream& os, const Es& ... elements) const
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

  }
};

}

template <
    typename Type = trompeloeil::wildcard,
    typename ... Es,
    typename R = make_matcher_return<
        Type,
        impl::starts_with_checker,
        impl::starts_with_printer,
        Es...>>
R range_starts_with(Es&& ... es)
{
  return trompeloeil::make_matcher<Type>(
      impl::starts_with_checker{},
      impl::starts_with_printer{},
      std::forward<Es>(es)...);
}

namespace impl {
struct ends_with_checker
{
  template <typename R, typename... Es>
  bool operator()(const R &range, const Es &...elements) const
  {
    using std::begin;
    using std::end;
    auto it = begin(range);
    const auto e = end(range);
    const auto num_values = static_cast<std::ptrdiff_t>(sizeof...(elements));
    const auto size = std::distance(it, e);
    if (size < num_values)
    {
      return false;
    }
    std::advance(it, size - num_values);
    bool all_true = true;
    const auto match = [&](const auto &compare)
    {
      if (it == e)
      {
        return false;
      }
      const auto &v = *it++;
      return trompeloeil::param_matches(compare, std::ref(v));
    };
    trompeloeil::ignore(std::initializer_list<bool>{
        (all_true = all_true && match(elements))...});
    return all_true;
  }
};

struct ends_with_printer
{
  template <typename... Es>
  void operator()(std::ostream &os, const Es &...elements) const
  {
    os << " range ends with {";
    const char *sep = "";
    const auto print = [&](const auto &v)
    {
      os << std::exchange(sep, ", ") << v;
      return 0;
    };
    trompeloeil::ignore(std::initializer_list<int>{(print(elements))...});
    os << " }";
  }
};

}
template <
    typename Type = trompeloeil::wildcard,
    typename ... Es,
    typename R = make_matcher_return<
        Type,
        impl::ends_with_checker,
        impl::ends_with_printer,
        Es...>
>
R range_ends_with(Es&& ... es)
{
  return trompeloeil::make_matcher<Type>(
      impl::ends_with_checker{},
      impl::ends_with_printer{},
      std::forward<Es>(es)...);
}

}
#endif // TROMPELOEIL_RANGE_HPP
