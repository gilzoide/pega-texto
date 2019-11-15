

.Space # ('#' [^\n]*) / \s
  byte \#
  jmpf not_comment
loop_not_eol:
  nbyte 10 # \n
  jmpf loop_not_eol_end
  jmp loop_not_eol
loop_not_eol_end:
  succ
  jmp success
not_comment:
  cls \s
success:
  ret
