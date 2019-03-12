For this experiment, I'll use pega-texto Regex-like syntax described in `Re.c`.
The product of the parsers will be a pega-texto grammar. Time will be
benchmarked in the parsing only, grammar creation will be an exercise to
compare the usage of the parsed data between tools.

Used tools: pega-texto, flex + yacc, lpeg, leg


Grammar
-------
This grammar should parse itself, so it will also be used as input.
```
Grammar <- S Definition+ !.
Definition <- Identifier Arrow Exp

Exp <- Seq ("/" S Seq)*
Seq <- Prefix+
Prefix <- ([&!] S)? Suffixed
Suffixed <- Primary S Suffix?
Suffix <- [+*?] S
        / "^" Number

Primary <- "(" S Exp ")" S
         / "{" S Exp "}" S
         / Defined
         / Literal
         / CaseInsensitive
         / CharacterSet
         / "." S
         / Identifier !Arrow

Literal <- String
CaseInsensitive <- "I" String
String <- '"' (!'"' Character)* '"' S
        / "'" (!"'" Character)* "'" S
CharacterSet <- "[" "^"? Item (!"]" Item)* "]" S
Item <- Defined / Range / .
Range <- . "-" [^]]
Character <- "\\" [abfnrtv'"\[\]\\]
           / "\\" [0-2][0-7][0-7]
           / "\\" [0-7][0-7]?
           / .
Defined <- "\\" [wWaAcCdDgGlLpPsSuUxX] S

S <- (\s / "#" [^\n]*)*
Identifier <- [A-Za-z_][A-Za-z0-9_-]* S
Arrow <- "<-" S
Number <- [+-]? \d+ S
```
