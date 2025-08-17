
- [ ] Automated tests
- [ ] Better documentation
- [X] Handle sized strings (pointer + size pairs)
- [X] Special common operations like "Not {Element,Literal,Set,Range}"?
      Nope, that isn't really necessary, much simpler to just support the Not expression.
- [ ] Add `#ifdef` guarded tracing logs to match algorithm, for debugging purposes
- [ ] Support for streaming FILEs?
      (I think streaming at points where failing would end the match anyway is
      safe, retrying match if fail and there is more content to be read,
      [stringstream style](https://github.com/gilzoide/stringstream-lua))
      (Streaming actions, on the other hand, would be quite a challenge)
- [ ] Ability to pass a preallocated buffer in pt_match_options
- [ ] Add `#ifdef` guarded event callbacks to match algorithm, making debuggers possible
- [ ] Make an interactive visualizer for the match algorithm
      (This is gonna be interesting, quite challenging)
