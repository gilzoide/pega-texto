pega-texto
==========
[![Build Status](https://travis-ci.org/gilzoide/pega-texto.svg?branch=master)](https://travis-ci.org/gilzoide/pega-texto)

A runtime engine for [Parsing Expression Grammars (PEG)](http://bford.info/packrat/)
in C, heavily inspired by [LPeg](http://www.inf.puc-rio.br/~roberto/lpeg/).

In __pega-texto__, PEGs are just Expression arrays, and can be easily combined
or rearranged dynamically. You can also provide a single expression (which, for
C, is the same as a 1 element array) for simple pattern-matching.

To use the values matched, each Expression may have a user defined Action
function that produces a value. Each Action receives as parameter the values
produced by Actions executed by inner matched Expressions, reducing them into a
single value down the stack (yup, recursion)!


Build
-----
Using [CMake](https://cmake.org/):

	$ mkdir build
	$ cd build
	$ cmake ..
	$ make


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
Everything is documented with [Doxygen](http://www.stack.nl/~dimitri/doxygen/).

@todo put the generated docs on github-pages =]


Change log
----------
+ 1.1.1 - Fixed validation error on Non-terminal cycles.
+ 1.1.0 - Added basic error handling support.
+ 1.0.0 - Expressions, Grammars, parsing, validation, actions.

