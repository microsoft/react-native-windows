
## fast_float number parsing library: 4x faster than strtod

[![Ubuntu 22.04 CI (GCC 11)](https://github.com/fastfloat/fast_float/actions/workflows/ubuntu22.yml/badge.svg)](https://github.com/fastfloat/fast_float/actions/workflows/ubuntu22.yml)

The fast_float library provides fast header-only implementations for the C++
from_chars functions for `float` and `double` types as well as integer types.
These functions convert ASCII strings representing decimal values (e.g.,
`1.3e10`) into binary types. We provide exact rounding (including round to
even). In our experience, these `fast_float` functions many times faster than
comparable number-parsing functions from existing C++ standard libraries.

Specifically, `fast_float` provides the following two functions to parse
floating-point numbers with a C++17-like syntax (the library itself only
requires C++11):

```C++
from_chars_result from_chars(char const *first, char const *last, float &value, ...);
from_chars_result from_chars(char const *first, char const *last, double &value, ...);
```

You can also parse integer types:

```C++
from_chars_result from_chars(char const *first, char const *last, int &value, ...);
from_chars_result from_chars(char const *first, char const *last, unsigned &value, ...);
```

The return type (`from_chars_result`) is defined as the struct:

```C++
struct from_chars_result {
  char const *ptr;
  std::errc ec;
};
```

It parses the character sequence `[first, last)` for a number. It parses
floating-point numbers expecting a locale-independent format equivalent to the
C++17 from_chars function. The resulting floating-point value is the closest
floating-point values (using either `float` or `double`), using the "round to
even" convention for values that would otherwise fall right in-between two
values. That is, we provide exact parsing according to the IEEE standard.

Given a successful parse, the pointer (`ptr`) in the returned value is set to
point right after the parsed number, and the `value` referenced is set to the
parsed value. In case of error, the returned `ec` contains a representative
error, otherwise the default (`std::errc()`) value is stored.

The implementation does not throw and does not allocate memory (e.g., with `new`
or `malloc`).

It will parse infinity and nan values.

Example:

```C++
#include "fast_float/fast_float.h"
#include <iostream>

int main() {
  std::string input = "3.1416 xyz ";
  double result;
  auto answer = fast_float::from_chars(input.data(), input.data() + input.size(), result);
  if (answer.ec != std::errc()) { std::cerr << "parsing failure\n"; return EXIT_FAILURE; }
  std::cout << "parsed the number " << result << std::endl;
  return EXIT_SUCCESS;
}
```

You can parse delimited numbers:

```C++
  std::string input = "234532.3426362,7869234.9823,324562.645";
  double result;
  auto answer = fast_float::from_chars(input.data(), input.data() + input.size(), result);
  if (answer.ec != std::errc()) {
    // check error
  }
  // we have result == 234532.3426362.
  if (answer.ptr[0] != ',') {
    // unexpected delimiter
  }
  answer = fast_float::from_chars(answer.ptr + 1, input.data() + input.size(), result);
  if (answer.ec != std::errc()) {
    // check error
  }
  // we have result == 7869234.9823.
  if (answer.ptr[0] != ',') {
    // unexpected delimiter
  }
  answer = fast_float::from_chars(answer.ptr + 1, input.data() + input.size(), result);
  if (answer.ec != std::errc()) {
    // check error
  }
  // we have result == 324562.645.
```

Like the C++17 standard, the `fast_float::from_chars` functions take an optional
last argument of the type `fast_float::chars_format`. It is a bitset value: we
check whether `fmt & fast_float::chars_format::fixed` and `fmt &
fast_float::chars_format::scientific` are set to determine whether we allow the
fixed point and scientific notation respectively. The default is
`fast_float::chars_format::general` which allows both `fixed` and `scientific`.

The library seeks to follow the C++17 (see
[28.2.3.(6.1)](https://eel.is/c++draft/charconv.from.chars#6.1)) specification.

* The `from_chars` function does not skip leading white-space characters (unless
  `fast_float::chars_format::skip_white_space` is set).
* [A leading `+` sign](https://en.cppreference.com/w/cpp/utility/from_chars) is
  forbidden (unless `fast_float::chars_format::allow_leading_plus` is set).
* It is generally impossible to represent a decimal value exactly as binary
  floating-point number (`float` and `double` types). We seek the nearest value.
  We round to an even mantissa when we are in-between two binary floating-point
  numbers.

Furthermore, we have the following restrictions:

* We support `float` and `double`, but not `long double`. We also support
  fixed-width floating-point types such as `std::float64_t`, `std::float32_t`,
  `std::float16_t`, and `std::bfloat16_t`.
* We only support the decimal format: we do not support hexadecimal strings.
* For values that are either very large or very small (e.g., `1e9999`), we
  represent it using the infinity or negative infinity value and the returned
  `ec` is set to `std::errc::result_out_of_range`.

We support Visual Studio, macOS, Linux, freeBSD. We support big and little
endian. We support 32-bit and 64-bit systems.

We assume that the rounding mode is set to nearest (`std::fegetround() ==
FE_TONEAREST`).

## Integer types

You can also parse integer types using different bases (e.g., 2, 10, 16). The
following code will print the number 22250738585072012 three times:

```C++
#include "fast_float/fast_float.h"
#include <iostream>

int main() {
  uint64_t i;
  std::string str = "22250738585072012";
  auto answer = fast_float::from_chars(str.data(), str.data() + str.size(), i);
  if (answer.ec != std::errc()) {
    std::cerr << "parsing failure\n";
    return EXIT_FAILURE;
  }
  std::cout << "parsed the number " << i << std::endl;

  std::string binstr = "1001111000011001110110111001001010110100111000110001100";

  answer = fast_float::from_chars(binstr.data(), binstr.data() + binstr.size(), i, 2);
  if (answer.ec != std::errc()) {
    std::cerr << "parsing failure\n";
    return EXIT_FAILURE;
  }
  std::cout << "parsed the number " << i << std::endl;

  std::string hexstr = "4f0cedc95a718c";

  answer = fast_float::from_chars(hexstr.data(), hexstr.data() + hexstr.size(), i, 16);
  if (answer.ec != std::errc()) {
    std::cerr << "parsing failure\n";
    return EXIT_FAILURE;
  }
  std::cout << "parsed the number " << i << std::endl;
  return EXIT_SUCCESS;
}
```

## Behavior of result_out_of_range

When parsing floating-point values, the numbers can sometimes be too small
(e.g., `1e-1000`) or too large (e.g., `1e1000`). The C language established the
precedent that these small values are out of range. In such cases, it is
customary to parse small values to zero and large values to infinity. That is
the behaviour of the C language (e.g., `stdtod`). That is the behaviour followed
by the fast_float library.

Specifically, we follow Jonathan Wakely's interpretation of the standard:

> In any case, the resulting value is one of at most two floating-point values
> closest to the value of the string matching the pattern.

It is also the approach taken by the [Microsoft C++
library](https://github.com/microsoft/STL/blob/62205ab155d093e71dd9588a78f02c5396c3c14b/tests/std/tests/P0067R5_charconv/test.cpp#L943-L946).

Hence, we have the following examples:

```cpp
  double result = -1;
  std::string str = "3e-1000";
  auto r = fast_float::from_chars(str.data(), str.data() + str.size(), result);
  // r.ec == std::errc::result_out_of_range
  // r.ptr == str.data() + 7
  // result == 0
```

```cpp
  double result = -1;
  std::string str = "3e1000";
  auto r = fast_float::from_chars(str.data(), str.data() + str.size(), result);
  // r.ec == std::errc::result_out_of_range
  // r.ptr == str.data() + 6
  // result == std::numeric_limits<double>::infinity()
```

Users who wish for the value to be left unmodified given
`std::errc::result_out_of_range` may do so by adding two lines of code:

```cpp
  double old_result = result; // make copy
  auto r = fast_float::from_chars(start, end, result);
  if (r.ec == std::errc::result_out_of_range) { result = old_result; }
```

## C++20: compile-time evaluation (constexpr)

In C++20, you may use `fast_float::from_chars` to parse strings at compile-time,
as in the following example:

```C++
// consteval forces compile-time evaluation of the function in C++20.
consteval double parse(std::string_view input) {
  double result;
  auto answer = fast_float::from_chars(input.data(), input.data() + input.size(), result);
  if (answer.ec != std::errc()) { return -1.0; }
  return result;
}

// This function should compile to a function which
// merely returns 3.1415.
constexpr double constexptest() {
  return parse("3.1415 input");
}
```

## C++23: Fixed width floating-point types

The library also supports fixed-width floating-point types such as
`std::float64_t`, `std::float32_t`, `std::float16_t`, and `std::bfloat16_t`.
E.g., you can write:

```C++
std::float32_t result;
auto answer = fast_float::from_chars(f.data(), f.data() + f.size(), result);
```

## Non-ASCII Inputs

We also support UTF-16 and UTF-32 inputs, as well as ASCII/UTF-8, as in the
following example:

```C++
#include "fast_float/fast_float.h"
#include <iostream>

int main() {
  std::u16string input = u"3.1416 xyz ";
  double result;
  auto answer = fast_float::from_chars(input.data(), input.data() + input.size(), result);
  if (answer.ec != std::errc()) { std::cerr << "parsing failure\n"; return EXIT_FAILURE; }
  std::cout << "parsed the number " << result << std::endl;
  return EXIT_SUCCESS;
}
```

## Advanced options: using commas as decimal separator, JSON and Fortran

The C++ standard stipulate that `from_chars` has to be locale-independent. In
particular, the decimal separator has to be the period (`.`). However, some
users still want to use the `fast_float` library with in a locale-dependent
manner. Using a separate function called `from_chars_advanced`, we allow the
users to pass a `parse_options` instance which contains a custom decimal
separator (e.g., the comma). You may use it as follows.

```C++
#include "fast_float/fast_float.h"
#include <iostream>

int main() {
  std::string input = "3,1416 xyz ";
  double result;
  fast_float::parse_options options{fast_float::chars_format::general, ','};
  auto answer = fast_float::from_chars_advanced(input.data(), input.data() + input.size(), result, options);
  if ((answer.ec != std::errc()) || ((result != 3.1416))) { std::cerr << "parsing failure\n"; return EXIT_FAILURE; }
  std::cout << "parsed the number " << result << std::endl;
  return EXIT_SUCCESS;
}
```

### You can also parse Fortran-like inputs

```C++
#include "fast_float/fast_float.h"
#include <iostream>

int main() {
  std::string input = "1d+4";
  double result;
  fast_float::parse_options options{fast_float::chars_format::fortran};
  auto answer = fast_float::from_chars_advanced(input.data(), input.data() + input.size(), result, options);
  if ((answer.ec != std::errc()) || ((result != 10000))) { std::cerr << "parsing failure\n"; return EXIT_FAILURE; }
  std::cout << "parsed the number " << result << std::endl;
  return EXIT_SUCCESS;
}
```

### You may also enforce the JSON format ([RFC 8259](https://datatracker.ietf.org/doc/html/rfc8259#section-6))

```C++
#include "fast_float/fast_float.h"
#include <iostream>

int main() {
  std::string input = "+.1"; // not valid
  double result;
  fast_float::parse_options options{fast_float::chars_format::json};
  auto answer = fast_float::from_chars_advanced(input.data(), input.data() + input.size(), result, options);
  if (answer.ec == std::errc()) { std::cerr << "should have failed\n"; return EXIT_FAILURE; }
  return EXIT_SUCCESS;
}
```

By default the JSON format does not allow `inf`:

```C++
#include "fast_float/fast_float.h"
#include <iostream>

int main() {
  std::string input = "inf"; // not valid in JSON
  double result;
  fast_float::parse_options options{fast_float::chars_format::json};
  auto answer = fast_float::from_chars_advanced(input.data(), input.data() + input.size(), result, options);
  if (answer.ec == std::errc()) { std::cerr << "should have failed\n"; return EXIT_FAILURE; }
  return EXIT_SUCCESS;
}
```

You can allow it with a non-standard `json_or_infnan` variant:

```C++
#include "fast_float/fast_float.h"
#include <iostream>

int main() {
  std::string input = "inf"; // not valid in JSON but we allow it with json_or_infnan
  double result;
  fast_float::parse_options options{fast_float::chars_format::json_or_infnan};
  auto answer = fast_float::from_chars_advanced(input.data(), input.data() + input.size(), result, options);
  if (answer.ec != std::errc() || (!std::isinf(result))) { std::cerr << "should have parsed infinity\n"; return EXIT_FAILURE; }
  return EXIT_SUCCESS;
}
```

## Users and Related Work

The fast_float library is part of:

* GCC (as of version 12): the `from_chars` function in GCC relies on fast_float,
* [Chromium](https://github.com/Chromium/Chromium), the engine behind Google
  Chrome, Microsoft Edge, and Opera,
* [WebKit](https://github.com/WebKit/WebKit), the engine behind Safari (Apple's
  web browser),
* [DuckDB](https://duckdb.org),
* [Redis](https://github.com/redis/redis) and [Valkey](https://github.com/valkey-io/valkey),
* [Apache Arrow](https://github.com/apache/arrow/pull/8494) where it multiplied
  the number parsing speed by two or three times,
* [Google Jsonnet](https://github.com/google/jsonnet),
* [ClickHouse](https://github.com/ClickHouse/ClickHouse).

The fastfloat algorithm is part of the [LLVM standard
libraries](https://github.com/llvm/llvm-project/commit/87c016078ad72c46505461e4ff8bfa04819fe7ba).
There is a [derived implementation part of
AdaCore](https://github.com/AdaCore/VSS).

The fast_float library provides a performance similar to that of the
[fast_double_parser](https://github.com/lemire/fast_double_parser) library but
using an updated algorithm reworked from the ground up, and while offering an
API more in line with the expectations of C++ programmers. The
fast_double_parser library is part of the [Microsoft LightGBM machine-learning
framework](https://github.com/microsoft/LightGBM).

## References

* Daniel Lemire, [Number Parsing at a Gigabyte per
  Second](https://arxiv.org/abs/2101.11408), Software: Practice and Experience
  51 (8), 2021.
* Noble Mushtak, Daniel Lemire, [Fast Number Parsing Without
  Fallback](https://arxiv.org/abs/2212.06644), Software: Practice and Experience
  53 (7), 2023.

## Other programming languages

* [There is an R binding](https://github.com/eddelbuettel/rcppfastfloat) called
  `rcppfastfloat`.
* [There is a Rust port of the fast_float
  library](https://github.com/aldanor/fast-float-rust/) called
  `fast-float-rust`.
* [There is a Java port of the fast_float
  library](https://github.com/wrandelshofer/FastDoubleParser) called
  `FastDoubleParser`. It used for important systems such as
  [Jackson](https://github.com/FasterXML/jackson-core).
* [There is a C# port of the fast_float
  library](https://github.com/CarlVerret/csFastFloat) called `csFastFloat`.

## How fast is it?

It can parse random floating-point numbers at a speed of 1 GB/s on some systems.
We find that it is often twice as fast as the best available competitor, and
many times faster than many standard-library implementations.

<img src="https://lemire.me/blog/wp-content/uploads/2020/11/fastfloat_speed.png"
width="400" alt="fast_float is many times faster than many standard-library
implementations">

```bash
$ ./build/benchmarks/benchmark
# parsing random integers in the range [0,1)
volume = 2.09808 MB
netlib                                  :   271.18 MB/s (+/- 1.2 %)    12.93 Mfloat/s
doubleconversion                        :   225.35 MB/s (+/- 1.2 %)    10.74 Mfloat/s
strtod                                  :   190.94 MB/s (+/- 1.6 %)     9.10 Mfloat/s
abseil                                  :   430.45 MB/s (+/- 2.2 %)    20.52 Mfloat/s
fastfloat                               :  1042.38 MB/s (+/- 9.9 %)    49.68 Mfloat/s
```

See the [Benchmarking](#benchmarking) Section for instructions on how to run our benchmarks.

## Video

[![Go Systems 2020](https://img.youtube.com/vi/AVXgvlMeIm4/0.jpg)](https://www.youtube.com/watch?v=AVXgvlMeIm4)

## Using as a CMake dependency

This library is header-only by design. The CMake file provides the `fast_float`
target which is merely a pointer to the `include` directory.

If you drop the `fast_float` repository in your CMake project, you should be
able to use it in this manner:

```cmake
add_subdirectory(fast_float)
target_link_libraries(myprogram PUBLIC fast_float)
```

Or you may want to retrieve the dependency automatically if you have a
sufficiently recent version of CMake (3.11 or better at least):

```cmake
FetchContent_Declare(
  fast_float
  GIT_REPOSITORY https://github.com/fastfloat/fast_float.git
  GIT_TAG tags/v8.0.0
  GIT_SHALLOW TRUE)

FetchContent_MakeAvailable(fast_float)
target_link_libraries(myprogram PUBLIC fast_float)
```

You should change the `GIT_TAG` line so that you recover the version you wish to
use.

You may also use [CPM](https://github.com/cpm-cmake/CPM.cmake), like so:

```cmake
CPMAddPackage(
  NAME fast_float
  GITHUB_REPOSITORY "fastfloat/fast_float"
  GIT_TAG v8.0.0)
```

## Using as single header

The script `script/amalgamate.py` may be used to generate a single header
version of the library if so desired. Just run the script from the root
directory of this repository. You can customize the license type and output file
if desired as described in the command line help.

You may directly download automatically generated single-header files:

<https://github.com/fastfloat/fast_float/releases/download/v8.0.0/fast_float.h>

## Benchmarking

The project has its own benchmarks with realistic data inputs. Under Linux or macOS,
you can use it as follows if your system supports C++17:

```
cmake -B build -D FASTFLOAT_BENCHMARKS=ON
cmake --build build
./build/benchmarks/realbenchmark
```

Importantly, by default, the benchmark is built in Release mode.

The instructions are similar under Windows.

Under Linux and macOS, it is recommended to run the benchmarks in a privileged manner to get access
to hardware performance counters. You may be able to do so with the `sudo` command
in some cases:

```
sudo ./build/benchmarks/realbenchmark
```

## Packages

* The fast_float library is part of the [Conan package
  manager](https://conan.io/center/recipes/fast_float).
* It is part of the [brew package
  manager](https://formulae.brew.sh/formula/fast_float).
* Some Linux distribution like Fedora include fast_float (e.g., as
  `fast_float-devel`).

## Credit

Though this work is inspired by many different people, this work benefited
especially from exchanges with Michael Eisel, who motivated the original
research with his key insights, and with Nigel Tao who provided invaluable
feedback. Rémy Oudompheng first implemented a fast path we use in the case of
long digits.

The library includes code adapted from Google Wuffs (written by Nigel Tao) which
was originally published under the Apache 2.0 license.

## License

<sup>
Licensed under either of <a href="LICENSE-APACHE">Apache License, Version
2.0</a> or <a href="LICENSE-MIT">MIT license</a> or <a
href="LICENSE-BOOST">BOOST license</a>.
</sup>

<br/>

<sub>
Unless you explicitly state otherwise, any contribution intentionally submitted
for inclusion in this repository by you, as defined in the Apache-2.0 license,
shall be triple licensed as above, without any additional terms or conditions.
</sub>
