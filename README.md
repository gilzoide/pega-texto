pega-texto
==========
A runtime engine for [Parsing Expression Grammars
(PEG)](http://bford.info/packrat/) in C.

In __pega-texto__, PEG grammars are just expression arrays, and can be easily
combined or rearranged dynamically (even while parsing!). You can also provide
a single expression (which, for C, is the same as a 1 element array) for simple
pattern-matching.


Build
-----
Using [CMake](https://cmake.org/):

	$ mkdir build
	$ cd build
	$ cmake ..
	$ make

Install
-------
TODO, but will be using CMake (from build directory):

	$ make install

Test
----
Run unit tests (from build directory):

	$ make test

