# FAQ

- Q. [Why a name that can neither be pronounced nor spelled?](#why_name)
- Q. [Which compilers supports *Trompeloeil*?](#compilers)
- Q. [How do I use *Trompeloeil* with XXX unit test framework?](#unit_test_adaptation)
- Q. [Is *Trompeloeil* thread-safe?](#thread_safety)
- Q. [Can a mock function be marked `override`?](#override)
- Q. [Why can't I **`.RETURN()`** a reference?](#return_reference)
- Q. [Why can't I change a local variable in **`.SIDE_EFFECT()`**?](#change_side_effect)
- Q. [Why the "local reference" **`.LR_*()`** variants? Why not always capture by reference?](#why_lr)
- Q. [Is it possible to allow all calls to all mocked functions for all mock objects?](#allow_all)
- Q. [Why are parameters referenced by position and not by name?](#why_pos)
- Q. [Why the need to provide the number of parameters in **`MAKE_MOCKn()`** when all information is in the signature?](#why_param_count)
- Q. [Why *`C++14`* and not *`C++11`* or *`C++03`* that is more widely spread?](#why_cpp14)
- Q. [Why are my parameter values printed as hexadecimal dumps in violation reports](#why_hex)
- Q. [Can I mock a C function API?](#func_mock)
- Q. [Can I match a value pointed to by a pointer parameter?](#match_deref)
- Q. [Can I negate the effect of a matcher?](#negate_matcher)
- Q. [Can I check if an expectation is fulfilled?](#query_expectation)
- Q. [What does it mean to mix **`IN_SEQUENCE`** and **`TIMES`**?](#sequence_times)
- Q. [How do I use *Trompeloeil* in a CMake project?](#cmake)
- Q. [Why are mock objects not move constructible?](#move_constructible)
- Q. [Why can't I mock a function that returns a template?](#return_template)

## <A name="why_name"/>Q. Why a name that can neither be pronounced nor spelled?

**A.** It's a parallel to arts.
[Trompe-l'œil](https://en.wikipedia.org/wiki/Trompe-l%27%C5%93il), which
literally means "trick the eye," refers to an art form where the artist
creates something that tricks the viewer into thinking they see something
other than what is there. Writing mocks for testing has resemblances to
creating Trompe-l'œil art, in that you create mocks that
"tricks" the test object as if it was interacting with the intended
real world. When you use mocks in a test program, you are the Trompe-l'œil
artist, tricking the code under test.

Perhaps *Illusionist* or *Puppeteer* would have sufficed as names, but
they were taken many times over for other projects, and besides, the author
has a soft spot for Trompe-l'œil art.

If you **really** cannot handle the name, you can use the following
renaming mechanism. Assume that you'd like the name
[`chimera`](http://www.merriam-webster.com/dictionary/chimera) instead.

Create a file `chimera.hpp` with the following contents:

```Cpp
#ifndef CHIMERA_HPP
#define CHIMERA_HPP

#include <trompeloeil.hpp>
namespace chimera = trompeloeil;

#endif /* include guard */
```

Your tests can now `#include <chimera.hpp>` and use (for example)
`chimera::expectation` and `chimera::deathwatched<T>`.

## <A name="compilers"/>Q. Which compilers supports *Trompeloeil*?

**A.** *Trompeloeil* is known to work well with:

- [g\+\+](http://gcc.gnu.org) 4.9.3 and later.
- [clang\+\+](http://clang.llvm.org) 3.5 and later.
- [VisualStudio](http://visualstudio.com) 2015 and later.

*Trompeloeil* is known to work somewhat with *g\+\+* 4.8.4 and 4.8.5, and
somewhat less with *g\+\+* 4.8.3.  *`g++ 4.8.x`* only compiles with a
C\+\+11 dialect (e.g. *`-std=c++11`*).  For details, see
["G\+\+ 4.8.x limitations"](Backward.md/#gxx48x_limitations).

## <A name="unit_test_adaptation"/>Q. How do I use *Trompeloeil* with XXX unit test framework?

**A.** By default, *Trompeloeil* reports violations by throwing an exception,
explaining the problem in the
[`what()`](http://en.cppreference.com/w/cpp/error/exception/what) string.

Depending on your test frame work and your runtime environment, this may,
or may not, suffice.

*Trompeloeil* offers support for adaptation to any test frame work. Adaptation
examples for some popular unit test frame works are listed in the
[Cook Book](CookBook.md/#unit_test_frameworks).

## <A name="thread_safety"/>Q. Is *Trompeloeil* thread-safe?

**A.** Yes, with caveats.

In a unit test you don't want to depend on the scheduler, which is typically
out of your control. However, some times it is convenient to use a unit test
like environment to exercise a larger aspect of your code. In this setting,
using [mock objects](reference.md/#mock_object) with different
[expectations](reference.md/#expectation) can make sense when statistically
searching for synchronization problems.

To enable this, *Trompeloeil* uses a global
[`recursive_mutex`](http://en.cppreference.com/w/cpp/thread/recursive_mutex)
which protects [expectations](reference.md/#expectation).

[Expectations](reference.md/#expectation) can come and go in different threads,
and [mock functions](reference.md/#mock_function) can be called in different
threads, all protected by the global lock. However, it is essential that the
[mock object](reference.md/#mock_object) is not deleted while establishing the
[expectation](reference.md/#expectation) or calling the
[mock function](reference.md/#mock_function), as per normal thread-safety
diligence.

Should you need to access the lock in your tests, you can do so with

```Cpp
  auto lock = trompeloeil::get_lock();
```

`lock` holds the
[`recursive_mutex`](http://en.cppreference.com/w/cpp/thread/recursive_mutex)
until it goes out of scope.

## <A name="override"/>Q. Can a mock function be marked `override`?

**A.** Yes, just add `override` a third parameter to
[**`MAKE_MOCKn()`**](reference.md/#MAKE_MOCKn) or
[**`MAKE_CONST_MOCKn()`**](reference.md/#MAKE_CONST_MOCKn)

Example:

```Cpp
class Interface
{
public:
  virtual ~Interface() = default;
  virtual int func1(int) = 0;
};

class Mock : public Interface
{
public:
  MAKE_MOCK1(func1, int(int), override); // overridden
  MAKE_MOCK1(func2, int(int));           // not overridden
};
```

## <A name="return_reference"/>Q. Why can't I [**`.RETURN()`**](reference.md/#RETURN) a reference?

**A.** You can, but the language is a bit peculiar.

For parameters or returned references from function calls, just use
**`.RETURN(value)`**. For local variables you need
[**`.LR_RETURN()`**](reference.md/#LR_RETURN), and for both global and local
variables you either need to use
[`std::ref(value)`](http://en.cppreference.com/w/cpp/utility/functional/ref) or
[`std::cref(value)`](http://en.cppreference.com/w/cpp/utility/functional/cref)
for it, or just enclose the value in an extra parenthesis, like this
[**`.LR_RETURN((value))`**](reference.md/#LR_RETURN)

Example:

```Cpp
class C
{
public:
  MAKE_MOCK1(lookup, std::string&(int));
};

using trompeloeil::_;
using trompeloeil::lt;

TEST(some_test)
{
  C mock_obj;

  std::map<int, std::string> dictionary{ {...} };

  std::string default_string;

  ALLOW_CALL(mock_obj, lookup(_))
    .LR_RETURN(dictionary.at(_1)); // function call

  ALLOW_CALL(mock_obj, lookup(trompeloeil::lt(0)))
    .LR_RETURN((default_string)); // extra parenthesis

  ALLOW_CALL(mock_obj, lookup(0))
    .LR_RETURN(std::ref(default_string));

  test_func(&mock_obj);
}
```

Above, the [expectations](reference.md/#expectation) on function
`lookup()` is that any call is allowed and will return an
[lvalue-reference](http://en.cppreference.com/w/cpp/language/reference)
to either a match in  `dictionary`, or to the local variable
`default_string`. The reference is non-const, so `test_func()` is allowed to
change the returned string.

## <A name="change_side_effect"/> Q. Why can't I change a local variable in [**`.SIDE_EFFECT()`**](reference.md/#SIDE_EFFECT)?

**A.** It would almost certainly be very confusing. All local variables
referenced in [**`.WITH()`**](reference.md/#WITH),
[**`.SIDE_EFFECT()`**](reference.md/#SIDE_EFFECT),
[**`.RETURN()`**](reference.md/#RETURN) and
[**`.THROW()`**](reference.md/#THROW)
are captured by value, i.e. each such clause has its own copy of the local
variable. If you could change it, it would change the value in that clause
only and not in any of the others.

Example:

```Cpp
class C
{
public:
  MAKE_MOCK1(func, void(int));
};

using trompeloeil::_;

TEST(some_test)
{
  C mock_obj;

  unsigned abs_sum = 0;

  ALLOW_CALL(mock_obj, func(trompeloeil::gt(0)))
    .SIDE_EFFECT(abs_sum+= _1); // illegal code!

  ALLOW_CALL(mock_obj, func(trompeloeil::lt(0))
    .SIDE_EFFECT(abs_sum-= _1); // illegal code!

  ALLOW_CALL(mock_obj, func(0));

  test_func(&mock_obj);
```

The two [**`SIDE_EFFECT()`**](reference.md/#SIDE_EFFECT) clauses above
each have their own copy of the local variable `abs_sum`. Allowing them
to update their own copies would be very confusing, and it would also be
difficult to get the value back to the test.

If you need to change the value of a local variable it is better to
use the alternative "local reference" forms
[**`LR_SIDE_EFFECT()`**](reference.md/#LR_SIDE_EFFECT),
[**`LR_WITH()`**](reference.md/#LR_WITH),
[**`LR_RETURN()`**](reference.md/#LR_RETURN) or
[**`LR_THROW()`**](reference.md/#LR_THROW).

## <A name="why_lr"/> Q. Why the "local reference" **`.LR_*()`** variants? Why not always capture by reference?

**A.** It's safer. Lifetime management can be tricky in *`C++`*, and even more
so when complex functionality is hiding behind hideous macros in a
frame work. Experiences from the alpha phase, where this distinction wasn't
made, made the problem glaringly obvious. Making the default safe, and
providing the option to very visibly use the potentially unsafe, is
considerably better, although it makes the test code somewhat visually
unpleasant.

## <A name="allow_all"/> Q. Is it possible to allow all calls to all mocked functions for all mock objects?

**A.** No, it is not. There are two reasons for this, technical and philosophical.

**Technical** There is a problem with the return value. It is difficult, if
at all possible, to come up with a generic return that works for all types.
This could be overcome by allowing all calls to all functions with a certain
return type, for all objects.

**Philosophical** While there are no doubt situations where this would be
convenient, it could be a very dangerous convenience that opens up for
relaxing tests unnecessarily, simply because it's so easy to allow
everything, and then when you introduce a bug, you never notice
because everything is allowed. If a safe way of allowing all calls is
thought of, then this may change, but having a perhaps unnecessarily strict
rule that can be relaxed is safer than the alternative.

## <A name="why_pos"/> Q. Why are parameters referenced by position and not by name?

**A.** If you can figure out a way to refer to parameters by name, please
[open an issue](https://github.com/rollbear/trompeloeil/issues) discussing the
idea. If you can provide a pull request, so much the better.

## <A name="why_param_count"/> Q. Why the need to provide the number of parameters in [**`MAKE_MOCKn()`**](reference.md/#MAKE_MOCKn) when all information is in the signature?

**A.** If you can figure out a way to infer the information necessary to
generate a mocked implementation without an explicit parameter count,
please [open an issue](https://github.com/rollbear/trompeloeil/issues)
discussing the idea. If you can provide a pull request, so much the better.

## <A name="why_cpp14"/> Q. Why *`C++14`* and not *`C++11`* or *`C++03`* that is more widely spread?

**A.** *`C++03`* and older is completely out. The
functionality needed for *Trompeloeil* isn't there.
[Lambdas](http://en.cppreference.com/w/cpp/language/lambda) and
[variadic templates](http://en.cppreference.com/w/cpp/language/parameter_pack)
are absolutely necessary.

The only thing "needed" that *`C++11`* doesn't provide is
[generic lambdas](https://en.wikipedia.org/wiki/C%2B%2B14#Generic_lambdas).
It is perhaps possible that "needed" is too strong a word, that it is
in fact possible without them, in which case a back port to *`C++11`* could be
made.

And indeed, since this FAQ question was first answered, a back port of a
useful subset of Trompeloeil has been completed for use with *`C++11`*.
For details, see ["Backward compatibility with earlier versions of C\+\+"](
  Backward.md
).

## <A name="why_hex"/> Q. Why are my parameter values printed as hexadecimal dumps in violation reports?

**A.** By default *Trompeloeil* prints parameter values using the
[stream insertion operators](http://en.cppreference.com/w/cpp/io/basic_ostream/operator_ltlt)
for the type, but if none exists, it presents a hexadecimal dump of the memory
occupied by the value.

You can change that either by providing a stream insertion operator for your
type, or by providing a [custom formatter](CookBook.md/#custom_formatting)
for it.

## <A name="func_mock"/> Q. Can I mock a C function API?

**A.** *Trompeloeil* can mock member functions only. However, there are
tricks you can use to mock a function API, provided that it is OK
to use a
[link seam](http://www.informit.com/articles/article.aspx?p=359417&seqNum=3)
and link your test program with a special test
implementation of the API that calls mocks. Here's an example:

```Cpp
/* c_api.h */
#ifdef __cplusplus
extern "C" {
#endif

int func1(const char*);
const char* func2(int);

#ifdef __cplusplus
}
#endif
```

With the above C-API mocks can be made:

```Cpp
/* mock_c_api.h */
#ifndef MOCK_C_API_H
#define MOCK_C_API_H

#include <c_api.h>
#include <cassert>
#include <string>
#include <trompeloeil.hpp>

struct mock_api
{
  static mock_api*& instance() { static mock_api* obj = nullptr; return obj; }
  mock_api() { assert(instance() == nullptr); instance() = this; }
  ~mock_api() { assert(instance() == this); instance() = nullptr; }
  mock_api(const mock_api&) = delete;
  mock_api& operator=(const mock_api&) = delete;

  MAKE_CONST_MOCK1(func1, int(std::string)); // strings are easier to deal with
  MAKE_CONST_MOCK1(func2, const char*(int));
};

endif /* include guard */
```

Note that the mock constructor stores a globally available pointer to the
instance, and the destructor clears it.

With the mock available the test version the C-API can easily be implemented:

```Cpp
#include "mock_c_api.h"

int func1(const char* str)
{
  auto obj = mock_api::instance();
  assert(obj);
  return obj->func1(str); // creates a std::string
}

const char* func2(int value)
{
  auto obj = mock_api::instance();
  assert(obj);
  return obj->func2(value);
}
```

Now your tests becomes simple:

```Cpp
#include "mock_c_api.h"
#include "my_obj.h"
TEST("my obj calls func1 with empty string when poked")
{
  mock_api api;
  my_obj tested;
  {
    REQUIRE_CALL(api, func1(""))
      .RETURN(0);
    tested.poke(0);
  }
}
```

## <A name="match_deref"/> Q. Can I match a value pointed to by a pointer parameter?

**A.** You can always match with [**`_`**](reference.md/#wildcard)
and use [**`LR_WITH()`**](reference.md/#LR_WITH) or
[**`WITH()`**](reference.md/#WITH) using whatever logic you
like. But by using [matchers](CookBook.md/#matching_conditions)
you can match the value pointed to using unary operator
[**`*`**](reference.md/#deref_matcher) on the *matcher*.

See [Matching pointers to values](CookBook.md/#matching_pointers)
in the [Cook Book](CookBook.md).

## <A name="negate_matcher"/> Q. Can I negate the effect of a matcher?

**A.** You can always match with [**`_`**](reference.md/#wildcard)
and use [**`LR_WITH()`**](reference.md/#LR_WITH) or
[**`WITH()`**](reference.md/#WITH) using whatever logic you
like. But by using [matchers](CookBook.md/#matching_conditions)
you can negate the effect of the matcher, allowing what the
matcher disallows and vice versa, using operator
[**`!`**](reference.md/#negate_matcher) on the *matcher*.

See [Matching the opposite of a matcher](CookBook.md/#negating_matchers)
in the [Cook Book](CookBook.md).

## <A name="query_expectation"/> Q. Can I check if an expectation is fulfilled?

Yes, if you use [**`NAMED_ALLOW_CALL(...)`**](reference.md/#NAMED_ALLOW_CALL),
[**`NAMED_REQUIRE_CALL(...)`**](reference.md/#NAMED_REQUIRE_CALL) or
[**`NAMED_FORBID_CALL(...)`**](reference.md/#NAMED_FORBID_CALL), then you can
ask [`is_satisfied()`](reference.md/#is_satisfied) and
[`is_saturated()`](reference.md/#is_saturated). Example:

```Cpp
TEST("something")
{
  mock_obj mock;
  auto ptr = NAMED_REQUIRE_CALL(mock, some_func())
               .TIMES(2,4);
  ...
  if (ptr->is_satisfied()) // at least two call have been made
  ...
  if (ptr->is_saturated()) // four calls have been made
}
```

Likewise you can ask [sequence objects](reference.md/#sequence_type) if the
sequence they describe [`is_completed()`](reference.md/#is_completed).

These are rarely useful in pure unit tests, but it can be useful for mini
integration tests, especially when threading is involved.

## <A name="sequence_times"/>Q. What does it mean to mix **`IN_SEQUENCE`** and **`TIMES`**?

**A.** Using [**`.TIMES()`**](reference.md/#TIMES) with
[**`.IN_SEQUENCE()`**](refecence.md/#IN_SEQUENCE) is confusing at best, and
especially when you have a (possibly open) interval for **`.TIMES()`**.

Trompeloeil always sees sequences as observed from a sequence object, and a
sequence object can only move forward in what it allows.

Example:

```Cpp
trompeloeil::sequence seq;
REQUIRE_CALL(mock, foo1)
  .TIMES(AT_LEAST(1))
  .IN_SEQUENCE(seq);
REQUIRE_CALL(mock, foo2)
  .IN_SEQUENCE(seq);
REQUIRE_CALL(mock, foo3)
  .IN_SEQUENCE(seq);

// later...

mock.foo1();
mock.foo2();
mock.foo1(); // boom!
mock.foo3();
```

In the example above, a sequence violation is reported on the second call to
`mock.foo1()`. It goes like this:

`mock.foo1();`
this is the first call for the sequence object, so it is allowed. It says
**`AT_LEAST(1)`**, so it may move to the next step, or it may repeat the same
call.

`mock.foo2();`
The current step in the sequence is `mock.foo1()`, but it is satisfied, so moving on
to the next one is allowed. The next one is `mock.foo2()`, which matches this call,
so everything is good.

`mock.foo1();`
The current step in the sequence is `mock.foo2()`. Is is satisfied and
saturated, so the sequence object must move to the next step. The next step is
`mock.foo3()`, which is a mismatch, so a sequence violation is reported.

## <A name="cmake"/>Q. How do I use *Trompeloeil* in a CMake project?

**A.** To use *Trompeloeil* in a project that is built with CMake, there are several
options to make it accessible to CMake. (The commands below of for Linux, but it works
similarly on other platforms.)

First, you could build and install it locally somewhere in your project (here,
in `./my_proj/toolkits`):

```cmake
git clone https://github.com/rollbear/trompeloeil.git
cd trompeloeil
mkdir build ; cd build
cmake -G "Unix Makefiles" .. -DCMAKE_INSTALL_PREFIX=../../my_proj/toolkits
cmake --build . --target install
```

This will create a directory structure inside `toolkits` that has `include/trompeoeil.hpp`
and the CMake find modules in `lib/cmake/trompeoeil`. Whether you add the entire *Trompeoeil*
repo to your source control is up to you, but the minimal set of files for proper CMake 
support in is in the `toolkits` directory.

Second, you could install it globally on your system by cloning the repo and installing with
root privileges:

```cmake
git clone https://github.com/rollbear/trompeloeil.git
cd trompeloeil
mkdir build ; cd build
cmake -G "Unix Makefiles" ..
sudo cmake --build . --target install
```

In either case, add a `find_package()` call in your project's `CMakeLists.txt`:

```cmake
find_package( Catch2      REQUIRED HINTS "${toolkitsDir}/Catch2"      ) # Sample unit test framework
find_package( trompeloeil REQUIRED HINTS "${toolkitsDir}/trompeloeil" )

add_executable( my_unit_tests
    test1.cpp
    test2.cpp
    test_main.cpp
)

target_link_libraries( my_unit_tests
    my_library_under_test # provided by an add_library() call elsewhere in your project

    # Nothing to link since both of these libs are header-only,
    # but this sets up the include path correctly too
    Catch2::Catch2 
    trompeloeil
)

# Optional: Use CTest to manage your tests
add_test( run_my_unit_tests my_unit_tests ) # May need to call enable_testing() elsewhere also
```

This assumes that you have defined a variable called `toolkitsDir` pointing to
`my_proj/toolkits/lib/cmake`, that you have Catch2 installed similarly, and
that you have defined a target called `my_library_under_test` in other parts of
the CMake files. (If you installed the libraries globally on your system, you
should be able to drop the hints in the `find_package()` calls.)

Finally, you can add *Trompeloeil* to your project and then either (a) use CMake's
`find_file()` to locate the header and add its path to
`include_directories()`; or (b) use `add_subdirectory()` (one or two argument
version) to add its path to your project.


### <A name="move_constructible"/> Q. Why are mock objects not move constructible?

**A.** Because a move is potentially dangerous in non-obvious ways. If a mock object is
moved, the actions associated with an expectation
([**`.WITH()`**](reference.md/#WITH),
 [**`.SIDE_EFFECT()`**](reference.md/#SIDE_EFFECT),
 [**`.RETURN()`**](reference.md/#RETURN),
 [**`.THROW()`**](reference.md/#THROW)) and their
 `LR_` versions, are *not* moved. If they refer to data members stored in a
 moved mock object, they will refer to dead data. This is an accepted cost
 in normal C++ code, but since the effect is hidden under the macros,
 it is better to play safe.
 
With that said, you can explicitly make mock objects movable, if you want to.
See: [**`trompeloeil_movable_mock`**](reference.md/#movable_mock).



### <A name="return_template"/> Q. Why can't I mock a function that returns a template?

Like this:

```Cpp
struct M
{
  MAKE_MOCK2(make, std::pair<int,int>(int,int));
};
```

**A.** You can, but there is a limitation in the preprocessor, that makes it
work poorly with templates. It sees the parameters to the
[**`MAKE_MOCK2()`**](reference.md/#MAKE_MOCKn)
macro above as `make`, `std::pair<int`, followed by `int>(int,int)`, which
of course is nonsense and causes compilation errors.

One easy way around this is to put the signature into parentheses:

```Cpp
struct M
{
  MAKE_MOCK2(make, (std::pair<int,int>(int,int)));
};
```

Or if you prefer the legacy way, create an alias:

```Cpp
using pair_int_int = std::pair<int,int>;

struct M
{
  MAKE_MOCK2(make, pair_int_int(int,int));
};
```

These work around the preprocessor parameter problem.

Another way, if you're mocking an interface, is to use
[**`trompeloeil::mock_interface<T>`**](reference.md/#mock_interface)
and [**`IMPLEMENT_MOCKn`**](reference.md/#IMPLEMENT_MOCKn). See
[CookBook](CookBook.md/#creating_mock_classes) for an intro.
