local lpeg = require "lpeglabel"
local re = require "relabel"

local grammar = re.compile([=[
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
Item <- Defined / Range / Character
Range <- . "-" [^]]
Character <- "\" [][abfnrtv'"\]
           / "\" [0-2][0-7][0-7]
           / "\" [0-7][0-7]?
           / .
Defined <- "\" [wWaAcCdDgGlLpPsSuUxX] S

S <- (%s / "#" [^%nl]*)*
Identifier <- [A-Za-z_][A-Za-z0-9_-]* S
Arrow <- "<-" S
Number <- [+-]? %d+ S
]=])

local input_file = arg[1] or "grammar.txt"
local content = assert(io.open(input_file)):read("*a")

local res, label, pos = grammar:match(content)
if res then
	print(res)
else
	local lin, col = re.calcline(content, pos)
	error(string.format("error at %d:%d", lin, col))
end
