 ## *Trompeloeil*
<table frame="void" rules="none">
<tr><td>
![trompeloeil logo](trompeloeil-logo.png)
</td><td>
*trompe l'oeil* noun    (Concise Encyclopedia)  
Style of representation in which a painted object is intended
to deceive the viewer into believing it is the object itself...
</td></tr></table>
What is it?
-----------

A header only mocking framework for C++14.

**NOTE! This is very early days. Concider this wildly experimental where
everything is subject to change**

Example usage
-------------

```Cpp
#include "trompeloeil.hpp"

class I
{
public:
  virtual bool foo(int, std::string& s) = 0;
  virtual bool bar(int) = 0;
};

class CUT
{
public:
  CUT(I* i);
  std::string work(int n);
};

MOCK_CLASS(I)
{
public:
  MOCK(foo, (int, std::string&));
  MOCK(bar, (int));
};

TEST(work_returns_the_string_obtained_from_I_foo)
{
  using trompeloeil::_;

  mock<I> mock_i;
  CUT out(&mock_i);

  {
    trompeloeil::sequence seq;
    REQUIRE_CALL(mock_i, foo(3, _))
    .WITH(_2 == "")
    .TIMES(1)
    .IN_SEQUENCE(seq)
    .SIDE_EFFECT(_2 = "cat")
    .RETURN(true);

    auto s = out.work(3);

    ASSERT(s == "cat");
  }
}
```

Limitations (TODO-list)
-----------------------
- Overloaded methods cannot be mocked
- EXPECT_DESTRUCTION is not supported
- Reporting really needs more work
- Support reporting types lacking output stream insertion
- Allow custom test output stream insertion operator
- Tracing
- WAY too many macros...

How to use
----------
The example above shows all currently supported functionality

## Macros

**`MOCK_CLASS`(** *interface_name* **)**  
Define a MOCK implementation for the interface.

**`MOCK`(** *method_name*, *parameter_list* **)**  
Make a mock implementation of the method named *method_name*. *method_name*
must be virtual, and may currently not be overloded with several signatures.

**`REQUIRE_CALL`(** *mock_object*, *method_name*(*parameter_list*)**)**  
Set up an expectaion that the method *method_name* is called on the object
*mock_object*. The parameter list may include exact match values, or the
wildcard `trompeloeil::_`. The expectation has scope lifetime and must be
met by the time the scope ends. When there are several expectations active on
the same object and method, they are tried in the reversed order they were
defined, i.e. last defined is tried first, until a match is found. This allows
you to specify a wide default early, and narrow specializations in short
scopes.

**`NAMED_REQUIRE_CALL`(** *mock_object*, *method_name*(*parameter_list*)**)**
Same as **`REQUIRE_CALL`**, except it instantiates a
*std::unique_ptr&lt;trompeloeil::expectation&gt;* which you can bind to a variable.

**`.WITH`(** *expr* **)**  
Add further conditions for a **`REQUIRE_CALL`**, typically used when the
wildcard `trompeloeil::_` has been used. *expr* can refer to parameters in the
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
positional names `_1`, `_2`, etc. This code may alter out-parameters.

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


## Report to test frameworks

By default *`trompeloeil`* reports all violations as exceptions. This
works with most test frameworks, but since the error of not having matched
a **`REQUIRE_CALL`** is not discovered until the match object goes out of
scope and is destroyed, this error will call *`std::terminate()`*, which
is typically not what you want.

There is a function  
```Cpp
trompeloeil::set_reporter(std::function<void(trompeloeil::severity,
                                             const char* location
                                             const std::string& msg)>)
```
which can be used to control the reporting. `trompeloeil::severity` is an enum
with the values `fatal` and `nonfatal`. Severity is `nonfatal` when called
from the destructor of a **`REQUIRE_CALL`** object due to unfulfilled
expectations. Some examples are:

### catch
```Cpp
  trompeloeil::set_reporter([](::trompeloeil::severity s,
                               const char *loc,
                               const std::string& msg)
    {
      std::string m(loc);
      m+="\n";
      m+= msg;
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
                               const char *loc,
                               const std::string& msg)
    {
      ::crpcut::fixed_string location(loc, std::strlen(loc))
      ::crpcut::comm::report(::crpcut::comm::exit_fail,
                             location,
                             std::ostringstream(msg));
    });
```
