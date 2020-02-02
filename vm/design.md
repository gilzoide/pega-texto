State
=====
- sp - String pointer, position of the current char in input stream
- ip - Instruction pointer, position of the current instruction in bytecode
- rf - Result flag, boolean indicating if match is failing
- qc - Quantifier counter

	
The bytecode
============
Opcode    | Argument | Description
----------|----------|------------
0x00 nop  |          | No operation
0x01 succ |          | Set `rf` with success status 1
0x02 fail |          | Set `rf` with fail status 0
0x03 flt  | 1 - Value expected | Set `rf` with fail status 0 if `qc` is less than the given expected value
0x04 qcz  |          | Set `qc` with success status 0
0x05 qci  |          | Increment `qc`

0x06 jr   | 1 - Offset  | Add offset to `ip`
0x07 jmp  | 2 - Address | Set `ip` to address
0x08 jrf  | 1 - Offset  | Add offset to `ip` if `rf` has failure status
0x09 jmpf | 2 - Address | Set `ip` to address if `rf` has failure status
0x0a call | 2 - Address | Push state and set `ip` to address, for non-terminal calls
0x0b ret  |          | Return from a non-terminal call

0x0c push |          | Push state into the state stack
0x0d peek |          | Reset state with state stack top
0x0e pop  |          | Pop state stack top

0x0f byte  | 1 - Byte | Match the given byte
0x10 nbyte | 1 - Byte | Match anything else than the given byte
0x11 str   | N - String literal | Match the given null terminated string
0x12 cls   | 1 - Char class identifier | Match character class from ctype functions
0x13 set   | N - Character set | Match byte with any from the given null terminated string
0x14 rng   | 2 - Minimum and maximum byte | Match within range

0x15 act  | 1 - Action index | Push action to be executed if match succeeds


PEG -> bytecode
===============
Being `T(e, L)` the transformation of expression `e` with failure label `L`

e1 e2:
```
T(e1, L)
T(e2, L)
```

e1 / e2:
```
  push
  T(e1, fail1)
  jmp success
fail1:
  peek
  T(e2, fail2)
  jmp success
fail2:
  peek
success:
  pop
  jmpf L
```

&e:
```
  push
  T(e, continue)
continue:
  peek
  pop
  jmpf L
```

!e:
```
  push
  T(e, fail)
  peek
  pop
  fail
  jmp L
fail:
  peek
  pop
  qcz
```

e?:
```
  push
  T(e, fail)
  jmp success
fail:
  peek
success:
  pop
  succ
```

e*:
```
expression:
  T(e, fail)
  jmp expression
fail:
  succ
```

e^N: (e+ -> e^1)
```
  push
  qcz
expression:
  T(e, fail)
  qci
  jmp expression
fail:
  flt N
  pop
```
