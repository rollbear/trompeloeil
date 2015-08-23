## *Trompeloeil*

![trompeloeil logo](trompeloeil-logo.png)  [![Build Status](https://travis-ci.org/rollbear/trompeloeil.svg?branch=master)](https://travis-ci.org/rollbear/trompeloeil) [![Coverage Status](https://coveralls.io/repos/rollbear/trompeloeil/badge.svg?branch=master&service=github)](https://coveralls.io/github/rollbear/trompeloeil?branch=master)

> *trompe l'oeil* noun    (Concise Encyclopedia)  
> Style of representation in which a painted object is intended
> to deceive the viewer into believing it is the object itself...

What is it?
-----------

A header only mocking framework for C++14.

See this [introduction](http://playfulprogramming.blogspot.com/2014/12/introducing-trompeloeil-c-mocking.html) to get up to speed.  

Follow up with the post on [sequencing](http://playfulprogramming.blogspot.se/2015/01/sequence-control-with-trompeloeil-c.html) for examples on how to restrict or relax allowed sequences of matching calls.  

If you want to contribute, read ACCU
[overload 125](http://accu.org/var/uploads/journals/Overload125.pdf)
to learn the internals.

Contents
--------

- [Example usage](#example-usage)
- [Limitations](#Limitations)
- [How to use](#how-to-use)
  - [Types and Templates](#types-and-templates)
  - [Macros](#macros)
  - [Matchers](#matchers)
  - [Printing values](#printing-values)
  - [Report to test frameworks](#report-to-test-frameworks)
    - [Catch!](#catch)
    - [crpcut](#crpcut)
    - [gtest](#gtest)
    - [boost Unit Test Framework](#boost-unit-test-framework)
  - [Writing custom matchers](#writing-custom-matchers)
- [Compiler compatibility](#compiler-compatibility)

Example usage
-------------

```Cpp
#include <trompeloeil.hpp>

class I
{
public:
  I(const char*);
  virtual ~I() {}
  virtual bool foo(int, std::string& s) = 0;
  virtual bool bar(int) = 0;
  virtual bool bar(std::string) = 0;
};

class CUT
{
public:
  CUT(I* i);
  std::string work(int n);
};

class MI : public I
{
public:
  using I::ID
  MAKE_MOCK2(foo, bool(int, std::string&));
  MAKE_MOCK1(bar, bool(int));
  MAKE_MOCK1(bar, bool(std::string));
};

TEST(work_returns_the_string_obtained_from_I_foo_and_calls_I_bar)
{
  trompeloeil::stream_tracer log(std::cout);

  using trompeloeil::_; // wild card for matching any value
  using trompeloeil::gt; // greater-than match

  auto raw_i = new trompeloeil::deathwatched<MI>("word");

  CUT out(raw_i);

  trompeloeil::sequence seq1, seq2;

  int local_var;

  {
    REQUIRE_CALL(*raw_i, bar(ANY(int)))
      .LR_SIDE_EFFECT(local_var = _1)
      .RETURN(_1 > 0)
      .IN_SEQUENCE(seq1)
      .TIMES(AT_LEAST(1));

    FORBID_CALL(*raw_i, bar(0));

    REQUIRE_CALL(*raw_i, bar("word"))
      .RETURN(true)
      .IN_SEQUENCE(seq2);

    REQUIRE_CALL(*raw_i, foo(gt(2), _))
      .WITH(_2 == "")
      .IN_SEQUENCE(seq1, seq2)
      .SIDE_EFFECT(_2 = "cat")
      .RETURN(true);

    REQUIRE_DESTRUCTION(*raw_i);

    auto s = out.work(3);

    ASSERT_TRUE(s == "cat");
  }

}
```

Limitations
-----------

- Function templates cannot be mocked
- WAY too many macros... but I think we'll have to make do with most of them

- Quirk! Matching a move-only type passed by value or by rvalue-refenence
  requires a bit of trickery in the expectation. Use a wild card in the
  **`REQUIRE_CALL`()**, and match in **`.WITH()`** or **`.LR_WITH()`** using
  alternative means. For example, a `std::unique_ptr<T>` passed by value can be
  matched with the raw pointer value in **`.WITH()`**.

How to use
----------
The example above shows most currently supported functionality.

## Types and Templates

**`trompeloeil::deathwatched<T>`**  
Template used when monitoring the lifetime of a mock object. If a
**`deathwatched`** object is destroyed before a **`REQUIRE_DESTRUCTION`**
is active, an error is reported. Copying or moving a **`deathwatched`** object
does not transfer a death expectation.
**NOTE!** **`T`** must have a virtual destructor.

**`trompeloeil::sequence`**  
Type of sequence objects, used to impose an order of matching invocations of
**`REQUIRE_CALL`** instances. Several sequence objects can be used to denote
parallel sequences, and several sequence objects can be joined in one
**`REQUIRE_CALL`**.

**`trompeloeil::wildcard`**  
The type of the wild card object `trompeloeil::_` . You typically never see
the type itself.

**`trompeloeil::expectation`**  
Base type of a named expectation object, as created by **`NAMED_REQUIRE_CALL`**,
**`NAMED_FORBID_CALL`** and **`NAMED_ALLOW_CALL`**.

**`trompeloeil::stream_tracer`**  
An object that traces matching calls to the `std::ostream` instance provided in
the constructor. Mostly useful with **`ALLOW_CALL`** when doing exploratory
testing of legacy code.

**`trompeloel::tracer`**  
Base class for tracers. Override the member function
`void trace(const char* file, unsigned long line, const std::string& call)` in your
own trace class if a **`trompeloeil::stream_tracer`** is not right for you.

## Macros

If the generic macro names conflicts with others in your sources, define the
macro **`TROMPELOEIL_LONG_MACROS`** before inclusion of `trompeloeil.hpp`. It
prefixes all macros listed below with **`TROMPELOEIL_`**.

**`ANY`(** *type* **)**  
Typed wild card to disambiguate overloaded functions on type when the exact
value is unimportant.

**`AT_LEAST`**( *number* **)**  
Used in **`.TIMES`()** to set the range *number*..infinity. *number* must be
`constexpr`.

**`AT_MOST`**( *number* **)**  
Used in **`.TIMES`()** to set the range 0..*number*. *number* must be
`constexpr`.

**`MAKE_MOCKn`(** *name*, *signature* **)**  
Make a mock implementation of the member function named *name*. It is a good
idea for this to implement a pure virtual function from an interface, but
it is not a requirement. If no virtual member function from a base class
matches the signature, a new member function is made.

**`MAKE_CONST_MOCKn`(** *method_name*, *signature* **)**  
Same as **`MAKE_MOCKn`(** *method_name*, *signature* **)** for `const`
member functions.

**`REQUIRE_CALL`(** *mock_object*, *func_name*(*parameter_list*)**)**  
Set up an expectation that the member function *func_name* is called on the
object *mock_object*. The parameter list may include exact match values, the
wild card `trompeloeil::_`, or the **`ANY`(** *type* **)** wildcard. The
expectation has scope lifetime and must be met by the time the scope ends.
When there are several expectations active on the same object and method, they
are tried in the reversed order they were defined, i.e. last defined is tried
first, until a match is found. This allows you to specify a wide default early,
and narrow specializations in short scopes.

**`NAMED_REQUIRE_CALL`(** *mock_object*, *func_name*(*parameter_list*)**)**
Same as **`REQUIRE_CALL`**, except it instantiates a
*std::unique_ptr&lt;trompeloeil::expectation&gt;* which you can bind to a
variable.
**NOTE!** Any named local objects referenced in attached **`.LR_WITH()`**,
**`.LR_SIDE_EFFECT()`**, **`.LR_RETURN()`** and **`.LR_THROW()`** are captured by
reference, so lifetime management is important.

**`REQUIRE_DESTRUCTION`(** *mock_object* **)**  
Makes it legal for a **`deathwatched`** *mock_object* to be destroyed. An erro
is reported if *mock_object* is still alive at end of scope.

**`NAMED_REQUIRE_DESTRUCTION`(** *mock_object* **)**  
Same as **`REQUIRE_DESTRUCTION`**, except it instantiates a
*std::unique_ptr&lt;trompeloeil::lifetime_monitor&gt;* which you can bind to a
variable.

**`.WITH`(** *expr* **)**  
Add further conditions for a **`REQUIRE_CALL`**, typically used when the
wild card `trompeloeil::_` has been used. *expr* can refer to parameters in the
call with their positional names `_1`, `_2`, etc. Even if the
function signature has parameter as non-`const` references, they are
immutable in this context. Several **`.WITH`** clauses can be added to a single
**`REQUIRE_CALL`**. Named local objects accessed here refers to a copy.

**`.LR_WITH`(** *expr* **)**  
Same as **`.WITH`**, except that named local objects are accessed by reference.

**`.SIDE_EFFECT`(** *expr* **)**  
Code to cause side effects. *expr* is only evaluated when all **`.WITH`**
clauses are matched. *expr* may refer to parameters in the call with their
positional names `_1`, `_2`, etc. This code may alter out-parameters.
Several **`.SIDE_EFFECT`** clauses can be added to a single **`REQUIRE_CALL`**.
Named local objects accessed here refers to a copy.

**`.LR_SIDE_EFFECT`(** *expr* **)**  
Same as **`.SIDE_EFFECT`**, except that named local objects are accessed by
reference.

**`.RETURN`(** *expr* **)**  
Set the return value after having evaluated every **`.SIDE_EFFECT`** . For
`void` functions **`.RETURN`** is illegal. For non-`void` functions
**`.RETURN`** is required exactly once. *expr* may refer to parameters in the
call with their positional names `_1`, `_2`, etc. This code may alter
out-parameters. If you need to return an lvalue reference, use `std::ref()`.
It is not legal to combine both **`.THROW`** and **`.RETURN`**.
Named local objects accessed here refers to a copy.

**`.LR_RETURN`(** *expr* **)**  
Same as **`.RETURN`**, except that named local objects are accessed by
reference.

**`.THROW`(** *expr* **)**  
Throw after having evaluated every **`.SIDE_EFFECT`** . *expr* may refer to
parameters in the call with their positional names `_1`, `_2`, etc. This code
may alter out-parameters. It is not legal to combine both **`.THROW`** and
**`.RETURN`**. Named local objects accessed here refers to a copy.

**`.LR_THROW`(** *expr* **)**  
Same as **`.THROW`**, except that named local objects are accessed by
reference.

**`.TIMES`(** *limit* **)**  
Set the number of times the call is allowed. *limits* must be `constexpr`.
*limits* may be one number specifying an exact amount, or two numbers
specifying a range. A lower limit of `0` means the call is not required.
By default a **`REQUIRE_CALL`** is expected to be tripped exactly once.
**`.TIMES`** may only be used once for a **`REQUIRE_CALL`**

**`.IN_SEQUENCE`(** *seq...* **)**  
Where *seq...* is one or more instances of `trompeloeil::sequence`. Impose an
order in which **`.REQUIRE_CALL`** must match. Several sequences can be parallel
and interleaved. A sequence for a **`.REQUIRE_CALL`** is no longer monitored
once the lower limit from **`.TIMES`** is reached.

**`ALLOW_CALL`(** *mock_object*, *method_name*(*parameter_list*)**)**  
Same as **`REQUIRE_CALL`**().**`TIMES`(** 0, infinity **)**. Matches any
number of times, but is not required to match. (_actually the limit is
0..~0ULL, but that is for all practical purposes "infinity"_)

**`NAMED_ALLOW_CALL`(** *mock_object*, *method_name*(*parameter_list*)**)**  
Same as **`NAMED_REQUIRE_CALL`**().**`TIMES`(** 0, infinity **)**.
Matches any number of times, but is not required to match. (_actually the limit
is 0..~0ULL, but that is for all practical purposes "infinity"_).
**NOTE!** Any named local objects referenced in attached **`.LR_WITH()`**,
**`.LR_SIDE_EFFECT()`**, **`.LR_RETURN()`** and **`.LR_THROW()`** are captured
by reference, so lifetime management is important.

**`FORBID_CALL`(** *mock_object*, *method_name*(*parameter_list*)**)**  
Same as **`REQUIRE_CALL`**().**`TIMES`(** 0 **)**, making any matching call
an error. No **`.RETURN`**() is needed for non-void functions.

**`NAMED_FORBID_CALL`(** *mock_object*, *method_name*(*parameter_list*)**)**  
Same as **`NAMED_REQUIRE_CALL`**().**`TIMES`(** 0 **)**, making any matching
call an error. No **`.RETURN`**() is needed for non-void functions.
**NOTE!** Any named local objects referenced in attached **`.LR_WITH()`** are
captured by reference, so lifetime management is important.

## Matchers

In any expectation (**`REQUIRE_CALL`**, **`ALLOW_CALL`**, or **`FORBID_CALL`**,)
you add the parameter values that signifies a matching call. Sometimes a
simple equality check is not enough, though. You can then choose to eiter
use **`WITH`** (or **`LR_WITH`**) clauses or matchers.

The matchers supplied by `trompeloeil` are:

**`trompeloeil::ne`(** `value` **)**  
Match parameters that are not equal to `value`. The comparison used is
`param != value`.

**`trompeloeil::gt`(** `value` **)**  
Match parameters that are greater than `value`. The comparison used is
`param > value`.

**`trompeoleil::ge`(** `value` **)**  
Match parameters that are greater than or equal to `value`. The comparison used
is `param >= value`.

**`tropmeloeil::lt`(** `value` **)**  
Match parameters that are less than `value`. The comparison used is
`param < value`.

**`trompeloeil::le`(** `value` **)**  
Match parameters that are less than or equal to `value`. The comparison used
is `param <= value`.

A matcher cannot operate on several parameters together. For more complex
matching conditions, **`WITH`** or **`LR_WITH`** clauses are required.

Also see [writing custom matchers](#writing-custom-matchers) for extending
your tests with your own matchers.

## Printing values

By default *`trompeloeil`* prints any values in violation reports using
the defined output stream `operator<<` for the type. If none is available,
the objects memory region is printed as a hex-dump.

Should you want to provide a special print function for use in tests,
define a function `void print(std::ostream&, const TYPE&)` in namespace
`trompeloeil` for your type.

## Report to test frameworks

By default *`trompeloeil`* reports all violations as exceptions. This
works with most test frameworks, but since the error of not having matched
a **`REQUIRE_CALL`** is not discovered until the match object goes out of
scope and is destroyed, this error will call *`std::terminate()`*, which
is typically not what you want.

There is a function  
```Cpp
trompeloeil::set_reporter(std::function<void(trompeloeil::severity,
                                             char const *file,
                                             unsigned long line,
                                             const std::string& msg)>)
```
which can be used to control the reporting. `trompeloeil::severity` is an enum
with the values `fatal` and `nonfatal`. Severity is `nonfatal` when called
from the destructor of a **`REQUIRE_CALL`** object due to unfulfilled
expectations.

**NOTE!** There are some violation that cannot be attributed to a source code
location. An example is an unexpected call to a mock function for which there
are no expectations. In these cases `file` will be an empty string and
`line` == 0.

Some examples for popular C++ unit test frameworks are:

### [Catch!](https://github.com/philsquared/Catch)
```Cpp
  trompeloeil::set_reporter([](::trompeloeil::severity s,
                               char const *file,
                               unsigned long line,
                               const std::string& msg)
    {
      std::ostringstream os;
      if (line) os << file << ':' << line << '\n';
      os << msg;
      if (s == ::trompeloeil::severity::fatal)
        {
          FAIL(os.str());
        }
      CHECK(os.str() == "");
    });
```

### [crpcut](http://crpcut.sourceforge.net)
```Cpp
  trompeloeil::set_reporter([](::trompeloeil::severity,
                               char const *file,
                               unsigned long line,
                               const std::string& msg)
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
    });
```

### [gtest](https://code.google.com/p/googletest/)
```Cpp
  trompeloeil::set_reporter([](trompeloeil::severity s,
                               char const *file,
                               unsigned long line,
                               const std::string& msg)
    {
      if (s == trompeloeil::severity::fatal)
      {
        std::ostringstream os;
        if (line != 0U)
        {
          os << file << ':' << line << '\n';
        }
        throw trompeloeil::expectation_violation(os.str() + msg);
      }

      ADD_FAILURE_AT(file, line) << msg;
    });

```

### [boost Unit Test Framework](http://www.boost.org/doc/libs/1_58_0/libs/test/doc/html/index.html)
```Cpp
  using trompeloeil::severity;
  trompeloeil::set_reporter([](severity s,
                               char const *file,
                               unsigned long line,
                               const std::string& msg)
    {
      std::ostringstream os;
      if (line != 0U) os << file << ':' << line << '\n';
      auto text = os.str() + msg;
      if (s == severity::fatal)
        BOOST_FAIL(text);
      else
        BOOST_ERROR(text);
    });
```

## Writing custom matchers

Writing a matcher for your own data types is easy.

All matchers need to
- inherit from `trompeloeil::matcher` or `trompeloeil::typed_matcher<T>`
- implement a `bool matches(parameter_value) const` member function
- implement an output stream insertion operator

### Typed matcher

The simplest matcher is a typed matcher. As an example of a typed matcher, an
`any_of` matcher is shown, mimicing the behaviour of the standard library
algorithm
[`std::any_of`](http://en.cppreference.com/w/cpp/algorithm/all_any_none_of),
allowing a parameter to match any of a set of values.

For templated matchers, it is often convenient to provide a function that creates the
matcher object. Below is the code for `any_of_t<T>`, which is the matcher created by
the `any_of(std::initialize_list<T>)` function template.

```Cpp
  template <typename T>
  class any_of_t : public trompeloeil::typed_matcher<T>
  {
  public:
    any_of_t(std::initializer_list<T> elements)
    : alternatives(std::begin(elements), std::end(elements))
    {
    }
    bool matches(T const& t) const
    {
      return std::any_of(std::begin(alternatives), std::end(alternatives),
                         [&t](T const& val) { return t == val; });
    }
    friend std::ostream& operator<<(std::ostream& os, any_of_t<T> const& t)
    {
      os << " matching any_of({";
      char const* prefix=" ";
      for (auto& n : t.alternatives)
      {
        os << prefix << n;
        prefix = ", ";
      }
      return os << " })";
    }
  private:
    std::vector<T> alternatives;
  };

  template <typename T>
  auto any_of(std::initializer_list<T> elements)
  {
    return any_of_t<T>(elements);
  }
```

The `matches` member function at accepts the parameter and returns
true if the value is in the specified set, in this case if it is any
of the values stored in the `elements` vector.

The output stream insertion operator is only called if a failure is reported.
The report will then look like:

```
No match for call of obj.foo with signature x(y) with.
  param  _1 = Z

Troid obj.foo(any_of({x,y,z}) at file.cpp:8
  Expected _1 XXX
```
Where `XXX` is the output from the stream insertion operator.

### Duck-typed matcher

A duck-typed matcher accepts any type that matches a required set of
operations. Duck-typed matchers have a type conversion operator that
selects which types it can operate on. The conversion operator is
never implemented, but the signature must be available since it
is used at compile time to select overload.

An example of a duck-typed matcher is a `not_empty` matcher, requiring
that a `.empty()` member function of the parameter returns false.

First the restricting [SFINAE](http://en.wikipedia.org/wiki/Substitution_failure_is_not_an_error)
predicate used to match only types that has a `.empty()` member function.

```Cpp
  template <typename T>
  class has_empty
  {
    struct no;
    static no func(...);
    template <typename U>
    static auto func(U const* u) -> decltype(u->empty());
  public:
    static const bool value = !std::is_same<no, decltype(func(std::declval<T*>()))>::value;
  };
```

Here `has_empty<T>::value` is true only for types `T` that has a `.empty()`
member function callable on const objects.

```Cpp
  class not_empty : public trompeloeil::matcher
  {
  public:
    template <typename T,
              typename = typename std::enable_if<has_empty<T>::value>::type>
    operator T() const;            //1
    template <typename T>
    bool matches(T const& t) const //2
    {
      return !t.empty();
    }
    friend std::ostream& operator<<(std::ostream& os, not_empty const&)
    {
      return os << " is not empty";
    }
  };
```

At **//1** the type conversion operator selects for types that has a
`.empty()` member function.
[`std::enable_if<>`](http://en.cppreference.com/w/cpp/types/enable_if)
ensures that mismatching types generates a compilation error at the site of
use (**`REQUIRE_CALL()`**, **`ALLOW_CALL()`** or **`FORBID_CALL()`**.)

The `matches(T const&)` member function at **//2** becomes very simple. It
does not need the [SFINAE](http://en.wikipedia.org/wiki/Substitution_failure_is_not_an_error)
[`std::enable_if<>`](http://en.cppreference.com/w/cpp/types/enable_if) to select
valid types, since a type mismatch gives a compilation error on the
type conversion operator at **//1**.

The output stream insertion operator is neither more or less tricky than with
typed matchers. Making violation reports readable may require some thought,
however.

Compiler compatibility
----------------------

Trompeloeil is known to work with:

* GCC 4.9, 5.1
* Clang 3.5, 3.6
* VisualStudio 2015
