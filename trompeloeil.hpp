#ifndef TROMPELOEIL_HPP_
#define TROMPELOEIL_HPP_

// trompe l'oeil noun    (Concise Encyclopedia)
// Style of representation in which a painted object is intended
// to deceive the viewer into believing it is the object itself...


// Missing features
// * Mocking overloaded methods
// * Unscoped expectations
// * Sequences
// * EXPECT_DESTRUCTION
// * Report types lacking output stream insertion operator
// * Support custom test output stream insertion operator

#if (!defined(__cplusplus) || __cplusplus <= 201103)
//#error requires C++14 or higher
#endif

#include <utility>
#include <tuple>
#include <iostream>
#include <sstream>
#include <exception>
#include <functional>

#define APPLY(x,...) x(__VA_ARGS__)
#define CONCAT5(x, ...) CONCAT4(x ## __VA_ARGS__)
#define CONCAT4(x, ...) CONCAT3(x ## __VA_ARGS__)
#define CONCAT3(x, ...) CONCAT2(x ## __VA_ARGS__)
#define CONCAT2(x, ...) CONCAT1(x ## __VA_ARGS__)
#define CONCAT1(x) x


#define ARG16(_0,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15, ...) _15
#define HAS_COMMA(...) ARG16(__VA_ARGS__, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  1,  1,  1,  0)

#define MAKE_COMMA(...) ,


#define ISEMPTY(...)                                                    \
  ISEMPTY_(                                                             \
           HAS_COMMA(__VA_ARGS__),                                      \
           HAS_COMMA(MAKE_COMMA __VA_ARGS__),                           \
           HAS_COMMA(__VA_ARGS__ (/*empty*/)),                          \
           HAS_COMMA(MAKE_COMMA __VA_ARGS__ ()))


#define ISEMPTY_(a,b,c,d) HAS_COMMA(CONCAT5(IS_EMPTY_CASE_,a,b,c,d))
#define IS_EMPTY_CASE_0001 ,

#define COUNT0(...) ARG16(__VA_ARGS__,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0)
#define COUNT1() 0
#define COUNT(...) APPLY(CONCAT2, COUNT,  ISEMPTY(__VA_ARGS__))(__VA_ARGS__)


#define PLIST15(c, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15)  \
  PLIST14(c, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14), \
  trompeloeil::c ## _type_catcher<void(P15)> p15

#define PLIST14(c, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14)  \
  PLIST13(c, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13), \
  trompeloeil::c ## _type_catcher<void(P14)> p14

#define PLIST13(c, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13)  \
  PLIST12(c, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12), \
  trompeloeil::c ## _type_catcher<void(P13)> p13

#define PLIST12(c, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12)  \
  PLIST11(c, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11), \
  trompeloeil::c ## _type_catcher<void(P12)> p12

#define PLIST11(c, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11)  \
  PLIST10(c, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10), \
  trompeloeil::c ## _type_catcher<void(P11)> p11

#define PLIST10(c, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10)  \
  PLIST9(c, P1, P2, P3, P4, P5, P6, P7, P8, P9), \
  trompeloeil::c ## _type_catcher<void(P10)> p10

#define PLIST9(c, P1, P2, P3, P4, P5, P6, P7, P8, P9) \
  PLIST8(c, P1, P2, P3, P4, P5, P6, P7, P8), \
  trompeloeil::c ## _type_catcher<void(P9)> p9

#define PLIST8(c, P1, P2, P3, P4, P5, P6, P7, P8) \
  PLIST7(c, P1, P2, P3, P4, P5, P6, P7), \
  trompeloeil::c ## _type_catcher<void(P8)> p8

#define PLIST7(c, P1, P2, P3, P4, P5, P6, P7) \
  PLIST6(c, P1, P2, P3, P4, P5, P6), \
  trompeloeil::c ## _type_catcher<void(P7)> p7

#define PLIST6(c, P1, P2, P3, P4, P5, P6) \
  PLIST5(c, P1, P2, P3, P4, P5), \
  trompeloeil::c ## _type_catcher<void(P6)> p6

#define PLIST5(c, P1, P2, P3, P4, P5) \
  PLIST4(c, P1, P2, P3, P4), \
  trompeloeil::c ## _type_catcher<void(P5)> p5

#define PLIST4(c, P1, P2, P3, P4)  \
  PLIST3(c, P1, P2, P3), \
  trompeloeil::c ## _type_catcher<void(P4)> p4

#define PLIST3(c, P1, P2, P3) \
  PLIST2(c, P1, P2), \
  trompeloeil::c ## _type_catcher<void(P3)> p3

#define PLIST2(c, P1, P2) \
  PLIST1(c, P1), \
  trompeloeil::c ## _type_catcher<void(P2)> p2

#define PLIST1(c, P1) trompeloeil::c ## _type_catcher<void(P1)> p1
#define PLIST0(c, P0)

#define VLIST15(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15) \
  VLIST14(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14),     \
  std::declval<trompeloeil::ref_type_catcher<void(P15)> >()

#define VLIST14(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14) \
  VLIST13(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13),          \
  std::declval<trompeloeil::base_type_catcher<void(P14)> >()

#define VLIST13(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13)     \
  VLIST12(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12),               \
  std::declval<trompeloeil::ref_type_catcher<void(P13)> >()

#define VLIST12(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12)          \
  VLIST11(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11),                    \
  std::declval<trompeloeil::ref_type_catcher<void(P12)> >()

#define VLIST11(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11)               \
  VLIST10(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10),                          \
  std::declval<trompeloeil::ref_type_catcher<void(P11)> >()

#define VLIST10(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10)                    \
  VLIST9(P1, P2, P3, P4, P5, P6, P7, P8, P9),                              \
  std::declval<trompeloeil::ref_type_catcher<void(P10)> >()

#define VLIST9(P1, P2, P3, P4, P5, P6, P7, P8, P9)                         \
  VLIST8(P1, P2, P3, P4, P5, P6, P7, P8),                                  \
  std::declval<trompeloeil::ref_type_catcher<void(P9)> >()

#define VLIST8(P1, P2, P3, P4, P5, P6, P7, P8)                             \
  VLIST7(P1, P2, P3, P4, P5, P6, P7),                                   \
  std::declval<trompeloeil::ref_type_catcher<void(P8)> >()

#define VLIST7(P1, P2, P3, P4, P5, P6, P7)                            \
  VLIST6(P1, P2, P3, P4, P5, P6),                                        \
  std::declval<trompeloeil::ref_type_catcher<void(P7)> >()

#define VLIST6(P1, P2, P3, P4, P5, P6)             \
  VLIST5(P1, P2, P3, P4, P5),                                      \
  std::declval<trompeloeil::ref_type_catcher<void(P6)> >()

#define VLIST5(P1, P2, P3, P4, P5) \
  VLIST4(P1, P2, P3, P4), \
  std::declval<trompeloeil::ref_type_catcher<void(P5)> >()

#define VLIST4(P1, P2, P3, P4) \
  VLIST3(P1, P2, P3), \
  std::declval<trompeloeil::ref_type_catcher<void(P4)> >()

#define VLIST3(P1, P2, P3)                         \
  VLIST2(P1, P2),                                                  \
  std::declval<trompeloeil::ref_type_catcher<void(P3)> >()

#define VLIST2(P1, P2) \
  VLIST1(P1),          \
  std::declval<trompeloeil::ref_type_catcher<void(P2)> >()

#define VLIST1(P1) \
  std::declval<trompeloeil::ref_type_catcher<void(P1)> >()
#define VLIST0()

#define CONCAT_(x, y) x ## y
#define CONCAT(x, y) CONCAT_(x, y)
#define CONST_REF_PLIST(...) CONCAT(PLIST, COUNT(__VA_ARGS__)) (const_ref, __VA_ARGS__)
#define REF_PLIST(...) CONCAT(PLIST, COUNT(__VA_ARGS__)) (ref, __VA_ARGS__)
#define VERBATIM_PLIST(...) CONCAT(PLIST, COUNT(__VA_ARGS__)) (verbatim, __VA_ARGS__)
#define VLIST(...) CONCAT(VLIST, COUNT(__VA_ARGS__))(__VA_ARGS__)

#define CLIST15(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15) \
  (p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15)

#define CLIST14(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14) \
  (p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14)

#define CLIST13(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13) \
  (p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13)

#define CLIST12(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12) \
  (p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12)

#define CLIST11(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11) \
  (p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11)

#define CLIST10(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10) \
  (p1, p2, p3, p4, p5, p6, p7, p8, p9, p10)

#define CLIST9(P1, P2, P3, P4, P5, P6, P7, P8, P9) \
  (p1, p2, p3, p4, p5, p6, p7, p8, p9)

#define CLIST8(P1, P2, P3, P4, P5, P6, P7, P8) \
  (p1, p2, p3, p4, p5, p6, p7, p8)

#define CLIST7(P1, P2, P3, P4, P5, P6, P7) \
  (p1, p2, p3, p4, p5, p6, p7)

#define CLIST6(P1, P2, P3, P4, P5, P6) \
  (p1, p2, p3, p4, p5, p6)

#define CLIST5(P1, P2, P3, P4, P5) \
  (p1, p2, p3, p4, p5)

#define CLIST4(P1, P2, P3, P4) \
  (p1, p2, p3, p4)
#define CLIST3(P1, P2, P3) \
  (p1, p2, p3)
#define CLIST2(P1, P2) \
  (p1, p2)
#define CLIST1(P1) \
  (p1)
#define CLIST0() ()
#define CLIST(...) CONCAT(CLIST, COUNT(__VA_ARGS__)) (__VA_ARGS__)

namespace trompeloeil
{
  class expectation_violation : public std::logic_error
  {
  public:
    using std::logic_error::logic_error;
  };

  enum class severity { fatal, nonfatal };

  inline
  std::function<void(severity, const char* loc, const std::string&)>& reporter_obj()
  {
    static std::function<void(severity, const char* loc, const std::string&)> obj
      = [](severity, const char *loc, const std::string& msg)
      {
        throw expectation_violation(std::string(loc) + "\n" + msg);
      };
    return obj;
  }

  inline
  void set_reporter(std::function<void(severity, const char*loc, const std::string& s)> f)
  {
    reporter_obj() = f;
  }


  inline
  void send_report(severity s, const char* loc, const std::string& msg)
  {
    reporter_obj()(s, loc, msg);
  }

  template<typename T>
  struct list_elem
  {
    list_elem(T *list) : n(list), p(list->p)
    {
      list->n = this;
      p->n = this;
    }

    list_elem() : n(this), p(this)
    {
    }

    list_elem(const list_elem &) = delete;

    list_elem(list_elem &&r) : n(r.n), p(r.p)
    {
      n->p = p->n = this;
      r.n = r.p = &r;
    }

    list_elem(T *p_, T *n_) : n(n_), p(p_)
    {
    }

    ~list_elem()
    {
      unlink();
    }

    list_elem &operator=(const list_elem &) = delete;

    bool is_empty() const
    {
      return n == this;
    }

    T *next()
    {
      return static_cast<T *>(n);
    }

    T *prev()
    {
      return static_cast<T *>(p);
    }

    void unlink()
    {
      auto pp = p;
      n->p = p;
      pp->n = n;
      n = p = this;
    }

    void link_before(T &b)
    {
      b.n = this;
      b.p = p;
      p->n = &b;
      p = &b;
    };
    list_elem *n;
    list_elem *p;
  };

  struct wildcard
  {
    template<typename T>
    operator T&&();
  };

  static const constexpr wildcard _{};

  template<typename T>
  class optional
  {
  public:
    optional() : valid(false)
    {
    }

    optional(const T &t) : valid(true)
    {
      new(buffer) T(t);
    }

    optional(T &&t) : valid(true)
    {
      new(buffer) T(std::move(t));
    }

    optional(const optional &c) : valid(c.valid)
    {
      if (valid) new(buffer) T(c.value());
    }

    optional(optional &&c) : valid(c.valid)
    {
      if (valid) new(buffer) T(c.rvalue());
    }

    ~optional()
    {
      if (valid)
      {value().~T();}
    }

    optional &operator=(const optional &c)
    {
      if (&c != this)
      {
        if (valid) value().~T();
        valid = c.valid;
        if (valid) new(buffer)T(c.value());
      }
      return *this;
    }

    optional &operator=(optional &&c)
    {
      if (valid) value().~T();
      valid = c.valid;
      if (valid) new(buffer)T(std::move(c.rvalue()));
      return *this;
    }

    optional &operator=(const T &t)
    {
      return operator=(optional(t));
    }

    optional &operator=(T &&t)
    {
      return operator=(optional(std::move(t)));
    }

    const T &value() const
    {
      return *reinterpret_cast<const T *>(buffer);
    }

    T &value()
    {
      return *reinterpret_cast<T *>(buffer);
    }

    T &&rvalue()
    {
      return std::move(value());
    }

    bool is_valid() const
    {
      return valid;
    }

    operator T &()
    {
      return value();
    }
  private:
    alignas(T) char buffer[sizeof(T)];
    bool valid;
  };

  template<typename T>
  struct value_matcher
  {
  public:
    value_matcher(wildcard)
    {
    };

    template<typename U,
             typename = std::enable_if<!std::is_same<U, wildcard>::value> >
    value_matcher(U &&u) : desired(std::forward<U>(u))
    {
    }

    template<typename U>
    bool operator==(const U &u) const
    {
      return !desired.is_valid() || desired.value() == u;
    }

    friend std::ostream &operator<<(std::ostream &os, const value_matcher &v)
    {
      if (v.desired.is_valid()) return os << v.desired.value();
      return os << "_";
    }

  private:
    optional<T> desired;
  };

  template<typename T>
  struct type_catcher_t;

  template<typename T>
  struct type_catcher_t<void(T)>
  {
    using type = T;
    using ref_type = T &;
    using const_ref_type = const T &;
    using bare_type = typename std::remove_reference<T>::type;
  };


  template <>
  struct type_catcher_t<void(void)>
  {
    using type = void;
    using ref_type = void;
    using const_ref_type = void;
    using bare_type = void;
  };
  template<typename T>
  using ref_type_catcher = typename type_catcher_t<T>::ref_type;
  template<typename T>
  using const_ref_type_catcher = typename type_catcher_t<T>::const_ref_type;
  template<typename T>
  using verbatim_type_catcher = typename type_catcher_t<T>::type;
  template<typename T>
  using bare_type_catcher = typename type_catcher_t<T>::bare_type;

  template<typename T>
  class mock_base : public T
  {
  protected:
    using mocked_type = T;
  public:
    using T::T;
  };

  template<typename T>
  struct return_of_t;

  template<typename R, typename ... A>
  struct return_of_t<R(A...)>
  {
    using type = R;
  };

  template<typename T>
  using return_of = typename return_of_t<T>::type;

  template<typename T>
  struct call_match_type_t;

  template<typename R, typename ... T>
  struct call_match_type_t<R(T...)>
  {
    using type = std::tuple<const typename std::remove_reference<T>::type &...>;
  };

  template<typename T>
  using call_match_type = typename call_match_type_t<T>::type;

  template<typename T>
  struct value_match_type_t;

  template<typename R, typename ... T>
  struct value_match_type_t<R(T...)>
  {
    using type = std::tuple<value_matcher<typename std::remove_reference<T>::type> ...>;
  };


  template<typename Sig>
  using value_match_type = typename value_match_type_t<Sig>::type;


  template<typename T>
  struct call_action_type_t;

  template<typename R, typename ... T>
  struct call_action_type_t<R(T...)>
  {
    using type = std::tuple<typename std::add_lvalue_reference<T>::type ...>;
  };

  template<typename Sig>
  using call_action_type = typename call_action_type_t<Sig>::type;

  template<typename Sig>
  struct call_matcher_base : public list_elem<call_matcher_base<Sig> >
  {
    using param_type = call_match_type<Sig>;
    call_matcher_base() = default;

    call_matcher_base(call_matcher_base *list)
      : list_elem<call_matcher_base>(list)
    {
    }

    virtual bool matches(const param_type &) = 0;
    virtual bool run_actions(call_action_type<Sig> &params) = 0;
    virtual std::ostream& report_mismatch(std::ostream&,const param_type &) = 0;
    virtual return_of<Sig> return_value(call_action_type<Sig> &params) = 0;
    virtual void report_missed() = 0;
  };

  template<typename ... A>
  struct call_matcher_base<void(A...)>
    : public list_elem<call_matcher_base<void(A...)> >
  {
    using param_type = call_match_type<void(A...)>;

    call_matcher_base() = default;

    call_matcher_base(call_matcher_base *list)
      : list_elem<call_matcher_base>(list)
    {
    }

    virtual bool matches(const param_type &) = 0;
    virtual bool run_actions(call_action_type<void(A...)> &params) = 0;
    virtual std::ostream& report_mismatch(std::ostream&, const param_type &) = 0;
    void return_value(call_action_type<void(A...)> &) {};

    virtual void report_missed() = 0;

  };

  template<typename T, size_t N = 0, bool b = N == std::tuple_size<T>::value>
  struct tuple_pair
  {
    template <typename stream>
    static stream &print_mismatch(stream &os, const T &t1, const T &t2)
    {
      os << "  Got param " << N << " = " << std::get<N>(t1) << '\n'
         << "  Expected:     " << std::get<N>(t2) << "\n\n";

      return tuple_pair<T, N + 1>::print_mismatch(os, t1, t2);
    }

    template <typename stream>
    static stream &print_missed(stream &os, const T &t)
    {
      os << "  param " << N << " = " << std::get<N>(t) << '\n';
      return tuple_pair<T, N + 1>::print_missed(os, t);
    }
  };

  template<typename T, size_t N>
  struct tuple_pair<T, N, true>
  {
    template <typename stream>
    static stream &print_mismatch(stream &os, const T &, const T &)
    {
      return os;
    }

    template <typename stream>
    static stream& print_missed(stream &os, const T &)
    {
      return os;
    }
  };

  template<typename Sig>
  struct call_matcher_list : public call_matcher_base<Sig>
  {
    using typename call_matcher_base<Sig>::param_type;

    template<typename ... U>
    call_matcher_list &operator()(const U &...) { return *this; }

    virtual bool matches(const param_type &) { return false; }

    virtual bool run_actions(call_action_type<Sig> &) { return false; }

    virtual std::ostream& report_mismatch(std::ostream& r, const param_type &) { return r;}

    virtual return_of<Sig> return_value(call_action_type<Sig> &)
    {
      return_of<Sig> *p = nullptr;
      return *p;
    }

    virtual void report_missed() {}
  };

  template<typename ... A>
  struct call_matcher_list<void(A...)> : public call_matcher_base<void(A...)>
  {
    using typename call_matcher_base<void(A...)>::param_type;

    template<typename ... U>
    call_matcher_list &operator()(const U &...) { return *this;}
    virtual bool matches(const param_type &) { return false; }
    virtual bool run_actions(call_action_type<void(A...)> &) { return false;}
    virtual std::ostream& report_mismatch(std::ostream& r, const param_type &) {return r;}
    virtual void report_missed() {}
  };

  template<typename Sig>
  struct condition_base : public list_elem<condition_base<Sig> >
  {
    virtual bool check(const typename call_matcher_base<Sig>::param_type &) = 0;
  };

  template<typename Sig>
  struct condition_list : public condition_base<Sig>
  {
    bool check(const typename call_matcher_base<Sig>::param_type &)
    {
      return false;
    }
  };

  template<typename Sig, typename Cond>
  struct condition : public condition_base<Sig>
  {
    condition(const char *str_, Cond c_) : c(c_), str(str_) {};

    bool check(const typename call_matcher_base<Sig>::param_type &t)
    {
      return c(t);
    }

    Cond c;
    const char *str;
  };


  template<typename Sig>
  struct side_effect_base : public list_elem<side_effect_base<Sig> >
  {
    virtual void action(call_action_type<Sig> &) = 0;
  };

  template<typename Sig>
  struct side_effect_list : public side_effect_base<Sig>
  {
    void action(call_action_type<Sig> &) {}
  };

  template<typename Sig, typename Action>
  struct side_effect : public side_effect_base<Sig>
  {
    side_effect(const char *str_, Action a_) : a(a_), str(str_) {};

    void action(call_action_type<Sig> &t) { a(t); }

    Action a;
    const char *str;
  };

  template<typename Sig>
  struct return_handler_base : public list_elem<return_handler_base<Sig> >
  {
    virtual return_of<Sig> return_value(call_action_type<Sig> &) = 0;
  };

  template<typename Sig>
  struct return_handler_list : public return_handler_base<Sig>
  {
    return_of<Sig> return_value(call_action_type<Sig> &)
    {
      return_of<Sig> *p = nullptr;
      return *p;
    }
  };

  template<typename ... A>
  struct return_handler_list<void(A...)>
    : public return_handler_base<void(A...)>
  {
    void return_value(call_action_type<void(A...)> &) {}
  };

  template<typename Sig, typename Handler>
  struct return_handler : public return_handler_base<Sig>
  {
    return_handler(const char *str_, Handler h_) : h(h_), str(str_) {};

    return_of<Sig> return_value(call_action_type<Sig> &t) { return h(t); }

    Handler h;
    const char *str;
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
  struct call_limit_injector : public P
  {
    static const bool call_limit_set = true;
    using P::P;

    template <typename ... A>
     call_limit_injector(A && ... a) : P(std::forward<A>(a)...) {}
  };

  template <typename Parent, typename U, typename Sig>
  struct call_data : public Parent
  {
    using typename Parent::return_type;
    using Parent::call_limit_set;

    call_data(Parent&& p) : Parent(std::move(p)) {}
    call_data(Parent&& p, U&& d) : Parent(std::move(p)), data(std::move(d))
    {
      add_last(data);
    }
    void add_last(side_effect_base<Sig>& s)
    {
      this->actions.link_before(s);
    }
    void add_last(condition_base<Sig>& s)
    {
      this->conditions.link_before(s);
    }
    void add_last(return_handler_base<Sig>& s)
    {
      this->return_handler.link_before(s);
    }
    void add_last(std::tuple<>) {}
    template <typename D>
    call_data<call_data, condition<Sig, D>, Sig> with(const char* str, D&& d)
    {
      return {std::move(*this), condition<Sig, D>(str, std::move(d))};
    }
    template <typename A>
    call_data<call_data, side_effect<Sig, A>, Sig> sideeffect(const char* str, A&& a)
    {
      return {std::move(*this), side_effect<Sig, A>(str, std::move(a))};
    }
    template <typename H>
    call_data<return_type_injector<return_of<Sig>, call_data>, return_handler<Sig, H>, Sig> handle_return(const char* str, H&& h)
    {
      static_assert(std::is_constructible<return_of<Sig>, decltype(h(std::declval<call_action_type<Sig>& >()))>::value || !std::is_same<return_of<Sig>, void>::value,
                    "RETURN does not make sense for void-function");
      static_assert(std::is_constructible<return_of<Sig>, decltype(h(std::declval<call_action_type<Sig>& >()))>::value || std::is_same<return_of<Sig>, void>::value,
                    "given RETURN type is not convertible to that of the function");
      static_assert(!std::is_same<return_type, decltype(h(std::declval<call_action_type<Sig>&>()))>::value || std::is_same<return_type, void>::value,
                    "A RETURN is already given");
      return {return_type_injector<return_of<Sig>, call_data>(std::move(*this)), return_handler<Sig, H>(str, std::move(h))};
    }
    template <unsigned long long L,
              unsigned long long H = L,
              bool               verboten = call_limit_set>
    call_data<call_limit_injector<call_data>, std::tuple<>, Sig> times()
    {
      static_assert(!verboten,
                    "A TIMES call limit is already given");
      static_assert(H >= L, "In TIMES Higher limit must exceed lower limit");
      static_assert(H > 0, "TIMES call limit of 0 does not make sense");
      this->min_calls = L;
      this->max_calls = H;
      return { std::move(*this), {} };
    }

    template <typename T = return_type>
    call_data& validate()
    {
      static_assert(std::is_same<return_of<Sig>, return_type>::value,
                    "RETURN missing for non-void function");
      return *this;
    }

    U data;
  };

  template<typename Sig>
  struct call_matcher : public call_matcher_base<Sig>
  {
    using return_type = void;
    using param_type = typename call_matcher_base<Sig>::param_type;
    template<typename ... U>
    call_matcher(U &&... u) : val(std::forward<U>(u)...) {}

    call_matcher(call_matcher &&) = default;

    ~call_matcher()
    {
      if (!reported && !this->is_empty() && call_count < min_calls) report_missed();
    }

    call_matcher hook_last(call_matcher_list<Sig> &list)
    {
      list.link_before(*this);
      return std::move(*this);
    }

    template <typename T = return_type>
    call_matcher& validate()
    {
      static_assert(std::is_same<T, return_of<Sig> >::value,
                    "missing RETURN for non-void function");
      return *this;
    }

    virtual bool matches(const param_type& params) override
    {
      return val == params && match_conditions(params);
    }

    bool match_conditions(const param_type& params)
    {
      for (auto p = conditions.next(); p != &conditions; p = p->next())
      {
        if (!p->check(params)) return false;
      }
      return true;
    }

    return_of<Sig> return_value(call_action_type<Sig>& params)
    {
      return return_handler.next()->return_value(params);
    }
    bool run_actions(call_action_type<Sig>& params)
    {
      auto limits = call_limits();
      if (++call_count > std::get<1>(limits))
      {
        reported = true;
        std::ostringstream os;
        os << "Expectation fulfilled " << call_count << " times when the limit is "
           << std::get<1>(limits) << "\n";
        send_report(severity::fatal, location, os.str());
      }
      for (auto p = actions.next(); p != &actions; p = p->next())
      {
        p->action(params);
      }
      return call_count == std::get<1>(limits);
    }
    std::ostream& report_mismatch(std::ostream& os, const param_type& params) override
    {
      reported = true;
      os << "No matching call for expectation at " << location << "\n";
      return tuple_pair<decltype(val)>::print_mismatch(os, params, val);
    }

    void report_missed() override
    {
      reported = true;
      std::ostringstream os;
      os << "Unfulfilled expectation:\n"
         << "Expected to be called " << min_calls
         << " times, actually called " << call_count << " time"
         << (call_count > 1 ? "s\n" : "\n");
      tuple_pair<decltype(val)>::print_missed(os, val);
      send_report(severity::nonfatal, location, os.str());
    }
    template <typename D>
    call_data<call_matcher, condition<Sig, D>, Sig> with(const char* str, D&& d)
    {
      return { std::move(*this), condition<Sig, D>(str, std::move(d)) };
    }
    template <typename A>
    call_data<call_matcher, side_effect<Sig, A>, Sig> sideeffect(const char* str, A&& a)
    {
      return {std::move(*this), side_effect<Sig, A>(str, std::move(a))};
    }

    template <typename H>
    call_data<return_type_injector<return_of<Sig>, call_matcher>, return_handler<Sig, H>, Sig> handle_return(const char* str, H&& h)
    {
      static_assert(!std::is_same<return_of<Sig>, void>::value || std::is_same<H, void>::value,
                    "RETURN does not make sense for void-function");
#if 0
      static_assert(std::is_constructible<return_of<Sig>, decltype(h(std::declval<call_action_type<Sig> >()))>::value || std::is_same<return_of<Sig>, void>::value,
                    "given RETURN type is not convertible to that of the function");
#endif
      return {std::move(*this), {str, std::move(h)}};
    }
    template <unsigned long long L, unsigned long long H = L>
    call_data<call_limit_injector<call_matcher>, std::tuple<>, Sig> times()
    {
      static_assert(H >= L, "In TIMES Higher limit must exceed lower limit");
      static_assert(L > 0, "TIMES call limit of 0 does not make sense");
      min_calls = L;
      max_calls = H;
      return { std::move(*this), {} };
    }

    call_matcher &set_location(const char *loc)
    {
      location = loc;
      return *this;
    }
    virtual std::tuple<unsigned long long, unsigned long long> call_limits() const
    {
      return std::make_tuple(min_calls, max_calls);
    }

    static const bool         call_limit_set = false;
    value_match_type<Sig>     val;
    condition_list<Sig>       conditions;
    side_effect_list<Sig>     actions;
    return_handler_list<Sig>  return_handler;
    const char               *location;
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
    static const illegal_argument<N>& value(T&)
    {
      static const constexpr illegal_argument<N> v{};
      return v;
    }

  };

  template <int N, typename T>
  auto mkarg(T& t) -> decltype(arg<T, N>::value(t)) { return arg<T, N>::value(t); }

  template <typename ... T>
  void ignore(const T& ...) {}

  template <typename ... T>
  call_match_type<void(T...)> make_value_match_obj(T& ... t)
  {
    call_match_type<void(T...)> rv{t...};
    return rv;
  }

  template <typename ... T>
  call_action_type<void(T...)> make_action_type_obj(T& ... t)
  {
    return call_action_type<void(T...)>(t...);
  }

  struct call_validator
  {
  template <typename T>
    T operator+(T&& t) { return std::move(t.validate()); }
  };
}

template<typename T>
class mock;

#define MOCK_CLASS(x) template <> class mock<x> \
  : public ::trompeloeil::mock_base<x>

#define MOCK(name, params)                                                    \
  using matcher_type ## name  = ::trompeloeil::call_matcher<decltype(std::declval<mocked_type>().name( VLIST params)) params>; \
  ::trompeloeil::call_matcher_list<decltype(std::declval<mocked_type>().name( VLIST params)) params> matcher_list ## name; \
  ::trompeloeil::call_matcher_list<decltype(std::declval<mocked_type>().name( VLIST params)) params> exhausted_matcher_list ## name; \
  auto name (VERBATIM_PLIST params) -> decltype(mocked_type::name CLIST params) override \
  {                                                                           \
    const auto param_value = ::trompeloeil::make_value_match_obj CLIST params;  \
    auto i = matcher_list ## name.next();                                     \
    while (i != &matcher_list ## name && !i->matches(param_value))            \
    {                                                                         \
      i = i->next();                                                          \
    }                                                                         \
    if (i == &matcher_list ## name)                                           \
    {                                                                         \
      std::ostringstream os;                                                  \
      os << "No match for call " #name #params ".\n";                         \
      for (auto ei = exhausted_matcher_list ## name.next();                   \
           ei != &exhausted_matcher_list ## name;                             \
           ei = ei->next())                                                   \
      {                                                                       \
        if (ei->matches(param_value))                                         \
        {                                                                     \
          os << "Did you mean exhausted match\n";                             \
          ei->report_mismatch(os, param_value);                               \
          ::trompeloeil::send_report(::trompeloeil::severity::fatal,          \
                                     "",                                      \
                                     os.str());                               \
        }                                                                     \
      }                                                                       \
      for (auto i = matcher_list ## name.next();                              \
           i != &matcher_list ## name;                                        \
           i = i->next())                                                     \
      {                                                                       \
        os << "Tried ";                                                       \
        i->report_mismatch(os, param_value);                                  \
      }                                                                       \
      ::trompeloeil::send_report(::trompeloeil::severity::fatal, "", os.str()); \
    }                                                                         \
    auto param_ref = ::trompeloeil::make_action_type_obj CLIST params;        \
    if (i->run_actions(param_ref))                                            \
      {                                                                       \
        i->unlink();                                                          \
        exhausted_matcher_list ## name.link_before(*i);                       \
      }                                                                       \
    return i->return_value(param_ref);                                        \
  }

#define STRINGIFY_(...) #__VA_ARGS__
#define STRINGIFY(...) STRINGIFY_(__VA_ARGS__)

#define REQUIRE_CALL(obj, func) \
auto CONCAT(call_obj, __COUNTER__) = ::trompeloeil::call_validator{} + CONCAT(decltype(obj)::matcher_type, func ) \
  .set_location(__FILE__ ":" STRINGIFY(__LINE__)) \
  . hook_last(CONCAT(obj.matcher_list, func))

#define WITH(...) with(#__VA_ARGS__, [&](const auto& x) { \
  auto& _1 = ::trompeloeil::mkarg<1>(x);                    \
  auto& _2 = ::trompeloeil::mkarg<2>(x);                    \
  auto& _3 = ::trompeloeil::mkarg<3>(x);                    \
  auto& _4 = ::trompeloeil::mkarg<4>(x);                    \
  auto& _5 = ::trompeloeil::mkarg<5>(x);                    \
  auto& _6 = ::trompeloeil::mkarg<6>(x);                    \
  auto& _7 = ::trompeloeil::mkarg<7>(x);                    \
  auto& _8 = ::trompeloeil::mkarg<8>(x);                    \
  auto& _9 = ::trompeloeil::mkarg<9>(x);                    \
  auto&_10 = ::trompeloeil::mkarg<10>(x);                   \
  auto&_11 = ::trompeloeil::mkarg<11>(x);                   \
  auto&_12 = ::trompeloeil::mkarg<12>(x);                   \
  auto&_13 = ::trompeloeil::mkarg<13>(x);                   \
  auto&_14 = ::trompeloeil::mkarg<14>(x);                   \
  auto&_15 = ::trompeloeil::mkarg<15>(x);                   \
  ::trompeloeil::ignore(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15); \
  return __VA_ARGS__;\
  })

#define SIDE_EFFECT(...) sideeffect(#__VA_ARGS__, [&](auto& x) { \
  auto& _1 = ::trompeloeil::mkarg<1>(x);                    \
  auto& _2 = ::trompeloeil::mkarg<2>(x);                    \
  auto& _3 = ::trompeloeil::mkarg<3>(x);                    \
  auto& _4 = ::trompeloeil::mkarg<4>(x);                    \
  auto& _5 = ::trompeloeil::mkarg<5>(x);                    \
  auto& _6 = ::trompeloeil::mkarg<6>(x);                    \
  auto& _7 = ::trompeloeil::mkarg<7>(x);                    \
  auto& _8 = ::trompeloeil::mkarg<8>(x);                    \
  auto& _9 = ::trompeloeil::mkarg<9>(x);                    \
  auto&_10 = ::trompeloeil::mkarg<10>(x);                   \
  auto&_11 = ::trompeloeil::mkarg<11>(x);                   \
  auto&_12 = ::trompeloeil::mkarg<12>(x);                   \
  auto&_13 = ::trompeloeil::mkarg<13>(x);                   \
  auto&_14 = ::trompeloeil::mkarg<14>(x);                   \
  auto&_15 = ::trompeloeil::mkarg<15>(x);                   \
  ::trompeloeil::ignore(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15); \
  __VA_ARGS__;\
  })

#define RETURN(...) handle_return(#__VA_ARGS__, [&](auto& x) {             \
  auto& _1 = ::trompeloeil::mkarg<1>(x);                                     \
  auto& _2 = ::trompeloeil::mkarg<2>(x);                                     \
  auto& _3 = ::trompeloeil::mkarg<3>(x);                                     \
  auto& _4 = ::trompeloeil::mkarg<4>(x);                                     \
  auto& _5 = ::trompeloeil::mkarg<5>(x);                                     \
  auto& _6 = ::trompeloeil::mkarg<6>(x);                                     \
  auto& _7 = ::trompeloeil::mkarg<7>(x);                                     \
  auto& _8 = ::trompeloeil::mkarg<8>(x);                                     \
  auto& _9 = ::trompeloeil::mkarg<9>(x);                                     \
  auto&_10 = ::trompeloeil::mkarg<10>(x);                                    \
  auto&_11 = ::trompeloeil::mkarg<11>(x);                                    \
  auto&_12 = ::trompeloeil::mkarg<12>(x);                                    \
  auto&_13 = ::trompeloeil::mkarg<13>(x);                                    \
  auto&_14 = ::trompeloeil::mkarg<14>(x);                                    \
  auto&_15 = ::trompeloeil::mkarg<15>(x);                                    \
  ::trompeloeil::ignore(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15); \
  return __VA_ARGS__;                                                      \
    })

#define TIMES(...) times<__VA_ARGS__>()
#endif // include guard
