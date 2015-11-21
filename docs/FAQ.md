# FAQ

- Q. [Why a name that can neither be pronounced nor spelled?](#why_name)
- Q. [Which compilers supports *Trompeloeil*?](#compilers)
- Q. [How do I use *Trompeloeil* with XXX unit test framework?](#unit_test_adaptation)
- Q. [Why can't I **`.RETURN()`** a reference?](#return_reference)
- Q. [Why can't I change a local variable in **`.SIDE_EFFECT()`**?](#change_side_effect)
- Q. [Why the "local reference" **`.LR_*()`** variants? Why not always capture by reference?](#why_lr)
- Q. [Is it possible to allow all calls to all mocked functions for all mock objects?](#allow_all)
- Q. [Why are parameters referenced by position and not by name?](#why_pos)
- Q. [Why the need to provide the number of parameters in **`MAKE_MOCKn()`** when all information is in the signature?](#why_param_count)
- Q. [Why *`C++14`* and not *`C++11`* or *`C++03`* that is more widely spread?](#why_cpp14)

## <A name="why_name"/>Q. Why a name that can neither be pronounced nor spelled?

**A.** It's a parallel to arts. 
[Trompe-l'œil](https://en.wikipedia.org/wiki/Trompe-l%27%C5%93il), which
literally means "trick the eye," refers to an art form where the artist
creates something that tricks the viewer into thinking they see something
other than what is there. Writing mocks for testing has resemblances to
creating Trompe-l'œil art, in that you create mocks that
"tricks" the test object as if it was interacting with the intended
real world. When you use mocks in a test program, you are the Trompe-l'œil
artist, tricking the test object.

Perhaps *Illusionist* or *Puppeteer* would have sufficed as names, but
they were taken many times over for other projects, and besides, the author
has a weak spot for Trompe-l'œil art.

## <A name="compilers"/>Q. Which compilers supports *Trompeloeil*?

**A.** *Trompeloeil* is known to work well with:
- [g++](http://gcc.gnu.org) 4.9 and later.
- [clang++](http://clang.llvm.org) 3.6 and later
- [VisualStudio](http://visualstudio.com) 2015 and later.

## <A name="unit_test_adaptation"/>Q. How do I use *Trompeloeil* with XXX unit test framework?

**A.** By default, *Trompeloeil* reports violations by throwing an exception,
explaining the problem in the
[`what()`](http://en.cppreference.com/w/cpp/error/exception/what) string.

Depending on your test frame work and your runtime environment, this may,
or may not, suffice.

*Trompeloeil* offers support for adaptation to any test frame work. Some
sample adaptations are:

- [Catch!](#adapt_catch)
- [crpcut](#adapt_crpcut)
- [gtest](#adapt_gtest)
- [boost Unit Test Framework](#adapt_boost_unit_test_framework)

What you need to know to adapt to other frame works is this function:

```Cpp
trompeloeil::set_reporter(std::function<void(trompeloeil::severity,
                                             char const *file,
                                             unsigned long line,
                                             const std::string& msg)>)
```

Call it with the adapter to your test frame work. The important thing to
understand is the first parameter `trompeloeil::severity`. It is an enum
with the values `trompeloeil::severity::fatal` and
`trompeloeil::severity::nonfatal`. The value `severity::nonfatal` is
used when reporting violations during stack rollback, typically during
the destruction of an [expectation](reference.md/#expectation). In this
case it is vital that no exception is thrown, or the process will
terminate.

**NOTE!** There are some violation that cannot be attributed to a source code
location. An example is an unexpected call to a mock function for which there
are no expectations. In these cases `file` will be `""` string and
`line` == 0.

### <A name="adapt_catch"/>Use *Trompeloeil* with [Catch!](https://github.com/philsquared/Catch)

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

### <A name="adapt_crpcut"/>Use *Trompeloeil* with [crpcut](http://crpcut.sourceforge.net)

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

### <A name="adapt_gtest"/>Use *Trompeloeil* with [gtest](https://code.google.com/p/googletest/)

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

### <A name="adapt_boost_unit_test_framework"/>Use *Trompeloeil* with [boost Unit Test Framework](http://www.boost.org/doc/libs/1_59_0/libs/test/doc/html/index.html)

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
## <A name="return_reference"/>Q. Why can't I [**`.RETURN()`**](reference.md/#RETURN) a reference?

**A.** You can, you just have to use
 [`std::ref()`](http://en.cppreference.com/w/cpp/utility/functional/ref) or
 [`std::cref()`](http://en.cppreference.com/w/cpp/utility/functional/cref)
   for it.

Example:
```Cpp
class C
{
public:
  MAKE_MOCK1(lookup, std::string&(int));
};

using trompeloeil::_;

TEST(some_test)
{
  C mock_obj;
  
  std::map<int, std::string> dictionary{ {...} };
  
  ALLOW_CALL(mock_obj, lookup(_))
    .LR_RETURN(std::ref(dictionary.at(_1)));
     
  test_func(&mock_obj);
}
```

Above, the [expectation](reference.md/#expectation) on function `lookup()`
is that any call is allowed and will return the reference to the item found
in `dictionary`. The reference is non-const, so `test_func()` is allowed to
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
open an issue discussing the idea. If you can provide a pull request, so
much the better.

## <A name="why_param_count"/> Q. Why the need to provide the number of parameters in [**`MAKE_MOCKn()`**](reference.md/#MAKE_MOCKn) when all information is in the signature?

**A.** If you can figure out a way to infer the information necessary to
generate a mocked implementation without an explicit parameter count,
please open an issue discussing the idea. If you can provide a pull request,
so much the better.

## <A name="why_cpp14"/> Q. Why *`C++14`* and not *`C++11`* or *`C++03`* that is more widely spread?

**A.** *`C++03`* and older is completely out. The
functionality needed for *Trompeloeil* isn't there.
[Lambdas](http://en.cppreference.com/w/cpp/language/lambda) and
[variadic templates](http://en.cppreference.com/w/cpp/language/parameter_pack)
are absolutely necessary.

The only thing "needed" that *`C++11`* doesn't provide is
[generic lambdas](https://en.wikipedia.org/wiki/C%2B%2B14#Generic_lambdas)
It is perhaps possible that "needed" is too strong a word, that it is
in fact possible without them, in which case a back port to *`C++11`* could be
made.
