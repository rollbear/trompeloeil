/*
 * Trompeloeil C++ mocking framework
 *
 * Copyright Björn Fahller 2014,2015
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

#if (!defined(__cplusplus) || __cplusplus <= 201103)
#error requires C++14 or higher
#endif

#include <tuple>
#include <iomanip>
#include <sstream>
#include <exception>
#include <functional>
#include <memory>
#include <cstring>
#include <cassert>
#include <algorithm>

#define TROMPELOEIL_ARG16(_0,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15, ...) _15

#define TROMPELOEIL_COUNT(...) TROMPELOEIL_ARG16(__VA_ARGS__, 15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0)


#define TROMPELOEIL_CONCAT_(x, y) x ## y
#define TROMPELOEIL_CONCAT(x, y) TROMPELOEIL_CONCAT_(x, y)

#define TROMPELOEIL_INIT_WITH_STR15(base, x, ...) base{#x, x}, TROMPELOEIL_INIT_WITH_STR14(base, __VA_ARGS__)
#define TROMPELOEIL_INIT_WITH_STR14(base, x, ...) base{#x, x}, TROMPELOEIL_INIT_WITH_STR13(base, __VA_ARGS__)
#define TROMPELOEIL_INIT_WITH_STR13(base, x, ...) base{#x, x}, TROMPELOEIL_INIT_WITH_STR12(base, __VA_ARGS__)
#define TROMPELOEIL_INIT_WITH_STR12(base, x, ...) base{#x, x}, TROMPELOEIL_INIT_WITH_STR11(base, __VA_ARGS__)
#define TROMPELOEIL_INIT_WITH_STR11(base, x, ...) base{#x, x}, TROMPELOEIL_INIT_WITH_STR10(base, __VA_ARGS__)
#define TROMPELOEIL_INIT_WITH_STR10(base, x, ...) base{#x, x}, TROMPELOEIL_INIT_WITH_STR9(base, __VA_ARGS__)
#define TROMPELOEIL_INIT_WITH_STR9(base, x, ...)  base{#x, x}, TROMPELOEIL_INIT_WITH_STR8(base, __VA_ARGS__)
#define TROMPELOEIL_INIT_WITH_STR8(base, x, ...)  base{#x, x}, TROMPELOEIL_INIT_WITH_STR7(base, __VA_ARGS__)
#define TROMPELOEIL_INIT_WITH_STR7(base, x, ...)  base{#x, x}, TROMPELOEIL_INIT_WITH_STR6(base, __VA_ARGS__)
#define TROMPELOEIL_INIT_WITH_STR6(base, x, ...)  base{#x, x}, TROMPELOEIL_INIT_WITH_STR5(base, __VA_ARGS__)
#define TROMPELOEIL_INIT_WITH_STR5(base, x, ...)  base{#x, x}, TROMPELOEIL_INIT_WITH_STR4(base, __VA_ARGS__)
#define TROMPELOEIL_INIT_WITH_STR4(base, x, ...)  base{#x, x}, TROMPELOEIL_INIT_WITH_STR3(base, __VA_ARGS__)
#define TROMPELOEIL_INIT_WITH_STR3(base, x, ...)  base{#x, x}, TROMPELOEIL_INIT_WITH_STR2(base, __VA_ARGS__)
#define TROMPELOEIL_INIT_WITH_STR2(base, x, ...)  base{#x, x}, TROMPELOEIL_INIT_WITH_STR1(base, __VA_ARGS__)
#define TROMPELOEIL_INIT_WITH_STR1(base, x)       base{#x, x}
#define TROMPELOEIL_INIT_WITH_STR0(base)
#define TROMPELOEIL_INIT_WITH_STR(base, ...) TROMPELOEIL_CONCAT(TROMPELOEIL_INIT_WITH_STR, TROMPELOEIL_COUNT(__VA_ARGS__))(base, __VA_ARGS__)

#define TROMPELOEIL_PARAM_LIST15(func_type)                  \
  TROMPELOEIL_PARAM_LIST14(func_type),                       \
  ::trompeloeil::param_list<func_type>::type<14> p15

#define TROMPELOEIL_PARAM_LIST14(func_type)                 \
  TROMPELOEIL_PARAM_LIST13(func_type),                       \
  ::trompeloeil::param_list<func_type>::type<13> p14

#define TROMPELOEIL_PARAM_LIST13(func_type)                 \
  TROMPELOEIL_PARAM_LIST12(func_type),                       \
  ::trompeloeil::param_list<func_type>::type<12> p13

#define TROMPELOEIL_PARAM_LIST12(func_type)                 \
  TROMPELOEIL_PARAM_LIST11(func_type),                       \
  ::trompeloeil::param_list<func_type>::type<11> p12

#define TROMPELOEIL_PARAM_LIST11(func_type)                 \
  TROMPELOEIL_PARAM_LIST10(func_type),                       \
  ::trompeloeil::param_list<func_type>::type<10> p11

#define TROMPELOEIL_PARAM_LIST10(func_type)                 \
  TROMPELOEIL_PARAM_LIST9(func_type),                       \
  ::trompeloeil::param_list<func_type>::type<9> p10

#define TROMPELOEIL_PARAM_LIST9(func_type)                 \
  TROMPELOEIL_PARAM_LIST8(func_type),                      \
  ::trompeloeil::param_list<func_type>::type<8> p9

#define TROMPELOEIL_PARAM_LIST8(func_type)                 \
  TROMPELOEIL_PARAM_LIST7(func_type),                      \
  ::trompeloeil::param_list<func_type>::type<7> p8

#define TROMPELOEIL_PARAM_LIST7(func_type)                 \
  TROMPELOEIL_PARAM_LIST6(func_type),                      \
  ::trompeloeil::param_list<func_type>::type<6> p7

#define TROMPELOEIL_PARAM_LIST6(func_type)                 \
  TROMPELOEIL_PARAM_LIST5(func_type),                      \
  ::trompeloeil::param_list<func_type>::type<5> p6

#define TROMPELOEIL_PARAM_LIST5(func_type)                 \
  TROMPELOEIL_PARAM_LIST4(func_type),                      \
    ::trompeloeil::param_list<func_type>::type<4> p5


#define TROMPELOEIL_PARAM_LIST4(func_type)                 \
  TROMPELOEIL_PARAM_LIST3(func_type),                      \
    ::trompeloeil::param_list<func_type>::type<3> p4

#define TROMPELOEIL_PARAM_LIST3(func_type)          \
  TROMPELOEIL_PARAM_LIST2(func_type),               \
  ::trompeloeil::param_list<func_type>::type<2> p3

#define TROMPELOEIL_PARAM_LIST2(func_type)         \
  TROMPELOEIL_PARAM_LIST1(func_type),              \
  ::trompeloeil::param_list<func_type>::type<1> p2

#define TROMPELOEIL_PARAM_LIST1(func_type) \
  ::trompeloeil::param_list<func_type>::type<0> p1

#define TROMPELOEIL_PARAM_LIST0(func_type)

#define TROMPELOEIL_PARAM_LIST(num, func_type) \
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
#define TROMPELOEIL_PARAMS1                        p1
#define TROMPELOEIL_PARAMS0

#define TROMPELOEIL_PARAMS(num) \
  TROMPELOEIL_CONCAT(TROMPELOEIL_PARAMS, num)

namespace trompeloeil
{
  template <typename T>
  struct param_list;

  template <typename R, typename ... P>
  struct param_list<R(P...)>
  {
    using param_tuple = std::tuple<P...>;
    static size_t const size = std::tuple_size<param_tuple>::value;
    template <size_t N>
    using type = typename std::tuple_element<N, param_tuple>::type;
  };

  class expectation_violation : public std::logic_error
  {
  public:
    using std::logic_error::logic_error;
  };

  enum class severity { fatal, nonfatal };

  using reporter_func = std::function<void(severity,
                                           std::string const &loc,
                                           std::string const &msg)>;
  inline
  reporter_func&
  reporter_obj()
  {
    static reporter_func obj = [](severity, auto const &loc, auto const &msg)
      {
        if (!std::current_exception())
        {
          throw expectation_violation(loc + "\n" + msg);
        }
      };
    return obj;
  }

  inline
  void set_reporter(reporter_func f)
  {
    reporter_obj() = std::move(f);
  }

  class tracer;

  inline
  tracer*&
  tracer_obj()
  {
    static tracer* ptr = nullptr;
    return ptr;
  }

  inline
  tracer*
  set_tracer(tracer* obj)
  {
    auto& ptr = tracer_obj();
    auto rv = ptr;
    ptr = obj;
    return rv;
  }

  class tracer
  {
  public:
    virtual void trace(const char *location, const std::string& call) = 0;
  protected:
    tracer() : previous(set_tracer(this)) {}
    tracer(const tracer&) = delete;
    tracer& operator=(const tracer&) = delete;
    virtual ~tracer() { set_tracer(previous); }
  private:
    tracer* previous = nullptr;
  };

  class stream_tracer : public tracer
  {
  public:
    stream_tracer(std::ostream& stream_) : stream(stream_) {}
    void trace(const char* location, const std::string& call) override
    {
      stream << location << '\n' << call << '\n';
    }
  private:
    std::ostream& stream;
  };


  inline
  void send_report(severity s, char const *loc, std::string const &msg)
  {
    reporter_obj()(s, loc, msg);
  }

  template <typename T, typename U>
  struct is_equal_comparable
  {
    template <typename T1, typename T2>
    static std::false_type func(...);
    template <typename T1, typename T2>
    static auto func(T1* p1, T2* p2) -> std::integral_constant<decltype(*p1 == *p2), true>;
    static bool const value = decltype(func<T,U>(nullptr, nullptr))::value;
  };

  template <typename T>
  struct is_value_type :
    std::integral_constant<bool,
                           std::is_same<T,
                                        typename std::decay<T>::type
                                        >::value
                           >
  {
  };

  template <typename T>
  class is_output_streamable
  {
    template <typename U>
    static std::false_type func(...);
    template <typename U>
    static auto func(U* u) -> std::is_same<std::ostream&, decltype(std::declval<std::ostream&>() << *u)>;
  public:
    static bool const value = decltype(func<T>(nullptr))::value;
  };

  struct stream_sentry
  {
    stream_sentry(std::ostream& os_)
      : os(os_)
      , width(os.width(0))
      , flags(os.flags(std::ios_base::dec | std::ios_base::left))
      , fill(os.fill(' '))
      {  }
    ~stream_sentry() { os.flags(flags); os.fill(fill); os.width(width);}
  private:
    std::ostream& os;
    std::streamsize width;
    std::ios_base::fmtflags flags;
    char fill;
  };

  template <typename T, bool b = is_output_streamable<T>::value>
  struct streamer
  {
    static void print(std::ostream& os, T const &t)
    {
      stream_sentry s(os);
      os << t;
    }
  };


  template <typename T>
  struct streamer<T, false>
  {
    static void print(std::ostream& os, T const &t)
    {
      stream_sentry s(os);

      os << sizeof(T) << "-byte object={";
      if (sizeof(T) > 8) os << '\n';
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
  void print(std::ostream& os, T const &t)
  {
    streamer<T>::print(os, t);
  }

  template <typename T, typename Deleter>
  class list;

  template <typename T>
  class list_elem
  {
  public:
    list_elem(const list_elem&) = delete;
    list_elem& operator=(const list_elem&) = delete;
    list_elem(list_elem &&r) noexcept : next(r.next), prev(&r)
    {
      r.invariant_check();

      next->prev = this;
      r.next = this;

      assert(next->prev == this);
      assert(prev->next == this);

      r.unlink();

      assert(!r.is_linked());
      invariant_check();
    }
    ~list_elem() { unlink(); }
    void unlink() noexcept
    {
      invariant_check();
      auto n = next;
      auto p = prev;
      n->prev = p;
      p->next = n;
      next = this;
      prev = this;
      invariant_check();
    };
    void invariant_check() const noexcept
    {
      assert(next->prev == this);
      assert(prev->next == this);
      assert((next == this) == (prev == this));
      assert((prev->next == next) == (next == this));
      assert((next->prev == prev) == (prev == this));
      auto pp = prev;
      auto nn = next;
      do {
        assert((nn == this) == (pp == this));
        assert(nn->next->prev == nn);
        assert(nn->prev->next == nn);
        assert(pp->next->prev == pp);
        assert(pp->prev->next == pp);
        assert((nn->next == nn) == (nn == this));
        assert((pp->prev == pp) == (pp == this));
        nn = nn->next;
        pp = pp->prev;
      } while (nn != this);
    }
    bool is_linked() const noexcept
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
    void dispose(T*) const {}
  };

  class delete_disposer
  {
  protected:
    template <typename T>
    void dispose(T* t) const { delete t; }
  };
  template <typename T, typename Disposer = ignore_disposer>
  class list : private list_elem<T>, private Disposer
  {
  public:
    ~list();
    class iterator;
    iterator begin() const noexcept;
    iterator end() const noexcept;
    iterator push_front(T* t) noexcept;
    iterator push_back(T* t) noexcept;
    void erase(iterator i) noexcept;
    auto empty() const noexcept { return begin() == end(); };
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
    iterator() noexcept = default;
    bool operator==(const iterator& rh) const noexcept { return p == rh.p; }
    bool operator!=(const iterator& rh) const noexcept { return !(*this == rh); }
    iterator& operator++() noexcept
    {
      p = p->next;
      return *this;
    }
    iterator operator++(int) noexcept
    {
      auto rv = *this;
      operator++();
      return rv;
    }
    T& operator*() noexcept
    {
      return static_cast<T&>(*p);
    }
    T* operator->() noexcept
    {
      return static_cast<T*>(p);
    }
  private:
    iterator(const list_elem<T>* t) noexcept : p{const_cast<list_elem<T>*>(t)} {}
    list_elem<T>* p;
  };

  template <typename T, typename Disposer>
  list<T, Disposer>::~list()
  {
    auto i = this->begin();
    while (i != this->end())
    {
      auto prev = i++;
      Disposer::dispose(&*prev);
    }
  }
  template <typename T, typename Disposer>
  auto list<T, Disposer>::begin() const noexcept -> iterator
  {
    return {next};
  }

  template <typename T, typename Disposer>
  auto list<T, Disposer>::end() const noexcept -> iterator
  {
    return {this};
  }

  template <typename T, typename Disposer>
  auto list<T, Disposer>::push_front(T* t) noexcept -> iterator
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
  auto list<T, Disposer>::push_back(T* t) noexcept -> iterator
  {
    invariant_check();
    t->prev = prev;
    t->next = this;
    prev->next = t;
    prev = t;
    invariant_check();
    return {t};
  }

  template <typename T, typename Disposer>
  void list<T, Disposer>::erase(iterator i) noexcept
  {
    i->unlink();
  }

  class sequence_matcher;

  class sequence
  {
  public:
    sequence() noexcept = default;
    sequence(sequence&&) = default;
    virtual ~sequence() = default;
    bool is_first(sequence_matcher const *m) const noexcept
    {
      return !matchers.empty() && &*matchers.begin() == m;
    }
    void add_last(sequence_matcher *m) noexcept;

    void check_match(
      char const *seq_name,
      char const* match_name,
      char const* loc)
    const;

  private:
    list<sequence_matcher> matchers;
  };

  struct sequence_matcher : list_elem<sequence_matcher>
  {
    sequence_matcher(
      char const* name_,
      sequence& s)
    noexcept
    : seq_name(name_)
    , seq(s)
    {
      s.add_last(this);
    }

    void
    set_expectation(char const *name, char const *loc)
    noexcept
    {
      exp_name = name;
      exp_loc = loc;
    }

    void
    check_match(char const *match_name, char const* loc)
    const
    {
      seq.check_match(seq_name, match_name, loc);
    }
    bool
    is_first()
    const noexcept
    {
      return seq.is_first(this);
    }

    void
    retire()
    noexcept
    {
      this->unlink();
    }
    void print_expectation(std::ostream& os) const
    {
      os << exp_name << " at " << exp_loc;
    }
  private:
    char const    *seq_name;
    char const    *exp_name = nullptr;
    char const    *exp_loc = nullptr;
    sequence&      seq;
  };

  inline
  void
  sequence::check_match(
    char const* seq_name,
    char const* match_name,
    char const* loc)
  const
  {
    for (auto& m : matchers)
    {
      std::ostringstream os;
      os << "Sequence mismatch for sequence \"" << seq_name
         << "\" with matching call of " << match_name << " at " << loc
         << ". Sequence \"" << seq_name << "\" has ";
      m.print_expectation(os);
      os << " first in line\n";
      send_report(severity::fatal, loc, os.str());
    }
  }

  inline
  void
  sequence::add_last(
    sequence_matcher *m)
  noexcept
  {
    matchers.push_back(m);
  }

  struct wildcard
  {
    template<typename T>
    operator T&&() const;
    template<typename T>
    operator T&() const;
  };

  template <typename T>
  bool operator==(wildcard const&, T const&) noexcept { return true; }
  template <typename T>
  bool operator==(T const&, wildcard const&) noexcept { return true; }

  static constexpr wildcard const _{};

  template <typename T>
  struct typed_wildcard
  {
    operator T() const;
  };

  template <typename T, typename U>
  auto operator==(typed_wildcard<T> const&, U const&) noexcept ->
    typename std::enable_if<std::is_same<typename std::decay<T>::type,
                                         typename std::decay<U>::type>::value,
                            bool>::type
  {
    return true;
  }

  template <typename T, typename U>
  auto operator==(T const&, typed_wildcard<U> const&) noexcept ->
    typename std::enable_if<std::is_same<typename std::decay<T>::type,
                                         typename std::decay<U>::type>::value,
                            bool>::type
  {
    return true;
  }

  template <typename T>
  std::ostream& operator<<(std::ostream& os, typed_wildcard<T> const&)
  {
    return os << '_';
  }

  struct lifetime_monitor;

  template <typename T>
  class deathwatched : public T
  {
    static_assert(std::has_virtual_destructor<T>::value,
                  "virtual destructor is a necessity for deathwatched to work");
  public:
    using T::T;
    ~deathwatched();
    trompeloeil::lifetime_monitor*&
    trompeloeil_expect_death(trompeloeil::lifetime_monitor* monitor)
    const noexcept
    {
      return trompeloeil_lifetime_monitor = monitor;
    }
  private:
    mutable trompeloeil::lifetime_monitor *trompeloeil_lifetime_monitor=nullptr;
  };

  struct lifetime_monitor
  {
    template <typename T>
    lifetime_monitor(
      ::trompeloeil::deathwatched<T> const &obj,
      char const* obj_name,
      char const *loc)
    noexcept
      : object_monitor(obj.trompeloeil_expect_death(this))
      , location(loc)
      , object_name(obj_name)
    {
    }
    ~lifetime_monitor() noexcept(false)
    {
      if (!died)
      {
        std::ostringstream os;
        os << "Object " << object_name << " is still alive";
        send_report(severity::nonfatal, location, os.str());
        object_monitor = nullptr; // prevent its death poking this cadaver
      }
    }
    void notify() noexcept { died = true; }

    bool               died = false;
    lifetime_monitor *&object_monitor;
    char const        *location;
    char const        *object_name;
  };

  template <typename T>
  deathwatched<T>::~deathwatched()
  {
    if (trompeloeil_lifetime_monitor)
    {
      trompeloeil_lifetime_monitor->notify();
      return;
    }
    std::ostringstream os;
    os << "Unexpected destruction of "
       << typeid(T).name() << "@" << this << '\n';
    trompeloeil::send_report(trompeloeil::severity::nonfatal,
                             "",
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

  template <typename Sig>
  struct default_return_t
  {
    static return_of_t<Sig> value()
    {
      typename std::remove_reference<return_of_t<Sig>>::type *p = nullptr;
      return std::forward<return_of_t<Sig>>(*p);
    }
  };

  template <typename ... A>
  struct default_return_t<void(A...)>
  {
    static void value() {}
  };

  template <typename Sig>
  return_of_t<Sig> default_return(call_params_type_t<Sig>&)
  {
    return default_return_t<Sig>::value();
  }


  template<typename Sig>
  struct call_matcher_list;

  template<typename Sig>
  struct call_matcher_base : public list_elem<call_matcher_base<Sig> >
  {
    call_matcher_base() = default;
    virtual
    ~call_matcher_base() = default;

    virtual
    bool
    matches(call_params_type_t<Sig> const&) const = 0;

    virtual
    bool
    first_in_sequence() const noexcept = 0;

    virtual
    void
    log_call(tracer* obj, call_params_type_t<Sig>& p) const = 0;

    virtual
    void
    run_actions(
      call_params_type_t<Sig> &,
      call_matcher_list<Sig> &saturated_list
    ) = 0;

    virtual
    std::ostream&
    report_signature(std::ostream&) const = 0;

    virtual
    std::ostream&
    report_mismatch(std::ostream&, call_params_type_t<Sig> const &) = 0;

    virtual
    return_of_t<Sig>
    return_value(call_params_type_t<Sig>& p)
    {
      return default_return<Sig>(p);
    }

    virtual
    void
    report_missed() = 0;
  };

  template<typename T, size_t N = 0, bool b = N == std::tuple_size<T>::value>
  struct tuple_print
  {
    template <typename stream, typename U>
    static stream &mismatch(stream &os, T const &t1, U const &t2)
    {
      if (!(std::get<N>(t1) == std::get<N>(t2)))
      {
        os << "  Expected " << std::setw((N<9)+1) << '_' << N+1 << " = ";
        print(os, std::get<N>(t1));
        os << '\n';
      }
      return tuple_print<T, N + 1>::mismatch(os, t1, t2);
    }

    template <typename stream, typename U>
    static stream &missed(stream &os, U const &t)
    {
      os << "  param " << std::setw((N<9)+1) << "_" << N+1 << " = ";
      print(os, std::get<N>(t));
      os << '\n';
      return tuple_print<T, N + 1>::missed(os, t);
    }
  };

  template<typename T, size_t N>
  struct tuple_print<T, N, true>
  {
    template <typename stream, typename U>
    static stream &mismatch(stream &os, T const &, U const &)
    {
      return os;
    }

    template <typename stream, typename U>
    static stream& missed(stream &os, U const &)
    {
      return os;
    }
  };

  template <typename T>
  std::string missed_values(T const &t)
  {
    std::ostringstream os;
    tuple_print<T>::missed(os, t);
    return os.str();
  }

  template <typename T, typename ... A>
  struct all_are
    : std::is_same<std::tuple<A...>,
                   std::tuple<typename std::conditional<true, T, A>::type...>>
  {
  };

  template<typename Sig>
  struct call_matcher_list : public list<call_matcher_base<Sig>>
  {
    template<typename ... U>
    call_matcher_list &operator()(U const &...) { return *this; }
  };

  template <typename Sig>
  call_matcher_base<Sig>*
  find(call_params_type_t<Sig> const &p,
       call_matcher_list<Sig>        &list)
  noexcept
  {
    {
      auto i = std::find_if(list.begin(), list.end(),
                            [&](auto& m)
                            { return m.matches(p) && m.first_in_sequence(); });
      if (i != list.end()) return &*i;
    }
    {
      auto i = std::find_if(list.begin(), list.end(),
                            [&](auto& m) { return m.matches(p); });
      if (i != list.end()) return &*i;
    }
    return nullptr;
  }

  template <typename Sig>
  void
  report_mismatch(char const                    *name,
                  call_params_type_t<Sig> const &p,
                  call_matcher_list<Sig>        &matcher_list,
                  call_matcher_list<Sig>        &saturated_list)
  {
    std::ostringstream os;
    os << "No match for call of " << name << " with.\n" << missed_values(p);
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
    trompeloeil::send_report(severity::fatal, "", os.str());
  }

  template<typename Sig>
  struct condition_base : public list_elem<condition_base<Sig> >
  {
    virtual ~condition_base() = default;
    virtual bool check(call_params_type_t<Sig> const&) const = 0;
    virtual char const* name() const noexcept = 0;
  };

  template <typename Sig>
  using condition_list = list<condition_base<Sig>, delete_disposer >;

  template<typename Sig, typename Cond>
  struct condition : public condition_base<Sig>
  {
    condition(char const *str_, Cond c_) : c(c_), str(str_) {}

    bool check(call_params_type_t<Sig> const & t) const override
    {
      return c(t);
    }

    char const *name() const noexcept override { return str; }
  private:
    Cond c;
    char const *str;
  };


  template<typename Sig>
  struct side_effect_base : public list_elem<side_effect_base<Sig> >
  {
    virtual ~side_effect_base() = default;
    virtual void action(call_params_type_t<Sig> &) const = 0;
  };

  template<typename Sig>
  using side_effect_list = list<side_effect_base<Sig>, delete_disposer>;

  template<typename Sig, typename Action>
  struct side_effect : public side_effect_base<Sig>
  {
    template <typename A>
    side_effect(A&& a_) : a(std::forward<A>(a_)) {}

    void action(call_params_type_t<Sig> &t) const { a(t); }
  private:
    Action a;
  };

  template <typename Sig>
  using return_handler_sig = return_of_t<Sig>(call_params_type_t<Sig>&);

  struct sequence_handler_base
  {
    virtual ~sequence_handler_base() noexcept = default;
    virtual void set_expectation(char const *, char const*) noexcept = 0;
    virtual void validate(char const*, char const*) = 0;
    virtual bool is_first() const noexcept = 0;
    virtual void retire() noexcept = 0;
  };

  template <typename ... Seq>
  struct sequence_handler : public sequence_handler_base
  {
  public:
    template <typename ... S>
    sequence_handler(S&& ... s) : matchers{std::forward<S>(s)...} {}
    void set_expectation(char const *exp_name, char const *exp_loc) noexcept
    {
      for (auto& e : matchers)
      {
        e.set_expectation(exp_name, exp_loc);
      }
    }
    void validate(char const *match_name, char const *loc)
    {
      if (is_first()) return;
      for (auto& e : matchers)
      {
        e.check_match(match_name, loc);
      }
    }
    bool is_first() const noexcept
    {
      return std::all_of(std::begin(matchers), std::end(matchers),
                         [](auto& e) { return e.is_first(); });
    }
    void retire() noexcept
    {
      for (auto& e : matchers)
      {
        e.retire();
      }
    }
  private:
    sequence_matcher matchers[sizeof...(Seq)];
  };

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
  struct sequence_injector : Parent
  {
    static bool const sequence_set = true;
  };

  template <typename Matcher, typename Parent = std::tuple<>>
  struct call_modifier : public Parent
  {
    using typename Parent::signature;
    using typename Parent::return_type;
    using Parent::call_limit_set;
    using Parent::upper_call_limit;
    using Parent::sequence_set;
    using Parent::throws;
    using Parent::side_effects;

    call_modifier(Matcher& m) : matcher(m) {}

    template <typename D>
    call_modifier&
    with(char const* str, D&& d)
    {
      matcher.add_condition(str, std::forward<D>(d));
      return *this;
    }

    template <typename A>
    call_modifier<Matcher, sideeffect_injector<Parent>>
    sideeffect(A&& a)
    {
      static_assert(upper_call_limit > 0,
                    "SIDE_EFFECT for forbidden call does not make sense");
      matcher.add_side_effect(std::forward<A>(a));
      return {matcher};
    }

    template <typename H,
              typename = typename std::enable_if<is_value_type<H>::value>::type>
    call_modifier<Matcher, return_injector<return_of_t<signature>, Parent > >
    handle_return(H&& h)
    {
      using params_type = call_params_type_t<signature>&;
      using ret = typename std::result_of<H(params_type)>::type;

      static_assert(std::is_constructible<return_of_t<signature>, ret>::value
                    || !std::is_same<return_of_t<signature>, void>::value,
                    "RETURN does not make sense for void-function");
      static_assert(std::is_constructible<return_of_t<signature>, ret>::value
                    || std::is_same<return_of_t<signature>, void>::value,
                    "RETURN value is not convertible to the return type of the function");
      static_assert(std::is_same<return_type, void>::value,
                    "Multiple RETURN does not make sense");
      static_assert(!throws || upper_call_limit == 0,
                    "THROW and RETURN does not make sense");
      static_assert(upper_call_limit > 0ULL,
                    "RETURN for forbidden call does not make sense");
      matcher.set_return(std::move(h));
      return {matcher};
    }

    template <typename H>
    call_modifier<Matcher, throw_injector<Parent> >
    handle_throw(H&& h)
    {
      static_assert(!throws,
                    "Multiple THROW does not make sense");
      static_assert(std::is_same<return_type, void>::value,
                    "THROW and RETURN does not make sense");
      matcher.set_return([=](auto& p)
                         {
                           h(p);
                           return default_return<signature>(p);
                         });
      return {matcher};
    }
    template <unsigned long long L,
              unsigned long long H = L,
              bool               verboten = call_limit_set>
    call_modifier<Matcher, call_limit_injector<Parent, H> >
    times()
    {
      static_assert(!verboten,
                    "Only one TIMES call limit is allowed, but it can express an interval");

      static_assert(H >= L,
                    "In TIMES the first value must not exceed the second");

      static_assert(H > 0 || !throws,
                    "THROW and TIMES(0) does not make sense");

      static_assert(H > 0 || std::is_same<return_type, void>::value,
                    "RETURN and TIMES(0) does not make sense");

      static_assert(H > 0 || !side_effects,
                    "SIDE_EFFECT and TIMES(0) does not make sense");

      matcher.min_calls = L;
      matcher.max_calls = H;
      return {matcher};
    }

    template <typename ... T,
              bool b = sequence_set>
    typename std::enable_if<all_are<sequence_matcher, T...>::value,
                            call_modifier<Matcher, sequence_injector<Parent> >
                            >::type
    in_sequence(T&& ... t)
    {
      static_assert(!b,
                    "Multiple IN_SEQUENCE does not make sense."
                    " You can list several sequence objects at once");

      matcher.set_sequence(std::forward<T>(t)...);
      return {matcher};
    }
    Matcher& matcher;
  };

  struct expectation {
    virtual ~expectation() = default;
  };

  inline
  void
  report_unfulfilled(char const        *name,
                     std::string const &values,
                     unsigned long long min_calls,
                     unsigned long long call_count,
                     char const        *location)
  {
    std::ostringstream os;
    os << "Unfulfilled expectation:\n"
       << "Expected " << name << " to be called ";
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
    send_report(severity::nonfatal, location, os.str());
  }

  inline
  void
  report_forbidden_call(
    char const *name,
    char const *loc,
    std::string const& values)
  {
    std::ostringstream os;
    os << "Match of forbidden call of " << name << " at " << loc << '\n'
       << values;
    send_report(severity::fatal, loc, os.str());
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
    template<typename ... U>
    call_matcher(U &&... u) : val(std::forward<U>(u)...) {}

    call_matcher(call_matcher &&r) = delete;

    ~call_matcher()
    {
      if (!reported && this->is_linked() && call_count < min_calls)
      {
        report_missed();
      }
    }

    call_matcher& hook_last(call_matcher_list<Sig> &list) noexcept
    {
      list.push_front(this);
      return *this;
    }

    bool
    matches(call_params_type_t<Sig> const& params)
    const override
    {
      return val == params && match_conditions(params);
    }

    bool
    match_conditions(call_params_type_t<Sig> const & params)
    const
    {
      return std::all_of(conditions.begin(), conditions.end(),
                         [&](auto& c) { return c.check(params); });
    }

    bool
    first_in_sequence()
    const noexcept override
    {
      auto saturated = call_count >= min_calls;
      return saturated || !sequences || sequences->is_first();
    }

    return_of_t<Sig>
    return_value(call_params_type_t<Sig>& params)
    override
    {
      return return_handler(params);
    }

    void
    log_call(
      tracer* obj,
      call_params_type_t<Sig>& params
    )
    const override
    {
      std::ostringstream os;
      os << name << " with.\n" << missed_values(params);
      obj->trace(location, os.str());
    }
    void
    run_actions(
      call_params_type_t<Sig>& params,
      call_matcher_list<Sig> &saturated_list)
    override
    {
      if (call_count < min_calls && sequences)
      {
        sequences->validate(name, location);
      }
      if (max_calls == 0)
      {
        reported = true;
        report_forbidden_call(name, location, missed_values(params));
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
    report_signature(std::ostream& os)
    const override
    {
      return os << name << " at " << location;
    }

    std::ostream&
    report_mismatch(
      std::ostream& os,
      call_params_type_t<Sig> const & params)
    override
    {
      reported = true;
      report_signature(os);
      if (val == params)
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
        tuple_print<Value>::mismatch(os, val, params);
      }
      return os;
    }

    void
    report_missed()
    override
    {
      reported = true;
      report_unfulfilled(name,
                         missed_values(val),
                         min_calls,
                         call_count,
                         location);
    }

    call_matcher
    &set_location(char const *loc)
    noexcept
    {
      location = loc;
      return *this;
    }

    call_matcher&
    set_name(char const *func_name)
    noexcept
    {
      name = func_name;
      return *this;
    }

    template <typename C>
    void
    add_condition(char const *str, C&& c)
    {
      auto cond = new condition<Sig, C>(str, std::forward<C>(c));
      conditions.push_back(cond);
    }
    template <typename S>
    void
    add_side_effect(S&& s)
    {
      auto effect = new side_effect<Sig, S>(std::forward<S>(s));
      actions.push_back(effect);
    }

    template <typename ... T>
    void
    set_sequence(T&& ... t)
    {
      auto seq = new sequence_handler<T...>(std::forward<T>(t)...);
      seq->set_expectation(name, location);
      sequences.reset(seq);
    }

    inline
    void
    set_return(std::function<return_handler_sig<Sig> >&& h)
    {
      return_handler = std::move(h);
    }

    condition_list<Sig>                    conditions;
    side_effect_list<Sig>                  actions;
    std::function<return_handler_sig<Sig>> return_handler = default_return<Sig>;
    std::unique_ptr<sequence_handler_base> sequences;
    char const                            *location;
    char const                            *name;
    unsigned long long                     call_count = 0;
    unsigned long long                     min_calls = 1;
    unsigned long long                     max_calls = 1;
    Value                                  val;
    bool                                   reported = false;
  };


  template <typename T, int N, bool b = N <= std::tuple_size<T>::value>
  struct arg
  {
    static typename std::tuple_element<N - 1,T>::type value(T& t)
    {
      return std::get<N - 1>(t);
    }
  };

  template <int N>
  struct illegal_argument {
    illegal_argument(illegal_argument&&) = delete;
    illegal_argument operator&() const = delete;
    template <typename T>
    operator T() const = delete;
  };

  template <typename T, int N>
  struct arg<T, N, false>
  {
    static illegal_argument<N> const & value(T&) noexcept
    {
      static illegal_argument<N> const constexpr v{};
      return v;
    }

  };

  template <int N, typename T>
  auto mkarg(T& t) noexcept -> decltype(arg<T, N>::value(t))
  {
    return arg<T, N>::value(t);
  }

  template <typename ... T>
  void ignore(T const& ...) noexcept {}

  template <typename ... T>
  call_params_type_t<void(T...)> make_params_type_obj(T&& ... t)
  {
    return call_params_type_t<void(T...)>(t...);
  }

  struct call_validator
  {
    template <typename T>
    static
    void
    assert_return_type(T&)
    noexcept
    {
      static_assert(T::throws
                    || std::is_same<typename T::return_type,
                                    return_of_t<typename T::signature> >::value,
                    "RETURN missing for non-void function");
    }

    template <typename M, typename Info>
    std::unique_ptr<expectation>
    operator+(call_modifier<M, Info>& t)
    const
    {
      assert_return_type(t);
      return std::unique_ptr<expectation>(&t.matcher);
    }

    template <typename M, typename Info>
    std::unique_ptr<expectation>
    operator+(call_modifier<M, Info>&& t)
    const
    {
      return operator+(t);
    }
  };

  template <typename Sig, typename Value>
  auto make_call_modifier(call_matcher<Sig, Value>& m)
    -> trompeloeil::call_modifier<call_matcher<Sig, Value>, matcher_info<Sig> >
  {
    return {m};
  }

  template <typename sig, typename ... U>
  auto make_call_matcher(U&& ... u)
    -> ::trompeloeil::call_matcher<sig, decltype(std::make_tuple(std::forward<U>(u)...))>&
  {
    return * new ::trompeloeil::call_matcher<sig, decltype(std::make_tuple(std::forward<U>(u)...))>( std::forward<U>(u)... );
  }

}

#define TROMPELOEIL_ID(name) \
  TROMPELOEIL_CONCAT(trompeloeil_ ## name ## _, __LINE__)

#define TROMPELOEIL_MAKE_MOCK1(name, sig) \
  TROMPELOEIL_MAKE_MOCK_(name,,1, sig, #name, #sig)
#define TROMPELOEIL_MAKE_MOCK2(name, sig) \
  TROMPELOEIL_MAKE_MOCK_(name,,2, sig, #name, #sig)
#define TROMPELOEIL_MAKE_MOCK3(name, sig) \
  TROMPELOEIL_MAKE_MOCK_(name,,3, sig, #name, #sig)
#define TROMPELOEIL_MAKE_MOCK4(name, sig) \
  TROMPELOEIL_MAKE_MOCK_(name,,4, sig, #name, #sig)
#define TROMPELOEIL_MAKE_MOCK5(name, sig) \
  TROMPELOEIL_MAKE_MOCK_(name,,5, sig, #name, #sig)
#define TROMPELOEIL_MAKE_MOCK6(name, sig) \
  TROMPELOEIL_MAKE_MOCK_(name,,6, sig, #name, #sig)
#define TROMPELOEIL_MAKE_MOCK7(name, sig) \
  TROMPELOEIL_MAKE_MOCK_(name,,7, sig, #name, #sig)
#define TROMPELOEIL_MAKE_MOCK8(name, sig) \
  TROMPELOEIL_MAKE_MOCK_(name,,8, sig, #name, #sig)
#define TROMPELOEIL_MAKE_MOCK9(name, sig) \
  TROMPELOEIL_MAKE_MOCK_(name,,9, sig, #name, #sig)
#define TROMPELOEIL_MAKE_MOCK10(name, sig) \
  TROMPELOEIL_MAKE_MOCK_(name,,10, sig, #name, #sig)
#define TROMPELOEIL_MAKE_MOCK11(name, sig) \
  TROMPELOEIL_MAKE_MOCK_(name,,11, sig, #name, #sig)
#define TROMPELOEIL_MAKE_MOCK12(name, sig) \
  TROMPELOEIL_MAKE_MOCK_(name,,12, sig, #name, #sig)
#define TROMPELOEIL_MAKE_MOCK13(name, sig) \
  TROMPELOEIL_MAKE_MOCK_(name,,13, sig, #name, #sig)
#define TROMPELOEIL_MAKE_MOCK14(name, sig) \
  TROMPELOEIL_MAKE_MOCK_(name,,14, sig, #name, #sig)
#define TROMPELOEIL_MAKE_MOCK15(name, sig) \
  TROMPELOEIL_MAKE_MOCK_(name,,15, sig, #name, #sig)

#define TROMPELOEIL_MAKE_CONST_MOCK1(name, sig) \
  TROMPELOEIL_MAKE_MOCK_(name, const, 1, sig, #name, #sig)
#define TROMPELOEIL_MAKE_CONST_MOCK2(name, sig) \
  TROMPELOEIL_MAKE_MOCK_(name,const,2, sig, #name, #sig)
#define TROMPELOEIL_MAKE_CONST_MOCK3(name, sig) \
  TROMPELOEIL_MAKE_MOCK_(name,const,3, sig, #name, #sig)
#define TROMPELOEIL_MAKE_CONST_MOCK4(name, sig) \
  TROMPELOEIL_MAKE_MOCK_(name,const,4, sig, #name, #sig)
#define TROMPELOEIL_MAKE_CONST_MOCK5(name, sig) \
  TROMPELOEIL_MAKE_MOCK_(name,const,5, sig, #name, #sig)
#define TROMPELOEIL_MAKE_CONST_MOCK6(name, sig) \
  TROMPELOEIL_MAKE_MOCK_(name,const,6, sig, #name, #sig)
#define TROMPELOEIL_MAKE_CONST_MOCK7(name, sig) \
  TROMPELOEIL_MAKE_MOCK_(name,const,7, sig, #name, #sig)
#define TROMPELOEIL_MAKE_CONST_MOCK8(name, sig) \
  TROMPELOEIL_MAKE_MOCK_(name,const,8, sig, #name, #sig)
#define TROMPELOEIL_MAKE_CONST_MOCK9(name, sig) \
  TROMPELOEIL_MAKE_MOCK_(name,const,9, sig, #name, #sig)
#define TROMPELOEIL_MAKE_CONST_MOCK10(name, sig) \
  TROMPELOEIL_MAKE_MOCK_(name,const,10, sig, #name, #sig)
#define TROMPELOEIL_MAKE_CONST_MOCK11(name, sig) \
  TROMPELOEIL_MAKE_MOCK_(name,const,11, sig, #name, #sig)
#define TROMPELOEIL_MAKE_CONST_MOCK12(name, sig) \
  TROMPELOEIL_MAKE_MOCK_(name,const,12, sig, #name, #sig)
#define TROMPELOEIL_MAKE_CONST_MOCK13(name, sig) \
  TROMPELOEIL_MAKE_MOCK_(name,const,13, sig, #name, #sig)
#define TROMPELOEIL_MAKE_CONST_MOCK14(name, sig) \
  TROMPELOEIL_MAKE_MOCK_(name,const,14, sig, #name, #sig)
#define TROMPELOEIL_MAKE_CONST_MOCK15(name, sig) \
  TROMPELOEIL_MAKE_MOCK_(name,const,15, sig, #name, #sig)


#define TROMPELOEIL_MAKE_MOCK_(name, constness, num, sig, name_s, sig_s) \
  using TROMPELOEIL_ID(matcher_list_type) = ::trompeloeil::call_matcher_list<sig>; \
  mutable TROMPELOEIL_ID(matcher_list_type) TROMPELOEIL_ID(matcher_list); \
  mutable TROMPELOEIL_ID(matcher_list_type) TROMPELOEIL_ID(saturated_matcher_list); \
  struct TROMPELOEIL_ID(tag_type_trompeloeil)                           \
  {                                                                     \
    template <typename ... U>                                           \
    static auto name(U&& ... u) -> decltype(::trompeloeil::make_call_matcher<sig>(std::forward<U>(u)...)) \
    {                                                                   \
      /* provoke warnings for sign mismatch with applicable compiler flags*/ \
      using param_type = ::trompeloeil::call_params_type_t<sig>;        \
      using call_type = decltype(std::make_tuple(std::forward<U>(u)...)); \
      auto check = ::trompeloeil::is_equal_comparable<param_type, call_type>::value; \
      ::trompeloeil::ignore(check);                                     \
      return ::trompeloeil::make_call_matcher<sig>(std::forward<U>(u)...); \
    }                                                                   \
  };                                                                    \
  TROMPELOEIL_ID(tag_type_trompeloeil) trompeloeil_tag_ ## name(TROMPELOEIL_PARAM_LIST(num, sig)) constness; \
  TROMPELOEIL_ID(matcher_list_type)&                                    \
  trompeloeil_matcher_list(TROMPELOEIL_ID(tag_type_trompeloeil)) constness noexcept \
  {                                                                     \
    return TROMPELOEIL_ID(matcher_list);                                \
  }                                                                     \
  auto name(TROMPELOEIL_PARAM_LIST(num, sig)) constness -> ::trompeloeil::return_of_t<sig> \
  {                                                                     \
    auto param_value = ::trompeloeil::make_params_type_obj(TROMPELOEIL_PARAMS(num)); \
    auto i = ::trompeloeil::find(param_value, TROMPELOEIL_ID(matcher_list));\
    if (!i)                                                             \
    {                                                                   \
      ::trompeloeil::report_mismatch(name_s " with signature " sig_s,   \
                                     param_value,                       \
                                     TROMPELOEIL_ID(matcher_list),      \
                                     TROMPELOEIL_ID(saturated_matcher_list)); \
    }                                                                   \
    if (auto obj = ::trompeloeil::tracer_obj())                         \
    {                                                                   \
      i->log_call(obj, param_value);                                    \
    }                                                                   \
    i->run_actions(param_value, TROMPELOEIL_ID(saturated_matcher_list)); \
    return i->return_value(param_value);                                \
  }                                                                     \
  using TROMPELOEIL_ID(signature_trompeloeil_ ## name) = sig



#define TROMPELOEIL_STRINGIFY_(...) #__VA_ARGS__
#define TROMPELOEIL_STRINGIFY(...) TROMPELOEIL_STRINGIFY_(__VA_ARGS__)

#define TROMPELOEIL_REQUIRE_CALL(obj, func)                    \
  TROMPELOEIL_REQUIRE_CALL_(obj, func, #obj, #func)

#define TROMPELOEIL_REQUIRE_CALL_(obj, func, obj_s, func_s)                    \
  auto TROMPELOEIL_ID(call_obj) =  TROMPELOEIL_REQUIRE_CALL_OBJ(obj, func, obj_s, func_s)


#define TROMPELOEIL_NAMED_REQUIRE_CALL(obj, func) \
  TROMPELOEIL_NAMED_REQUIRE_CALL(obj, func, #obj, #func)

#define TROMPELOEIL_NAMED_REQUIRE_CALL_(obj, func, obj_s, func_s)       \
  TROMPELOEIL_REQUIRE_CALL_OBJ(obj, func, obj_s, func_s)

#define TROMPELOEIL_REQUIRE_CALL_OBJ(obj, func, obj_s, func_s)          \
  ::trompeloeil::call_validator{} +                                     \
  ::trompeloeil::make_call_modifier(decltype((obj).TROMPELOEIL_CONCAT(trompeloeil_tag_, func) )::func \
  .set_location(__FILE__ ":" TROMPELOEIL_STRINGIFY(__LINE__))           \
  .set_name(obj_s "." func_s)                                           \
  .hook_last((obj).trompeloeil_matcher_list(decltype(TROMPELOEIL_CONCAT((obj).trompeloeil_tag_, func)){})))


#define TROMPELOEIL_ALLOW_CALL(obj, func) \
  TROMPELOEIL_ALLOW_CALL_(obj, func, #obj, #func)

#define TROMPELOEIL_ALLOW_CALL_(obj, func, obj_s, func_s)               \
  TROMPELOEIL_REQUIRE_CALL_(obj, func, obj_s, func_s).TIMES(0, ~0ULL)


#define TROMPELOEIL_NAMED_ALLOW_CALL(obj, func) \
  TROMPELOEIL_NAMED_ALLOW_CALL(obj, func, #obj, #func)

#define TROMPELOEIL_NAMED_ALLOW_CALL_(obj, func, obj_s, func_s)      \
  TROMPELOEIL_NAMED_REQUIRE_CALL_(obj, func, obj_s, func_s).TIMES(0, ~0ULL)

#define TROMPELOEIL_FORBID_CALL(obj, func) \
  TROMPELOEIL_FORBID_CALL_(obj, func, #obj, #func)

#define TROMPELOEIL_FORBID_CALL_(obj, func, obj_s, func_s)     \
  TROMPELOEIL_REQUIRE_CALL_(obj, func, obj_s, func_s).TIMES(0).THROW(false)

#define TROMPELOEIL_NAMED_FORBID_CALL(obj, func) \
  TROMPELOEIL_NAMED_FORBID_CALL_(obj, func, #obj, #func)

#define TROMPELOEIL_NAMED_FORBID_CALL_(obj, func, obj_s, func_s)       \
  TROMPELOEIL_NAMED_REQUIRE_CALL_(obj, func, obj_s, func_s).TIMES(0).THROW(false)



#define TROMPELOEIL_WITH(...) \
  TROMPELOEIL_WITH(=,#__VA_ARGS__, __VA_ARGS__)
#define TROMPELOEIL_LR_WITH(...) \
  TROMPELOEIL_WITH(&,#__VA_ARGS__, __VA_ARGS__)

#define TROMPELOEIL_WITH_(capture, arg_s, ...)                          \
  with(arg_s, [capture](auto const& trompeloeil_x) {                    \
    auto& _1 = ::trompeloeil::mkarg<1>(trompeloeil_x);                  \
    auto& _2 = ::trompeloeil::mkarg<2>(trompeloeil_x);                  \
    auto& _3 = ::trompeloeil::mkarg<3>(trompeloeil_x);                  \
    auto& _4 = ::trompeloeil::mkarg<4>(trompeloeil_x);                  \
    auto& _5 = ::trompeloeil::mkarg<5>(trompeloeil_x);                  \
    auto& _6 = ::trompeloeil::mkarg<6>(trompeloeil_x);                  \
    auto& _7 = ::trompeloeil::mkarg<7>(trompeloeil_x);                  \
    auto& _8 = ::trompeloeil::mkarg<8>(trompeloeil_x);                  \
    auto& _9 = ::trompeloeil::mkarg<9>(trompeloeil_x);                  \
    auto&_10 = ::trompeloeil::mkarg<10>(trompeloeil_x);                 \
    auto&_11 = ::trompeloeil::mkarg<11>(trompeloeil_x);                 \
    auto&_12 = ::trompeloeil::mkarg<12>(trompeloeil_x);                 \
    auto&_13 = ::trompeloeil::mkarg<13>(trompeloeil_x);                 \
    auto&_14 = ::trompeloeil::mkarg<14>(trompeloeil_x);                 \
    auto&_15 = ::trompeloeil::mkarg<15>(trompeloeil_x);                 \
    ::trompeloeil::ignore(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15); \
    return __VA_ARGS__;                                                 \
  })

#define TROMPELOEIL_SIDE_EFFECT(...) \
  TROMPELOEIL_SIDE_EFFECT_(=, __VA_ARGS__)
#define TROMPELOEIL_LR_SIDE_EFFECT(...) \
  TROMPELOEIL_SIDE_EFFECT_(&, __VA_ARGS__)

#define TROMPELOEIL_SIDE_EFFECT_(capture, ...)                          \
  sideeffect([capture](auto& trompeloeil_x) {                           \
    auto& _1 = ::trompeloeil::mkarg<1>(trompeloeil_x);                  \
    auto& _2 = ::trompeloeil::mkarg<2>(trompeloeil_x);                  \
    auto& _3 = ::trompeloeil::mkarg<3>(trompeloeil_x);                  \
    auto& _4 = ::trompeloeil::mkarg<4>(trompeloeil_x);                  \
    auto& _5 = ::trompeloeil::mkarg<5>(trompeloeil_x);                  \
    auto& _6 = ::trompeloeil::mkarg<6>(trompeloeil_x);                  \
    auto& _7 = ::trompeloeil::mkarg<7>(trompeloeil_x);                  \
    auto& _8 = ::trompeloeil::mkarg<8>(trompeloeil_x);                  \
    auto& _9 = ::trompeloeil::mkarg<9>(trompeloeil_x);                  \
    auto&_10 = ::trompeloeil::mkarg<10>(trompeloeil_x);                 \
    auto&_11 = ::trompeloeil::mkarg<11>(trompeloeil_x);                 \
    auto&_12 = ::trompeloeil::mkarg<12>(trompeloeil_x);                 \
    auto&_13 = ::trompeloeil::mkarg<13>(trompeloeil_x);                 \
    auto&_14 = ::trompeloeil::mkarg<14>(trompeloeil_x);                 \
    auto&_15 = ::trompeloeil::mkarg<15>(trompeloeil_x);                 \
    ::trompeloeil::ignore(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15); \
    __VA_ARGS__;                                                        \
  })

#define TROMPELOEIL_RETURN(...) \
  TROMPELOEIL_RETURN_(=, __VA_ARGS__)
#define TROMPELOEIL_LR_RETURN(...) \
  TROMPELOEIL_RETURN_(&, __VA_ARGS__)

#define TROMPELOEIL_RETURN_(capture, ...)                               \
  handle_return([capture](auto& trompeloeil_x) {                        \
    auto& _1 = ::trompeloeil::mkarg<1>(trompeloeil_x);                  \
    auto& _2 = ::trompeloeil::mkarg<2>(trompeloeil_x);                  \
    auto& _3 = ::trompeloeil::mkarg<3>(trompeloeil_x);                  \
    auto& _4 = ::trompeloeil::mkarg<4>(trompeloeil_x);                  \
    auto& _5 = ::trompeloeil::mkarg<5>(trompeloeil_x);                  \
    auto& _6 = ::trompeloeil::mkarg<6>(trompeloeil_x);                  \
    auto& _7 = ::trompeloeil::mkarg<7>(trompeloeil_x);                  \
    auto& _8 = ::trompeloeil::mkarg<8>(trompeloeil_x);                  \
    auto& _9 = ::trompeloeil::mkarg<9>(trompeloeil_x);                  \
    auto&_10 = ::trompeloeil::mkarg<10>(trompeloeil_x);                 \
    auto&_11 = ::trompeloeil::mkarg<11>(trompeloeil_x);                 \
    auto&_12 = ::trompeloeil::mkarg<12>(trompeloeil_x);                 \
    auto&_13 = ::trompeloeil::mkarg<13>(trompeloeil_x);                 \
    auto&_14 = ::trompeloeil::mkarg<14>(trompeloeil_x);                 \
    auto&_15 = ::trompeloeil::mkarg<15>(trompeloeil_x);                 \
    ::trompeloeil::ignore(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15); \
    return __VA_ARGS__;                                                 \
  })

#define TROMPELOEIL_THROW(...) \
  TROMPELOEIL_THROW_(=, __VA_ARGS__)
#define TROMPELOEIL_LR_THROW(...) \
  TROMPELOEIL_THROW_(&, __VA_ARGS__)

#define TROMPELOEIL_THROW_(capture, ...)                                \
  handle_throw([capture](auto& trompeloeil_x) {                         \
    auto& _1 = ::trompeloeil::mkarg<1>(trompeloeil_x);                  \
    auto& _2 = ::trompeloeil::mkarg<2>(trompeloeil_x);                  \
    auto& _3 = ::trompeloeil::mkarg<3>(trompeloeil_x);                  \
    auto& _4 = ::trompeloeil::mkarg<4>(trompeloeil_x);                  \
    auto& _5 = ::trompeloeil::mkarg<5>(trompeloeil_x);                  \
    auto& _6 = ::trompeloeil::mkarg<6>(trompeloeil_x);                  \
    auto& _7 = ::trompeloeil::mkarg<7>(trompeloeil_x);                  \
    auto& _8 = ::trompeloeil::mkarg<8>(trompeloeil_x);                  \
    auto& _9 = ::trompeloeil::mkarg<9>(trompeloeil_x);                  \
    auto&_10 = ::trompeloeil::mkarg<10>(trompeloeil_x);                 \
    auto&_11 = ::trompeloeil::mkarg<11>(trompeloeil_x);                 \
    auto&_12 = ::trompeloeil::mkarg<12>(trompeloeil_x);                 \
    auto&_13 = ::trompeloeil::mkarg<13>(trompeloeil_x);                 \
    auto&_14 = ::trompeloeil::mkarg<14>(trompeloeil_x);                 \
    auto&_15 = ::trompeloeil::mkarg<15>(trompeloeil_x);                 \
    ::trompeloeil::ignore(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15); \
    throw __VA_ARGS__;                                                  \
 })


#define TROMPELOEIL_TIMES(...) times<__VA_ARGS__>()

#define TROMPELOEIL_IN_SEQUENCE(...) \
  in_sequence(TROMPELOEIL_INIT_WITH_STR(::trompeloeil::sequence_matcher, __VA_ARGS__))

#define TROMPELOEIL_ANY(type) ::trompeloeil::typed_wildcard<type>()

#define TROMPELOEIL_AT_LEAST(num) num, ~0ULL
#define TROMPELOEIL_AT_MOST(num) 0, num

#define TROMPELOEIL_REQUIRE_DESTRUCTION(obj) \
  TROMPELOEIL_REQUIRE_DESTRUCTION_(obj, #obj)

#define TROMPELOEIL_REQUIRE_DESTRUCTION_(obj, obj_s)                          \
  ::trompeloeil::lifetime_monitor TROMPELOEIL_CONCAT(trompeloeil_death_monitor_, __LINE__)(obj, obj_s, __FILE__ ":" TROMPELOEIL_STRINGIFY(__LINE__))

#define TROMPELOEIL_NAMED_REQUIRE_DESTRUCTION(obj) \
  TROMPELOEIL_NAMED_REQUIRE_DESTRUCTION(obj, #obj)

#define TROMPELOEIL_NAMED_REQUIRE_DESTRUCTION_(obj, obj_s)                    \
  std::unique_ptr<::trompeloeil::lifetime_monitor>(new ::trompeloeil::lifetime_monitor(obj, obj_s, __FILE__ ":" TROMPELOEIL_STRINGIFY(__LINE__)))

#ifndef TROMPELOEIL_LONG_MACROS
#define MAKE_MOCK0(name, sig) \
  TROMPELOEIL_MAKE_MOCK_(name,,0, sig, #name, #sig)
#define MAKE_MOCK1(name, sig) \
  TROMPELOEIL_MAKE_MOCK_(name,,1, sig, #name, #sig)
#define MAKE_MOCK2(name, sig) \
  TROMPELOEIL_MAKE_MOCK_(name,,2, sig, #name, #sig)
#define MAKE_MOCK3(name, sig) \
  TROMPELOEIL_MAKE_MOCK_(name,,3, sig, #name, #sig)
#define MAKE_MOCK4(name, sig) \
  TROMPELOEIL_MAKE_MOCK_(name,,4, sig, #name, #sig)
#define MAKE_MOCK5(name, sig) \
  TROMPELOEIL_MAKE_MOCK_(name,,5, sig, #name, #sig)
#define MAKE_MOCK6(name, sig) \
  TROMPELOEIL_MAKE_MOCK_(name,,6, sig, #name, #sig)
#define MAKE_MOCK7(name, sig) \
  TROMPELOEIL_MAKE_MOCK_(name,,7, sig, #name, #sig)
#define MAKE_MOCK8(name, sig) \
  TROMPELOEIL_MAKE_MOCK_(name,,8, sig, #name, #sig)
#define MAKE_MOCK9(name, sig) \
  TROMPELOEIL_MAKE_MOCK_(name,,9, sig, #name, #sig)
#define MAKE_MOCK10(name, sig) \
  TROMPELOEIL_MAKE_MOCK_(name,,10, sig, #name, #sig)
#define MAKE_MOCK11(name, sig) \
  TROMPELOEIL_MAKE_MOCK_(name,,11, sig, #name, #sig)
#define MAKE_MOCK12(name, sig) \
  TROMPELOEIL_MAKE_MOCK_(name,,12, sig, #name, #sig)
#define MAKE_MOCK13(name, sig) \
  TROMPELOEIL_MAKE_MOCK_(name,,13, sig, #name, #sig)
#define MAKE_MOCK14(name, sig) \
  TROMPELOEIL_MAKE_MOCK_(name,,14, sig, #name, #sig)
#define MAKE_MOCK15(name, sig) \
  TROMPELOEIL_MAKE_MOCK_(name,,15, sig, #name, #sig)

#define MAKE_CONST_MOCK0(name, sig) \
  TROMPELOEIL_MAKE_MOCK_(name, const, 0, sig, #name, #sig)
#define MAKE_CONST_MOCK1(name, sig) \
  TROMPELOEIL_MAKE_MOCK_(name, const, 1, sig, #name, #sig)
#define MAKE_CONST_MOCK2(name, sig) \
  TROMPELOEIL_MAKE_MOCK_(name,const,2, sig, #name, #sig)
#define MAKE_CONST_MOCK3(name, sig) \
  TROMPELOEIL_MAKE_MOCK_(name,const,3, sig, #name, #sig)
#define MAKE_CONST_MOCK4(name, sig) \
  TROMPELOEIL_MAKE_MOCK_(name,const,4, sig, #name, #sig)
#define MAKE_CONST_MOCK5(name, sig) \
  TROMPELOEIL_MAKE_MOCK_(name,const,5, sig, #name, #sig)
#define MAKE_CONST_MOCK6(name, sig) \
  TROMPELOEIL_MAKE_MOCK_(name,const,6, sig, #name, #sig)
#define MAKE_CONST_MOCK7(name, sig) \
  TROMPELOEIL_MAKE_MOCK_(name,const,7, sig, #name, #sig)
#define MAKE_CONST_MOCK8(name, sig) \
  TROMPELOEIL_MAKE_MOCK_(name,const,8, sig, #name, #sig)
#define MAKE_CONST_MOCK9(name, sig) \
  TROMPELOEIL_MAKE_MOCK_(name,const,9, sig, #name, #sig)
#define MAKE_CONST_MOCK10(name, sig) \
  TROMPELOEIL_MAKE_MOCK_(name,const,10, sig, #name, #sig)
#define MAKE_CONST_MOCK11(name, sig) \
  TROMPELOEIL_MAKE_MOCK_(name,const,11, sig, #name, #sig)
#define MAKE_CONST_MOCK12(name, sig) \
  TROMPELOEIL_MAKE_MOCK_(name,const,12, sig, #name, #sig)
#define MAKE_CONST_MOCK13(name, sig) \
  TROMPELOEIL_MAKE_MOCK_(name,const,13, sig, #name, #sig)
#define MAKE_CONST_MOCK14(name, sig) \
  TROMPELOEIL_MAKE_MOCK_(name,const,14, sig, #name, #sig)
#define MAKE_CONST_MOCK15(name, sig) \
  TROMPELOEIL_MAKE_MOCK_(name,const,15, sig, #name, #sig)

#define REQUIRE_CALL(obj, func)       TROMPELOEIL_REQUIRE_CALL_(obj, func, #obj, #func)
#define NAMED_REQUIRE_CALL(obj, func) TROMPELOEIL_NAMED_REQUIRE_CALL_(obj, func, #obj, #func)
#define ALLOW_CALL(obj, func)         TROMPELOEIL_ALLOW_CALL_(obj, func, #obj, #func)
#define NAMED_ALLOW_CALL(obj, func)   TROMPELOEIL_NAMED_ALLOW_CALL_(obj, func, #obj, #func)
#define FORBID_CALL(obj, func)        TROMPELOEIL_FORBID_CALL_(obj, func, #obj, #func)
#define NAMED_FORBID_CALL(obj, func)  TROMPELOEIL_NAMED_FORBID_CALL_(obj, func, #obj, #func)
#define WITH(...)                     TROMPELOEIL_WITH_(=,#__VA_ARGS__, __VA_ARGS__)
#define LR_WITH(...)                  TROMPELOEIL_WITH_(&,#__VA_ARGS__, __VA_ARGS__)
#define SIDE_EFFECT(...)              TROMPELOEIL_SIDE_EFFECT_(=, __VA_ARGS__)
#define LR_SIDE_EFFECT(...)           TROMPELOEIL_SIDE_EFFECT_(&, __VA_ARGS__)
#define RETURN(...)                   TROMPELOEIL_RETURN_(=,__VA_ARGS__)
#define LR_RETURN(...)                TROMPELOEIL_RETURN_(&, __VA_ARGS__)
#define THROW(...)                    TROMPELOEIL_THROW_(=, __VA_ARGS__)
#define LR_THROW(...)                 TROMPELOEIL_THROW_(&, __VA_ARGS__)
#define TIMES(...)                    TROMPELOEIL_TIMES(__VA_ARGS__)
#define IN_SEQUENCE(...)              TROMPELOEIL_IN_SEQUENCE(__VA_ARGS__)
#define ANY(type)                     TROMPELOEIL_ANY(type)
#define AT_LEAST(num)                 TROMPELOEIL_AT_LEAST(num)
#define AT_MOST(num)                  TROMPELOEIL_AT_MOST(num)
#define REQUIRE_DESTRUCTION(obj)      TROMPELOEIL_REQUIRE_DESTRUCTION_(obj, #obj)
#define NAMED_REQUIRE_DESTRUCTION(obj)TROMPELOEIL_NAMED_REQUIRE_DESTRUCTION_(obj, #obj)
#endif

#endif // include guard
