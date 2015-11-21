### Reference

- [Notions]
  - [Mock function](#mock_function)
  - [Mock object](#mock_object)
  - [Expectation](#expectation)
  - [Matcher](#matcher)
    - [_](#wildcard)
    - [**`ANY(`** *type* **`)`**](#ANY)
    - [**`ne(`** *value* **`)`**](#ne)
    - [**`gt(`** *value* **`)`**](#gt)
    - [**`ge(`** *value* **`)`**](#ge)
    - [**`lt(`** *value* **`)`**](#lt)
    - [**`le(`** *value* **`)`**](#le)
- [Macros](#Macros) (alphabetical order)
  - [**`ALLOW_CALL(`** *mock_object*, *method_name*(*parameter_list*)**`)`**](#ALLOW_CALL)
  - [**`ANY(`** *type* **`)`**](#ANY_MACRO)
  - [**`AT_LEAST(`** *number* **`)`**](#AT_LEAST)
  - [**`AT_MOST(`** *number* **`)`**](#AT_MOST)
  - [**`FORBID_CALL(`** *mock_object*, *method_name*(*parameter_list*)**`)`**](#FORBID_CALL)
  - [**`IN_SEQUENCE(`** *seq...* **`)`**](#IN_SEQUENCE)
  - [**`LR_RETURN(`** *expr* **`)`**](#LR_RETURN)
  - [**`LR_SIDE_EFFECT(`** *expr* **`)`**](#LR_SIDE_EFFECT)
  - [**`LR_THROW(`** *expr* **`)`**](#LR_THROW)
  - [**`LR_WITH(`** *expr* **`)`**](#LR_WITH)
  - [**`MAKE_CONST_MOCKn(`** *method_name*, *signature* **`)`**](#MAKE_CONST_MOCKn)
  - [**`MAKE_MOCKn(`** *name*, *signature* **`)`**](#MAKE_MOCKn)
  - [**`NAMED_ALLOW_CALL(`** *mock_object*, *method_name*(*parameter_list*)**`)`**](#NAMED_ALLOW_CALL)
  - [**`NAMED_FORBID_CALL(`** *mock_object*, *method_name*(*parameter_list*)**`)`**](#NAMED_FORBID_CALL)
  - [**`NAMED_REQUIRE_CALL(`** *mock_object*, *func_name*(*parameter_list*)**`)`**](#NAMED_REQUIRE_CALL)
  - [**`NAMED_REQUIRE_DESTRUCTION(`** *mock_object* **`)`**](#NAMED_REQUIRE_DESTRUCTION)
  - [**`REQUIRE_CALL(`** *mock_object*, *func_name*(*parameter_list*)**`)`**](#REQUIRE_CALL)
  - [**`REQUIRE_DESTRUCTION(`** *mock_object* **`)`**](#REQUIRE_DESTRUCTION)
  - [**`RETURN(`** *expr* **`)`**](#RETURN)
  - [**`SIDE_EFFECT(`** *expr* **`)`**](#SIDE_EFFECT)
  - [**`THROW(`** *expr* **`)`**](#THROW)
  - [**`TIMES(`** *limit* **`)`**](#TIMES)
  - [**`WITH(`** *expr* **`)`**](#WITH)
  
## <A name="mock_function"/>Mock function

A member function that is mocked with
[**`MAKE_MOCKn(name, signature`**](#MAKE_MOCKn) or
[**`MAKE_CONST_MOCKn(name, signature`**](#MAKE_CONST_MOCKn).

Example:
```Cpp
class C
{
public:
  MAKE_MOCK1(func, void(int));
  MAKE_CONST_MOCK2(cfunc, int(std::string, int));
};
```

Above `C` is a type that has two mock functions `void func(int)` and
`int cfunc(std::string, int) const`. With a [mock object](#mock_object)
of type `C` it is possible to place [expectations](#expectation)
on the functions `func(...)` and `cfunc(...)`.


## <A name="mock_object"/>Mock object

A mock object is an object of a type that has [mock functions](#mock_function).

Example:
```Cpp
class C
{
public:
  MAKE_MOCK1(func, void(int));
};

C obj;
```

Above `obj` is a mock object. It is possible to place
[expectations](#expectation) on [mock functions](#mock_function) for the object
`obj`.

## <A name="expectation"/>Expectation

By default it is illegal to call [mock functions](#mock_function). Expectations
change that. Example:

```Cpp
class C
{
public:
  MAKE_MOCK1(func, void(int));
};

TEST(atest)
{
  C mock_obj;
  
  REQUIRE_CALL(mock_obj, func(3));
  
  tested_function(mock_obj);
}
```

Above `mock_obj` is a [mock object](#mock_object) with one
[mock function](#mock_function) `void func(int)`.

The line [*`REQUIRE_CALL(mock_obj, func(3))`*](#REQUIRE_CALL) places an
expectation that `mock_obj.func(3)` is called before the end of the scope.
Unless `tested_function(mock_obj)` calls `mock_obj.func(3)` a violation is
reported.

The ways to set expectations are:
- [**`REQUIRE_CALL(`** *mock_object*, *func_name*(*parameter_list*)**`)`**](#REQUIRE_CALL)
- [**`ALLOW_CALL(`** *mock_object*, *method_name*(*parameter_list*)**`)`**](#ALLOW_CALL)
- [**`FORBID_CALL(`** *mock_object*, *method_name*(*parameter_list*)**`)`**](#FORBID_CALL)
- [**`NAMED_REQUIRE_CALL(`** *mock_object*, *func_name*(*parameter_list*)**`)`**](#NAMED_REQUIRE_CALL)
- [**`NAMED_ALLOW_CALL(`** *mock_object*, *method_name*(*parameter_list*)**`)`**](#NAMED_ALLOW_CALL)
- [**`NAMED_FORBID_CALL(`** *mock_object*, *method_name*(*parameter_list*)**`)`**](#NAMED_FORBID_CALL)

The **NAMED** variants returns expectations as
`std::unique_ptr<trompeloeil::expectation>` which can be saved in variables
for storage in test fixtures or other programmatic lifetime control.

If expectations are not met by the time they go out or scope (or in case of the
**NAMED** variants, when the object held by the `std::unique_ptr<>` is
destroyed) a violation is reported.

By default there is no order imposed on expectations. One way to impose order is
through their lifetimes. Another is by using
[**`IN_SEQUENCE(...)`**](#IN_SEQUENCE)

If there are several expectations that matches the same call, they are tried in
the reverse order of creation, and the first found match is accepted. In other
words, the last created matching expectation is the one used.

Example:
```Cpp
class C
{
public:
  MAKE_MOCK1(func, void(int));
};

using trompeloeil::_;

TEST(atest)
{
  C mock_obj;
  
  ALLOW_CALL(mock_obj, func(_));
  FORBID_CALL(mock_obj, func(3));
  FORBID_CALL(mock_obj, func(4));
  
  tested_function(mock_obj);
}
```

Above, the first expectation [**`ALLOW_CALL(...)`**](#ALLOW_CALL) matches
everything using the wildcard [`trompeloeil::_`](#wildcard), but the two
[**`FORBID_CALL(...)`**](#FORBID_CALL) are created later and are thus matched
first. This means that if `tested_function(...)` calls `mock_obj.func(int)` with
`5`, the two [**`FORBID_CALL(...)`**](#FORBID_CALL) do not match, but the
[**`ALLOW_CALL(...)`**](#ALLOW_CALL) does, so the call is allowed. A call with 
`3` or `4`, results in a violation is report since a
[**`FORBID_CALL(...)`**](#FORBID_CALL) is matched.

## <A name="matcher"/>Matcher

Each parameter in the parameter list of an [expectation](#expectation) can be
an exact value to match for equality (using `operator==`,) or a matcher.
Matchers check a condition on the parameter value. Trompeloeil provides the
matchers

- [_](#wildcard)
- [**`ANY(`** *type* **`)`**](#ANY)
- [**`ne(`** *value* **`)`**](#ne)
- [**`gt(`** *value* **`)`**](#gt)
- [**`ge(`** *value* **`)`**](#ge)
- [**`lt(`** *value* **`)`**](#lt)
- [**`le(`** *value* **`)`**](#le)

You can also provide your own matchers.


### <A name="wildcard"/>**`_`**

Used in the parameter list of an [expectation](#expectation) `trompeloeil::_`
matches any value of any type.

Example:
```Cpp
class C
{
public:
  MAKE_MOCK1(func, int(int));
};

trompeloeil::_;

TEST(atest)
{
  C mock_obj;
  ALLOW_CALL(mock_obj, func(_))
    .RETURN(_1 + 1);
    
  test_function(&mock_obj);
}
```

Above, `mock_obj.func()` is allowed to be called with any value, and it will
return 1 + the value provided.

If type is needed, for example to disambiguate overloads, use
[**`ANY(`** *type* **`)`**](#ANY).

### <A name="ANY"/>**`ANY(`** *type* **`)`**

Used in the parameter list of an [expectation](#expectation) to match any value
of a specified type. This can be used as an alternative to
[`trompeloeil::_`](#wildcard) when it is important to disambiguate between
overloads.

Example:
```Cpp
class C
{
public:
  MAKE_MOCK1(func, void(int));
  MAKE_MOCK2(func, void(std::string));
};

TEST(atest)
{
  C mock_obj;
  ALLOW_CALL(mock_obj, func(ANY(int)));
    
  test_function(&mock_obj);
}
```

Above, any call to `mock_obj.func(int)` is accepted, but calls to
`mock_obj.func(std::string)` renders a violation report since there is no
matching [expectation](#expectation).

### <A name="ne"/>**`ne(`** *value* **`)`**

Used in the parameter list of an [expectation](#expectation) to match a
value not equal to the one provided.

Example:
```Cpp
class C
{
public:
  MAKE_MOCK1(func, void(const char*));
};

using trompeloeil::ne;

TEST(atest)
{
  C mock_obj;
  ALLOW_CALL(mock_obj, func(ne(nullptr)));
    
  test_function(&mock_obj);
}
```

Above, the [expectation](#expectation) matches only calls to
`mock_obj.func(const char*)` with non-null pointer. Any call with a `nullptr`
renders a violation report since no [expectation](#expectation) matches.

If needed for disambiguation, it is possible to explicitly state the type
of *value*, like `ne<const char*>(nullptr)`.

### <A name="gt"/>**`gt(`** *value* **`)`**

Used in the parameter list of an [expectation](#expectation) to match a
value greater than the one provided.

Example:
```Cpp
class C
{
public:
  MAKE_MOCK1(func, void(int));
};

using trompeloeil::gt;

TEST(atest)
{
  C mock_obj;
  ALLOW_CALL(mock_obj, func(gt(0)));
    
  test_function(&mock_obj);
}
```

Above, the [expectation](#expectation) matches only calls to
`mock_obj.func(int)` with positive values. Any call with 0 or negative
renders a violation report since no [expectation](#expectation) matches.

If needed for disambiguation, it is possible to explicitly state the type
of *value*, like `gt<int>(0)`.


### <A name="ge"/>**`ge(`** *value* **`)`**

Used in the parameter list of an [expectation](#expectation) to match a
value greater than on equal to the one provided.

Example:
```Cpp
class C
{
public:
  MAKE_MOCK1(func, void(int));
};

using trompeloeil::ge;

TEST(atest)
{
  C mock_obj;
  ALLOW_CALL(mock_obj, func(ge(0)));
    
  test_function(&mock_obj);
}
```

Above, the [expectation](#expectation) matches only calls to
`mock_obj.func(int)` with zero or positive values. Any call with a negative
value renders a violation report since no [expectation](#expectation) matches.

If needed for disambiguation, it is possible to explicitly state the type
of *value*, like `ge<int>(0)`.

### <A name="lt"/>**`lt(`** *value* **`)`**

Used in the parameter list of an [expectation](#expectation) to match a
value less than the one provided.

Example:
```Cpp
class C
{
public:
  MAKE_MOCK1(func, void(int));
};

using trompeloeil::lt;

TEST(atest)
{
  C mock_obj;
  ALLOW_CALL(mock_obj, func(lt(0)));
    
  test_function(&mock_obj);
}
```

Above, the [expectation](#expectation) matches only calls to
`mock_obj.func(int)` with negative values. Any call with 0 or positive
renders a violation report since no [expectation](#expectation) matches.

If needed for disambiguation, it is possible to explicitly state the type
of *value*, like `lt<int>(0)`.

### <A name="le"/>**`le(`** *value* **`)`**

Used in the parameter list of an [expectation](#expectation) to match a
value less than on equal to the one provided.

Example:
```Cpp
class C
{
public:
  MAKE_MOCK1(func, void(int));
};

using trompeloeil::le;

TEST(atest)
{
  C mock_obj;
  ALLOW_CALL(mock_obj, func(le(0)));
    
  test_function(&mock_obj);
}
```

Above, the [expectation](#expectation) matches only calls to
`mock_obj.func(int)` with zero or negative values. Any call with a positive
value renders a violation report since no [expectation](#expectation) matches.

If needed for disambiguation, it is possible to explicitly state the type
of *value*, like `le<int>(0)`.

## Macros

<A name="ALLOW_CALL"/>
#### **`ALLOW_CALL(`** *mock_object*, *method_name*(*parameter_list*)**`)`**  
Make an expectation that *mock_object*.*method_name*(*parameter_list*) may be
called zero or more times until the end of the surrounding scope.
*parameter_list* may contain exact values or [matchers](#matcher)
that describes matching calls.

This is the same as
[**`REQUIRE_CALL(...)`**](#REQUIRE_CALL).[**`TIMES(`**](#TIMES) 0,infinity **`)`**.

Matches any number of times, but is not required to match. (_actually the limit is
0..~0ULL, but that is for all practical purposes "infinity"_)

Example:
```Cpp
class C
{
public:
  MAKE_MOCK1(func, int(int));
};

trompeloeil::_;

TEST(atest)
{
  C mock_obj;
  ALLOW_CALL(mock_obj, func(_))
    .RETURN(_1 + 1);
    
  test_function(&mock_obj);
}
```

Above **`ALLOW_CALL(mock_obj, func(_))`** places an expectation that
`mock_obj.func()` may be called any number of times with any parameter value
and will always return the parameter value + 1. `test_function(...)`
is allowed to call `mock_obj.func()` any number of times (including no call at
all.) 

The expectation is valid until the end of the scope, which in the example above
is until after the return from `test_function(...)`.

See also [**`NAMED_ALLOW_CALL(...)`**](#NAMED_ALLOW_CALL) which creates an
expectation as a `std::unique_ptr<trompeloeil::expectation>` which can be stored
in test fixtures or otherwise have its lifetime programmatically controlled.

<A name="ANY_MACRO"/>
#### **`ANY(`** *type* **`)`**  

A [matcher](#matcher) for use in the parameter list of an
[expectation](#expectation) to disambiguate overloaded functions on type when
the exact value is unimportant. See the matcher [**`ANY(`** *type* **`)`**](#ANY) above.

<A name="AT_LEAST"/>
#### **`AT_LEAST(`** *number* **`)`**  
Used in [**`TIMES(...)`**](#TIMES) to set the range *number*..infinity.
*number* must be `constexpr`.

Example:

```Cpp
class C
{
public:
  MAKE_MOCK1(func, void(int));
};

using trompeloeil::_;

TEST(atest)
{
  C mock_obj;
  REQUIRE_CALL(mock_obj, func(_))
    .TIMES(AT_LEAST(3));
  tested_function(&mock_obj);
}
```

Above, the line [**`TIMES(`**](#TIMES)**`AT_LEAST(3))`** modifies the
[expectation](#expectation) such that *mock_obj.func()* must be called 3 times
or more, before the end of the scope, or a violation is reported.

_In reality the upper limit is ~0ULL, but that is for all practical purposes
"infinity"_.

<A name="AT_MOST"/>
#### **`AT_MOST(`** *number* **`)`**  
Used in [**`TIMES(...)`**](#TIMES) to set the range 0..*number*.
*number* must be `constexpr`.
Example:

```Cpp
class C
{
public:
  MAKE_MOCK1(func, void(int));
};

using trompeloeil::_;

TEST(atest)
{
  C mock_obj;
  REQUIRE_CALL(mock_obj, func(_))
    .TIMES(AT_MOST(3));
  tested_function(&mock_obj);
}
```

Above, the line [**`TIMES(`**](#TIMES)**`AT_MOST(3))`** modifies the
[expectation](#expectation) such that *mock_obj.func()* must be called 3 times
or less (including no call at all,) before the end of the scope, or a violation
is reported.


<A name="FORBID_CALL"/>
#### **`FORBID_CALL(`** *mock_object*, *method_name*(*parameter_list*)**`)`**  
Make an expectation that *mock_object*.*method_name*(*parameter_list*) must not
be called until the end of the scope. *parameter_list* may contain exact values
or [matchers](#matcher) that describes matching calls.


This is the same as
[**`REQUIRE_CALL(...)`**](#REQUIRE_CALL).[**`TIMES(`**](#TIMES) 0 **`)`**,
making any matching call an error. This is often done in a narrow scope
where the wider scope would allow the call. [**`RETURN(...)`**](#RETURN) and
[**`THROW(...)`**](#THROW) are illegal in a **`FORBID_CALL(...)`**.

Example:
```Cpp
class C
{
public:
  MAKE_MOCK1(func, void(int));
};

using trompeloeil::_;

TEST(atest)
{
  C mock_obj;
  ALLOW_CALL(mock_obj, func(_));
  
  tested_function(1, &mock_obj);
  
  {
    FORBID_CALL(mock_obj, func(2));
    
    tested_function(2, &mock_obj);
  }
  
  tested_function(3, &mock_obj);
}
```

Above, the [mock function](#mock_function) *C::func(int)* may be called with any
value for *mock_obj*, except in the scope of the *tested_function(2, &amp;mock_obj)*,
where *mock_obj.func(2)* would lead to a violation being reported. At
*tested_function(3, &amp;mock_obj)* any value is allowed again.

See also [**`NAMED_FORBID_CALL(...)`**](#NAMED_FORBID_CALL) which creates an
expectation as a `std::unique_ptr<trompeloeil::expectation>` which can be
stored in test fixtures.

<A name="IN_SEQUENCE"/>
#### **`IN_SEQUENCE(`** *seq...* **`)`**  
Where *seq...* is one or more instances of `trompeloeil::sequence`. Impose an
order in which [expectations](#expectation) must match.
Several sequences can be parallel and interleaved. A sequence for an 
[expectation](#expectation) is no longer monitored
once the lower limit from [**`TIMES(...)`**](#TIMES) is reached.

Example:
```Cpp
class C
{
public:
  MAKE_MOCK1(func, void(int));
  MAKE_MOCK1(func, void(const std::string&));
};

TEST(atest)
{
  C mock_obj[2];
  
  trompeloeil::sequence seq1, seq2;
  
  REQUIRE_CALL(mock_obj[0], func(ANY(int))
    .IN_SEQUENCE(seq1, seq2);
    
  REQUIRE_CALL(mock_obj[0], func(ANY(const std::string&))
    .IN_SEQUENCE(seq1);
    
  REQUIRE_CALL(mock_obj[1], func(ANY(const std::string&))
    .IN_SEQUENCE(seq2);
    
  REQUIRE_CALL(mock_obj[1], func(ANY(int))
    .IN_SEQUENCE(seq1, seq2);
  
  tested_func(&mock_obj[0], &mock_obj[1]);
}
```

All sequence objects are listed in the first [**`REQUIRE_CALL(...)`**](#REQUIRE_CALL),
thus it must be the first. Likewise all sequences are listed in the last
[**`REQUIRE_CALL(...)`**](#REQUIRE_CALL), so it must be last. The intermediate
[expectations](#expectation) has one sequence object each, thus they have no
order imposed between them.

The above allows the following two sequences only.
- *mock_obj[0].func(int)* -> *mock_obj[0].func(string)* -> *mock_obj[1].func(int)*
- *mock_obj[0].func(int)* -> *mock_obj[1].func(string)* -> *mock_obj[1].func(int)*

Any other sequence of calls renders a violation report.

<A name="LR_RETURN"/>
#### **`LR_RETURN(`** *expr* **`)`**  
Used in [expectations](#expectation) to set the return value after having
evaluated every [**`SIDE_EFFECT(...)`**](#SIDE_EFFECT) and
[**`LR_SIDE_EFFECT(...)`**](#LR_SIDE_EFFECT).
For `void` functions **`LR_RETURN(...)`** is illegal. For non-`void` functions
exactly one of [**`RETURN(...)`**](#RETURN), **`LR_RETURN(...)`**,
[**`LR_THROW(...)`**](#LR_THROW) or [**`THROW(...)`**](#THROW) is required.
*expr* may refer to parameters in the call with their positional names `_1`,
`_2`, etc.
This code may alter out-parameters. If you need to return an lvalue reference,
use `std::ref()`.

**NOTE!** Any named local objects named in *expr* are captured by reference so
lifetime management is important.

Example:
```Cpp
class C
{
public:
  MAKE_MOCK1(func, int&(unsigned));
};

using trompeloeil::_;

std::vector<int> values{3,2,1,0};

TEST(atest)
{
  C mock_obj;

  int offset = 1;
  ALLOW_CALL(mock_obj, func(_))
    .LR_WITH(_1 + offset < values.size())
    .LR_RETURN(std::ref(values[_1 + offset]));

  offset = 2;
  test_func(&mock_obj);
}
```

Above, the **`LR_RETURN(...)`** clause tells matching calls of
`mock_obj.func(...)` to return a reference to an element in the global
`std::vector<int> values`. Since **`LR_RETURN(...)`** accesses local variables
by reference, the value of `offset` is 2 in the index calculation if called
from within `test_func(...)`.

See also [**`RETURN(...)`**](#RETURN) which accesses copies of local variables.

<A name="LR_SIDE_EFFECT"/>
#### **`LR_SIDE_EFFECT(`** *expr* **`)`**  
Used in [expectations](#expectation) to cause side effects for matching calls.
*expr* is only evaluated when all [**`WITH(...)`**](#WITH) and
[**`LR_WITH(...)`**](#LR_WITH) clauses are matched. *expr* may refer to
parameters in the call with their positional names `_1`, `_2`, etc. This code
may alter out-parameters. Several **`LR_SIDE_EFFECT(...)`** and
[**`SIDE_EFFECT(...)`**](#SIDE_EFFECT)
clauses can be added to a single [expectation](#expectation).

See also [**`SIDE_EFFECT(...)`**](#SIDE_EFFECT) which accesses copies of local
objects.

Example:
```Cpp
class C
{
public:
  MAKE_MOCK1(func, void(unsigned));
};

TEST(atest)
{
  C mock_obj;
  unsigned sum = 0;
  ALLOW_CALL(mock_obj, func(ANY(unsigned))
    .LR_SIDE_EFFECT(sum += _1);
    
  tested_func(&mock_obj);
  
  std::cout << "parameter sum=" << sum << "\n";
}
```

Above, *tested_func(&amp;mock_obj)* is allowed to call *C::func(int)* any
number of times on *mock_obj*. Each time a side effect is that the local
variable *sum* gets the parameter value added to it. Since
**`LR_SIDE_EFFECT(...)`** refers to *sum* by reference, it is the actual
local variable that is changed is every call.

<A name="LR_THROW"/>
#### **`LR_THROW(`** *expr* **`)`**  
Used in [expectations](#expectation) to throw after having evaluated every
[**`SIDE_EFFECT(...)`**](#SIDE_EFFECT) and
[**`LR_SIDE_EFFECT(...)`**](#LR_SIDE_EFFECT) for a matching call.
*expr* may refer to parameters in the call with their positional names `_1`,
`_2`, etc. This code may alter out-parameters. It is not legal to combine
**`LR_THROW(...)`** with any of [**`THROW(...)`**](#THROW),
[**`LR_RETURN(...)`**](#LR_RETURN) or [**`RETURN(...)`**](#RETURN). Named local
objects are accessed by reference so lifetime management is important.

Example:
```Cpp
class C
{
public:
  MAKE_MOCK1(func, void(unsigned));
};

TEST(atest)
{
  C mock_obj;
  const char* what="";
  
  ALLOW_CALL(mock_obj, func(3))
    .LR_THROW(std::invalid_argument(what));
  
  what = "nonsense";
  tested_func(&mock_obj);
}
```

Above, **`LR_THROW(std::invalid_argument(what))`** will refer to the c-string
*`what`* with the value it has at the time of a call to `mock_obj.func(3)`, i.e.
`"nonsense"` if `tested_func()` does the call.

See also [**`THROW(...)`**](#THROW) which accesses copies of local objects.

<A name="LR_WITH"/>
#### **`LR_WITH(`** *expr* **`)`**
Used with [expectations](#expectation) to add further conditions for a
matching call. Typically used when [matchers](#matcher) are used for the
parameters, and often when the condition requires several parameter values
together.
*expr* can refer to parameters in the call with their positional names `_1`,
`_2`, etc. Even if the function signature has parameters as non-`const`
references, they are immutable in this context. Several **`LR_WITH(...)`**
and [**`WITH(...)`**](#WITH) clauses can be added to a single expectation.

Named local objects are accessed by reference so lifetime management is
important.

Example:
```Cpp
class C
{
public:
  MAKE_MOCK1(func, void(const char*));
};

using trompeloeil::_;

TEST(atest)
{
  C mock_obj;
  
  const char buff[] = "string";
  
  REQUIRE_CALL(mock_obj, func(_))
    .LR_WITH(_1 == buff);
  
  tested_func(buff, &mock_obj);
}
```
Above, **`LR_WITH(_1 == buff)`** checks the condition that the `const char*`
parameter is the same pointer value as the address to the local array `buff`.

**NOTE!** It is legal, but a *very* bad idea, to modify global/static objects in
**`LR_WITH(...)`**. If several [expectations](#expectation) could match and
are disambiguated by **`LR_WITH(...)`** and [**`WITH(...)`**](#WITH) the
global/static objects will be modified also by those
[expectations](#expectation) that do not match.

See also [**`WITH(...)`**](#WITH) which accesses copies of local objects.

<A name="MAKE_CONST_MOCKn"/>
#### **`MAKE_CONST_MOCKn(`** *method_name*, *signature* **`)`**  
Make a `const` [mock function](#mock_function) named *method_name*. It is a good
idea for this to implement a pure virtual function from an interface, but
it is not a requirement. `n` is the number of parameters in *signature*.

Example:
```Cpp
class C
{
public:
  MAKE_CONST_MOCK2(func, int(int, const std::vector<int>&));
};
```

Above, class `C` will effectively become:
```Cpp
class C
{
public:
  int func(int, const std::vector<int>&) const;
};
```

It is not possible to mock operators, constructors or the destructor, but
you can call [mock functions](#mock_function) from those.

See also [**`MAKE_MOCKn(...)`**](#MAKE_MOCKn) for non-`const`
member functions.

<A name="MAKE_MOCKn"/>
#### **`MAKE_MOCKn(`** *name*, *signature* **`)`**  
Make a non-`const` [mock function](#mock_function) named *method_name*. It is a
good idea for this to implement a pure virtual function from an interface, but
it is not a requirement. `n` is the number of parameters in *signature*.

Example:
```Cpp
class C
{
public:
  MAKE_MOCK2(func, int(int, const std::vector<int>&));
};
```

Above, class `C` will effectively become:
```Cpp
class C
{
public:
  int func(int, const std::vector<int>&);
};
```

It is not possible to mock operators, constructors or the destructor, but
you can call [mock functions](#mock_function) from those.

See also [**`MAKE_CONST_MOCKn(...)`**](#MAKE_MOCKn) for `const`
member functions.

<A name="NAMED_ALLOW_CALL"/>
#### **`NAMED_ALLOW_CALL(`** *mock_object*, *method_name*(*parameter_list*)**`)`**  
Make a `std::unique_ptr<trompeloeil::expectation>` allowing
*mock_object*.*method_name*(*parameter_list*) to be
called zero or more times until the expectation object is destroyed.
*parameter_list* may contain exact values or [matchers](#matcher)
that describes matching calls.

This is the same as
[**`NAMED_REQUIRE_CALL(...)`**](#NAMED_REQUIRE_CALL).[**`TIMES(`**](#TIMES) 0,infinity **`)`**.

Matches any number of times, but is not required to match. (_actually the limit is
0..~0ULL, but that is for all practical purposes "infinity"_)

**NOTE!** Any named local objects referenced in attached
[**`LR_WITH(...)`**](#LR_WITH), [**`LR_SIDE_EFFECT(...)`**](#LR_SIDE_EFFECT),
[**`LR_RETURN(...)`**](#LR_RETURN) and [**`LR_THROW(...)`**](#LR_THROW) are
captured by reference so lifetime management is important.

Example:
```Cpp
class C
{
public:
  MAKE_MOCK1(func, void(int));
};

using trompeloeil::gt;
using trompeloeil::lt;
using expectation = std::unique_ptr<trompeloeil::expectation>;

TEST(atest)
{
  C mock_obj;

  expectation x1 = NAMED_ALLOW_CALL(mock_obj, func(gt(0));
  
  test_function(0, &mock_obj);
  
  expectation x2 = NAMED_ALLOW_CALL(mock_obj, func(lt(0));
  
  test_function(1, &mock_obj);

  x1.reset(); // no longer allow calls with positive values

  test_function(2, &mock_obj);
}
```

Above each **`NAMED_ALLOW_CALL(mock_obj, func(...))`** creates an expectation
that *mock_obj.func()* may be called any number of times. Each expectation is
valid for as long at the expectation object is alive. In the example above,
this means that `x1` is valid for the first two calls to `test_function(...)`,
while `x2` is valid for the last two calls to `test_function(...)`.

See also [**`ALLOW_CALL(...)`**](#ALLOW_CALL) which creates an expectation
that is valid until the end of the surrounding scope.


<A name="NAMED_FORBID_CALL"/>
#### **`NAMED_FORBID_CALL(`** *mock_object*, *method_name*(*parameter_list*)**`)`**  
Make a `std::unique_ptr<trompeloeil::expectation>` disallowing calls to
*mock_object*.*method_name*(*parameter_list*) until the expectation object is
destroyed. *parameter_list* may contain exact values or [matchers](#matcher)
that describes matching calls.


This is the same as
[**`NAMED_REQUIRE_CALL(...)`**](#NAMED_REQUIRE_CALL).[**`TIMES(`**](#TIMES)
0 **`)`**, making any matching call an error. This is typically done when a wider
scope would allow the call. [**`RETURN(...)`**](#RETURN) and
[**`THROW(...)`**](#THROW) are illegal in a **`NAMED_FORBID_CALL(...)`**.

**NOTE!** Any named local objects referenced in attached
[**`LR_WITH(...)`**](#LR_WITH) are captured by reference so lifetime management
is important.

Example:
```Cpp
class C
{
public:
  MAKE_MOCK1(func, void(int));
}

using trompeloeil::_;
using trompeloeil::gt;
using trompeloeil::lt;

using expectation = std::unique_ptr<trompeloeil::expectation>;

TEST(atest)
{
  C mock_obj;
  
  ALLOW_CALL(mock_obj, func(_));
  
  expectation x1 = NAMED_FORBID_CALL(mock_obj, func(gt(0));
  
  test_function(0, &mock_obj);
  
  expectation x2 = NAMED_FORBID_CALL(mock_obj, func(lt(0));
  
  test_function(1, &mock_obj);
  
  x1.reset(); // allow calls with positive values again
  
  test_function(2, &mock_obj);
}
```

Above, calls to `mock_obj.func()` are generally allowed throughout the test.
However, `x1` imposes a restriction that calls with positive values are illegal,
and that restriction is in place for the first two calls to
`test_function(...)`. `x2` imposes a restrictions that calls with negative
values are illegal, and that restriction is in place for the last two calls to
`test_function(...)`.

See also [**`FORBID_CALL(...)`**](#FORBID_CALL) which creates an
[expectation](#expectation) that is valid until the end of the surrounding scope.

<A name="NAMED_REQUIRE_CALL"/>
#### **`NAMED_REQUIRE_CALL(`** *mock_object*, *func_name*(*parameter_list*)**`)`**  
Make a `std::unique_ptr<trompeloeil::expectation>` requiring that
*mock_obj*.*func_name*(*parameter_list*) is called exactly once before
the expectation object is destroyed. *parameter_list* may contain exact values
or [matchers](#matcher) that describes matching calls.

The number of matches required before the [expectation](#expectation) object
is destroyed can be changed with an optional [**`TIMES(...)`**](#TIMES) clause.

**NOTE!** Any named local objects referenced in attached
[**`LR_WITH(...)`**](#LR_WITH), [**`LR_SIDE_EFFECT(...)`**](#LR_SIDE_EFFECT),
[**`LR_RETURN(...)`**](#LR_RETURN) and [**`LR_THROW(...)`**](#LR_THROW) are
captured by reference so lifetime management is important.

Example:
```Cpp
class C
{
public:
  MAKE_MOCK1(func, void(int));
}

using trompeloeil::gt;
using trompeloeil::lt;

using expectation = std::unique_ptr<trompeloeil::expectation>;

TEST(atest)
{
  C mock_obj;
  
  expectation x1 = NAMED_REQUIRE_CALL(mock_obj, func(gt(0));
  
  test_function(0, &mock_obj);
  
  expectation x2 = NAMED_REQUIRE_CALL(mock_obj, func(lt(0));
  
  test_function(1, &mock_obj);
  
  x1.reset(); // The call with positive number must be done here.
  
  test_function(2, &mock_obj);
}
```

Above, the first two calls to `test_function(...)` must together call
`mock_obj.func(...)` exactly once with a positive value, and the last two
calls to `test_function(...)` must together call `mock_obj.func(...)`
exactly once with a negative number.

See also [**`REQUIRE_CALL(...)`**](#REQUIRE_CALL) which creates an
[expectation](#expectation) that is valid until the end of the surrounding scope.

<A name="NAMED_REQUIRE_DESTRUCTION"/>
#### **`NAMED_REQUIRE_DESTRUCTION(`** *mock_object* **`)`**
Create a `std::unique_ptr<trompeloeil::lifetime_monitor>` object which
reports a violation if the [**`deathwatched`**](#deathwatched) [mock object](#mock_object)
is not destroyed by the time the `lifetime_monitor` is destroyed.

Example:
```Cpp
class C
{
public:
  virtual ~C() = default; // must be virtual for deathwatched
  MAKE_MOCK1(func, void(int));
}

using monitor = std::unique_ptr<trompeloeil::lifetime_monitor>;
using trompeloeil::deathwatched;

TEST(atest)
{
  C* p = new deathwatched<C>();
  
  test_function(0, p); // must not destroy *p
  
  monitor m = NAMED_REQUIRE_DESTRUCTION(*p);
  
  test_function(1, p);
  
  m.reset(); // *p must have been destroyed here
}
```

Above, `p` points to a `deathwatched` [mock object](#mock_object), meaning that
a violation is reported if `*p` is destroyed without having a
destruction requirement.

The monitor `m` is a requirement that `*p` is destroyed before the 
`lifetime_monitor` held by `m` is destroyed.

It is thus a violation if the first call to `test_function(...)` destroys
`*p`, and another violation if the second call to `test_function(...)`
does not destroy `*p`

See also [**`REQUIRE_DESTRUCTION(...)`**](#REQUIRE_DESTRUCTION) which places
a requirement that the `deathwatched` [mock object](#mock_object) is destroyed
before the end of the scope.

<A name="REQUIRE_CALL"/>
#### **`REQUIRE_CALL(`** *mock_object*, *func_name*(*parameter_list*)**`)`**  
Make an [expectation](#expectation) requiring that
*mock_obj*.*func_name*(*parameter_list*) is called exactly once before
the end of the scope. *parameter_list* may contain exact values
or [matchers](#matcher) that describes matching parameter values for the
[expectation](#expectation).

The number of matches required before the [expectation](#expectation) object
is destroyed can be changed with an optional [**`TIMES(...)`**](#TIMES) clause.

Example:
```Cpp
class C
{
public:
  MAKE_MOCK1(func, void(int));
}

using trompeloeil::gt;
using trompeloeil::lt;

TEST(atest)
{
  C mock_obj;
  {  
    REQUIRE_CALL(mock_obj, func(gt(0));
  
    test_function(0, &mock_obj);
    // end of scope, requirement must be fulfilled here
  }
  {
    REQUIRE_CALL(mock_obj, func(lt(0));
  
    test_function(1, &mock_obj);
    // end of scope, requirement must be fulfilled here
  }
}
```

Above, the first call to `test_function(...)` must call
`mock_obj.func(...)` exactly once with a positive value, and the second
call to `test_function(...)` must call `mock_obj.func(...)`
exactly once with a negative number.

See also [**`NAMED_REQUIRE_CALL(...)`**](#NAMED_REQUIRE_CALL) which creates an
[expectation](#expectation) that is held by a
`std::unique_ptr<trompeloeil::expectation>` which can be stored in test
fixtures.


<A name="REQUIRE_DESTRUCTION"/>
#### **`REQUIRE_DESTRUCTION(`** *mock_object* **`)`**  
Create an anonymous `lifetime_monitor` which reports a violation if the
[**`deathwatched`**](#deathwatched) [mock object](#mock_object) is not destroyed
by the end of the scope.

Example:
```Cpp
class C
{
public:
  virtual ~C() = default; // must be virtual for deathwatched
  MAKE_MOCK1(func, void(int));
}

using trompeloeil::deathwatched;

TEST(atest)
{
  C* p = new deathwatched<C>();
  
  test_function(0, p); // must not destroy *p
  
  {
    REQUIRE_DESTRUCTION(*p);
  
    test_function(1, p);
    // end of scope, *p must have been destroyed here
  }
}
```

Above, `p` points to a `deathwatched` [mock object](#mock_object), meaning that
a violation is reported if `*p` is destroyed without having a
destruction requirement.

[**`REQUIRE_DESTRUCTION(...)`**] in the local scope puts a requirement on
`*p` that it must be destroyed by the end of the scope.

It is thus a violation if the first call to `test_function(...)` destroys
`*p`, and another violation if the second call to `test_function(...)`
does not destroy `*p`

See also [**`NAMED_REQUIRE_DESTRUCTION(...)`**](#NAMED_REQUIRE_DESTRUCTION)
which creates the requirement that the `deathwatched` [mock object](#mock_object)
is destroyed as a `std::unique_ptr<trompeloeil::lifetime_monitor>` which can
be stored in test fixtures.

<A name="RETURN"/>
#### **`RETURN(`** *expr* **`)`**  
Used in [expectations](#expectation) to set the return value after having
evaluated every [**`SIDE_EFFECT(...)`**](#SIDE_EFFECT) and
[**`LR_SIDE_EFFECT(...)`**](#LR_SIDE_EFFECT).
For `void` functions **`RETURN(...)`** is illegal. For non-`void` functions
exactly one of [**`LR_RETURN(...)`**](#LR_RETURN), **`RETURN(...)`**,
[**`LR_THROW(...)`**](#LR_THROW) or [**`THROW(...)`**](#THROW) is required.
*expr* may refer to parameters in the call with their positional names `_1`,
`_2`, etc.
This code may alter out-parameters. If you need to return an lvalue reference,
use `std::ref()`.

Named local objects accessed here refers to a copy.

Example:
```Cpp
class C
{
public:
  MAKE_MOCK1(func, int&(unsigned));
};

using trompeloeil::_;

std::vector<int> values{3,2,1,0};

TEST(atest)
{
  C mock_obj;

  int offset = 1;
  ALLOW_CALL(mock_obj, func(_))
    .WITH(_1 + offset < values.size())
    .RETURN(std::ref(values[_1 + offset]));

  offset = 2;
  test_func(&mock_obj);
}
```

Above, the **`RETURN(...)`** clause tells matching calls of
`mock_obj.func(...)` to return a reference to an element in the global
`std::vector<int> values`. Since **`RETURN(...)`** accesses copies of local
variables, the value of `offset` is 1 in the index calculation if called from
within `test_func(...)`.

See also [**`LR_RETURN(...)`**](#LR_RETURN) which accesses local variables
by reference.

<A name="SIDE_EFFECT"/>
#### **`SIDE_EFFECT(`** *expr* **`)`**
Used in [expectations](#expectation) to cause side effects for matching calls.
*expr* is only evaluated when all [**`WITH(...)`**](#WITH) and
[**`LR_WITH(...)`**](#LR_WITH) clauses are matched. *expr* may refer to
parameters in the call with their positional names `_1`, `_2`, etc. This code
may alter out-parameters.
Several **`SIDE_EFFECT(...)`** and [**`LR_SIDE_EFFECT(...)`**](#LR_SIDE_EFFECT)
clauses can be added to a single [expectation](#expectation).

Named local objects accessed here refers to a copy.

Example:

```Cpp
class C
{
public:
  MAKE_MOCK1(func, void(unsigned));
};

unsigned sum = 0;

TEST(atest)
{
  C mock_obj;
  unsigned offset = 0;
  ALLOW_CALL(mock_obj, func(ANY(unsigned))
    .SIDE_EFFECT(sum += offset + _1);
    
  offset = 2;
  tested_func(&mock_obj);
  
  std::cout << "offset corrected parameter sum=" << sum << "\n";
}
```

Above, `tested_func(...)` is allowed to call `mock_obj.func()` any
number of times. Each time a side effect is that the global
variable `sum` gets the parameter value added to it adjusted for `offset`.
Since **`SIDE_EFFECT(...)`** refers to a copy of `offset`, the value of
`offset` is `0` in any matching calls from within `tested_func(...)`

See also [**`LR_SIDE_EFFECT(...)`**](#LR_SIDE_EFFECT) which accesses local
objects by reference.


<A name="THROW"/>
#### **`THROW(`** *expr* **`)`**  
Used in [expectations](#expectation) to throw after having evaluated every
[**`SIDE_EFFECT(...)`**](#SIDE_EFFECT) and
[**`LR_SIDE_EFFECT(...)`**](#LR_SIDE_EFFECT) for a matching call.
*expr* may refer to parameters in the call with their positional names `_1`,
`_2`, etc. This code may alter out-parameters. It is not legal to combine
**`LR_THROW(...)`** with any of [**`THROW(...)`**](#THROW),
[**`LR_RETURN(...)`**](#LR_RETURN) or [**`RETURN(...)`**](#RETURN).
 
Named local objects here refers to a copy.

Example:
```Cpp
class C
{
public:
  MAKE_MOCK1(func, void(unsigned));
};

TEST(atest)
{
  C mock_obj;
  std::string what="<unknown>";
  
  ALLOW_CALL(mock_obj, func(3))
    .THROW(std::invalid_argument(what));
  
  what = "";
  tested_func(&mock_obj);
}
```

Above, **`THROW(...)`** will refer to a copy of the string `what` with the value
`"<unknown>"` if a matching call is made from within `tested_func(...)`

See also [**`LR_THROW(...)`**](#LR_THROW) which accesses copies of local objects.


<A name="TIMES"/>
#### **`TIMES(`** *limits* **`)`**  
Used in [**`REQUIRE_CALL(...)`**](#REQUIRE_CALL) and
[**`NAMED_REQUIRE_CALL(...)`**](NAMED_REQUIRE_CALL) to set the limits on
the number of matching calls required.

*limits* may be a single number, in which case it is the exact number of
matching calls required.

*limits* may also be two numbers, describing a range *min-inclusive*,
*max-inclusive*.

If the minimum number of matching calls in not met before the end of the
lifetime of the [expectation](#expectation), a violation is reported.

If the maximum number of matching calls is exceeded, a violation is reported.

*limits* must be `constexpr`.

**`TIMES(...)`** may only be used once for each
[**`REQUIRE_CALL(..)`**](#REQUIRE_CALL) or
[**`NAMED_REQUIRE_CALL(...)`**](#NAMED_REQUIRE_CALL).

Example:
```Cpp
class C
{
public:
  MAKE_MOCK1(func, void(unsigned));
};

using trompeloeil::_;

TEST(atest)
{
  C mock_obj;
  
  REQUIRE_CALL(mock_obj, func(_))
    .TIMES(2, 5);
  
  tested_func(&mock_obj);
```

Above, `tested_func(...)` is expected to call `mock_obj.func()` at least two
times, and no more than 5 times.

See also the helpers [**`AT_LEAST(...)`**](#AT_LEAST) and
[**`AT_MOST(...)`**](#AT_MOST).

<A name="WITH"/>
#### **`WITH(`** *expr* **`)`**  
Used with [expectations](#expectation) to add further conditions for a
matching call. Typically used when [matchers](#matcher) are used for the
parameters, and often when the condition requires several parameter values
together.
*expr* can refer to parameters in the call with their positional names `_1`,
`_2`, etc. Even if the function signature has parameters as non-`const`
references, they are immutable in this context. Several **`LR_WITH(...)`**
and [**`WITH(...)`**](#WITH) clauses can be added to a single expectation.

Named local objects are copied.

Example:
```Cpp
class C
{
public:
  MAKE_MOCK1(func, void(const char*, size_t));
};

using trompeloeil::_;

TEST(atest)
{
  C mock_obj;

  std::string str = "string";
  
  REQUIRE_CALL(mock_obj, func(_,_))
    .WITH(std::string(_1, _2) == str);
  
  str = ""; // does not alter the copy in the expectation above.
  
  tested_func(buff, &mock_obj);
}
```
Above, **`WITH(std::string(_1, _2) == str)`** checks the condition that the
string constructed from the parameters is equal to a copy of the local variable
`str`. To pass the test, `tested_func(...)` must in other words call
`mock_obj.func()` with string `"string"`.

**NOTE!** It is legal, but a *very* bad idea, to modify global/static objects in
**`WITH(...)`**. If several [expectations](#expectation) could match and
are disambiguated by [**`LR_WITH(...)`**](#LR_WITH) and **`WITH(...)`** the
global/static objects will be modified also by those
[expectations](#expectation) that do not match.

See also [**`LR_WITH(...)`**](#LR_WITH) which accesses local objects by
reference.
