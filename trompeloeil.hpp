/*
 * Trompeloeil C++ mocking framework
 *
 * Copyright Björn Fahller 2014-2017
 *
 *  Use, modification and distribution is subject to the
 *  Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at
 *  http://www.boost.org/LICENSE_1_0.txt)
 *
 * Project home: https://github.com/rollbear/trompeloeil
 */

#ifndef TROMPELOEIL_HPP_
#define TROMPELOEIL_HPP_


// trompe l'oeil noun    (Concise Encyclopedia)
// Style of representation in which a painted object is intended
// to deceive the viewer into believing it is the object itself...

// project home: https://github.com/rollbear/trompeloeil


// Deficiencies and missing features
// * Mocking function templates is not supported
// * If a macro kills a kitten, this threatens extinction of all felines!

#if defined(_MSC_VER)
#  define TROMPELOEIL_NORETURN
#  if (!defined(__cplusplus) || _MSC_VER < 1900)
#    error requires C++ in Visual Studio 2015 RC or later
#  endif
#else
#  define TROMPELOEIL_NORETURN [[noreturn]]
#  if (!defined(__cplusplus) || __cplusplus <= 201103)
#    error requires C++14 or higher
#  endif
#endif
#include <tuple>
#include <iomanip>
#include <sstream>
#include <exception>
#include <functional>
#include <memory>
#include <cstring>
#include <regex>
#include <mutex>

#ifdef TROMPELOEIL_SANITY_CHECKS
#include <cassert>
#define TROMPELOEIL_ASSERT(x) assert(x)
#else
#define TROMPELOEIL_ASSERT(x) do {} while (false)
#endif

#define TROMPELOEIL_IDENTITY(...) __VA_ARGS__ // work around stupid MS VS2015 RC bug

#define TROMPELOEIL_ARG16(_0,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15, ...) _15

#define TROMPELOEIL_COUNT(...)                                                 \
  TROMPELOEIL_IDENTITY(TROMPELOEIL_ARG16(__VA_ARGS__,                          \
                                         15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0))

#define TROMPELOEIL_CONCAT_(x, y) x ## y
#define TROMPELOEIL_CONCAT(x, y) TROMPELOEIL_CONCAT_(x, y)

#define TROMPELOEIL_INIT_WITH_STR15(base, x, ...)                              \
  base{#x, x}, TROMPELOEIL_INIT_WITH_STR14(base, __VA_ARGS__)

#define TROMPELOEIL_INIT_WITH_STR14(base, x, ...)                              \
  base{#x, x}, TROMPELOEIL_INIT_WITH_STR13(base, __VA_ARGS__)

#define TROMPELOEIL_INIT_WITH_STR13(base, x, ...)                              \
  base{#x, x}, TROMPELOEIL_INIT_WITH_STR12(base, __VA_ARGS__)

#define TROMPELOEIL_INIT_WITH_STR12(base, x, ...)                              \
  base{#x, x}, TROMPELOEIL_INIT_WITH_STR11(base, __VA_ARGS__)

#define TROMPELOEIL_INIT_WITH_STR11(base, x, ...)                              \
  base{#x, x}, TROMPELOEIL_INIT_WITH_STR10(base, __VA_ARGS__)

#define TROMPELOEIL_INIT_WITH_STR10(base, x, ...)                              \
  base{#x, x}, TROMPELOEIL_INIT_WITH_STR9(base, __VA_ARGS__)

#define TROMPELOEIL_INIT_WITH_STR9(base, x, ...)                               \
  base{#x, x}, TROMPELOEIL_INIT_WITH_STR8(base, __VA_ARGS__)

#define TROMPELOEIL_INIT_WITH_STR8(base, x, ...)                               \
  base{#x, x}, TROMPELOEIL_INIT_WITH_STR7(base, __VA_ARGS__)

#define TROMPELOEIL_INIT_WITH_STR7(base, x, ...)                               \
  base{#x, x}, TROMPELOEIL_INIT_WITH_STR6(base, __VA_ARGS__)

#define TROMPELOEIL_INIT_WITH_STR6(base, x, ...)                               \
  base{#x, x}, TROMPELOEIL_INIT_WITH_STR5(base, __VA_ARGS__)

#define TROMPELOEIL_INIT_WITH_STR5(base, x, ...)                               \
  base{#x, x}, TROMPELOEIL_INIT_WITH_STR4(base, __VA_ARGS__)

#define TROMPELOEIL_INIT_WITH_STR4(base, x, ...)                               \
  base{#x, x}, TROMPELOEIL_INIT_WITH_STR3(base, __VA_ARGS__)

#define TROMPELOEIL_INIT_WITH_STR3(base, x, ...)                               \
  base{#x, x}, TROMPELOEIL_INIT_WITH_STR2(base, __VA_ARGS__)

#define TROMPELOEIL_INIT_WITH_STR2(base, x, ...)                               \
  base{#x, x}, TROMPELOEIL_INIT_WITH_STR1(base, __VA_ARGS__)

#define TROMPELOEIL_INIT_WITH_STR1(base, x)                                    \
  base{#x, x}

#define TROMPELOEIL_INIT_WITH_STR0(base)

#define TROMPELOEIL_INIT_WITH_STR(base, ...)                                   \
  TROMPELOEIL_CONCAT(TROMPELOEIL_INIT_WITH_STR,                                \
                     TROMPELOEIL_COUNT(__VA_ARGS__))(base, __VA_ARGS__)

#define TROMPELOEIL_PARAM_LIST15(func_type)                                    \
  TROMPELOEIL_PARAM_LIST14(func_type),                                         \
  ::trompeloeil::param_list_t<func_type, 14> p15

#define TROMPELOEIL_PARAM_LIST14(func_type)                                    \
  TROMPELOEIL_PARAM_LIST13(func_type),                                         \
  ::trompeloeil::param_list_t<func_type, 13> p14

#define TROMPELOEIL_PARAM_LIST13(func_type)                                    \
  TROMPELOEIL_PARAM_LIST12(func_type),                                         \
  ::trompeloeil::param_list_t<func_type, 12> p13

#define TROMPELOEIL_PARAM_LIST12(func_type)                                    \
  TROMPELOEIL_PARAM_LIST11(func_type),                                         \
  ::trompeloeil::param_list_t<func_type, 11> p12

#define TROMPELOEIL_PARAM_LIST11(func_type)                                    \
  TROMPELOEIL_PARAM_LIST10(func_type),                                         \
  ::trompeloeil::param_list_t<func_type, 10> p11

#define TROMPELOEIL_PARAM_LIST10(func_type)                                    \
  TROMPELOEIL_PARAM_LIST9(func_type),                                          \
  ::trompeloeil::param_list_t<func_type, 9> p10

#define TROMPELOEIL_PARAM_LIST9(func_type)                                     \
  TROMPELOEIL_PARAM_LIST8(func_type),                                          \
  ::trompeloeil::param_list_t<func_type, 8> p9

#define TROMPELOEIL_PARAM_LIST8(func_type)                                     \
  TROMPELOEIL_PARAM_LIST7(func_type),                                          \
  ::trompeloeil::param_list_t<func_type, 7> p8

#define TROMPELOEIL_PARAM_LIST7(func_type)                                     \
  TROMPELOEIL_PARAM_LIST6(func_type),                                          \
  ::trompeloeil::param_list_t<func_type, 6> p7

#define TROMPELOEIL_PARAM_LIST6(func_type)                                     \
  TROMPELOEIL_PARAM_LIST5(func_type),                                          \
  ::trompeloeil::param_list_t<func_type, 5> p6

#define TROMPELOEIL_PARAM_LIST5(func_type)                                     \
  TROMPELOEIL_PARAM_LIST4(func_type),                                          \
    ::trompeloeil::param_list_t<func_type, 4> p5


#define TROMPELOEIL_PARAM_LIST4(func_type)                                     \
  TROMPELOEIL_PARAM_LIST3(func_type),                                          \
    ::trompeloeil::param_list_t<func_type, 3> p4

#define TROMPELOEIL_PARAM_LIST3(func_type)                                     \
  TROMPELOEIL_PARAM_LIST2(func_type),                                          \
  ::trompeloeil::param_list_t<func_type, 2> p3

#define TROMPELOEIL_PARAM_LIST2(func_type)                                     \
  TROMPELOEIL_PARAM_LIST1(func_type),                                          \
  ::trompeloeil::param_list_t<func_type, 1> p2

#define TROMPELOEIL_PARAM_LIST1(func_type)                                     \
  ::trompeloeil::param_list_t<func_type, 0> p1

#define TROMPELOEIL_PARAM_LIST0(func_type)

#define TROMPELOEIL_PARAM_LIST(num, func_type)                                 \
  TROMPELOEIL_CONCAT(TROMPELOEIL_PARAM_LIST, num)(func_type)


#define TROMPELOEIL_PARAMS15 TROMPELOEIL_PARAMS14, p15
#define TROMPELOEIL_PARAMS14 TROMPELOEIL_PARAMS13, p14
#define TROMPELOEIL_PARAMS13 TROMPELOEIL_PARAMS12, p13
#define TROMPELOEIL_PARAMS12 TROMPELOEIL_PARAMS11, p12
#define TROMPELOEIL_PARAMS11 TROMPELOEIL_PARAMS10, p11
#define TROMPELOEIL_PARAMS10 TROMPELOEIL_PARAMS9,  p10
#define TROMPELOEIL_PARAMS9  TROMPELOEIL_PARAMS8,  p9
#define TROMPELOEIL_PARAMS8  TROMPELOEIL_PARAMS7,  p8
#define TROMPELOEIL_PARAMS7  TROMPELOEIL_PARAMS6,  p7
#define TROMPELOEIL_PARAMS6  TROMPELOEIL_PARAMS5,  p6
#define TROMPELOEIL_PARAMS5  TROMPELOEIL_PARAMS4,  p5
#define TROMPELOEIL_PARAMS4  TROMPELOEIL_PARAMS3,  p4
#define TROMPELOEIL_PARAMS3  TROMPELOEIL_PARAMS2,  p3
#define TROMPELOEIL_PARAMS2  TROMPELOEIL_PARAMS1,  p2
#define TROMPELOEIL_PARAMS1  ,                     p1
#define TROMPELOEIL_PARAMS0

#define TROMPELOEIL_PARAMS(num) TROMPELOEIL_CONCAT(TROMPELOEIL_PARAMS, num)

namespace trompeloeil
{
  class specialized;

  template <typename T = void>
  std::unique_lock<std::recursive_mutex> get_lock()
  {
    static std::recursive_mutex lock;
    return std::unique_lock<std::recursive_mutex>{ lock };
  }
  template <size_t N, typename T>
  using conditional_tuple_element
    = std::conditional_t<(N < std::tuple_size<T>::value),
                         typename std::tuple_element<N, T>::type,
                         int>;

  template <typename T>
  struct param_list;

  template <typename R, typename ... P>
  struct param_list<R(P...)>
  {
    static size_t constexpr const size = sizeof...(P);
    template <size_t N>
    using type = conditional_tuple_element<N, std::tuple<P...>>;
  };

  template <typename Sig, size_t N>
  using param_list_t = typename param_list<Sig>::template type<N>;

  class expectation_violation : public std::logic_error
  {
  public:
    using std::logic_error::logic_error;
  };

  struct location
  {
    location()
    noexcept
      : file("")
      , line(0U)
    {}

    location(
      char const* file_,
      unsigned long line_
    )
    noexcept
      : file{file_}
      , line{line_}
    {}

    char const *file;
    unsigned long line;
  };

  inline
  std::ostream&
  operator<<(
    std::ostream& os,
    const location& loc)
  {
    if (loc.line != 0U) os << loc.file << ':' << loc.line;
    return os;
  }

  enum class severity { fatal, nonfatal };

  using reporter_func = std::function<void(severity,
                                           char const *file,
                                           unsigned long line,
                                           std::string const &msg)>;

  inline
  void default_reporter(
    severity,
    char const *file,
    unsigned long line,
    std::string const &msg)
  {
    if (!std::current_exception())
    {
      std::stringstream os;
      os << location{ file, line } << "\n" << msg;
      throw expectation_violation(os.str());
    }
  }

  inline
  reporter_func&
  reporter_obj()
  {
    static reporter_func obj = default_reporter;
    return obj;
  }

  inline
  reporter_func set_reporter(
    reporter_func f)
  {
    return std::exchange(reporter_obj(), std::move(f));
  }

  class tracer;

  inline
  tracer*&
  tracer_obj()
    noexcept
  {
    static tracer* ptr = nullptr;
    return ptr;
  }

  inline
  tracer*
  set_tracer(
    tracer* obj)
  noexcept
  {
    // std::exchange would be sane here, but it costs compilation time
    auto& ptr = tracer_obj();
    auto rv = ptr;
    ptr = obj;
    return rv;
  }

  class tracer
  {
  public:
    virtual
    void
    trace(
      char const *file,
      unsigned long line,
      std::string const &call) = 0;
  protected:
    tracer()
    noexcept
      : previous{set_tracer(this)} {}
    tracer(tracer const&) = delete;
    tracer& operator=(tracer const&) = delete;
    virtual
    ~tracer()
    {
      set_tracer(previous);
    }
  private:
    tracer* previous = nullptr;
  };

  class stream_tracer : public tracer
  {
  public:
    stream_tracer(
      std::ostream& stream_)
      : stream(stream_) {}
    void
    trace(
      char const *file,
      unsigned long line,
      std::string const &call)
      override
    {
      stream << location{file, line} << '\n' << call << '\n';
    }
  private:
    std::ostream& stream;
  };

  class trace_agent;

  template <typename T>
  struct reporter;

  template <typename T>
  void
  send_report(
    severity s,
    location loc,
    std::string const &msg)
  {
    reporter<T>::send(s, loc.file, loc.line, msg.c_str());
  }

  template <typename T>
  struct reporter
  {
    static
    void
    send(
      severity s,
      char const *file,
      unsigned long line,
      char const *msg);
  };

  template <typename T>
  void reporter<T>::
    send(
      severity s,
      char const *file,
      unsigned long line,
      char const *msg)
    {
      reporter_obj()(s, file, line, msg);
    }

  template <typename ... T>
  inline
  constexpr
  bool
  ignore(
    T const& ...)
  noexcept
  {
    return true;
  }

  struct illegal_argument
  {
    template <bool b = false>
    constexpr
    illegal_argument const& operator&() const
    {
      static_assert(b, "illegal argument");
      return *this;
    }

    template <bool b = false>
    constexpr
    illegal_argument const& operator*() const
    {
      static_assert(b, "illegal argument");
      return *this;
    }

    template <typename T, bool b = false>
    constexpr
    illegal_argument const& operator=(T const&) const
    {
      static_assert(b, "illegal argument");
      return *this;
    }

    template <typename T, bool b = false>
    constexpr
    operator T() const
    {
      static_assert(b, "illegal argument");
      return {};
    }
  };

  struct matcher { };

  template <typename T>
  using is_matcher = std::is_base_of<matcher, std::decay_t<T>>;

  template <typename T>
  struct typed_matcher : matcher
  {
    operator T() const;
  };

  template <>
  struct typed_matcher<std::nullptr_t> : matcher
  {
    template <typename T, typename = decltype(std::declval<T>() == nullptr)>
    operator T&&() const;

    template <typename T,
              typename = decltype(std::declval<T>() == nullptr),
              typename = std::enable_if_t<std::is_copy_constructible<T>::value>>
    operator T&()const;

    template <typename T, typename C>
    operator T C::*() const;
  };

  template <typename Pred, typename ... T>
  class duck_typed_matcher : public matcher
  {
  public:
    template <typename V,
              typename = decltype(std::declval<Pred>()(std::declval<V&&>(), std::declval<T>()...))>
    operator V&&() const;

    template <typename V,
              typename = decltype(std::declval<Pred>()(std::declval<V&>(), std::declval<T>()...))>
    operator V&() const;
  };

  constexpr inline std::false_type is_output_streamable_(...) { return {}; }

  template <typename T>
  constexpr
  inline
  auto
  is_output_streamable_(
    T*)
  -> decltype((std::declval<std::ostream&>() << std::declval<T&>()),std::true_type{})
  {
    return {};
  }

  template <typename T, size_t N>
  inline
  constexpr
  auto
  is_output_streamable_(
    T(*)[N])
  -> std::false_type
  {
    return {};
  }

  template <typename T>
  inline
  constexpr
  auto
  is_output_streamable()
  {
    return is_output_streamable_(static_cast<T*>(nullptr));
  }

  struct stream_sentry
  {
    stream_sentry(
      std::ostream& os_)
      : os(os_)
      , width(os.width(0))
      , flags(os.flags(std::ios_base::dec | std::ios_base::left))
      , fill(os.fill(' '))
      {  }
    ~stream_sentry()
    {
      os.flags(flags);
      os.fill(fill);
      os.width(width);
    }
  private:
    std::ostream& os;
    std::streamsize width;
    std::ios_base::fmtflags flags;
    char fill;
  };


  template <typename T>
  struct is_null_comparable
  {
    template <typename U>
    static auto func(...)
      -> std::false_type;

    template <typename U>
    static auto func(U* u)
      -> std::integral_constant<decltype(*u == nullptr), !is_matcher<U>::value>;

    using type = decltype(func<T>(nullptr));
    static constexpr type value = {};
  };

  template <typename T>
  inline
  constexpr
  bool
  is_null(
    T const &t,
    std::true_type)
  noexcept(noexcept(std::declval<T>() == nullptr))
  {
    return t == nullptr;
  }

  template <typename T>
  inline
  constexpr
  bool
  is_null(
    T const &,
    std::false_type)
  noexcept
  {
    return false;
  }

  template <typename T>
  inline
  constexpr
  bool
  is_null(
    T const &t)
  {
    return ::trompeloeil::is_null(t,
                                  ::trompeloeil::is_null_comparable<T>::value);
  }

  template <typename T, size_t N>
  inline
  constexpr
  bool
  is_null(
    T(&)[N])
  noexcept
  {
      return false;
  }

  template <typename T>
  void
  print(
    std::ostream& os,
    T const &t);

  template <typename T>
  constexpr
  std::integral_constant<decltype(std::next(std::begin(std::declval<T>()))
                                  != std::end(std::declval<T>())),
                         true>
  is_collection_(T*)
  {
      return {};
  }

  template <typename T>
  constexpr
  auto
  is_collection_(...)
    -> std::false_type
  {
    return {};
  }

  template <typename T, bool b = is_collection_<T>(nullptr)>
  struct array_discriminator : std::true_type  {  };

  template <typename T, size_t N>
  struct array_discriminator<T[N], false> : std::true_type { };

  template <typename T>
  struct array_discriminator<T, false> : std::false_type { };

  template <typename T>
  constexpr
  auto
  is_collection()
  {
    return array_discriminator<T>{};
  }

  template <typename T,
            bool = is_output_streamable<T>(),
            bool = is_collection<std::remove_reference_t<T>>()>
  struct streamer
  {
    static
    void
    print(
      std::ostream& os,
      T const &t)
    {
      stream_sentry s(os);
      os << t;
    }
  };

  template <typename ... T>
  struct streamer<std::tuple<T...>, false, false>
  {
    static
    void
    print(
      std::ostream& os,
      std::tuple<T...> const&  t)
    {
      print_tuple(os, t, std::index_sequence_for<T...>{});
    }
    template <size_t ... I>
    static
    void
    print_tuple(
      std::ostream& os,
      std::tuple<T...> const& t,
      std::index_sequence<I...>)
    {
      os << "{ ";
      const char* sep = "";
      std::initializer_list<const char*> v{((os << sep),
                                            ::trompeloeil::print(os, std::get<I>(t)),
                                            (sep = ", "))...};
      ignore(v);
      os << " }";
    }
  };

  template <typename T, typename U>
  struct streamer<std::pair<T, U>, false, false>
  {
    static
    void
    print(
      std::ostream& os,
      std::pair<T, U> const& t)
    {
      os << "{ ";
      ::trompeloeil::print(os, t.first);
      os << ", ";
      ::trompeloeil::print(os, t.second);
      os << " }";
    }
  };

  template <typename T>
  struct streamer<T, false, true>
  {
    static
    void
    print(
      std::ostream& os,
      T const& t)
    {
      os << "{ ";
      const char* sep = "";
      for (auto& elem : t)
      {
        os << sep;
        ::trompeloeil::print(os, elem);
        sep = ", ";
      }
      os << " }";
    }
  };

  template <typename T>
  struct streamer<T, false, false>
  {
    static
    void
    print(
      std::ostream& os,
      T const &t)
    {
      stream_sentry s(os);
      static const char *linebreak = "\n";
      os << sizeof(T) << "-byte object={";
      os << (linebreak + (sizeof(T) <= 8)); // stupid construction silences VS2015 warining
      os << std::setfill('0') << std::hex;
      auto p = reinterpret_cast<uint8_t const*>(&t);
      for (size_t i = 0; i < sizeof(T); ++i)
      {
        os << " 0x" << std::setw(2) << unsigned(p[i]);
        if ((i & 0xf) == 0xf) os << '\n';
      }
      os << " }";
    }
  };

  template <typename T>
  void
  print(
    std::ostream& os,
    T const &t)
  {
    if (is_null(t))
    {
      os << "nullptr";
    }
    else
    {
      streamer<T>::print(os, t);
    }
  }

  inline
  constexpr
  auto
  param_compare_operator(
    ...)
  {
    return " == ";
  }

  inline
  constexpr
  auto
  param_compare_operator(
    matcher const*)
  {
    return "";
  }

  template <typename T>
  void
  print_expectation(
    std::ostream& os,
    T const& t)
  {
    os << param_compare_operator(&t);
    print(os, t);
    os << '\n';
  }

  template <typename T>
  class list_elem
  {
  public:
    list_elem(
      const list_elem&)
    = delete;

    list_elem(
      list_elem &&r)
    noexcept
    {
      *this = std::move(r);
    }

    list_elem&
    operator=(
      list_elem &&r)
    noexcept
    {
      if (this != &r)
      {
        next = r.next;
        prev = &r;
        r.invariant_check();

        next->prev = this;
        r.next = this;

        TROMPELOEIL_ASSERT(next->prev == this);
        TROMPELOEIL_ASSERT(prev->next == this);

        r.unlink();

        TROMPELOEIL_ASSERT(!r.is_linked());
        invariant_check();
      }
      return *this;
    }

    list_elem&
    operator=(
      const list_elem&)
    = delete;

    virtual
    ~list_elem()
    {
      unlink();
    }

    void
    unlink()
    noexcept
    {
      invariant_check();
      auto n = next;
      auto p = prev;
      n->prev = p;
      p->next = n;
      next = this;
      prev = this;
      invariant_check();
    }

    void
    invariant_check()
    const
    noexcept
    {
#ifdef TROMPELOEIL_SANITY_CHECKS
      TROMPELOEIL_ASSERT(next->prev == this);
      TROMPELOEIL_ASSERT(prev->next == this);
      TROMPELOEIL_ASSERT((next == this) == (prev == this));
      TROMPELOEIL_ASSERT((prev->next == next) == (next == this));
      TROMPELOEIL_ASSERT((next->prev == prev) == (prev == this));
      auto pp = prev;
      auto nn = next;
      do {
        TROMPELOEIL_ASSERT((nn == this) == (pp == this));
        TROMPELOEIL_ASSERT(nn->next->prev == nn);
        TROMPELOEIL_ASSERT(nn->prev->next == nn);
        TROMPELOEIL_ASSERT(pp->next->prev == pp);
        TROMPELOEIL_ASSERT(pp->prev->next == pp);
        TROMPELOEIL_ASSERT((nn->next == nn) == (nn == this));
        TROMPELOEIL_ASSERT((pp->prev == pp) == (pp == this));
        nn = nn->next;
        pp = pp->prev;
      } while (nn != this);
#endif
    }

    bool
    is_linked()
    const
    noexcept
    {
      invariant_check();
      return next != this;
    }
  protected:
    list_elem() noexcept = default;
  public:
    list_elem* next = this;
    list_elem* prev = this;
  };

  class ignore_disposer
  {
  protected:
    template <typename T>
    TROMPELOEIL_NORETURN
    void
    dispose(
      T*)
    const
    noexcept
    {
      std::abort(); // must never be called
    }
  };

  class delete_disposer
  {
  protected:
    template <typename T>
    void
    dispose(
      T* t)
    const
    {
      delete t;
    }
  };

  template <typename T, typename Disposer = ignore_disposer>
  class list : private list_elem<T>, private Disposer
  {
  public:
    list() noexcept;
    list(list&&) noexcept;
    list(const list&) = delete;
    list& operator=(list&&) noexcept;
    list& operator=(const list&) = delete;
    ~list();
    class iterator;
    iterator begin() const noexcept;
    iterator end() const noexcept;
    iterator push_front(T* t) noexcept;
    iterator push_back(T* t) noexcept;
    bool empty() const noexcept { return begin() == end(); }
  private:
    using list_elem<T>::invariant_check;
    using list_elem<T>::next;
    using list_elem<T>::prev;
  };

  template <typename T, typename Disposer>
  class list<T, Disposer>::iterator
    : public std::iterator<std::bidirectional_iterator_tag, T>
  {
    friend class list<T, Disposer>;
  public:
    iterator()
    noexcept
      : p{nullptr}
    {}

    friend
    bool
    operator==(
      iterator const &lh,
      iterator const &rh)
    noexcept
    {
      return lh.p == rh.p;
    }

    friend
    bool
    operator!=(
      iterator const &lh,
      iterator const &rh)
    noexcept
    {
      return !(lh == rh);
    }

    iterator&
    operator++()
    noexcept
    {
      p = p->next;
      return *this;
    }

    iterator
    operator++(int)
    noexcept
    {
      auto rv = *this;
      operator++();
      return rv;
    }

    T&
    operator*()
    noexcept
    {
      return static_cast<T&>(*p);
    }

    T*
    operator->()
    noexcept
    {
      return static_cast<T*>(p);
    }

  private:
    iterator(
      list_elem<T> const *t)
    noexcept
    : p{const_cast<list_elem<T>*>(t)}
    {}

    list_elem<T>* p;
  };

  template <typename T, typename Disposer>
  list<T, Disposer>::list() noexcept = default;

  template <typename T, typename Disposer>
  list<T, Disposer>::list(list&&) noexcept = default;

  template <typename T, typename Disposer>
  list<T, Disposer>& list<T, Disposer>::operator=(list&&) noexcept = default;

  template <typename T, typename Disposer>
  list<T, Disposer>::~list()
  {
    auto i = this->begin();
    while (i != this->end())
    {
      auto p = i++;
      Disposer::dispose(&*p);
    }
  }

  template <typename T, typename Disposer>
  auto
  list<T, Disposer>::begin()
  const
  noexcept
  -> iterator
  {
    return {next};
  }

  template <typename T, typename Disposer>
  auto
  list<T, Disposer>::end()
  const
  noexcept
  -> iterator
  {
    return {this};
  }

  template <typename T, typename Disposer>
  auto
  list<T, Disposer>::push_front(
    T* t)
  noexcept
  -> iterator
  {
    invariant_check();
    t->next = next;
    t->prev = this;
    next->prev = t;
    next = t;
    invariant_check();
    return {t};
  }

  template <typename T, typename Disposer>
  auto
  list<T, Disposer>::push_back(
    T* t)
  noexcept
  -> iterator
  {
    invariant_check();
    t->prev = prev;
    t->next = this;
    prev->next = t;
    prev = t;
    invariant_check();
    return {t};
  }

  class sequence_matcher;

  class sequence_type
  {
  public:
    sequence_type() noexcept = default;
    sequence_type(sequence_type&&) noexcept = delete;
    sequence_type(const sequence_type&) = delete;
    sequence_type& operator=(sequence_type&&) noexcept = delete;
    sequence_type& operator=(const sequence_type&) = delete;
    ~sequence_type();

    bool
    is_first(
      sequence_matcher const *m)
    const
    noexcept;

    void
    add_last(
      sequence_matcher *m)
    noexcept;

    void
    validate_match(
      severity s,
      sequence_matcher const *matcher,
      char const *seq_name,
      char const *match_name,
      location loc)
    const;

  private:
    list<sequence_matcher> matchers;
  };

  class sequence
  {
  public:
    sequence() : obj(new sequence_type) {}
    sequence_type& operator*() { return *obj; }
  private:
    std::unique_ptr<sequence_type> obj;
  };

  class sequence_matcher : public list_elem<sequence_matcher>
  {
  public:
    using init_type = std::pair<char const*, sequence&>;
    sequence_matcher(
      char const *exp,
      location loc,
      init_type i)
    noexcept
    : seq_name(i.first)
    , exp_name(exp)
    , exp_loc(loc)
    , seq(*i.second)
    {
      seq.add_last(this);
    }

    void
    validate_match(
      severity s,
      char const *match_name,
      location loc)
    const
    {
      seq.validate_match(s, this, seq_name, match_name, loc);
    }

    bool
    is_first()
    const
    noexcept
    {
      return seq.is_first(this);
    }

    void
    retire()
    noexcept
    {
      this->unlink();
    }

    void
    print_expectation(std::ostream& os)
    const
    {
      os << exp_name << " at " << exp_loc;
    }

    char const*
    sequence_name()
    noexcept
    {
      return seq_name;
    }
  private:
    char const *seq_name;
    char const *exp_name;
    location    exp_loc;
    sequence_type& seq;
  };

  inline
  bool
  sequence_type::is_first(
    sequence_matcher const *m)
  const
  noexcept
  {
    return !matchers.empty() && &*matchers.begin() == m;
  }

  inline
  void
  sequence_type::validate_match(
    severity s,
    sequence_matcher const *matcher,
    char const* seq_name,
    char const* match_name,
    location loc)
  const
  {
    if (is_first(matcher)) return;
    for (auto& m : matchers)
    {
      std::ostringstream os;
      os << "Sequence mismatch for sequence \"" << seq_name
         << "\" with matching call of " << match_name
         << " at " << loc
         << ". Sequence \"" << seq_name << "\" has ";
      m.print_expectation(os);
      os << " first in line\n";
      send_report<specialized>(s, loc, os.str());
    }
  }

  inline
  sequence_type::~sequence_type()
  {
    bool touched = false;
    std::ostringstream os;
    while (!matchers.empty())
    {
      auto m = matchers.begin();
      if (!touched)
      {
        os << "Sequence expectations not met at destruction of sequence object \""
           << m->sequence_name() << "\":";
        touched = true;
      }
      os << "\n  missing ";
      m->print_expectation(os);
      m->unlink();
    }
    if (touched)
    {
      os << "\n";
      send_report<specialized>(severity::nonfatal, location{}, os.str());
    }
  }

  inline
  void
  sequence_type::add_last(
    sequence_matcher *m)
  noexcept
  {
    matchers.push_back(m);
  }

  struct wildcard : public matcher
  {
    // This abomination of constructor seems necessary for g++ 4.9 and 5.1
    template <typename ... T>
    constexpr
    wildcard(
      T&& ...)
    noexcept
    {}

    template<typename T,
             typename = std::enable_if_t<!std::is_lvalue_reference<T>::value>>
    operator T&&()
    const;

    template<typename T,
             typename = std::enable_if_t<std::is_copy_constructible<T>::value
                                         || !std::is_move_constructible<T>::value>>
    operator T&()
    const;

    template <typename T>
    constexpr
    bool
    matches(
      T const&)
    const
    noexcept
    {
      return true;
    }

    friend
    std::ostream&
    operator<<(
      std::ostream& os,
      wildcard const&)
    noexcept
    {
      return os << " matching _";
    }
  };


  static constexpr wildcard const _{};

  template <typename T>
  void can_match_parameter(T&);

  template <typename T>
  void can_match_parameter(T&&);

  template <typename M>
  class ptr_deref : public matcher
  {
  public:
    template <typename U,
              typename = decltype(can_match_parameter<std::remove_reference_t<decltype(*std::declval<U>())>>(std::declval<M>()))>
    operator U() const;

    template <typename U>
    explicit
    ptr_deref(
      U&& m_)
      : m( std::forward<U>(m_) )
    {}

    template <typename U>
    bool
    matches(
      const U& u)
    const
    noexcept(noexcept(std::declval<M>().matches(*u)))
    {
      return (u != nullptr) && m.matches(*u);
    }

    friend
    std::ostream&
    operator<<(
      std::ostream& os,
      ptr_deref<M> const& p)
    {
      return os << p.m;
    }
  private:
    M m;
  };

  template <typename M>
  class neg_matcher : public matcher
  {
  public:
    template <typename U,
              typename = decltype(can_match_parameter<std::remove_reference_t<decltype(std::declval<U>())>>(std::declval<M>()))>
    operator U() const;

    template <typename U>
    explicit
    neg_matcher(
      U&& m_)
      : m( std::forward<U>(m_) )
    {}

    template <typename U>
    bool
    matches(
      const U& u)
    const
    noexcept(noexcept(!std::declval<M>().matches(u)))
    {
      return !m.matches(u);
    }

    friend
    std::ostream&
    operator<<(
      std::ostream& os,
      neg_matcher<M> const& p)
    {
      return os << p.m;
    }
  private:
    M m;
  };

  template <typename MatchType, typename Predicate, typename ... ActualType>
  struct matcher_kind
  {
    using type = typed_matcher<MatchType>;
  };

  template <typename Predicate, typename ... ActualType>
  struct matcher_kind<wildcard, Predicate, ActualType...>
  {
    using type = duck_typed_matcher<Predicate, ActualType...>;
  };

  template <typename MatchType, typename Predicate, typename ... ActualType>
  using matcher_kind_t =
    typename matcher_kind<MatchType, Predicate, ActualType...>::type;

  template <typename Predicate, typename Printer, typename MatcherType, typename ... T>
  class predicate_matcher
    : private Predicate
    , private Printer
    , public MatcherType
  {
  public:
    template <typename ... U>
    constexpr
    predicate_matcher(
      Predicate&& pred,
      Printer&& printer,
      U&& ... v)
      noexcept(noexcept(std::tuple<T...>(std::declval<U>()...)) && noexcept(Predicate(std::declval<Predicate&&>())) && noexcept(Printer(std::declval<Printer&&>())))
      : Predicate(std::move(pred))
      , Printer(std::move(printer))
      , value(std::forward<U>(v)...)
    {}

    template <typename V>
    constexpr
    bool
    matches(
      V&& v)
      const
      noexcept(noexcept(std::declval<Predicate const&>()(std::declval<V&&>(), std::declval<const T&>()...)))
    {
      return matches_(std::forward<V>(v), std::make_index_sequence<sizeof...(T)>{});
    }

    friend
    std::ostream&
    operator<<(
      std::ostream& os,
      predicate_matcher const& v)
    {
      return v.print_(os, std::make_index_sequence<sizeof...(T)>{});
    }
  private:
    // The below function call operator must be declared to
    // work around gcc bug 78446
    //
    // For some reason microsoft compiler from VS2015 update 3
    // requires the function call operator to be private to avoid
    // ambiguities.
    template <typename ... U>
    void operator()(U&&...) const = delete;

    template <typename V, size_t ... I>
    bool matches_(V&& v, std::index_sequence<I...>) const
    {
      return Predicate::operator()(std::forward<V>(v), std::get<I>(value)...);
    }

    template <size_t ... I>
    std::ostream& print_(std::ostream& os_, std::index_sequence<I...>) const
    {
      Printer::operator()(os_, std::get<I>(value)...);
      return os_;
    }

    std::tuple<T...> value;
  };

  namespace lambdas {
    // The below must be classes/structs to work with VS 2015 update 3
    // since it doesn't respect the trailing return type declaration on
    // the lambdas of template deduction context

    #define TROMPELOEIL_MK_PRED_BINOP(name, op)                         \
    struct name {                                                       \
      template <typename X, typename Y>                                 \
      auto operator()(X const& x, Y const& y) const -> decltype(x op y) \
      {                                                                 \
        ::trompeloeil::ignore(x,y);                                     \
        return x op y;                                                  \
        }                                                               \
    }
    TROMPELOEIL_MK_PRED_BINOP(equal, ==);
    TROMPELOEIL_MK_PRED_BINOP(not_equal, !=);
    TROMPELOEIL_MK_PRED_BINOP(less, <);
    TROMPELOEIL_MK_PRED_BINOP(less_equal, <=);
    TROMPELOEIL_MK_PRED_BINOP(greater, >);
    TROMPELOEIL_MK_PRED_BINOP(greater_equal, >=);
    #undef TROMPELOEIL_MK_PRED_BINOP
  }
  template <typename MatchType, typename Predicate, typename Printer, typename ... T>
  inline
  predicate_matcher <Predicate, Printer, matcher_kind_t<MatchType, Predicate, std::decay_t<T>...>, std::decay_t<T>...>
  make_matcher(Predicate pred, Printer print, T&& ... t)
  {
    return {std::move(pred), std::move(print), std::forward<T>(t)...};
  }

  template <typename T>
  inline
  auto
  any_matcher(char const* type_name)
  {
    return make_matcher<T>([](auto&&) { return true; },
                           [type_name](std::ostream& os)
                           { os << " matching ANY(" << type_name << ")";});
  }
  template <typename T = wildcard, typename V>
  inline
  auto
  eq(
    V&& v)
  {
    return make_matcher<T>(lambdas::equal(),
                           [](std::ostream& os, auto const& value) {
                             os << " == ";
                             ::trompeloeil::print(os, value);
                           },
                           std::forward<V>(v));
  }
  template <typename  T = wildcard, typename V>
  inline
  auto
  ne(
    V&& v)
  {
    return make_matcher<T>(lambdas::not_equal(),
                           [](std::ostream& os, auto const& value) {
                             os << " != ";
                             ::trompeloeil::print(os, value);
                           },
                           std::forward<V>(v));
  }


  template <typename T = wildcard, typename V>
  inline
  auto
  ge(
    V&& v)
  {
    return make_matcher<T>(lambdas::greater_equal(),
                           [](std::ostream& os, auto const& value) {
                             os << " >= ";
                             ::trompeloeil::print(os, value);
                           },
                           std::forward<V>(v));
  }

  template <typename T = wildcard, typename V>
  inline
  auto
  gt(
    V&& v)
  {
    return make_matcher<T>(lambdas::greater(),
                           [](std::ostream& os, auto const& value) {
                             os << " > ";
                             ::trompeloeil::print(os, value);
                           },
                           std::forward<V>(v));
  }

  template <typename T = wildcard, typename V>
  inline
  auto
  lt(
    V&& v)
  {
    return make_matcher<T>(lambdas::less(),
                           [](std::ostream& os, auto const& value) {
                             os << " < ";
                             ::trompeloeil::print(os, value);
                           },
                           std::forward<V>(v));
  }

  template <typename T = wildcard, typename V>
  inline
  auto
  le(
    V&& v)
  {
    return make_matcher<T>(lambdas::less_equal(),
                           [](std::ostream& os, auto const& value) {
                             os << " <= ";
                             ::trompeloeil::print(os, value);
                           },
                           std::forward<V>(v));
  }

  namespace lambdas {


    inline
    auto
    regex_check(
      std::regex&& re,
      std::regex_constants::match_flag_type match_type)
    {

      class string_helper // a vastly simplified string_view type of class
      {
      public:
        string_helper(
          std::string const& s)
        noexcept
          : str(s.c_str())
        {}

        constexpr
        string_helper(
          char const* s)
        noexcept
          : str(s)
        {}

        char const*
        c_str()
          const
          noexcept
        {
          return str;
        }
      private:
        char const* str;
      };

      return [re, match_type](string_helper str, auto const&)
        -> decltype(std::regex_search(str.c_str(), re, match_type))
        {
          return !::trompeloeil::is_null(str.c_str())
            && std::regex_search(str.c_str(), re, match_type);
        };
    }

    inline auto regex_printer()
    {
      return [](std::ostream& os, auto const& str)
        {
          os << " matching regular expression /" << str << "/";
        };
    }
  }
  template <typename Kind = wildcard>
  auto
  re(
    std::string s,
    std::regex_constants::syntax_option_type opt = std::regex_constants::ECMAScript,
    std::regex_constants::match_flag_type match_type = std::regex_constants::match_default)
  {
    return make_matcher<Kind>(lambdas::regex_check(std::regex(s, opt),
                                                   match_type),
                              lambdas::regex_printer(),
                              std::move(s));
  }

  template <typename Kind = wildcard>
  auto
  re(
    std::string s,
    std::regex_constants::match_flag_type match_type)
  {
    return make_matcher<Kind>(lambdas::regex_check(std::regex(s), match_type),
                              lambdas::regex_printer(),
                              std::move(s));
  }

  inline
  std::string
  param_name_prefix(
    ...)
  {
    return "";
  }

  template <typename M>
  std::string
  param_name_prefix(
    const ptr_deref<M>*)
  {
    return "*" + ::trompeloeil::param_name_prefix(static_cast<M*>(nullptr));
  }

  template <typename M>
  std::string
  param_name_prefix(
    const neg_matcher<M>*)
  {
    return "not " + ::trompeloeil::param_name_prefix(static_cast<M*>(nullptr));
  }

  template <typename T>
  struct null_on_move
  {
  public:
    null_on_move()
    noexcept
      : p{nullptr}
    {}

    null_on_move(
      T* p_)
    noexcept
      : p{p_}
    {}

    null_on_move(
      null_on_move&&)
    noexcept
      : p{nullptr}
    {}

    null_on_move(
      null_on_move const&)
    noexcept
      : p{nullptr}
    {}

    null_on_move&
    operator=(
      const null_on_move&)
    noexcept
    {
      p = nullptr;
      return *this;
    }

    null_on_move&
    operator=(
      null_on_move&&)
    noexcept
    {
      p = nullptr;
      return *this;
    }

    null_on_move&
    operator=(
      T* t)
    noexcept
    {
      p = t;
      return *this;
    }

    T*&
    leak()
    noexcept
    {
      return p;
    }

    T&
    operator*()
    const
    noexcept
    {
      return *p;
    }

    T*
    operator->()
    const
    noexcept
    {
      return p;
    }

    explicit
    operator bool()
    const
    noexcept
    {
      return p != nullptr;
    }
  private:
    T* p;
  };

  struct sequence_handler_base
  {
    virtual
    ~sequence_handler_base()
    noexcept = default;

    virtual
    void
      validate(severity s, char const *, location) = 0;

    virtual
    bool
      is_first()
      const
      noexcept = 0;

    virtual
    void
      retire()
      noexcept = 0;
  };

  template <size_t N>
  struct sequence_handler : public sequence_handler_base
  {
  public:
    template <typename ... S>
    sequence_handler(
      char const *name,
      location loc,
      S&& ... s)
    noexcept
      : matchers{{name, loc, std::forward<S>(s)}...}
    {
    }

    void
    validate(
      severity s,
      char const *match_name,
      location loc)
    override
    {
      for (auto& e : matchers)
      {
        e.validate_match(s, match_name, loc);
      }
    }

    bool
    is_first()
    const
    noexcept
    override
    {
      // std::all_of() is almost always preferable. The only reason
      // for using a hand rolled loop is because it cuts compilation
      // times quite noticeably (almost 10% with g++5.1)
      for (auto& m : matchers)
      {
        if (!m.is_first()) return false;
      }
      return true;
    }

    void
    retire()
    noexcept
    override
    {
      for (auto& e : matchers)
      {
        e.retire();
      }
    }
  private:
    sequence_matcher matchers[N];
  };

  struct lifetime_monitor;

  template <typename T>
  class deathwatched : public T
  {
    static_assert(std::has_virtual_destructor<T>::value,
                  "virtual destructor is a necessity for deathwatched to work");
  public:
    template <typename ... U,
              typename = std::enable_if_t<std::is_constructible<T,U...>::value>>
    deathwatched(
      U&& ...u)
    noexcept(noexcept(T(std::declval<U>()...)))
      : T(std::forward<U>(u)...)
    {}

    ~deathwatched();

    trompeloeil::lifetime_monitor*&
    trompeloeil_expect_death(
      trompeloeil::lifetime_monitor* monitor)
    const
    noexcept
    {
      auto lock = get_lock();
      trompeloeil_lifetime_monitor = monitor;
      return trompeloeil_lifetime_monitor.leak();
    }
  private:
    mutable null_on_move<trompeloeil::lifetime_monitor> trompeloeil_lifetime_monitor;
  };

  struct expectation {
    virtual ~expectation() = default;
  };

  struct lifetime_monitor : public expectation
  {
    template <typename T>
    lifetime_monitor(
      ::trompeloeil::deathwatched<T> const &obj,
      char const* obj_name_,
      char const* invocation_name_,
      char const* call_name_,
      location loc_)
    noexcept
      : object_monitor(obj.trompeloeil_expect_death(this))
      , loc(loc_)
      , object_name(obj_name_)
      , invocation_name(invocation_name_)
      , call_name(call_name_)
    {
    }

    lifetime_monitor(lifetime_monitor const&) = delete;

    ~lifetime_monitor()
    {
      auto lock = get_lock();
      if (!died)
      {
        std::ostringstream os;
        os << "Object " << object_name << " is still alive";
        send_report<specialized>(severity::nonfatal, loc, os.str());
        object_monitor = nullptr; // prevent its death poking this cadaver
      }
    }

    void
    notify()
    noexcept
    {
      died = true;
      if (sequences) sequences->validate(severity::nonfatal, call_name, loc);
    }

    template <typename ... T>
    void
    set_sequence(
      T&& ... t)
    {
      auto seq = new sequence_handler<sizeof...(T)>(invocation_name,
                                                    loc,
                                                    std::forward<T>(t)...);
      sequences.reset(seq);
    }
  private:
    bool               died = false;
    lifetime_monitor *&object_monitor;
    location           loc;
    char const        *object_name;
    char const        *invocation_name;
    char const        *call_name;
    std::unique_ptr<sequence_handler_base>  sequences;
  };

  template <typename T>
  deathwatched<T>::~deathwatched()
  {
    auto lock = get_lock();
    if (trompeloeil_lifetime_monitor)
    {
      trompeloeil_lifetime_monitor->notify();
      return;
    }
    std::ostringstream os;
    os << "Unexpected destruction of "
       << typeid(T).name() << "@" << this << '\n';
    send_report<specialized>(severity::nonfatal,
                             location{},
                             os.str());
  }

  template<typename T>
  struct return_of;

  template<typename R, typename ... A>
  struct return_of<R(A...)>
  {
    using type = R;
  };

  template<typename T>
  using return_of_t = typename return_of<T>::type;

  template<typename T>
  struct call_params_type;

  template<typename R, typename ... T>
  struct call_params_type<R(T...)>
  {
    using type = std::tuple<typename std::add_lvalue_reference<T>::type...>;
  };

  template<typename T>
  using call_params_type_t = typename call_params_type<T>::type;

  template <typename R>
  struct default_return_t
  {
    TROMPELOEIL_NORETURN static R value()
    {
      std::abort(); // must never be called
    }
  };

  template <typename R>
  inline
  R
  default_return()
  {
    return default_return_t<R>::value();
  }

  template <>
  inline
  void
  default_return<void>()
  {
  }

  template<typename Sig>
  struct call_matcher_base;

  template <typename Sig>
  struct call_matcher_list : public list<call_matcher_base<Sig>>
  {
    void decommission()
    {
      auto lock = get_lock();
      auto iter = this->begin();
      auto const e = this->end();
      while (iter != e)
      {
        auto i = iter++;
        auto &m = *i;
        m.mock_destroyed();
        m.unlink();
      }
    }
  };

  template<typename Sig>
  struct call_matcher_base : public list_elem<call_matcher_base<Sig> >
  {
    call_matcher_base(
      location loc_,
      char const* name_)
    : loc{loc_}
    , name{name_}
    {
    }

    call_matcher_base(call_matcher_base&&) = delete;

    virtual
    ~call_matcher_base() = default;

    virtual
    void
    mock_destroyed() = 0;

    virtual
    bool
    matches(
      call_params_type_t<Sig> const&)
    const = 0;

    virtual
    bool
    first_in_sequence()
    const
    noexcept = 0;

    virtual
    void
    run_actions(
      call_params_type_t<Sig> &,
      call_matcher_list<Sig> &saturated_list
    ) = 0;

    virtual
    std::ostream&
    report_signature(
      std::ostream&)
    const = 0;

    TROMPELOEIL_NORETURN
    virtual
    std::ostream&
    report_mismatch(
      std::ostream&,
      call_params_type_t<Sig> const &) = 0;

    virtual
    return_of_t<Sig>
    return_value(
      trace_agent&,
      call_params_type_t<Sig>& p) = 0;

    virtual
    void
    report_missed(
      char const *reason) = 0;

    location loc;
    char const *name;
  };

  template <typename T, typename U>
  bool
  param_matches_impl(
    T const& t,
    U const& u,
    matcher const*)
  noexcept(noexcept(t.matches(u)))
  {
    return t.matches(u);
  }

  template <typename T, typename U>
  struct is_equal_comparable
  {
    template <typename P, typename V>
    static constexpr std::false_type func(...) { return {}; }

    template <typename P, typename V>
    static constexpr auto func(P* p, V* v) -> decltype((*p == *v), std::true_type{})
    {
      return ::trompeloeil::ignore(p,v),std::true_type{};
    }

    static constexpr auto value = decltype(func<T, U>(nullptr, nullptr))::value;
    // The obvious solution, to just call func<T,U>(0,0) gives true_type* in VS!!!
  };

  template <typename T, typename U>
  inline
  std::enable_if_t<is_equal_comparable<T, U>::value, U&>
  identity(
    U& t)
  noexcept
  {
    return t;
  }

  template <typename T, typename U>
  inline
  std::enable_if_t<!is_equal_comparable<T, U>::value, T>
  identity(
    const U& u)
  noexcept(noexcept(T(u)))
  {
    return u;
  }

  template <typename T, typename U>
  bool
  param_matches_impl(
    T const& t,
    U const& u,
    ...)
  noexcept(noexcept(::trompeloeil::identity<U>(t) == u))
  {
    return ::trompeloeil::identity<U>(t) == u;
  }

  template <typename T, typename U>
  bool
  param_matches(
    T const& t,
    U const& u)
  noexcept(noexcept(param_matches_impl(t, u, &t)))
  {
    return ::trompeloeil::param_matches_impl(t, u, &t);
  }

  template <size_t ... I, typename T, typename U>
  bool
  match_parameters(
    std::index_sequence<I...>,
    T const& t,
    U const& u)
    noexcept(noexcept(std::initializer_list<bool>{trompeloeil::param_matches(std::get<I>(t),std::get<I>(u))...}))
  {
    bool all_true = true;
    ::trompeloeil::ignore(t, u); // Kills unmotivated VS2015 warning in the empty case
    ::trompeloeil::ignore(std::initializer_list<bool>{all_true = all_true && ::trompeloeil::param_matches(std::get<I>(t), std::get<I>(u))...});
    return all_true;
  }

  template <typename ... T, typename ... U>
  bool
  match_parameters(
    std::tuple<T...> const& t,
    std::tuple<U...> const& u)
  noexcept(noexcept(match_parameters(std::make_index_sequence<sizeof...(T)>{}, t, u)))
  {
    return ::trompeloeil::match_parameters(std::make_index_sequence<sizeof...(T)>{}, t, u);
  }

  template <typename V, typename P>
  void print_mismatch(
    std::ostream& os,
    size_t num,
    V const& v,
    P const& p)
  {
    if (!::trompeloeil::param_matches(v, p))
    {
      auto prefix = ::trompeloeil::param_name_prefix(&v) + "_";
      os << "  Expected " << std::setw((num < 9) + 1) << prefix << num+1;
      ::trompeloeil::print_expectation(os, v);
    }
  }

  template <typename ... V, typename ... P, size_t ... I>
  void print_mismatch(
    std::ostream& os,
    std::index_sequence<I...>,
    std::tuple<V...> const& v,
    std::tuple<P...> const& p)
  {
    ::trompeloeil::ignore(os, v, p);  // Kills unmotivated VS2015 warning in the empty case
    ::trompeloeil::ignore(std::initializer_list<int>{(print_mismatch(os, I, std::get<I>(v), std::get<I>(p)),0)...});
  }

  template <typename ... V, typename ... P>
  void print_mismatch(
    std::ostream& os,
    std::tuple<V...> const& v,
    std::tuple<P...> const& p)
  {
    print_mismatch(os, std::make_index_sequence<sizeof...(V)>{}, v, p);
  }

  template <typename T>
  void missed_value(
    std::ostream& os,
    int i,
    T const& t)
  {
    auto prefix = ::trompeloeil::param_name_prefix(&t) + "_";
    os << "  param " << std::setw((i < 9) + 1) << prefix << i + 1
       << ::trompeloeil::param_compare_operator(&t);
    ::trompeloeil::print(os, t);
    os << '\n';
  }

  template <size_t ... I, typename ... T>
  void stream_params(
    std::ostream &os,
    std::index_sequence<I...>,
    std::tuple<T...> const &t)
  {
    ::trompeloeil::ignore(os, t);  // Kills unmotivated VS2015 warning in the empty case
    ::trompeloeil::ignore(std::initializer_list<int>{(missed_value(os, I, std::get<I>(t)),0)...});
  }

  template <typename ... T>
  void
  stream_params(
    std::ostream &os,
    std::tuple<T...> const &t)
  {
    stream_params(os, std::make_index_sequence<sizeof...(T)>{}, t);
  }

  template <typename ... T>
  std::string
  params_string(
    std::tuple<T...> const& t)
  {
    std::ostringstream os;
    stream_params(os, t);
    return os.str();
  }

  class trace_agent
  {
  public:
    trace_agent(
      location loc_,
      char const* name_,
      tracer* t_)
    : loc{loc_}
    , t{t_}
    {
      if (t)
      {
        os << name_ << " with.\n";
      }
    }

    trace_agent(trace_agent const&) = delete;

    trace_agent(trace_agent &&) = delete;

    ~trace_agent()
    {
      if (t)
      {
        t->trace(loc.file, loc.line, os.str());
      }
    }

    trace_agent&
    operator=(trace_agent const&) = delete;

    trace_agent&
    operator=(trace_agent &&) = delete;

    template <typename ... T>
    void
    trace_params(
      std::tuple<T...> const& params)
    {
      if (t)
      {
        stream_params(os, params);
      }
    }

    template <typename T>
    auto
    trace_return(
      T&& rv)
    -> T
    {
      if (t)
      {
        os << " -> ";
        print(os, rv);
        os << '\n';
      }
      return std::forward<T>(rv);
    }

    void
    trace_exception()
    {
      if (t)
      {
        try {
          throw;
        }
        catch (std::exception& e)
        {
          os << "threw exception: what() = " << e.what() << '\n';
        }
        catch (...)
        {
          os << "threw unknown exception\n";
        }
      }
    }
  private:
    location loc;
    tracer* t;
    std::ostringstream os;
  };

  template <typename Sig>
  call_matcher_base <Sig> *
  find(
    call_matcher_list <Sig> &list,
    call_params_type_t <Sig> const &p)
  noexcept
  {
    call_matcher_base<Sig>* first_match = nullptr;
    for (auto& i : list)
    {
      if (i.matches(p))
      {
        if (i.first_in_sequence())
        {
          return &i;
        }
        if (!first_match)
        {
          first_match = &i;
        }
      }
    }
    return first_match;
  }

  template <typename Sig>
  TROMPELOEIL_NORETURN
  void
  report_mismatch(
    call_matcher_list <Sig> &matcher_list,
    call_matcher_list <Sig> &saturated_list,
    std::string const &name,
    call_params_type_t <Sig> const &p)
  {
    std::ostringstream os;
    os << "No match for call of " << name << " with.\n";
    stream_params(os, p);
    bool saturated_match = false;
    for (auto& m : saturated_list)
    {
      if (m.matches(p))
      {
        if (!saturated_match)
        {
          os << "\nMatches saturated call requirement\n";
          saturated_match = true;
        }
        os << "  ";
        m.report_signature(os) << '\n';
      }
    }
    if (!saturated_match)
    {
      for (auto& m : matcher_list)
      {
        os << "\nTried ";
        m.report_mismatch(os, p);
      }
    }
    send_report<specialized>(severity::fatal, location{}, os.str());
    std::abort(); // must never get here.
  }

  template <typename Sig>
  class return_handler
  {
  public:
    virtual
    ~return_handler() = default;

    virtual
    return_of_t<Sig>
    call(
      trace_agent&,
      call_params_type_t<Sig>& params) = 0;
  };


  template <typename Ret, typename F, typename P, typename = std::enable_if_t<std::is_void<Ret>::value>>
  void
  trace_return(
    trace_agent&,
    F& func,
    P& params)
  {
    func(params);
  }

  template <typename Ret, typename F, typename P, typename = std::enable_if_t<!std::is_void<Ret>::value>>
  Ret
  trace_return(
    trace_agent& agent,
    F& func,
    P& params)
  {
    return agent.trace_return(func(params));
  }

  template <typename Sig, typename T>
  class return_handler_t : public return_handler<Sig>
  {
  public:
    template <typename U>
    return_handler_t(
      U&& u)
    : func(std::forward<U>(u))
    {}

    return_of_t<Sig>
    call(
      trace_agent& agent,
      call_params_type_t<Sig>& params)
    override
    {
      return trace_return<return_of_t<Sig>>(agent, func, params);
    }
  private:
    T func;
  };

  template<typename Sig>
  class condition_base : public list_elem<condition_base<Sig> >
  {
  public:
    condition_base(
      char const *n)
    noexcept
      : id(n)
    {}

    virtual
    ~condition_base() = default;

    virtual
    bool
    check(
      call_params_type_t<Sig> const&)
    const = 0;

    virtual
    char const*
    name()
    const
    noexcept
    {
      return id;
    }
  private:
    char const *id;
  };

  template <typename Sig>
  using condition_list = list<condition_base<Sig>, delete_disposer >;

  template<typename Sig, typename Cond>
  struct condition : public condition_base<Sig>
  {
    condition(
      char const *str_,
      Cond c_)
      : condition_base<Sig>(str_)
      , c(c_) {}

    bool
    check(
      call_params_type_t<Sig> const & t)
    const
    override
    {
      return c(t);
    }

  private:
    Cond c;
  };

  template<typename Sig>
  struct side_effect_base : public list_elem<side_effect_base<Sig> >
  {
    virtual
    ~side_effect_base() = default;

    virtual
    void
    action(
      call_params_type_t<Sig> &)
    const = 0;
  };

  template<typename Sig>
  using side_effect_list = list<side_effect_base<Sig>, delete_disposer>;

  template<typename Sig, typename Action>
  struct side_effect : public side_effect_base<Sig>
  {
    template <typename A>
    side_effect(
      A&& a_)
    : a(std::forward<A>(a_))
    {}

    void
    action(
      call_params_type_t<Sig> &t)
    const
    override
    {
      a(t);
    }
  private:
    Action a;
  };

  template <unsigned long long L, unsigned long long H = L>
  struct multiplicity { };

  template<typename R, typename Parent>
  struct return_injector : Parent
  {
    using return_type = R;
  };

  template <typename Parent>
  struct throw_injector : Parent
  {
    static bool const throws = true;
  };

  template <typename Parent>
  struct sideeffect_injector : Parent
  {
    static bool const side_effects = true;
  };

  template <typename Parent, unsigned long long H>
  struct call_limit_injector : Parent
  {
    static bool               const call_limit_set = true;
    static unsigned long long const upper_call_limit = H;
  };

  template <typename Parent>
  struct call_limit_injector<Parent, 0ULL> : Parent
  {
    static bool const call_limit_set = true;
    static unsigned long long const upper_call_limit = 0ULL;
  };

  template <typename Parent>
  struct sequence_injector : Parent
  {
    static bool const sequence_set = true;
  };

  template <typename Matcher, typename modifier_tag, typename Parent>
  struct call_modifier : public Parent
  {
    using typename Parent::signature;
    using typename Parent::return_type;
    using Parent::call_limit_set;
    using Parent::upper_call_limit;
    using Parent::sequence_set;
    using Parent::throws;
    using Parent::side_effects;

    call_modifier(
       Matcher* m)
    noexcept
      : matcher{m}
    {}

    template <typename D>
    call_modifier&&
    with(
      char const* str,
      D&& d)
    &&
    {
      matcher->add_condition(str, std::forward<D>(d));
      return std::move(*this);
    }

    template <typename A>
    call_modifier<Matcher, modifier_tag, sideeffect_injector<Parent>>
    sideeffect(
      A&& a)
    {
      constexpr bool forbidden = upper_call_limit == 0U;
      static_assert(!forbidden,
                    "SIDE_EFFECT for forbidden call does not make sense");
      matcher->add_side_effect(std::forward<A>(a));
      return {std::move(matcher)};
    }

    template <typename H>
    call_modifier<Matcher, modifier_tag, return_injector<return_of_t<signature>, Parent >>
    handle_return(
      H&& h)
    {
      using params_type = call_params_type_t<signature>&;
      using sigret = return_of_t<signature>;
      using ret = decltype(std::declval<H>()(std::declval<params_type>()));
      // don't know why MS VS 2015 RC doesn't like std::result_of

      constexpr bool is_illegal_type   = std::is_same<std::decay_t<ret>, illegal_argument>::value;
      constexpr bool is_first_return   = std::is_same<return_type, void>::value;
      constexpr bool void_signature    = std::is_same<sigret, void>::value;
      constexpr bool is_pointer_sigret = std::is_pointer<sigret>::value;
      constexpr bool is_pointer_ret    = std::is_pointer<std::decay_t<ret>>::value;
      constexpr bool ptr_const_mismatch = is_pointer_ret && is_pointer_sigret && !std::is_const<std::remove_pointer_t<sigret>>{} && std::is_const<std::remove_pointer_t<std::decay_t<ret>>>{};
      constexpr bool is_ref_sigret     = std::is_reference<sigret>::value;
      constexpr bool is_ref_ret        = std::is_reference<ret>::value;
      constexpr bool ref_const_mismatch=
        is_ref_ret &&
        is_ref_sigret &&
        !std::is_const<std::remove_reference_t<sigret>>{} &&
        std::is_const<std::remove_reference_t<ret>>{};
      constexpr bool matching_ret_type = std::is_constructible<sigret, ret>::value;
      constexpr bool ref_value_mismatch = !is_ref_ret && is_ref_sigret;

      static_assert(matching_ret_type || !void_signature,
                    "RETURN does not make sense for void-function");
      static_assert(!is_illegal_type,
                    "RETURN illegal argument");
      static_assert(!ptr_const_mismatch,
                    "RETURN const* from function returning pointer to non-const");
      static_assert(!ref_value_mismatch || matching_ret_type,
                    "RETURN non-reference from function returning reference");
      static_assert(ref_value_mismatch || !ref_const_mismatch,
                    "RETURN const& from function returning non-const reference");

      static_assert(ptr_const_mismatch || ref_const_mismatch || is_illegal_type || matching_ret_type || void_signature,
                    "RETURN value is not convertible to the return type of the function");
      static_assert(is_first_return,
                    "Multiple RETURN does not make sense");
      static_assert(!throws || upper_call_limit == 0,
                    "THROW and RETURN does not make sense");
      static_assert(upper_call_limit > 0ULL,
                    "RETURN for forbidden call does not make sense");

      constexpr bool valid = !is_illegal_type && matching_ret_type && is_first_return && !throws && upper_call_limit > 0ULL;
      using tag = std::integral_constant<bool, valid>;
      matcher->set_return(tag{}, std::forward<H>(h));
      return {matcher};
    }

    template <typename H>
    call_modifier<Matcher, modifier_tag, throw_injector<Parent> >
    handle_throw(
      H&& h)
    {
      static_assert(!throws,
                    "Multiple THROW does not make sense");
      constexpr bool has_return = !std::is_same<return_type, void>::value;
      static_assert(!has_return,
                    "THROW and RETURN does not make sense");

      constexpr bool forbidden = upper_call_limit == 0U;

      static_assert(!forbidden,
                    "THROW for forbidden call does not make sense");

      constexpr bool valid = !throws && !has_return;// && !forbidden;
      using tag = std::integral_constant<bool, valid>;
      auto handler = [=](auto& p) -> decltype(auto)
      {
        h(p);
        return trompeloeil::default_return<return_of_t<signature>>();
      };
      matcher->set_return(tag{}, std::move(handler));
      return {matcher};
    }
    template <unsigned long long L,
              unsigned long long H,
              bool               times_set = call_limit_set>
    call_modifier<Matcher, modifier_tag, call_limit_injector<Parent, H>>
    times(
      multiplicity<L, H>)
    {
      static_assert(!times_set,
                    "Only one TIMES call limit is allowed, but it can express an interval");

      static_assert(H >= L,
                    "In TIMES the first value must not exceed the second");

      static_assert(H > 0 || !throws,
                    "THROW and TIMES(0) does not make sense");

      static_assert(H > 0 || std::is_same<return_type, void>::value,
                    "RETURN and TIMES(0) does not make sense");

      static_assert(H > 0 || !side_effects,
                    "SIDE_EFFECT and TIMES(0) does not make sense");

      static_assert(H > 0 || !sequence_set,
                    "IN_SEQUENCE and TIMES(0) does not make sense");

      matcher->min_calls = L;
      matcher->max_calls = H;
      return {matcher};
    }

    template <typename ... T,
              bool b = sequence_set>
    call_modifier<Matcher, modifier_tag, sequence_injector<Parent>>
    in_sequence(
      T&& ... t)
    {
      static_assert(!b,
                    "Multiple IN_SEQUENCE does not make sense."
                    " You can list several sequence objects at once");

      static_assert(upper_call_limit > 0ULL,
                    "IN_SEQUENCE for forbidden call does not make sense");

      matcher->set_sequence(std::forward<T>(t)...);
      return {matcher};
    }
    Matcher* matcher;
  };

  inline
  void
  report_unfulfilled(
    const char* reason,
    char const        *name,
    std::string const &values,
    unsigned long long min_calls,
    unsigned long long call_count,
    location           loc)
  {
    std::ostringstream os;
    os << reason
       << ":\nExpected " << name << " to be called ";
    if (min_calls == 1)
      os << "once";
    else
      os << min_calls << " times";
    os << ", actually ";
    switch (call_count)
    {
    case 0:
      os << "never called\n"; break;
    case 1:
      os << "called once\n"; break;
    default:
      os << "called " << call_count << " times\n";
    }
    os << values;
    send_report<specialized>(severity::nonfatal, loc, os.str());
  }

  inline
  void
  report_forbidden_call(
    char const *name,
    location loc,
    std::string const& values)
  {
    std::ostringstream os;
    os << "Match of forbidden call of " << name
       << " at " << loc << '\n' << values;
    send_report<specialized>(severity::fatal, loc, os.str());
  }

  template <typename Sig>
  struct matcher_info
  {
    using signature = Sig;
    using return_type = void;
    static unsigned long long const upper_call_limit = 1;
    static bool const throws = false;
    static bool const call_limit_set = false;
    static bool const sequence_set = false;
    static bool const side_effects = false;
  };


  template<typename Sig, typename Value>
  struct call_matcher : public call_matcher_base<Sig>, expectation
  {
    using call_matcher_base<Sig>::name;
    using call_matcher_base<Sig>::loc;

    template<typename ... U>
    call_matcher(
      char const *file,
      unsigned long line,
      char const *call_string,
      U &&... u)
    : call_matcher_base<Sig>(location{file, line}, call_string)
    , val(std::forward<U>(u)...)
    {}

    call_matcher(call_matcher &&r) = delete;

    ~call_matcher()
    {
      auto lock = get_lock();
      if (is_unfulfilled())
      {
        report_missed("Unfulfilled expectation");
      }
      this->unlink();
    }

    bool
    is_unfulfilled()
    const
    noexcept
    {
      return !reported && this->is_linked() && call_count < min_calls;
    }

    void
    mock_destroyed()
    override
    {
      if (is_unfulfilled())
      {
        report_missed("Pending expectation on destroyed mock object");
      }
    }

    call_matcher*
    hook_last(
      call_matcher_list<Sig> &list)
    noexcept
    {
      list.push_front(this);
      return this;
    }

    bool
    matches(
      call_params_type_t<Sig> const& params)
    const
    override
    {
      return match_parameters(val, params) && match_conditions(params);
    }

    bool
    match_conditions(
      call_params_type_t<Sig> const & params)
    const
    {
      // std::all_of() is almost always preferable. The only reason
      // for using a hand rolled loop is because it cuts compilation
      // times quite noticeably (almost 10% with g++5.1)
      for (auto& c : conditions)
      {
        if (!c.check(params)) return false;
      }
      return true;
    }

    bool
    first_in_sequence()
    const
    noexcept
    override
    {
      auto saturated = call_count >= min_calls;
      return saturated || !sequences || sequences->is_first();
    }

    return_of_t<Sig>
    return_value(
      trace_agent& agent,
      call_params_type_t<Sig>& params)
    override
    {
      if (!return_handler_obj) return default_return<return_of_t<Sig>>();
      return return_handler_obj->call(agent, params);
    }

    void
    run_actions(
      call_params_type_t<Sig>& params,
      call_matcher_list<Sig> &saturated_list)
    override
    {
      if (call_count < min_calls && sequences)
      {
        sequences->validate(severity::fatal, name, loc);
      }
      if (max_calls == 0)
      {
        reported = true;
        report_forbidden_call(name, loc, params_string(params));
      }
      if (++call_count == min_calls && sequences)
      {
        sequences->retire();
      }
      if (call_count == max_calls)
      {
        this->unlink();
        saturated_list.push_back(this);
      }
      for (auto& a : actions) a.action(params);
    }

    std::ostream&
    report_signature(
      std::ostream& os)
    const override
    {
      return os << name << " at " << loc;
    }

    std::ostream&
    report_mismatch(
      std::ostream& os,
      call_params_type_t<Sig> const & params)
    override
    {
      reported = true;
      report_signature(os);
      if (match_parameters(val, params))
      {
        for (auto& cond : conditions)
        {
          if (!cond.check(params))
          {
            os << "\n  Failed WITH(" << cond.name() << ')';
          }
        }
      }
      else
      {
        os << '\n';
        ::trompeloeil::print_mismatch(os, val, params);
      }
      return os;
    }

    void
    report_missed(
      char const *reason)
    override
    {
      reported = true;
      report_unfulfilled(
        reason,
        name,
        params_string(val),
        min_calls,
        call_count,
        loc);
    }
    template <typename C>
    void
    add_condition(
      char const *str,
      C&& c)
    {
      auto cond = new condition<Sig, C>(str, std::forward<C>(c));
      conditions.push_back(cond);
    }

    template <typename S>
    void
    add_side_effect(
      S&& s)
    {
      auto effect = new side_effect<Sig, S>(std::forward<S>(s));
      actions.push_back(effect);
    }

    template <typename ... T>
    void
    set_sequence(
      T&& ... t)
    {
      auto seq = new sequence_handler<sizeof...(T)>(name,
                                                    loc,
                                                    std::forward<T>(t)...);
      sequences.reset(seq);
    }

    template <typename T>
    inline
    void
    set_return(
      std::true_type,
      T&& h)
    {
      using basic_t = typename std::remove_reference<T>::type;
      using handler = return_handler_t<Sig, basic_t>;
      return_handler_obj.reset(new handler(std::forward<T>(h)));
    }

    template <typename T>
    inline                           // Never called. Used to limit errmsg
    static                           // with RETURN of wrong type and after:
    void                             //   FORBIDDEN_CALL
    set_return(std::false_type, T&&t)//   RETURN
      noexcept;                      //   THROW

    condition_list<Sig>                    conditions;
    side_effect_list<Sig>                  actions;
    std::unique_ptr<return_handler<Sig>>   return_handler_obj;
    std::unique_ptr<sequence_handler_base> sequences;
    unsigned long long                     call_count = 0;
    unsigned long long                     min_calls = 1;
    unsigned long long                     max_calls = 1;
    Value                                  val;
    bool                                   reported = false;
  };


  template<int N, typename T>
  constexpr
  inline
  decltype(auto)
  arg(
    T* t,
    std::true_type)
  {
    return std::get<N-1>(*t);
  }

  template <int N>
  inline
  constexpr
  illegal_argument const
  arg(
    void const*,
    std::false_type)
  noexcept
  {
    return {};
  }

  template <int N, typename T>
  decltype(auto)
  mkarg(
    T& t)
  noexcept
  {
    return arg<N>(&t, std::integral_constant<bool, (N <= std::tuple_size<T>::value)>{});
  }

  template <typename Mock>
  struct call_validator_t
  {
    template <typename M, typename Tag, typename Info>
    auto
    make_expectation(
      std::true_type,
      call_modifier<M, Tag, Info>&& m)
    const
    noexcept
    {
      auto lock = get_lock();
      m.matcher->hook_last(obj.trompeloeil_matcher_list(static_cast<Tag*>(nullptr)));
      return std::unique_ptr<expectation>(m.matcher);
    }

    template <typename T>
    static                                           // Never called. Used to
    std::unique_ptr<expectation>                     // limit errmsg when RETURN
    make_expectation(std::false_type, T&&) noexcept; // is missing in non-void
                                                     // function

    template <typename M, typename Tag, typename Info>
    inline
    auto
    operator+(
      call_modifier<M, Tag, Info>&& t)
    const
    {
      using call = call_modifier<M, Tag, Info>;
      using sigret = return_of_t<typename call::signature>;
      using ret = typename call::return_type;
      constexpr bool retmatch = std::is_same<ret, sigret>::value;
      constexpr bool forbidden = call::upper_call_limit == 0ULL;
      constexpr bool valid_return_type = call::throws || retmatch || forbidden;
      static_assert(valid_return_type, "RETURN missing for non-void function");
      auto tag = std::integral_constant<bool, valid_return_type>{};
      return make_expectation(tag, std::move(t));
    }
    Mock& obj;
  };

  template <typename T,
            typename = std::enable_if_t<std::is_lvalue_reference<T&&>::value>>
  inline
  T&&
  decay_return_type(
    T&& t)
  {
    return std::forward<T>(t);
  }

  template <typename T,
            typename = std::enable_if_t<std::is_rvalue_reference<T&&>::value>>
  inline
  T
  decay_return_type(
    T&& t)
  {
    return std::forward<T>(t);
  }

  template <typename T, size_t N>
  inline
  T*
  decay_return_type(
    T (&t)[N])
  {
    return t;
  }

  template <bool sequence_set>
  struct lifetime_monitor_modifier
  {
    operator std::unique_ptr<lifetime_monitor>() { return std::move(monitor);}
    operator std::unique_ptr<expectation>() { return std::move(monitor);}
    template <typename ... T, bool b = sequence_set>
    lifetime_monitor_modifier<true>
    in_sequence(T&& ... t)
    {
      static_assert(!b,
                    "Multiple IN_SEQUENCE does not make sense."
                      " You can list several sequence objects at once");
      monitor->set_sequence(std::forward<T>(t)...);
      return { std::move(monitor) };
    }
    std::unique_ptr<lifetime_monitor> monitor;
  };

  template <typename Sig>
  struct expectations
  {
    ~expectations() {
      active.decommission();
      saturated.decommission();
    }
    call_matcher_list<Sig> active;
    call_matcher_list<Sig> saturated;
  };

  template <typename Sig, typename ... P>
  return_of_t<Sig> mock_func(std::false_type, P&& ...);


  template <typename Sig, typename ... P>
  return_of_t<Sig>
  mock_func(std::true_type,
            expectations<Sig>& e,
            char const *func_name,
            char const *sig_name,
            P&& ... p)
  {
    auto lock = get_lock();

    call_params_type_t<void(P...)> param_value(std::forward<P>(p)...);

    auto i = find(e.active, param_value);
    if (!i)
    {
      report_mismatch(e.active,
                      e.saturated,
                      func_name + std::string(" with signature ") + sig_name,
                      param_value);
    }
    trace_agent ta{i->loc, i->name, tracer_obj()};
    try
    {
      ta.trace_params(param_value);
      i->run_actions(param_value, e.saturated);
      return i->return_value(ta, param_value);
    }
    catch (...)
    {
      ta.trace_exception();
      throw;
    }
  }

  template <typename ... U>
  struct param_helper {
	using type = decltype(std::make_tuple(std::declval<U>()...));
  };

  template <typename ... U>
  using param_t = typename param_helper<U...>::type;

  template <typename sig, typename tag, typename... U>
  using modifier_t = call_modifier<call_matcher<sig, param_t<U...>>,
                                   tag,
                                   matcher_info<sig>>;


  template <typename M>
  inline
  std::enable_if_t<::trompeloeil::is_matcher<M>::value, ::trompeloeil::ptr_deref<std::decay_t<M>>>
  operator*(
    M&& m)
  {
    return ::trompeloeil::ptr_deref<std::decay_t<M>>{std::forward<M>(m)};
  }

  template <typename M>
  inline
  std::enable_if_t<::trompeloeil::is_matcher<M>::value, ::trompeloeil::neg_matcher<std::decay_t<M>>>
  operator!(
    M&& m)
  {
    return ::trompeloeil::neg_matcher<std::decay_t<M>>{std::forward<M>(m)};
  }

}

#define TROMPELOEIL_LINE_ID(name)                                        \
  TROMPELOEIL_CONCAT(trompeloeil_l_ ## name ## _, __LINE__)
#define TROMPELOEIL_COUNT_ID(name)                                       \
  TROMPELOEIL_CONCAT(trompeloeil_c_ ## name ## _, __COUNTER__)

#ifdef _MSC_VER
#define TROMPELOEIL_MAKE_MOCK0(name, sig, ...)                           \
  TROMPELOEIL_MAKE_MOCK_(name,,0, sig, __VA_ARGS__,,)
#define TROMPELOEIL_MAKE_MOCK1(name, sig, ...)                           \
  TROMPELOEIL_MAKE_MOCK_(name,,1, sig, __VA_ARGS__,,)
#define TROMPELOEIL_MAKE_MOCK2(name, sig, ...)                           \
  TROMPELOEIL_MAKE_MOCK_(name,,2, sig, __VA_ARGS__,,)
#define TROMPELOEIL_MAKE_MOCK3(name, sig, ...)                           \
  TROMPELOEIL_MAKE_MOCK_(name,,3, sig, __VA_ARGS__,,)
#define TROMPELOEIL_MAKE_MOCK4(name, sig, ...)                           \
  TROMPELOEIL_MAKE_MOCK_(name,,4, sig, __VA_ARGS__,,)
#define TROMPELOEIL_MAKE_MOCK5(name, sig, ...)                           \
  TROMPELOEIL_MAKE_MOCK_(name,,5, sig, __VA_ARGS__,,)
#define TROMPELOEIL_MAKE_MOCK6(name, sig, ...)                           \
  TROMPELOEIL_MAKE_MOCK_(name,,6, sig, __VA_ARGS__,,)
#define TROMPELOEIL_MAKE_MOCK7(name, sig, ...)                           \
  TROMPELOEIL_MAKE_MOCK_(name,,7, sig, __VA_ARGS__,,)
#define TROMPELOEIL_MAKE_MOCK8(name, sig, ...)                           \
  TROMPELOEIL_MAKE_MOCK_(name,,8, sig, __VA_ARGS__,,)
#define TROMPELOEIL_MAKE_MOCK9(name, sig, ...)                           \
  TROMPELOEIL_MAKE_MOCK_(name,,9, sig, __VA_ARGS__,,)
#define TROMPELOEIL_MAKE_MOCK10(name, sig, ...)                          \
  TROMPELOEIL_MAKE_MOCK_(name,,10, sig, __VA_ARGS__,,)
#define TROMPELOEIL_MAKE_MOCK11(name, sig, ...)                          \
  TROMPELOEIL_MAKE_MOCK_(name,,11, sig, __VA_ARGS__,,)
#define TROMPELOEIL_MAKE_MOCK12(name, sig, ...)                          \
  TROMPELOEIL_MAKE_MOCK_(name,,12, sig, __VA_ARGS__,,)
#define TROMPELOEIL_MAKE_MOCK13(name, sig, ...)                          \
  TROMPELOEIL_MAKE_MOCK_(name,,13, sig, __VA_ARGS__,,)
#define TROMPELOEIL_MAKE_MOCK14(name, sig, ...)                          \
  TROMPELOEIL_MAKE_MOCK_(name,,14, sig, __VA_ARGS__,,)
#define TROMPELOEIL_MAKE_MOCK15(name, sig, ...)                          \
  TROMPELOEIL_MAKE_MOCK_(name,,15, sig, __VA_ARGS__,,)

#define TROMPELOEIL_MAKE_CONST_MOCK0(name, sig, ...)                     \
  TROMPELOEIL_MAKE_MOCK_(name,const,0, sig, __VA_ARGS__,,)
#define TROMPELOEIL_MAKE_CONST_MOCK1(name, sig, ...)                     \
  TROMPELOEIL_MAKE_MOCK_(name,const,1, sig, __VA_ARGS__,,)
#define TROMPELOEIL_MAKE_CONST_MOCK2(name, sig, ...)                     \
  TROMPELOEIL_MAKE_MOCK_(name,const,2, sig, __VA_ARGS__,,)
#define TROMPELOEIL_MAKE_CONST_MOCK3(name, sig, ...)                     \
  TROMPELOEIL_MAKE_MOCK_(name,const,3, sig, __VA_ARGS__,,)
#define TROMPELOEIL_MAKE_CONST_MOCK4(name, sig, ...)                     \
  TROMPELOEIL_MAKE_MOCK_(name,const,4, sig, __VA_ARGS__,,)
#define TROMPELOEIL_MAKE_CONST_MOCK5(name, sig, ...)                     \
  TROMPELOEIL_MAKE_MOCK_(name,const,5, sig, __VA_ARGS__,,)
#define TROMPELOEIL_MAKE_CONST_MOCK6(name, sig, ...)                     \
  TROMPELOEIL_MAKE_MOCK_(name,const,6, sig, __VA_ARGS__,,)
#define TROMPELOEIL_MAKE_CONST_MOCK7(name, sig, ...)                     \
  TROMPELOEIL_MAKE_MOCK_(name,const,7, sig, __VA_ARGS__,,)
#define TROMPELOEIL_MAKE_CONST_MOCK8(name, sig, ...)                     \
  TROMPELOEIL_MAKE_MOCK_(name,const,8, sig, __VA_ARGS__,,)
#define TROMPELOEIL_MAKE_CONST_MOCK9(name, sig, ...)                     \
  TROMPELOEIL_MAKE_MOCK_(name,const,9, sig, __VA_ARGS__,,)
#define TROMPELOEIL_MAKE_CONST_MOCK10(name, sig, ...)                    \
  TROMPELOEIL_MAKE_MOCK_(name,const,10, sig, __VA_ARGS__,,)
#define TROMPELOEIL_MAKE_CONST_MOCK11(name, sig, ...)                    \
  TROMPELOEIL_MAKE_MOCK_(name,const,11, sig, __VA_ARGS__,,)
#define TROMPELOEIL_MAKE_CONST_MOCK12(name, sig, ...)                    \
  TROMPELOEIL_MAKE_MOCK_(name,const,12, sig, __VA_ARGS__,,)
#define TROMPELOEIL_MAKE_CONST_MOCK13(name, sig, ...)                    \
  TROMPELOEIL_MAKE_MOCK_(name,const,13, sig, __VA_ARGS__,,)
#define TROMPELOEIL_MAKE_CONST_MOCK14(name, sig, ...)                    \
  TROMPELOEIL_MAKE_MOCK_(name,const,14, sig, __VA_ARGS__,,)
#define TROMPELOEIL_MAKE_CONST_MOCK15(name, sig, ...)                    \
  TROMPELOEIL_MAKE_MOCK_(name,const,15, sig, __VA_ARGS__,,)


#else
// sane standards compliant preprocessor

#define TROMPELOEIL_MAKE_MOCK0(name, ...)                                \
  TROMPELOEIL_MAKE_MOCK_(name,,0, __VA_ARGS__,,)
#define TROMPELOEIL_MAKE_MOCK1(name, ...)                                \
  TROMPELOEIL_MAKE_MOCK_(name,,1, __VA_ARGS__,,)
#define TROMPELOEIL_MAKE_MOCK2(name, ...)                                \
  TROMPELOEIL_MAKE_MOCK_(name,,2, __VA_ARGS__,,)
#define TROMPELOEIL_MAKE_MOCK3(name, ...)                                \
  TROMPELOEIL_MAKE_MOCK_(name,,3, __VA_ARGS__,,)
#define TROMPELOEIL_MAKE_MOCK4(name, ...)                                \
  TROMPELOEIL_MAKE_MOCK_(name,,4, __VA_ARGS__,,)
#define TROMPELOEIL_MAKE_MOCK5(name, ...)                                \
  TROMPELOEIL_MAKE_MOCK_(name,,5, __VA_ARGS__,,)
#define TROMPELOEIL_MAKE_MOCK6(name, ...)                                \
  TROMPELOEIL_MAKE_MOCK_(name,,6, __VA_ARGS__,,)
#define TROMPELOEIL_MAKE_MOCK7(name, ...)                                \
  TROMPELOEIL_MAKE_MOCK_(name,,7, __VA_ARGS__,,)
#define TROMPELOEIL_MAKE_MOCK8(name, ...)                                \
  TROMPELOEIL_MAKE_MOCK_(name,,8, __VA_ARGS__,,)
#define TROMPELOEIL_MAKE_MOCK9(name, ...)                                \
  TROMPELOEIL_MAKE_MOCK_(name,,9, __VA_ARGS__,,)
#define TROMPELOEIL_MAKE_MOCK10(name, ...)                               \
  TROMPELOEIL_MAKE_MOCK_(name,,10, __VA_ARGS__,,)
#define TROMPELOEIL_MAKE_MOCK11(name, ...)                               \
  TROMPELOEIL_MAKE_MOCK_(name,,11, __VA_ARGS__,,)
#define TROMPELOEIL_MAKE_MOCK12(name, ...)                               \
  TROMPELOEIL_MAKE_MOCK_(name,,12, __VA_ARGS__,,)
#define TROMPELOEIL_MAKE_MOCK13(name, ...)                               \
  TROMPELOEIL_MAKE_MOCK_(name,,13, __VA_ARGS__,,)
#define TROMPELOEIL_MAKE_MOCK14(name, ...)                               \
  TROMPELOEIL_MAKE_MOCK_(name,,14,__VA_ARGS__,,)
#define TROMPELOEIL_MAKE_MOCK15(name, ...)                               \
  TROMPELOEIL_MAKE_MOCK_(name,,15, __VA_ARGS__,,)

#define TROMPELOEIL_MAKE_CONST_MOCK0(name, ...)                          \
  TROMPELOEIL_MAKE_MOCK_(name,const,0, __VA_ARGS__,,)
#define TROMPELOEIL_MAKE_CONST_MOCK1(name, ...)                          \
  TROMPELOEIL_MAKE_MOCK_(name,const,1, __VA_ARGS__,,)
#define TROMPELOEIL_MAKE_CONST_MOCK2(name, ...)                          \
  TROMPELOEIL_MAKE_MOCK_(name,const,2, __VA_ARGS__,,)
#define TROMPELOEIL_MAKE_CONST_MOCK3(name, ...)                          \
  TROMPELOEIL_MAKE_MOCK_(name,const,3, __VA_ARGS__,,)
#define TROMPELOEIL_MAKE_CONST_MOCK4(name, ...)                          \
  TROMPELOEIL_MAKE_MOCK_(name,const,4, __VA_ARGS__,,)
#define TROMPELOEIL_MAKE_CONST_MOCK5(name, ...)                          \
  TROMPELOEIL_MAKE_MOCK_(name,const,5, __VA_ARGS__,,)
#define TROMPELOEIL_MAKE_CONST_MOCK6(name, ...)                          \
  TROMPELOEIL_MAKE_MOCK_(name,const,6, __VA_ARGS__,,)
#define TROMPELOEIL_MAKE_CONST_MOCK7(name, ...)                          \
  TROMPELOEIL_MAKE_MOCK_(name,const,7, __VA_ARGS__,,)
#define TROMPELOEIL_MAKE_CONST_MOCK8(name, ...)                          \
  TROMPELOEIL_MAKE_MOCK_(name,const,8, __VA_ARGS__,,)
#define TROMPELOEIL_MAKE_CONST_MOCK9(name, ...)                          \
  TROMPELOEIL_MAKE_MOCK_(name,const,9, __VA_ARGS__,,)
#define TROMPELOEIL_MAKE_CONST_MOCK10(name, ...)                         \
  TROMPELOEIL_MAKE_MOCK_(name,const,10, __VA_ARGS__,,)
#define TROMPELOEIL_MAKE_CONST_MOCK11(name, ...)                         \
  TROMPELOEIL_MAKE_MOCK_(name,const,11, __VA_ARGS__,,)
#define TROMPELOEIL_MAKE_CONST_MOCK12(name, ...)                         \
  TROMPELOEIL_MAKE_MOCK_(name,const,12, __VA_ARGS__,,)
#define TROMPELOEIL_MAKE_CONST_MOCK13(name, ...)                         \
  TROMPELOEIL_MAKE_MOCK_(name,const,13, __VA_ARGS__,,)
#define TROMPELOEIL_MAKE_CONST_MOCK14(name, ...)                         \
  TROMPELOEIL_MAKE_MOCK_(name,const,14, __VA_ARGS__,,)
#define TROMPELOEIL_MAKE_CONST_MOCK15(name, ...)                         \
  TROMPELOEIL_MAKE_MOCK_(name,const,15, __VA_ARGS__,,)

#endif

#define TROMPELOEIL_MAKE_MOCK_(name, constness, num, sig, spec, ...)           \
  using TROMPELOEIL_LINE_ID(cardinality_match) =                               \
    std::integral_constant<bool, num == ::trompeloeil::param_list<sig>::size>; \
  static_assert(TROMPELOEIL_LINE_ID(cardinality_match)::value,                 \
                "Function signature does not have " #num " parameters");       \
  using TROMPELOEIL_LINE_ID(matcher_list_t) = ::trompeloeil::call_matcher_list<sig>;\
  using TROMPELOEIL_LINE_ID(expectation_list_t) = ::trompeloeil::expectations<sig>; \
  mutable TROMPELOEIL_LINE_ID(expectation_list_t) TROMPELOEIL_LINE_ID(expectations);\
  struct TROMPELOEIL_LINE_ID(tag_type_trompeloeil)                             \
  {                                                                            \
    const char* trompeloeil_expectation_file;                                  \
    unsigned long trompeloeil_expectation_line;                                \
    const char *trompeloeil_expectation_string;                                \
                                                                               \
    template <typename ... trompeloeil_param_type>                             \
    auto name(                                                                 \
      trompeloeil_param_type&& ... trompeloeil_param)                          \
      -> ::trompeloeil::modifier_t<sig,                                        \
                                   TROMPELOEIL_LINE_ID(tag_type_trompeloeil),  \
                                   trompeloeil_param_type...>                  \
    {                                                                          \
      using matcher = ::trompeloeil::call_matcher<                             \
                          sig,                                                 \
                          ::trompeloeil::param_t<trompeloeil_param_type...>>;  \
      return {                                                                 \
          new matcher {                                                        \
                trompeloeil_expectation_file,                                  \
                trompeloeil_expectation_line,                                  \
                trompeloeil_expectation_string,                                \
                std::forward<trompeloeil_param_type>(trompeloeil_param)...     \
              }                                                                \
      };                                                                       \
    }                                                                          \
  };                                                                           \
                                                                               \
  TROMPELOEIL_LINE_ID(matcher_list_t)&                                         \
  trompeloeil_matcher_list(                                                    \
    TROMPELOEIL_LINE_ID(tag_type_trompeloeil)*)                                \
  constness                                                                    \
  noexcept                                                                     \
  {                                                                            \
    return TROMPELOEIL_LINE_ID(expectations).active;                           \
  }                                                                            \
  ::trompeloeil::return_of_t<sig>                                              \
  name(                                                                        \
    TROMPELOEIL_PARAM_LIST(num, sig))                                          \
  constness                                                                    \
  spec                                                                         \
  {                                                                            \
    return ::trompeloeil::mock_func<sig>(TROMPELOEIL_LINE_ID(cardinality_match){},  \
                                         TROMPELOEIL_LINE_ID(expectations),    \
                                         #name,                                \
                                         #sig                                  \
                                         TROMPELOEIL_PARAMS(num));             \
  }                                                                            \
                                                                               \
  TROMPELOEIL_LINE_ID(tag_type_trompeloeil)                                    \
  trompeloeil_tag_ ## name(TROMPELOEIL_PARAM_LIST(num, sig)) constness




#define TROMPELOEIL_REQUIRE_CALL(obj, func)                                    \
  TROMPELOEIL_REQUIRE_CALL_(obj, func, #obj, #func)

#define TROMPELOEIL_REQUIRE_CALL_(obj, func, obj_s, func_s)                    \
  auto TROMPELOEIL_COUNT_ID(call_obj) = TROMPELOEIL_REQUIRE_CALL_OBJ(obj, func,\
                                                               obj_s, func_s)


#define TROMPELOEIL_NAMED_REQUIRE_CALL(obj, func)                              \
  TROMPELOEIL_NAMED_REQUIRE_CALL_(obj, func, #obj, #func)

#define TROMPELOEIL_NAMED_REQUIRE_CALL_(obj, func, obj_s, func_s)              \
  TROMPELOEIL_REQUIRE_CALL_OBJ(obj, func, obj_s, func_s)

#define TROMPELOEIL_REQUIRE_CALL_OBJ(obj, func, obj_s, func_s)                 \
    ::trompeloeil::call_validator_t<decltype(obj)>{(obj)} +                    \
    std::conditional_t<false,                                                  \
                       decltype((obj).func),                                   \
                       decltype((obj).TROMPELOEIL_CONCAT(trompeloeil_tag_,func))>\
    {__FILE__, static_cast<unsigned long>(__LINE__), obj_s "." func_s}.func

#define TROMPELOEIL_ALLOW_CALL(obj, func)               \
  TROMPELOEIL_ALLOW_CALL_(obj, func, #obj, #func)

#define TROMPELOEIL_ALLOW_CALL_(obj, func, obj_s, func_s)                      \
  TROMPELOEIL_REQUIRE_CALL_(obj, func, obj_s, func_s)                          \
    .TROMPELOEIL_TIMES(0, ~0ULL)


#define TROMPELOEIL_NAMED_ALLOW_CALL(obj, func)                                \
  TROMPELOEIL_NAMED_ALLOW_CALL_(obj, func, #obj, #func)

#define TROMPELOEIL_NAMED_ALLOW_CALL_(obj, func, obj_s, func_s)                \
  TROMPELOEIL_NAMED_REQUIRE_CALL_(obj, func, obj_s, func_s)                    \
    .TROMPELOEIL_TIMES(0, ~0ULL)

#define TROMPELOEIL_FORBID_CALL(obj, func)                                     \
  TROMPELOEIL_FORBID_CALL_(obj, func, #obj, #func)

#define TROMPELOEIL_FORBID_CALL_(obj, func, obj_s, func_s)                     \
  TROMPELOEIL_REQUIRE_CALL_(obj, func, obj_s, func_s)                          \
    .TROMPELOEIL_TIMES(0)

#define TROMPELOEIL_NAMED_FORBID_CALL(obj, func)                               \
  TROMPELOEIL_NAMED_FORBID_CALL_(obj, func, #obj, #func)

#define TROMPELOEIL_NAMED_FORBID_CALL_(obj, func, obj_s, func_s)               \
  TROMPELOEIL_NAMED_REQUIRE_CALL_(obj, func, obj_s, func_s)                    \
    .TROMPELOEIL_TIMES(0)


#define TROMPELOEIL_WITH(...)    TROMPELOEIL_WITH_(=,#__VA_ARGS__, __VA_ARGS__)
#define TROMPELOEIL_LR_WITH(...) TROMPELOEIL_WITH_(&,#__VA_ARGS__, __VA_ARGS__)

#define TROMPELOEIL_WITH_(capture, arg_s, ...)                                 \
  with(arg_s, [capture](auto const& trompeloeil_x) {                           \
    auto& _1 = ::trompeloeil::mkarg<1>(trompeloeil_x);                         \
    auto& _2 = ::trompeloeil::mkarg<2>(trompeloeil_x);                         \
    auto& _3 = ::trompeloeil::mkarg<3>(trompeloeil_x);                         \
    auto& _4 = ::trompeloeil::mkarg<4>(trompeloeil_x);                         \
    auto& _5 = ::trompeloeil::mkarg<5>(trompeloeil_x);                         \
    auto& _6 = ::trompeloeil::mkarg<6>(trompeloeil_x);                         \
    auto& _7 = ::trompeloeil::mkarg<7>(trompeloeil_x);                         \
    auto& _8 = ::trompeloeil::mkarg<8>(trompeloeil_x);                         \
    auto& _9 = ::trompeloeil::mkarg<9>(trompeloeil_x);                         \
    auto&_10 = ::trompeloeil::mkarg<10>(trompeloeil_x);                        \
    auto&_11 = ::trompeloeil::mkarg<11>(trompeloeil_x);                        \
    auto&_12 = ::trompeloeil::mkarg<12>(trompeloeil_x);                        \
    auto&_13 = ::trompeloeil::mkarg<13>(trompeloeil_x);                        \
    auto&_14 = ::trompeloeil::mkarg<14>(trompeloeil_x);                        \
    auto&_15 = ::trompeloeil::mkarg<15>(trompeloeil_x);                        \
    ::trompeloeil::ignore(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15); \
    return __VA_ARGS__;                                                        \
  })

#define TROMPELOEIL_SIDE_EFFECT(...)    TROMPELOEIL_SIDE_EFFECT_(=, __VA_ARGS__)
#define TROMPELOEIL_LR_SIDE_EFFECT(...) TROMPELOEIL_SIDE_EFFECT_(&, __VA_ARGS__)

#define TROMPELOEIL_SIDE_EFFECT_(capture, ...)                                 \
  sideeffect([capture](auto& trompeloeil_x) {                                  \
    auto& _1 = ::trompeloeil::mkarg<1>(trompeloeil_x);                         \
    auto& _2 = ::trompeloeil::mkarg<2>(trompeloeil_x);                         \
    auto& _3 = ::trompeloeil::mkarg<3>(trompeloeil_x);                         \
    auto& _4 = ::trompeloeil::mkarg<4>(trompeloeil_x);                         \
    auto& _5 = ::trompeloeil::mkarg<5>(trompeloeil_x);                         \
    auto& _6 = ::trompeloeil::mkarg<6>(trompeloeil_x);                         \
    auto& _7 = ::trompeloeil::mkarg<7>(trompeloeil_x);                         \
    auto& _8 = ::trompeloeil::mkarg<8>(trompeloeil_x);                         \
    auto& _9 = ::trompeloeil::mkarg<9>(trompeloeil_x);                         \
    auto&_10 = ::trompeloeil::mkarg<10>(trompeloeil_x);                        \
    auto&_11 = ::trompeloeil::mkarg<11>(trompeloeil_x);                        \
    auto&_12 = ::trompeloeil::mkarg<12>(trompeloeil_x);                        \
    auto&_13 = ::trompeloeil::mkarg<13>(trompeloeil_x);                        \
    auto&_14 = ::trompeloeil::mkarg<14>(trompeloeil_x);                        \
    auto&_15 = ::trompeloeil::mkarg<15>(trompeloeil_x);                        \
    ::trompeloeil::ignore(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15); \
    __VA_ARGS__;                                                               \
  })

#define TROMPELOEIL_RETURN(...)    TROMPELOEIL_RETURN_(=, __VA_ARGS__)
#define TROMPELOEIL_LR_RETURN(...) TROMPELOEIL_RETURN_(&, __VA_ARGS__)

#define TROMPELOEIL_RETURN_(capture, ...)                                      \
  handle_return([capture](auto& trompeloeil_x) -> decltype(auto) {             \
    auto& _1 = ::trompeloeil::mkarg<1>(trompeloeil_x);                         \
    auto& _2 = ::trompeloeil::mkarg<2>(trompeloeil_x);                         \
    auto& _3 = ::trompeloeil::mkarg<3>(trompeloeil_x);                         \
    auto& _4 = ::trompeloeil::mkarg<4>(trompeloeil_x);                         \
    auto& _5 = ::trompeloeil::mkarg<5>(trompeloeil_x);                         \
    auto& _6 = ::trompeloeil::mkarg<6>(trompeloeil_x);                         \
    auto& _7 = ::trompeloeil::mkarg<7>(trompeloeil_x);                         \
    auto& _8 = ::trompeloeil::mkarg<8>(trompeloeil_x);                         \
    auto& _9 = ::trompeloeil::mkarg<9>(trompeloeil_x);                         \
    auto&_10 = ::trompeloeil::mkarg<10>(trompeloeil_x);                        \
    auto&_11 = ::trompeloeil::mkarg<11>(trompeloeil_x);                        \
    auto&_12 = ::trompeloeil::mkarg<12>(trompeloeil_x);                        \
    auto&_13 = ::trompeloeil::mkarg<13>(trompeloeil_x);                        \
    auto&_14 = ::trompeloeil::mkarg<14>(trompeloeil_x);                        \
    auto&_15 = ::trompeloeil::mkarg<15>(trompeloeil_x);                        \
    ::trompeloeil::ignore(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15); \
    return ::trompeloeil::decay_return_type(__VA_ARGS__);                                                        \
  })

#define TROMPELOEIL_THROW(...)    TROMPELOEIL_THROW_(=, __VA_ARGS__)
#define TROMPELOEIL_LR_THROW(...) TROMPELOEIL_THROW_(&, __VA_ARGS__)

#define TROMPELOEIL_THROW_(capture, ...)                                       \
  handle_throw([capture](auto& trompeloeil_x)  {                               \
    auto& _1 = ::trompeloeil::mkarg<1>(trompeloeil_x);                         \
    auto& _2 = ::trompeloeil::mkarg<2>(trompeloeil_x);                         \
    auto& _3 = ::trompeloeil::mkarg<3>(trompeloeil_x);                         \
    auto& _4 = ::trompeloeil::mkarg<4>(trompeloeil_x);                         \
    auto& _5 = ::trompeloeil::mkarg<5>(trompeloeil_x);                         \
    auto& _6 = ::trompeloeil::mkarg<6>(trompeloeil_x);                         \
    auto& _7 = ::trompeloeil::mkarg<7>(trompeloeil_x);                         \
    auto& _8 = ::trompeloeil::mkarg<8>(trompeloeil_x);                         \
    auto& _9 = ::trompeloeil::mkarg<9>(trompeloeil_x);                         \
    auto&_10 = ::trompeloeil::mkarg<10>(trompeloeil_x);                        \
    auto&_11 = ::trompeloeil::mkarg<11>(trompeloeil_x);                        \
    auto&_12 = ::trompeloeil::mkarg<12>(trompeloeil_x);                        \
    auto&_13 = ::trompeloeil::mkarg<13>(trompeloeil_x);                        \
    auto&_14 = ::trompeloeil::mkarg<14>(trompeloeil_x);                        \
    auto&_15 = ::trompeloeil::mkarg<15>(trompeloeil_x);                        \
    ::trompeloeil::ignore(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15); \
    throw __VA_ARGS__;                                                         \
 })


#define TROMPELOEIL_TIMES(...) times(::trompeloeil::multiplicity<__VA_ARGS__>{})

#define TROMPELOEIL_IN_SEQUENCE(...)                                           \
  in_sequence(TROMPELOEIL_INIT_WITH_STR(::trompeloeil::sequence_matcher::init_type, __VA_ARGS__))

#define TROMPELOEIL_ANY(type) ::trompeloeil::any_matcher<type>(#type)

#define TROMPELOEIL_AT_LEAST(num) num, ~0ULL
#define TROMPELOEIL_AT_MOST(num) 0, num

#define TROMPELOEIL_REQUIRE_DESTRUCTION(obj)                                   \
  TROMPELOEIL_REQUIRE_DESTRUCTION_(obj, #obj)

#define TROMPELOEIL_REQUIRE_DESTRUCTION_(obj, obj_s)                           \
  std::unique_ptr<trompeloeil::expectation>                                    \
    TROMPELOEIL_CONCAT(trompeloeil_death_monitor_, __LINE__)                   \
    = TROMPELOEIL_NAMED_REQUIRE_DESTRUCTION_(,obj, obj_s)

#define TROMPELOEIL_NAMED_REQUIRE_DESTRUCTION(obj)                             \
  TROMPELOEIL_NAMED_REQUIRE_DESTRUCTION_("NAMED_", obj, #obj)

#define TROMPELOEIL_NAMED_REQUIRE_DESTRUCTION_(prefix, obj, obj_s)                 \
  trompeloeil::lifetime_monitor_modifier<false>{                               \
    std::make_unique<trompeloeil::lifetime_monitor>(                           \
      obj,                                                                     \
      obj_s,                                                                   \
      prefix "REQUIRE_DESTRUCTION(" obj_s ")",                                 \
      "destructor for " obj_s,                                                 \
      ::trompeloeil::location{__FILE__,                                        \
                              static_cast<unsigned long>(__LINE__)})           \
  }

#ifndef TROMPELOEIL_LONG_MACROS
#define MAKE_MOCK0                TROMPELOEIL_MAKE_MOCK0
#define MAKE_MOCK1                TROMPELOEIL_MAKE_MOCK1
#define MAKE_MOCK2                TROMPELOEIL_MAKE_MOCK2
#define MAKE_MOCK3                TROMPELOEIL_MAKE_MOCK3
#define MAKE_MOCK4                TROMPELOEIL_MAKE_MOCK4
#define MAKE_MOCK5                TROMPELOEIL_MAKE_MOCK5
#define MAKE_MOCK6                TROMPELOEIL_MAKE_MOCK6
#define MAKE_MOCK7                TROMPELOEIL_MAKE_MOCK7
#define MAKE_MOCK8                TROMPELOEIL_MAKE_MOCK8
#define MAKE_MOCK9                TROMPELOEIL_MAKE_MOCK9
#define MAKE_MOCK10               TROMPELOEIL_MAKE_MOCK10
#define MAKE_MOCK11               TROMPELOEIL_MAKE_MOCK11
#define MAKE_MOCK12               TROMPELOEIL_MAKE_MOCK12
#define MAKE_MOCK13               TROMPELOEIL_MAKE_MOCK13
#define MAKE_MOCK14               TROMPELOEIL_MAKE_MOCK14
#define MAKE_MOCK15               TROMPELOEIL_MAKE_MOCK15

#define MAKE_CONST_MOCK0          TROMPELOEIL_MAKE_CONST_MOCK0
#define MAKE_CONST_MOCK1          TROMPELOEIL_MAKE_CONST_MOCK1
#define MAKE_CONST_MOCK2          TROMPELOEIL_MAKE_CONST_MOCK2
#define MAKE_CONST_MOCK3          TROMPELOEIL_MAKE_CONST_MOCK3
#define MAKE_CONST_MOCK4          TROMPELOEIL_MAKE_CONST_MOCK4
#define MAKE_CONST_MOCK5          TROMPELOEIL_MAKE_CONST_MOCK5
#define MAKE_CONST_MOCK6          TROMPELOEIL_MAKE_CONST_MOCK6
#define MAKE_CONST_MOCK7          TROMPELOEIL_MAKE_CONST_MOCK7
#define MAKE_CONST_MOCK8          TROMPELOEIL_MAKE_CONST_MOCK8
#define MAKE_CONST_MOCK9          TROMPELOEIL_MAKE_CONST_MOCK9
#define MAKE_CONST_MOCK10         TROMPELOEIL_MAKE_CONST_MOCK10
#define MAKE_CONST_MOCK11         TROMPELOEIL_MAKE_CONST_MOCK11
#define MAKE_CONST_MOCK12         TROMPELOEIL_MAKE_CONST_MOCK12
#define MAKE_CONST_MOCK13         TROMPELOEIL_MAKE_CONST_MOCK13
#define MAKE_CONST_MOCK14         TROMPELOEIL_MAKE_CONST_MOCK14
#define MAKE_CONST_MOCK15         TROMPELOEIL_MAKE_CONST_MOCK15

#define REQUIRE_CALL              TROMPELOEIL_REQUIRE_CALL
#define NAMED_REQUIRE_CALL        TROMPELOEIL_NAMED_REQUIRE_CALL
#define ALLOW_CALL                TROMPELOEIL_ALLOW_CALL
#define NAMED_ALLOW_CALL          TROMPELOEIL_NAMED_ALLOW_CALL
#define FORBID_CALL               TROMPELOEIL_FORBID_CALL
#define NAMED_FORBID_CALL         TROMPELOEIL_NAMED_FORBID_CALL
#define WITH                      TROMPELOEIL_WITH
#define LR_WITH                   TROMPELOEIL_LR_WITH
#define SIDE_EFFECT               TROMPELOEIL_SIDE_EFFECT
#define LR_SIDE_EFFECT            TROMPELOEIL_LR_SIDE_EFFECT
#define RETURN                    TROMPELOEIL_RETURN
#define LR_RETURN                 TROMPELOEIL_LR_RETURN
#define THROW                     TROMPELOEIL_THROW
#define LR_THROW                  TROMPELOEIL_LR_THROW
#define TIMES                     TROMPELOEIL_TIMES
#define IN_SEQUENCE               TROMPELOEIL_IN_SEQUENCE
#define ANY                       TROMPELOEIL_ANY
#define AT_LEAST                  TROMPELOEIL_AT_LEAST
#define AT_MOST                   TROMPELOEIL_AT_MOST
#define REQUIRE_DESTRUCTION       TROMPELOEIL_REQUIRE_DESTRUCTION
#define NAMED_REQUIRE_DESTRUCTION TROMPELOEIL_NAMED_REQUIRE_DESTRUCTION

#endif // TROMPELOEIL_LONG_MACROS

#endif // include guard
