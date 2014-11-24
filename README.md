## *Trompeloeil*

![trompeloeil logo](trompeloeil-logo.png)  

> *trompe l'oeil* noun    (Concise Encyclopedia)  
> Style of representation in which a painted object is intended
> to deceive the viewer into believing it is the object itself...

What is it?
-----------

A header only mocking framework for C++14.

**NOTE! These are still early days. Bugs will be found, and important features
are missing. Code breaking changes are no longer expected, but may still
occur.**

**NOTE!! A code breaking change did just occur. MOCK(obj, params) and
MOCK_CONST(obj, params) are no longer. Instead use MAKE_MOCKn(obj, signature)
and MAKE_MOCK_CONSTn(obj, signature), where n is the number of parameters in the
signature. Check the example below, and the description of the macros.**

**NOTE!! Another code breaking change is in the pipe and is likely to appear
soon. Watch this space!**

Example usage
-------------

```Cpp
#include <trompeloeil.hpp>

class I
{
public:
  I(const char*);
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

class MI : public trompeloeil::mocked_class<I>
{
public:
  using mocked_class::mocked_class;
  MAKE_MOCK2(foo, bool(int, std::string&));
  MAKE_MOCK1(bar, bool(int));
  MAKE_MOCK1(bar, bool(std::string));
};

TEST(work_returns_the_string_obtained_from_I_foo_and_calls_I_bar)
{
  using trompeloeil::_; // wild card for matching any value

  auto raw_i = new MI("word");

  DEATHWATCH(*raw_i);

  CUT out(raw_i);


  trompeloeil::sequence seq1, seq2;

  {
    REQUIRE_CALL(*raw_i, bar(ANY(int)))
      .RETURN(_1 > 0)
      .IN_SEQUENCE(seq1)
      .TIMES(AT_LEAST(1));

    FORBID_CALL(*raw_i, bar(0))
      .RETURN(false);

    REQUIRE_CALL(*raw_i, bar("word"))
      .RETURN(true)
      .IN_SEQUENCE(seq2);

    REQUIRE_CALL(*raw_i, foo(3, _))
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

Limitations (TODO-list)
-----------------------
- Function templates cannot be mocked
- Tracing
- WAY too many macros... but I think we'll have to make do with most of them.

How to use
----------
The example above shows most currently supported functionality.

## Types & Templates

**`trompeloeil::sequence`**  
Type of sequence objects, used to impose an order of matching invocations of
**`REQUIRE_CALL`** instances. Several sequence objects can be used to denote
parallel sequences, and several sequence objects can be joined in one
**`REQUIRE_CALL`**.

**`trompeloeil::mocked_class<T>`**  
All mock objects inherit from an instantiation of **`mocked_class<>`**
of the type they mock. The template provides the necessary information for
the rest of the machinery to work. Template instantiations inherits all
constructors of `T`.

**`trompeloeil::wildcard`**  
The type of the wild card object `trompeloeil::_` . You typically never see
the type itself.

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
wild card `trompeloeil::_`, or the **`ANY`(** *type* **) wildcard. The
expectation has scope lifetime and must be met by the time the scope ends.
When there are several expectations active on the same object and method, they
are tried in the reversed order they were defined, i.e. last defined is tried
first, until a match is found. This allows you to specify a wide default early,
and narrow specializations in short scopes.

**`NAMED_REQUIRE_CALL`(** *mock_object*, *func_name*(*parameter_list*)**)**
Same as **`REQUIRE_CALL`**, except it instantiates a
*std::unique_ptr&lt;trompeloeil::expectation&gt;* which you can bind to a
variable.

**`DEATHWATCH`(** *mock_object* **)**  
Set up a deathwatch for *mock_object*, meaning its destruction is monitored.
It is an error if *mock_object* is destroyed before a
**`REQUIRE_DESTRUCTION`()** is active.

**`REQUIRE_DESTRUCTION`(** *mock_object* **)**  
Makes it legal for *mock_object* to be destroyed when a **`DEATHWATCH`**() is
active for it. It is an error if *mock_object* is still alive at end of scope.

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
**`REQUIRE_CALL`**

**`.SIDE_EFFECT`(** *expr* **)**  
Code to cause side effects. *expr* is only evaluated when all **`.WITH`**
clauses are matched. *expr* may refer to parameters in the call with their
positional names `_1`, `_2`, etc. This code may alter out-parameters.
Several **`.SIDE_EFFECT`** clauses can be added to a single **`REQUIRE_CALL`**

**`.RETURN`(** *expr* **)**  
Set the return value after having evaluated every **`.SIDE_EFFECT`** . For `void`
functions **`.RETURN`** is illegal. For non-`void` functions **`.RETURN`** is
required exactly once. *expr* may refer to parameters in the call with their
positional names `_1`, `_2`, etc. This code may alter out-parameters. If you
need to return an lvalue reference, use `std::ref()`.  It is not legal to
combine both **`.THROW`** and **`.RETURN`**.

**`.THROW`(** *expr* **)**  
Throw after having evaluated every **`.SIDE_EFFECT`** . *expr* may refer to
parameters in the call with their positional names `_1`, `_2`, etc. This code
may alter out-parameters. It is not legal to combine both **`.THROW`** and
**`.RETURN`**.

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
is 0..~0ULL, but that is for all practical purposes "infinity"_)

**`FORBID_CALL`(** *mock_object*, *method_name*(*parameter_list*)**)**  
Same as **`REQUIRE_CALL`**().**`TIMES`(** 0 **)**, making any matching call
an error.

**`NAMED_FORBID_CALL`(** *mock_object*, *method_name*(*parameter_list*)**)**  
Same as **`NAMED_REQUIRE_CALL`**().**`TIMES`(** 0 **)**, making any matching
call an error.

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
                                             const std::string& location
                                             const std::string& msg)>)
```
which can be used to control the reporting. `trompeloeil::severity` is an enum
with the values `fatal` and `nonfatal`. Severity is `nonfatal` when called
from the destructor of a **`REQUIRE_CALL`** object due to unfulfilled
expectations. Some examples are:

### catch
```Cpp
  trompeloeil::set_reporter([](::trompeloeil::severity s,
                               const std::string& loc,
                               const std::string& msg)
    {
      auto m = loc + "\n" + msg;
      if (s == ::trompeloeil::severity::fatal)
        {
          FAIL(m);
        }
      CHECK(m == "");
    });
```

### crpcut
```Cpp
  trompeloeil::set_reporter([](::trompeloeil::severity,
                               const std::string& loc,
                               const std::string& msg)
    {
      auto location = loc.empty()
        ? ::crpcut::crpcut_test_monitor::current_test()->get_location()
        : ::crpcut::datatypes::fixed_string{loc.c_str(), loc.length()};
      ::crpcut::comm::report(::crpcut::comm::exit_fail,
                             std::ostringstream(msg),
                             location);
    });
```

Compatibility
-------------

Trompeloeil is known to work with:

* GCC 4.9
* Clang 3.5
