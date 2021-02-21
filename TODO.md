
- [ ] Automated tests
- [ ] Better documentation
- [ ] Handle sized strings (pointer + size pairs)
- [ ] Special common operations like "Not {Element,Literal,Set,Range}"?
- [ ] Add `#ifdef` guarded tracing logs to match algorithm, for debugging purposes
- [ ] Support for streaming FILEs?
      (I think streaming at points where failing would end the match anyway is
      safe, retrying match if fail and there is more content to be read,
      [stringstream style](https://github.com/gilzoide/stringstream-lua))
      (Streaming actions, on the other hand, would be a real challenge)
- [ ] Ability to pass a preallocated buffer in pt_match_options
- [ ] Add `#ifdef` guarded event callbacks to match algorithm, making debuggers possible
- [ ] Make an interactive visualizer for the match algorithm
      (This is gonna be interesting, quite challenging)
