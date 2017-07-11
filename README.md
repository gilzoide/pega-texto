pega-texto
==========
[![Build Status](https://travis-ci.org/gilzoide/pega-texto.svg?branch=master)](https://travis-ci.org/gilzoide/pega-texto)

A runtime engine for [Parsing Expression Grammars (PEG)](http://bford.info/packrat/) in C.

In __pega-texto__, PEGs are just expression arrays, and can be easily combined
or rearranged dynamically (even while parsing!). You can also provide a single
expression (which, for C, is the same as a 1 element array) for simple
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
From build directory:

	$ make install


Test
----
From build directory, after building:

	$ make test

