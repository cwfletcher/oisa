
.*

Contents of section \.data:
#------------------------------------------------------------------------------
#------ 00010000: foo
#------------------------------------------------------------------------------
 10000 (44332211 00800000 18800000|11223344 00008000 00008018)           .*
Contents of section \.got:
#------------------------------------------------------------------------------
#------ 00011000: .got.plt
#------ 00011004: reserved .got.plt entry
#------ 00011008: reserved .got.plt entry
#------ 0001100c: f1's .igot.plt entry [R_ARM_IRELATIVE]
#------------------------------------------------------------------------------
 11000 00000000 00000000 00000000 (00a00000|0000a000)  .*
#------------------------------------------------------------------------------
#------ 00011010: f3's .igot.plt entry [R_ARM_IRELATIVE]
#------ 00011014: f2's .igot.plt entry [R_ARM_IRELATIVE]
#------ 00011018: .got entry for foo
#------ 0001101c: .got entry for foo
#------------------------------------------------------------------------------
 11010 (08a00000 04a00000 00000100 00000100|0000a008 0000a004 00010000 00010000)  .*