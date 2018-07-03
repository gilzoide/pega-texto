pega-texto
==========
[![Build Status](https://travis-ci.org/gilzoide/pega-texto.svg?branch=master)](https://travis-ci.org/gilzoide/pega-texto)

A runtime engine for [Parsing Expression Grammars (PEG)](http://bford.info/packrat/)
in pure C, heavily inspired by [LPeg](http://www.inf.puc-rio.br/~roberto/lpeg/).

In __pega-texto__, PEGs are just Expression arrays, and can be easily combined
or rearranged dynamically. You can also provide a single expression (which, for
C, is the same as a 1 element array) for simple pattern-matching.

To use the values matched, each Expression may have a user defined Action
function that produces a value. Each Action receives as parameter the values
produced by Actions executed by inner matched Expressions, reducing them into a
single value down the stack.


Build
-----
Using [CMake](https://cmake.org/):

	$ mkdir build
	$ cd build
	$ cmake ..
	$ make


Package
-------
[CPack](https://gitlab.kitware.com/cmake/community/wikis/doc/cpack/Packaging-With-CPack)
is configured for packaging __pega-texto__. From build directory, after
building:

	$ cpack

By default, it uses the TGZ, STGZ and TZ generators. You may override this with
other [available generators](https://gitlab.kitware.com/cmake/community/wikis/doc/cpack/PackageGenerators)
if you want:

	$ cpack -DCPACK_GENERATOR=<generator>


Install
-------
From build directory:

	$ make install


Test
----
From build directory, after building:

	$ make test


Documentation
-------------
The API is documented using [Doxygen](http://www.stack.nl/~dimitri/doxygen/)
and is available at [github pages](https://gilzoide.github.io/pega-texto-docs/).
Also check out the [tutorial](tutorial.md).


Change log
----------
+ 2.0.1 - Put `extern "C"` declarations in inner headers.
+ 2.0.0 - ABI change on `pt_match_options`, included Case Insensitive and
  Character Class Expressions (the old Custom Matcher), changed Custom Matcher
  Expressions to allow operating on strings, also receiving userdata.
+ 1.2.7 - Removed all the Action sequence computation, as Actions are already
  stacked in the right sequence. Running actions is now iterative, O(n) and use
  far less memory.
+ 1.2.6 - Fixed `SEQ` and `OR` expression macros on C++, turns out they behave
  differently about temporary lifetime of arrays.
+ 1.2.5 - Fixed `SEQ` and `OR` expression macros to compile on both C and C++
  using preprocessor macros and `initializer_list` directly on `macro-on.h`.
+ 1.2.4 - Added `extern "C"` declaration on `pega-texto.h` for using in C++. 
+ 1.2.3 - Fixed validation error code emmited when `pt_is_nullable` returned
  true, as it may find an error other than `PT_VALIDATE_LOOP_EMPTY_STRING`.
+ 1.2.2 - Added `NULL` string check on match.
+ 1.2.1 - Fixed validation error on empty `SEQ` and `OR` Expressions, which
  are valid with a `NULL` pointer.
+ 1.2.0 - Macros for Expressions to not own memory buffers, empty `SEQ` and
  `OR` Expressions don't allocate a 0-byte buffer anymore, fixed validation
  error on Non-terminal cycles.
+ 1.1.1 - Fixed validation error on Non-terminal cycles.
+ 1.1.0 - Added basic error handling support.
+ 1.0.0 - Expressions, Grammars, parsing, validation, actions.

