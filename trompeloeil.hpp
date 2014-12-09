/*
 * Trompeloeil C++ mocking framework
 *
 * Copyright Björn Fahller 2014
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


// Deficiencies and missing features
// * Mocking function templates is not supported
// * implement tracing
// * If a macro kills a kitten, this threatens extinction of all felines!

#if (!defined(__cplusplus) || __cplusplus <= 201103)
#error requires C++14 or higher
#endif

#include <utility>
#include <tuple>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <exception>
#include <functional>
#include <memory>
#include <cstring>
#include <cassert>

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
    static const size_t size = std::tuple_size<param_tuple>::value;
    template <size_t N>
    using type = typename std::tuple_element<N, param_tuple>::type;
  };

  class expectation_violation : public std::logic_error
  {
  public:
    using std::logic_error::logic_error;
  };

  enum class severity { fatal, nonfatal };

  inline
  std::function<void(severity, const std::string& loc, const std::string&)>& reporter_obj()
  {
    static std::function<void(severity, const std::string& loc, const std::string&)> obj
      = [](severity, const std::string& loc, const std::string& msg)
      {
        throw expectation_violation(loc + "\n" + msg);
      };
    return obj;
  }

  inline
  void set_reporter(std::function<void(severity, const std::string& loc, const std::string& s)> f)
  {
    reporter_obj() = f;
  }


  inline
  void send_report(severity s, const char* loc, const std::string& msg)
  {
    reporter_obj()(s, loc, msg);
  }

  template <typename T, typename U>
  struct is_equal_comparable
  {
    struct no {};
    template <typename T1, typename T2>
    static no func(...);
    template <typename T1, typename T2>
    static auto func(T1* p1, T2* p2) -> decltype(*p1 == *p2);
    static const bool value = !std::is_same<no, decltype(func<T,U>(nullptr, nullptr))>::value;
  };

  template <typename T>
  class is_output_streamable
  {
    struct no;
    template <typename U>
    static no func(...);
    template <typename U>
    static auto func(U* u) -> decltype(std::declval<std::ostream&>() << *u);
  public:
    static const bool value = !std::is_same<no, decltype(func<T>(nullptr))>::value;
  };

  struct stream_sentry
  {
    stream_sentry(std::ostream& os_) : os(os_), width(os.width(0)), flags(os.flags(std::ios_base::dec | std::ios_base::left)), fill(os.fill(' ')) {  }
    ~stream_sentry() { os.flags(flags); os.fill(fill); os.width(width);}
    std::ostream& os;
    std::streamsize width;
    std::ios_base::fmtflags flags;
    char fill;
  };

  template <typename T, bool b = is_output_streamable<T>::value>
  struct streamer
  {
    static void print(std::ostream& os, const T& t)
    {
      stream_sentry s(os);
      os << t;
    }
  };


  template <typename T>
  struct streamer<T, false>
  {
    static void print(std::ostream& os, const T& t)
    {
      stream_sentry s(os);

      os << sizeof(T) << "-byte object={";
      if (sizeof(T) > 8) os << '\n';
      os << std::setfill('0') << std::hex;
      auto p = reinterpret_cast<const uint8_t*>(&t);
      for (size_t i = 0; i < sizeof(T); ++i)
      {
        os << " 0x" << std::setw(2) << unsigned(p[i]);
        if ((i & 0xf) == 0xf) os << '\n';
      }
      os << " }";
    }
  };
  template <typename T>
  void print(std::ostream& os, const T& t)
  {
    streamer<T>::print(os, t);
  }

  template<typename T>
  struct list_elem
  {
    list_elem(T *list) noexcept : n(list), p(list->p)
    {
      list->invariant_check();

      p->n = n->p = this;

      invariant_check();
      list->invariant_check();
    }

    list_elem() noexcept : n(this), p(this) { invariant_check(); }

    list_elem(const list_elem &) = delete;

    list_elem(list_elem &&r) noexcept : n(r.n), p(&r)
    {
      r.invariant_check();

      n->p = this;
      r.n = this;

      assert(n->p == this);
      assert(p->n == this);

      r.unlink();

      assert(r.is_empty());
      invariant_check();
    }

    ~list_elem()
    {
      unlink();
    }

    void invariant_check() const
    {
      assert(n->p == this);
      assert(p->n == this);
      assert((n == this) == (p == this));
      assert((p->n == n) == (n == this));
      assert((n->p == p) == (p == this));
      auto pp = p;
      auto nn = n;
      do {
        assert((nn == this) == (pp == this));
        assert(nn->n->p == nn);
        assert(nn->p->n == nn);
        assert(pp->n->p == pp);
        assert(pp->p->n == pp);
        assert((nn->n == nn) == (nn == this));
        assert((pp->p == pp) == (pp == this));
        nn = nn->n;
        pp = pp->p;
      } while (nn != this);
    }
    list_elem &operator=(const list_elem &) = delete;
    list_elem &operator=(list_elem&&) = delete;

    bool is_empty() const noexcept
    {
      invariant_check();
      return n == this;
    }

    T       *next()       noexcept { return static_cast<T       *>(n); }
    T const *next() const noexcept { return static_cast<T const *>(n); }
    T       *prev()       noexcept { return static_cast<T       *>(p); }
    T const *prev() const noexcept { return static_cast<T const *>(p); }

    void unlink() noexcept
    {
      invariant_check();
      auto pp = p;
      auto nn = n;
      nn->p = pp;
      pp->n = nn;
      n = p = this;
      invariant_check();
    }

    void link_before(T &b) noexcept
    {
      invariant_check();
      b.invariant_check();

      for (auto i = n; i != this; i = i->n)
      {
        assert(i != &b);
      }

      auto pp = p;
      auto bn = b.n;
      pp->n = bn;
      bn->p = pp;
      b.n = this;
      p = &b;

      invariant_check();
    };
  private:
    list_elem *n;
    list_elem *p;
  };

  struct sequence : public list_elem<sequence>
  {
    sequence() noexcept = default;
    virtual void print_expectation(std::ostream&) const {}
  protected:
    sequence(sequence* obj) noexcept : list_elem(obj) {}
  };

  struct sequence_elem : public sequence
  {
    sequence_elem(sequence& r) noexcept : sequence(&r) {}
    void print_expectation(std::ostream& os) const override
    {
      os << exp_name << " at " << exp_loc;
    }
    const char *exp_name;
    const char *exp_loc;
  };

  struct sequence_matcher : public list_elem<sequence_matcher>
  {
    sequence_matcher(const char* name_, sequence& s) noexcept
    : seq_name(name_)
    , seq(s)
    , seq_elem(s)
    {}
    void set_expectation(const char *name, const char *loc) noexcept
    {
      seq_elem.exp_name = name;
      seq_elem.exp_loc = loc;
    }
    void check_match(const char *match_name, const char* loc) const
    {
      auto next_seq = seq.next();
      if (next_seq != &seq_elem)
      {
        auto ns = static_cast<sequence_elem*>(next_seq);
        std::ostringstream os;
        os << "Sequence mismatch for sequence \"" << seq_name
           << "\" with matching call of " << match_name << " at " << loc
           << ". Sequence \"" << seq_name << "\" has ";
        ns->print_expectation(os);
        os << " first in line\n";
        send_report(severity::fatal, loc, os.str());
      }
    }
    bool is_first() const
    {
      auto next_seq = seq.next();
      return next_seq == &seq_elem;
    }
    void retire() noexcept { seq_elem.unlink(); }
    const char    *seq_name;
    sequence&      seq;
    sequence_elem  seq_elem;
  };

  struct wildcard
  {
    template<typename T>
    operator T&&() const;
    template<typename T>
    operator T&() const;
  };

  template <typename T>
  bool operator==(const wildcard&, const T&) { return true; }
  template <typename T>
  bool operator==(const T&, const wildcard&) { return true; }

  static constexpr const wildcard _{};

  template <typename T>
  struct typed_wildcard
  {
    operator T() const;
  };

  template <typename T, typename U>
  auto operator==(const typed_wildcard<T>&, const U&) -> typename std::enable_if<std::is_same<typename std::decay<T>::type, typename std::decay<U>::type>::value, bool>::type
  { return true; }

  template <typename T, typename U>
  auto operator==(const T&, const typed_wildcard<U>&) -> typename std::enable_if<std::is_same<typename std::decay<T>::type, typename std::decay<U>::type>::value, bool>::type
  { return true; }

  template <typename T>
  std::ostream& operator<<(std::ostream& os, const typed_wildcard<T>&)
  {
    return os << '_';
  }

  struct lifetime_monitor;

  template <typename T>
  class deathwatched : public T
  {
  public:
    using T::T;
    ~deathwatched();
    lifetime_monitor*& trompeloeil_expect_death(trompeloeil::lifetime_monitor* monitor) const noexcept
    {
      return trompeloeil_lifetime_monitor = monitor;
    }
  private:
    mutable trompeloeil::lifetime_monitor *trompeloeil_lifetime_monitor = nullptr;

  };

  struct lifetime_monitor
  {
    template <typename T>
    lifetime_monitor(const ::trompeloeil::deathwatched<T>&obj,
                     const char* obj_name,
                     const char *loc) noexcept
      : object_monitor(obj.trompeloeil_expect_death(this)),
        location(loc),
        object_name(obj_name)
    {
    }
    ~lifetime_monitor()
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
    const char        *location;
    const char        *object_name;
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
  return_of_t<Sig> default_return()
  {
    return default_return_t<Sig>::value();
  }


  template<typename Sig>
  struct call_matcher_list;

  template<typename Sig>
  struct call_matcher_base : public list_elem<call_matcher_base<Sig> >
  {
    call_matcher_base() = default;

    call_matcher_base(call_matcher_base *list)
      : list_elem<call_matcher_base>(list)
    {
    }

    virtual bool matches(const call_params_type_t<Sig>&) const = 0;
    virtual bool first_in_sequence() const = 0;
    virtual void run_actions(call_params_type_t<Sig> &, call_matcher_list<Sig> &saturated_list) = 0;
    virtual std::ostream& report_signature(std::ostream&) const = 0;
    virtual std::ostream& report_mismatch(std::ostream&,const call_params_type_t<Sig> &) = 0;
    virtual return_of_t<Sig> return_value(call_params_type_t<Sig>&) {
      return default_return<Sig>(); }
    virtual void report_missed() = 0;
  };

  template<typename T, size_t N = 0, bool b = N == std::tuple_size<T>::value>
  struct tuple_print
  {
    template <typename stream, typename U>
    static stream &mismatch(stream &os, const T &t1, const U &t2)
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
    static stream &missed(stream &os, const U &t)
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
    static stream &mismatch(stream &os, const T &, const U &)
    {
      return os;
    }

    template <typename stream, typename U>
    static stream& missed(stream &os, const U &)
    {
      return os;
    }
  };

  template <typename T, typename ... A>
  struct all_are;

  template <typename T>
  struct all_are<T> : public std::true_type {};

  template <typename T, typename ... A>
  struct all_are<T, T, A...> : public all_are<T, A...> {};

  template <typename T, typename U, typename ... A>
  struct all_are<T, U, A...> : public std::false_type {};


  template<typename Sig>
  struct call_matcher_list : public call_matcher_base<Sig>
  {
    template<typename ... U>
    call_matcher_list &operator()(const U &...) { return *this; }

    virtual bool matches(const call_params_type_t<Sig> &) const { return false; }
    virtual bool first_in_sequence() const { return false; }

    virtual void run_actions(call_params_type_t<Sig> &, call_matcher_list<Sig> &) {}

    virtual std::ostream& report_signature(std::ostream& r ) const  override { return r; }
    virtual std::ostream& report_mismatch(std::ostream& r, const call_params_type_t<Sig> &) override { return r;}

    virtual return_of_t<Sig> return_value(call_params_type_t<Sig> &)
    {
      return default_return<Sig>();
    }

    virtual void report_missed() {}
  };

  template <typename Sig>
  call_matcher_base<Sig>* find(const call_params_type_t<Sig> &p,
                               call_matcher_list<Sig>        &list)
  {
    for (auto i = list.prev(); i != &list; i = i->prev())
    {
      if (i->matches(p) && i->first_in_sequence())
      {
        return i;
      }
    }
    for (auto i = list.prev(); i != &list; i = i->prev())
    {
      if (i->matches(p))
      {
        return i;
      }
    }
    return nullptr;
  }

  template <typename Sig>
  void report_mismatch(const char                    *name,
                       const call_params_type_t<Sig> &p,
                       call_matcher_list<Sig>        &matcher_list,
                       call_matcher_list<Sig>        &saturated_list)
  {
    std::ostringstream os;
    os << "No match for call of " << name << " with.\n";
    tuple_print<call_params_type_t<Sig>>::missed(os, p);
    bool saturated_match = false;
    for (auto i = saturated_list.next(); i != &saturated_list; i = i->next())
    {
      if (i->matches(p))
      {
        if (!saturated_match)
        {
          os << "\nMatches saturated call requirement\n";
          saturated_match = true;
        }
        os << "  ";
        i->report_signature(os) << '\n';
      }
    }
    if (!saturated_match)
    {
      for (auto i = matcher_list.prev(); i != &matcher_list; i = i->prev())
      {
        os << "\nTried ";
        i->report_mismatch(os, p);
      }
    }
    trompeloeil::send_report(severity::fatal, "", os.str());
  }

  template<typename Sig>
  struct condition_base : public list_elem<condition_base<Sig> >
  {
    condition_base() = default;
    condition_base(condition_base&& r) = default;
    virtual ~condition_base() = default;
    virtual bool check(const call_params_type_t<Sig>&) const = 0;
    virtual const char* name() const noexcept = 0;
  };

  template<typename Sig>
  struct condition_list : public condition_base<Sig>
  {
    condition_list() = default;
    condition_list(condition_list&& r) = default;
    ~condition_list()
    {
      while (!this->is_empty())
      {
        delete this->next();
      }
    }
    bool check(const call_params_type_t<Sig>&) const override
    {
      return false;
    }
    const char *name() const noexcept override { return nullptr; }
  };

  template<typename Sig, typename Cond>
  struct condition : public condition_base<Sig>
  {
    condition(const char *str_, Cond c_) : c(c_), str(str_) {};

    bool check(const call_params_type_t<Sig>& t) const override
    {
      return c(t);
    }

    const char *name() const noexcept override { return str; }
    Cond c;
    const char *str;
  };


  template<typename Sig>
  struct side_effect_base : public list_elem<side_effect_base<Sig> >
  {
    side_effect_base() = default;
    side_effect_base(side_effect_base&&) = default;
    virtual ~side_effect_base() = default;
    virtual void action(call_params_type_t<Sig> &) = 0;
  };

  template<typename Sig>
  struct side_effect_list : public side_effect_base<Sig>
  {
    side_effect_list() = default;
    side_effect_list(side_effect_list&&) = default;
    ~side_effect_list()
    {
      while (!this->is_empty())
      {
        delete this->next();
      }
    }
    void action(call_params_type_t<Sig> &) {}
  };

  template<typename Sig, typename Action>
  struct side_effect : public side_effect_base<Sig>
  {
    side_effect(const char *str_, Action a_) : a(a_), str(str_) {};

    void action(call_params_type_t<Sig> &t) { a(t); }

    Action a;
    const char *str;
  };

  template<typename Sig>
  struct return_handler_base : public list_elem<return_handler_base<Sig> >
  {
    virtual return_of_t<Sig> return_value(call_params_type_t<Sig> &) = 0;
  };

  template<typename Sig>
  struct return_handler_list : public return_handler_base<Sig>
  {
    return_of_t<Sig> return_value(call_params_type_t<Sig> &)
    {
      return default_return<Sig>();
    }
  };

  template<typename Sig, typename Handler>
  struct return_handler : public return_handler_base<Sig>
  {
    return_handler(const char *str_, Handler h_) : h(h_), str(str_) {}

    return_of_t<Sig> return_value(call_params_type_t<Sig> &t) { return h(t); }

    Handler h;
    const char *str;
  };

  template <typename Sig, typename Handler>
  struct throw_handler : public return_handler_base<Sig>
  {
    throw_handler(const char *str_, Handler h_) : h(h_), str(str_) {}

    return_of_t<Sig> return_value(call_params_type_t<Sig>& t) { h(t); return default_return<Sig>(); }

    Handler h;
    const char *str;
  };
  template <typename T, std::size_t N, size_t M>
  struct sequence_validator_t;

  template <typename ... T, size_t N, size_t M>
  struct sequence_validator_t<std::tuple<T...>, N, M>
  {
    static void set_expectation(const char *exp_name, const char *exp_loc, std::tuple<T...>& t) noexcept
    {
      std::get<N>(t).set_expectation(exp_name, exp_loc);
      sequence_validator_t<std::tuple<T...>, N + 1, M>::set_expectation(exp_name, exp_loc, t);
    }
    static void validate(const char*match_name, const char *loc, const std::tuple<T...>& t)
    {
      std::get<N>(t).check_match(match_name, loc);
      sequence_validator_t<std::tuple<T...>, N + 1, M>::validate(match_name, loc, t);
    }
    static bool is_first(const std::tuple<T...>& t)
    {
      return std::get<N>(t).is_first() && sequence_validator_t<std::tuple<T...>, N + 1, M>::is_first(t);
    }
    static void retire(std::tuple<T...>& t) noexcept
    {
      std::get<N>(t).retire();
      sequence_validator_t<std::tuple<T...>, N + 1, M>::retire(t);
    }
  };

  template <typename ... T, size_t N>
  struct sequence_validator_t<std::tuple<T...>, N, N>
  {
    static void set_expectation(const char*, const char*, std::tuple<T...>&) noexcept {}
    static void validate(const char*, const char*, const std::tuple<T...>& ) {}
    static bool is_first(const std::tuple<T...>&) { return true; }
    static void retire(std::tuple<T...>& ) noexcept {}
  };


  template <typename T>
  struct sequence_validator
  : sequence_validator_t<T, 0, std::tuple_size<T>::value>
  {
  };

  struct sequence_handler_base : public list_elem<sequence_handler_base >
  {
    virtual void set_expectation(const char *, const char*) noexcept = 0;
    virtual void validate(const char*, const char*) = 0;
    virtual bool is_first() const = 0;
    virtual void retire() noexcept = 0;
  };

  struct sequence_handler_list : public sequence_handler_base
  {
    void set_expectation(const char*, const char*) noexcept override {}
    void validate(const char*, const char*) override {}
    bool is_first() const override { return true; }
    void retire() noexcept override {}
  };


  template <typename T>
  struct sequence_handler;

  template <typename ... Seq>
  struct sequence_handler<std::tuple<Seq...> > : public sequence_handler_base
  {
    using seq_tuple = std::tuple<Seq...>;
    sequence_handler(seq_tuple&& t) : sequences(std::move(t)) {}
    void set_expectation(const char *exp_name, const char *exp_loc) noexcept
    {
      sequence_validator<seq_tuple>::set_expectation(exp_name, exp_loc, sequences);
    }
    void validate(const char* match_name, const char *loc)
    {
      sequence_validator<seq_tuple>::validate(match_name, loc, sequences);
    }
    bool is_first() const
    {
      return sequence_validator<seq_tuple>::is_first(sequences);
    }

    void retire() noexcept
    {
      sequence_validator<seq_tuple>::retire(sequences);
    }
    seq_tuple sequences;
  };

  template<typename R, typename P>
  struct return_type_injector : public P
  {
    using return_type = R;
    using P::P;

    template<typename ... A>
    return_type_injector(A &&... a) : P(std::forward<A>(a)...) {}
  };

  template <typename P>
  struct throw_injector : public P
  {
    static const bool throws = true;
    using P::P;

    template <typename ... A>
    throw_injector(A&& ... a) : P(std::forward<A>(a)...) {}
  };
  template <typename P>
  struct call_limit_injector : public P
  {
    static const bool call_limit_set = true;
    using P::P;

    template <typename ... A>
     call_limit_injector(A && ... a) : P(std::forward<A>(a)...) {}
  };

  template <typename P>
  struct sequence_injector : public P
  {
    static const bool sequence_set = true;
    using P::P;
    template <typename ... A>
    sequence_injector(A&& ... a) : P(std::forward<A>(a)...) {}
  };
  template <typename Parent, typename U, typename Sig>
  struct call_data : public Parent
  {
    using typename Parent::return_type;
    using Parent::call_limit_set;
    using Parent::sequence_set;
    using Parent::throws;
    call_data(Parent&& parent) : Parent(std::move(parent)) {}
    call_data(Parent&& parent, U&& d) : Parent(std::move(parent)), data(std::move(d))
    {
      add_last(data);
    }
    void add_last(return_handler_base<Sig>& s) noexcept
    {
      this->return_handler.link_before(s);
    }
    void add_last(sequence_handler_base& s) noexcept
    {
      this->sequence_handler.link_before(s);
      s.set_expectation(this->name, this->location);
    }
    void add_last(std::tuple<>) noexcept {}
    template <typename D>
    call_data<call_data, std::tuple<>, Sig> with(const char* str, D&& d)
    {
      auto cond = new condition<Sig, D>(str, std::move(d));
      Parent::add_condition(cond);
      return std::move(*this);
    }
    template <typename A>
    call_data<call_data, std::tuple<>, Sig> sideeffect(const char* str, A&& a)
    {
      auto effect = new side_effect<Sig, A>(str, std::move(a));
      this->add_side_effect(effect);
      return std::move(*this);
    }
    template <typename H>
    call_data<return_type_injector<return_of_t<Sig>, call_data>, return_handler<Sig, H>, Sig> handle_return(const char* str, H&& h)
    {
      static_assert(std::is_constructible<return_of_t<Sig>, decltype(h(std::declval<call_params_type_t<Sig>& >()))>::value || !std::is_same<return_of_t<Sig>, void>::value,
                    "RETURN does not make sense for void-function");
      static_assert(std::is_constructible<return_of_t<Sig>, decltype(h(std::declval<call_params_type_t<Sig>& >()))>::value || std::is_same<return_of_t<Sig>, void>::value,
                    "RETURN value is not convertible to the return type of the function");
      static_assert(!std::is_same<return_type, decltype(h(std::declval<call_params_type_t<Sig>&>()))>::value || std::is_same<return_type, void>::value,
                    "Multiple RETURN does not make sense");
      static_assert(!throws,
                    "THROW and RETURN does not make sense");
      return {return_type_injector<return_of_t<Sig>, call_data>(std::move(*this)), return_handler<Sig, H>(str, std::move(h))};
    }
    template <typename H>
    call_data<throw_injector<call_data>, throw_handler<Sig, H>, Sig> handle_throw(const char* str, H&& h)
    {
      static_assert(!throws,
                    "Multiple THROW does not make sense");
      static_assert(std::is_same<return_type, void>::value,
                    "THROW and RETURN does not make sense");
      return { return_type_injector<return_of_t<Sig>, call_data>(std::move(*this)), throw_handler<Sig, H>(str, std::move(h))};
    }
    template <unsigned long long L,
              unsigned long long H = L,
              bool               verboten = call_limit_set>
    call_data<call_limit_injector<call_data>, std::tuple<>, Sig> times()
    {
      static_assert(!verboten,
                    "Only one TIMES call limit is allowed, but it can express an interval");
      static_assert(H >= L, "In TIMES the first value must not exceed the second");
      this->min_calls = L;
      this->max_calls = H;
      return { std::move(*this), {} };
    }

    template <typename ... T, bool b = sequence_set, typename = typename std::enable_if<all_are<sequence_matcher, T...>::value>::type >
    call_data<sequence_injector<call_data>,sequence_handler<std::tuple<T...> >,Sig> in_sequence(T&& ... t)
    {
      static_assert(!b, "Multiple IN_SEQUENCE does not make sense. You can list several sequence objects at once");
      return { std::move(*this), { std::tuple<typename std::remove_reference<T>::type...>(std::forward<T>(t)...) } };
    }

    U data;
  };

  struct expectation {
    virtual ~expectation() = default;
  };

  inline void
  unfulfilled_header(std::ostream& os,
                     const char*   name,
                     unsigned      min_calls,
                     unsigned      call_count)
  {
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
  }

  template<typename Sig, typename Value>
  struct call_matcher : public call_matcher_base<Sig>, expectation
  {
    using return_type = void;
    static const bool throws = false;

    template<typename ... U>
    call_matcher(U &&... u) : val(std::forward<U>(u)...) {}

    call_matcher(call_matcher &&r) = default;

    ~call_matcher()
    {
      if (!reported && !this->is_empty() && call_count < min_calls) report_missed();
    }

    call_matcher& hook_last(call_matcher_list<Sig> &list) noexcept
    {
      list.link_before(*this);
      return *this;
    }

    virtual bool matches(const call_params_type_t<Sig>& params) const override
    {
      return val == params && match_conditions(params);
    }

    bool match_conditions(const call_params_type_t<Sig>& params) const
    {
      for (auto c = conditions.next(); c != &conditions; c = c->next())
      {
        if (!c->check(params)) return false;
      }
      return true;
    }
    bool first_in_sequence() const
    {
      auto saturated = call_count >= std::get<0>(call_limits());
      return  saturated || sequence_handler.next()->is_first();
    }

    return_of_t<Sig> return_value(call_params_type_t<Sig>& params)
    {
      return return_handler.next()->return_value(params);
    }
    void run_actions(call_params_type_t<Sig>& params, call_matcher_list<Sig> &saturated_list)
    {
      auto limits = call_limits();
      if (call_count < std::get<0>(limits))
      {
        sequence_handler.next()->validate(name, location);
      }
      if (std::get<1>(limits) == 0)
      {
        reported = true;
        std::ostringstream os;
        os << "Match of forbidden call of " << name << " at " << location << '\n';
        tuple_print<Value>::missed(os, params);
        send_report(severity::fatal, location, os.str());
      }
      if (++call_count == std::get<0>(limits))
      {
        sequence_handler.next()->retire();
      }
      if (call_count == std::get<1>(limits))
      {
        this->unlink();
        saturated_list.link_before(*this);
      }
      for (auto a = actions.next(); a != &actions; a = a->next())
      {
        a->action(params);
      }
    }
    std::ostream& report_signature(std::ostream& os) const override
    {
      return os << name << " at " << location;
    }
    std::ostream& report_mismatch(std::ostream& os, const call_params_type_t<Sig>& params) override
    {
      reported = true;
      report_signature(os);
      if (val == params)
      {
        for (auto i = conditions.next(); i != &conditions; i = i->next())
        {
          if (!i->check(params))
          {
            os << "\n  Failed WITH(" << i->name() << ')';
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

    void report_missed() override
    {
      reported = true;
      std::ostringstream os;
      unfulfilled_header(os, name, min_calls, call_count);
      tuple_print<Value>::missed(os, val);
      send_report(severity::nonfatal, location, os.str());
    }
    template <typename D>
    call_data<call_matcher, std::tuple<>, Sig> with(const char* str, D&& d)
    {
      auto cond = new condition<Sig, D>(str, std::move(d));
      add_condition(cond);
      return std::move(*this);
    }
    template <typename A>
    call_data<call_matcher, std::tuple<>, Sig> sideeffect(const char* str, A&& a)
    {
      auto effect = new side_effect<Sig, A>(str, std::move(a));
      add_side_effect(effect);
      return std::move(*this);
    }

    template <typename H>
    call_data<return_type_injector<return_of_t<Sig>, call_matcher>, return_handler<Sig, H>, Sig> handle_return(const char* str, H&& h)
    {
      static_assert(!std::is_same<return_of_t<Sig>, void>::value || std::is_same<H, void>::value,
                    "RETURN does not make sense for void-function");

      static_assert(std::is_constructible<return_of_t<Sig>, decltype(h(std::declval<call_params_type_t<Sig>& >()))>::value || std::is_same<return_of_t<Sig>, void>::value,
                    "RETURN value is not convertible to the return type of the function");
      return {std::move(*this), {str, std::move(h)}};
    }
    template <typename H>
    call_data<throw_injector<call_matcher>, throw_handler<Sig, H>, Sig> handle_throw(const char* str, H&& h)
    {
      return { std::move(*this), {str, std::move(h)}};
    }
    template <unsigned long long L, unsigned long long H = L>
    call_data<call_limit_injector<call_matcher>, std::tuple<>, Sig> times()
    {
      static_assert(H >= L, "In TIMES the first value must not exceed the second");
      min_calls = L;
      max_calls = H;
      return { std::move(*this), {} };
    }

    template <typename ... T, typename = typename std::enable_if<all_are<sequence_matcher, T...>::value>::type >
    call_data<sequence_injector<call_matcher>,sequence_handler<std::tuple<T...> >,Sig> in_sequence(T&& ... t)
    {
      std::tuple<T...> tup(std::forward<T>(t)...);
      using h = ::trompeloeil::sequence_handler<std::tuple<T...> >;
      return { std::move(*this), h( std::move(tup) ) };
    }
    call_matcher &set_location(const char *loc) noexcept
    {
      location = loc;
      return *this;
    }
    call_matcher& set_name(const char* func_name) noexcept
    {
      name = func_name;
      return *this;
    }
    virtual std::tuple<unsigned long long, unsigned long long> call_limits() const noexcept
    {
      return std::make_tuple(min_calls, max_calls);
    }

    void add_condition(condition_base<Sig>* c) noexcept
    {
      conditions.link_before(*c);
    }
    void add_side_effect(side_effect_base<Sig>* c) noexcept
    {
      actions.link_before(*c);
    }
    static const bool         call_limit_set = false;
    static const bool         sequence_set = false;
    Value                     val;
    condition_list<Sig>       conditions;
    side_effect_list<Sig>     actions;
    return_handler_list<Sig>  return_handler;
    sequence_handler_list     sequence_handler;
    const char               *location;
    const char               *name;
    unsigned long long        call_count = 0;
    unsigned long long        min_calls = 1;
    unsigned long long        max_calls = 1;
    bool                      reported = false;
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
  struct illegal_argument {};

  template <typename T, int N>
  struct arg<T, N, false>
  {
    static const illegal_argument<N>& value(T&) noexcept
    {
      static const constexpr illegal_argument<N> v{};
      return v;
    }

  };

  template <int N, typename T>
  auto mkarg(T& t) noexcept -> decltype(arg<T, N>::value(t)) { return arg<T, N>::value(t); }

  template <typename ... T>
  void ignore(const T& ...) noexcept {}

  template <typename ... T>
  call_params_type_t<void(T...)> make_params_type_obj(T&& ... t)
  {
    return call_params_type_t<void(T...)>(t...);
  }

  template <typename T>
  struct is_call_matcher
  {
    struct no {};
    struct yes {};
    static no func(...);
    template <typename U, typename P, typename Sig>
    static Sig func(const call_data<P, U, Sig>*);
    using TT = typename std::remove_reference<T>::type;
    static const bool value = std::is_same<decltype(func(std::declval<TT*>())), yes>::value;
    using Sig = decltype(func(std::declval<TT*>()));
  };

  struct call_validator
  {
    template <typename Sig, typename T>
    static void assert_return_type(T&)
    {
      static_assert(T::throws || std::is_same<typename T::return_type, return_of_t<Sig> >::value,
                    "RETURN missing for non-void function");
    }
    template <typename Sig, typename Val>
    call_matcher<Sig, Val> operator+(trompeloeil::call_matcher<Sig, Val>& t) {
      assert_return_type<Sig>(t);
      return std::move(t);
    }
    template <typename P, typename U, typename Sig>
    call_data<P, U, Sig> operator+(::trompeloeil::call_data<P, U, Sig>&& t) {
      assert_return_type<Sig>(t);
      return std::move(t);
    }
  };

  struct heap_elevator
  {
    template <typename T>
    std::unique_ptr<T> operator&&(T&& t) const
    {
      return std::unique_ptr<T>(new T(std::move(t)));
    }
  };
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

namespace trompeloeil {
  template <typename sig, typename ... U>
  auto make_call_matcher(U&& ... u) -> ::trompeloeil::call_matcher<sig, decltype(std::make_tuple(std::forward<U>(u)...))>
  {
    return { std::forward<U>(u)... };
  }
}

#define TROMPELOEIL_MAKE_MOCK_(name, constness, num, sig, name_s, sig_s) \
  using TROMPELOEIL_ID(matcher_list_type) = ::trompeloeil::call_matcher_list<sig>; \
  mutable TROMPELOEIL_ID(matcher_list_type) TROMPELOEIL_ID(matcher_list); \
  mutable TROMPELOEIL_ID(matcher_list_type) TROMPELOEIL_ID(saturated_matcher_list); \
  struct TROMPELOEIL_ID(tag_type_trompeloeil)                           \
  {                                                                     \
    template <typename ... U>                                           \
      static auto name(U&& ... u) -> decltype(::trompeloeil::make_call_matcher<sig>(std::forward<U>(u)...)) \
    {                                                                   \
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
      ::trompeloeil::report_mismatch(name_s " with signature " sig_s, \
                                     param_value,                       \
                                     TROMPELOEIL_ID(matcher_list),      \
                                     TROMPELOEIL_ID(saturated_matcher_list)); \
    }                                                                   \
    i->run_actions(param_value, TROMPELOEIL_ID(saturated_matcher_list)); \
    return i->return_value(param_value);                                \
  }



#define TROMPELOEIL_STRINGIFY_(...) #__VA_ARGS__
#define TROMPELOEIL_STRINGIFY(...) TROMPELOEIL_STRINGIFY_(__VA_ARGS__)

#define TROMPELOEIL_REQUIRE_CALL(obj, func)                    \
  TROMPELOEIL_REQUIRE_CALL_(obj, func, #obj, #func)

#define TROMPELOEIL_REQUIRE_CALL_(obj, func, obj_s, func_s)                    \
  auto TROMPELOEIL_ID(call_obj) =  TROMPELOEIL_REQUIRE_CALL_OBJ(obj, func, obj_s, func_s)


#define TROMPELOEIL_NAMED_REQUIRE_CALL(obj, func) \
  TROMPELOEIL_NAMED_REQUIRE_CALL(obj, func, #obj, #func)

#define TROMPELOEIL_NAMED_REQUIRE_CALL_(obj, func, obj_s, func_s)              \
  ::trompeloeil::heap_elevator{} && TROMPELOEIL_REQUIRE_CALL_OBJ(obj, func, obj_s, func_s)

#define TROMPELOEIL_REQUIRE_CALL_OBJ(obj, func, obj_s, func_s)          \
  ::trompeloeil::call_validator{} +                                     \
  decltype((obj).TROMPELOEIL_CONCAT(trompeloeil_tag_, func) )::func     \
  .set_location(__FILE__ ":" TROMPELOEIL_STRINGIFY(__LINE__))           \
  .set_name(obj_s "." func_s)                                             \
  .hook_last((obj).trompeloeil_matcher_list(decltype(TROMPELOEIL_CONCAT((obj).trompeloeil_tag_, func)){}))


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
  TROMPELOEIL_WITH(#__VA_ARGS__, __VA_ARGS__)

#define TROMPELOEIL_WITH_(arg_s, ...)                                   \
  with(arg_s, [&](const auto& trompeloeil_x) {                          \
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
  TROMPELOEIL_SIDE_EFFECT_(#__VA_ARGS__, __VA_ARGS__)

#define TROMPELOEIL_SIDE_EFFECT_(arg_s, ...) \
  sideeffect(arg_s, [&](auto& trompeloeil_x) {                          \
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
  TROMPELOEIL_RETURN_(#__VA_ARGS__, __VA_ARGS__)

#define TROMPELOEIL_RETURN_(arg_s, ...)                                 \
  handle_return(arg_s, [&](auto& trompeloeil_x) {                       \
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
  TROMPELOEIL_THROW_(#__VA_ARGS__, __VA_ARGS__)

#define TROMPELOEIL_THROW_(arg_s, ...)                                  \
  handle_throw(arg_s, [&](auto& trompeloeil_x) {                        \
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
#define WITH(...)                     TROMPELOEIL_WITH_(#__VA_ARGS__, __VA_ARGS__)
#define SIDE_EFFECT(...)              TROMPELOEIL_SIDE_EFFECT_(#__VA_ARGS__, __VA_ARGS__)
#define RETURN(...)                   TROMPELOEIL_RETURN_(#__VA_ARGS__, __VA_ARGS__)
#define THROW(...)                    TROMPELOEIL_THROW_(#__VA_ARGS__, __VA_ARGS__)
#define TIMES(...)                    TROMPELOEIL_TIMES(__VA_ARGS__)
#define IN_SEQUENCE(...)              TROMPELOEIL_IN_SEQUENCE(__VA_ARGS__)
#define ANY(type)                     TROMPELOEIL_ANY(type)
#define AT_LEAST(num)                 TROMPELOEIL_AT_LEAST(num)
#define AT_MOST(num)                  TROMPELOEIL_AT_MOST(num)
#define REQUIRE_DESTRUCTION(obj)      TROMPELOEIL_REQUIRE_DESTRUCTION_(obj, #obj)
#define NAMED_REQUIRE_DESTRUCTION(obj)TROMPELOEIL_NAMED_REQUIRE_DESTRUCTION_(obj, #obj)
#endif

#endif // include guard

