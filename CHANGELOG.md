# Changelog
## [Unreleased](https://github.com/gilzoide/pega-texto/compare/v4.1.0...HEAD)
### Added
- Support for passing the size of the input string to the matching algorithm.
  + Call `pt_match_n` instead of `pt_match` to pass the string size.
  + `pt_match` simply forwards the call to `pt_match_n` passing `SIZE_MAX` as the string size.
  + A new `PT_OP_CUSTOM_MATCHER_N` operation was added for custom matchers that receive the input string size.
    Use the `PT_CUSTOM_MATCHER_N` macro or `CUSTOM_MATCHER_N` alias to create such expression.


## [v4.1.0](https://github.com/gilzoide/lua-gdextension/releases/tag/v4.1.0)
### Added
- Support for passing custom `malloc`/ `realloc` / `free` functions in `pt_match_options`

### Fixed
- Compilation and support for literals, case insensitive and set expressions when `PT_ELEMENT_TYPE` is not `char`

### Deprecated
- `PT_MALLOC`, `PT_REALLOC` and `PT_FREE` macros.
  Pass custom memory functions in `pt_match_options` instead.


## [v4.0.0](https://github.com/gilzoide/lua-gdextension/releases/tag/v4.0.0)
### Changed
- Refactor project as a single header implementation
- Split Quantifier Expressions into At Least and At Most expressions
- Make `PT_DATA` type configurable, just `#define PT_DATA <your type here>`
- Add Action Expressions to simplify implementation and make code more readable
- Reimplement match algorithm using recursion.
  This is much faster than the previous approach.
- Make Expressions be layed out contiguously in memory and remove heap based creation and destruction of them
- Grammar literals are now definable at compile-time


## [v3.0.0](https://github.com/gilzoide/lua-gdextension/releases/tag/v3.0.0)
### Added
- Add Byte expression

### Changed
- Change actions to receive the capture with pointer and size, instead of pointer to input string, start and end of capture
- Change Character Class Expressions to use only functions defined in `ctype.h`
- Change Range Expressions to use 2 bytes instead of a NULL terminated string
- Use Grammars without malloc'ing them.


## [v2.1.0](https://github.com/gilzoide/lua-gdextension/releases/tag/v2.1.0)
### Added
- Populate `pt_match_result.data.i` with the first syntactic error code when syntactic errors occur


## [v2.0.1](https://github.com/gilzoide/lua-gdextension/releases/tag/v2.0.1)
### Fixed
- Put `extern "C"` declarations in inner headers.


## [v2.0.0](https://github.com/gilzoide/lua-gdextension/releases/tag/v2.0.0)
### Changed
- ABI change on `pt_match_options`
- Included Case Insensitive and Character Class Expressions (the old Custom Matcher)
- Changed Custom Matcher Expressions to allow operating on strings, also receiving userdata


## [v1.2.7](https://github.com/gilzoide/lua-gdextension/releases/tag/v1.2.7)
### Changed
- Removed all the Action sequence computation, as Actions are already stacked in the right sequence.
  Running actions is now iterative, O(n) and use far less memory.


## [v1.2.6](https://github.com/gilzoide/lua-gdextension/releases/tag/v1.2.6)
### Fixed
- Fixed `SEQ` and `OR` expression macros on C++, turns out they behave differently about temporary lifetime of arrays.


## [v1.2.5](https://github.com/gilzoide/lua-gdextension/releases/tag/v1.2.5)
### Fixed
- Fixed `SEQ` and `OR` expression macros to compile on both C and C++ using preprocessor macros and `initializer_list` directly on `macro-on.h`.


## [v1.2.4](https://github.com/gilzoide/lua-gdextension/releases/tag/v1.2.4)
### Fixed
- Added `extern "C"` declaration on `pega-texto.h` for using in C++. 


## [v1.2.3](https://github.com/gilzoide/lua-gdextension/releases/tag/v1.2.3)
### Fixed
- Fixed validation error code emmited when `pt_is_nullable` returned true, as it may find an error other than `PT_VALIDATE_LOOP_EMPTY_STRING`.


## [v1.2.2](https://github.com/gilzoide/lua-gdextension/releases/tag/v1.2.2)
### Fixed
- Added `NULL` string check on match.


## [v1.2.1](https://github.com/gilzoide/lua-gdextension/releases/tag/v1.2.1)
### Fixed
- Fixed validation error on empty `SEQ` and `OR` Expressions, which are valid with a `NULL` pointer.


## [v1.2.0](https://github.com/gilzoide/lua-gdextension/releases/tag/v1.2.0)
### Chaged
- Macros for Expressions to not own memory buffers
- Empty `SEQ` and `OR` Expressions don't allocate a 0-byte buffer anymore

### Fixed
- Fixed validation error on Non-terminal cycles.


## [v1.1.1](https://github.com/gilzoide/lua-gdextension/releases/tag/v1.1.1)
### Fixed
- Fixed validation error on Non-terminal cycles.


## [v1.1.0](https://github.com/gilzoide/lua-gdextension/releases/tag/v1.1.0)
### Added
- Basic error handling support.


## [v1.0.0](https://github.com/gilzoide/lua-gdextension/releases/tag/v1.0.0)
### Added
- Expressions, Grammars, parsing, validation, actions.

