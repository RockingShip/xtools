# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/en/1.0.0/)
and this project adheres to [Semantic Versioning](http://semver.org/spec/v2.0.0.html).

## [Unreleased]

```
2020-06-14 22:57:31 Redesigned `constexpr()`.
                    Preperation for numeric instructions and evaluation stack.
                    With an evaluation stack, `lval[]` is not sufficient to fold constant expressions.
                    Drop `calc()`.
                    The 12 extra functions and removals generate 355 extra lines of assembler.
2020-06-14 00:46:31 Stack grows in incrementing `"(*sp++)"` direction (`untangle` migration).
                    Arguments are in same stack order as parameters (key feature).
                    Added linker symbol `___END`.
                    Stack starts at `___END`.
                    Image has header, starting with `jz.a r0,entry(r0)`.
                    Update `svc` calls in lib/main.c`.
                    Dropped linker `stksiz`.
2020-06-13 20:56:11 Switched from MSB to LSB (`untangle` migration).
                    Vectors start serially from bit 0 in increasing/growing direction.
                    Also, byte arguments start at same offset as words.
2020-06-13 19:38:33 Optimize `expr_ternary()` to better share final register.
                    Release final register after 'when-true' path so that 'when-false' path can reuse it.
2020-06-13 00:45:10 Optimize `expr_addsub()` for constant rvalue.
                    ADDRESS+constant does not require code.
2020-06-13 00:22:33 FIX: Stack adjustments for SP_AUTO should be in early `primary()`.
                    Were situations where `loadlval()` wrongly optimised because of late adjustment.
2020-06-13 00:18:16 FIX: Sign extend parameters of `calc()`.
                    And correct parameter order for `xasm` (oops).
2020-06-12 15:27:22 Drop support for variadic functions.
2020-06-12 15:07:39 Dropped usage of variadic functions including (sadly) `printf()/scanf()`.
                    Replaced `printf()` with `fputd()` for `printf("%d")` and `fput0x()` for `printf("%0x")`.
                    Adapted `cattle.c` for this change (aww).
2020-06-12 00:01:24 FIX: Use `genlabel()/genequ()` to generate labels.
                    Also reduce `fprintf()` usage.
```

## [Release 2.2.0] 2020-06-11 21:47:31

Classic RISC architecture and absence of condition code register.
16 data+8 flow machine instructions and 3 addressing modes.
Traditional-C. Keep it simple.

Known issues:
- Variable initializers

```
2020-06-11 21:27:29 Pointer Arithmetic.
                    Currently assuming only for `"int*"`.
                    Renaming.
2020-06-11 20:55:53 Restore usage `add_res()``.
                    Constructing strings goes against simplicity.
2020-06-11 20:49:23 Set `NAMEMAX` to 0x821=2081.
                    2081 is prime and has 3 bits set.
                    Multiply can be easily replaced with `"(x<<11) + (x<<5) + x"`
2020-06-11 20:21:32 Cleanup, optimise `poststep()`.
                    Dropped `lval_1[]` and `lval_BPW[]`.
                    Optimise `poststep()`.
2020-06-11 20:17:12 Instruction set: swap positions result/index register.
2020-06-11 00:58:32 Cleanup, remove all non-risc and condition code register.
                    Re-enumerate opcodes.
                    Rename functions.
                    Dropped reserved registers `REG_0/REG_1/REG_BPW`.
2020-06-11 00:55:59 FIX: constant folding for `<` in `expr_rel()`.
2020-06-10 22:44:40 FIX: `expr_postfix()` use `loadlval()` to merge registers.
                    Old code did not always work well with arrays.
2020-06-10 00:08:25 Adapt remaining flow-control to risc instructions.
                    Added `jsb/rsb/push/pshr/popr` risc instructions.
2020-06-09 21:38:38 Adapt unary `"-"` and `"~"` to risc instructions.
                    Added `neg/not` risc instructions.
2020-06-09 21:03:40 Simplify cascading in `expr_assign()`.
                    Also, continue with lvalue, not rvalue.
                    Added `and/or` risc instructions.
2020-06-09 18:14:03 Adapt `expr_muldiv()` to risc instructions.
                    Drop multiply of power 2 and convert to `lsl`.
                    Added `mul/div/mod/lsl/lsr` risc instructions.
2020-06-09 14:23:41 Dropped `Bcc/CMP/JMP` instructions and condition code register.
                    Condition code register requires a lot of runtime resources.
                    On a different level it also hinders parallelism and vectoring.
2020-06-09 13:21:57 Adapt `dumpsw()` to risc instructions.
                    Converted all of the `CMP/Bcc` instructions.
                    Split `gencode_risc` into `vlal[]` and raw variants.
2020-06-09 02:25:05 Optimize `expr_rel()` and `expr_equ()`.
                    Suppress `loadlval(rval)` if `gencode()` supports ea.
2020-06-09 01:19:36 Rename to `jz/jnz`.
                    Better descriptive.
2020-06-08 23:10:21 Adapt `expr_rel()` and `expr_equ()` to risc instructions.
                    Added `xor/sgt/slt` risc instructions.
2020-06-08 00:49:39 Statement flow-control to risc instructions.
2020-06-08 00:49:39 FIX: `reglock` includes `regresvd`.
                    `reglock` is the soft (local) version of `regresvd` (global).
2020-06-08 00:49:12 Adapt `expr_land()` ("&&") and `expr_lor()` ("||") to risc instructions.
                    `LTYPE=BRANCH` now uses `lval[LREG]` as register containing the condition.
                    Added `jeq/jne` risc instructions.
2020-06-07 21:04:29 FIX: makefile stage3.
                    .xo3 as stage3 extension results in a different hasttable causing linker to load in different order.
2020-06-07 19:16:22 FIX: unary "!".
                    Need to invert last instruction AND all prior by swapping T/F labels.
2020-06-06 01:18:01 Adapt `step()` to risc instructions.
                    Split into `prestep()` and `poststep()`.
                    Cannot optimize generated code until condition codes dropped.
                    Added `add/sub/ld/st` risc instructions.
                    Choose `step()` because it is the smallest and complex.
2020-06-05 20:27:27 Re-enumerate opcodes and relocate 0x6? block to 0x3?.
                    Free block to free a large range for risc opcodes.
2020-06-04 23:06:18 `pshr/popr` pair uses assembler "=" instead of ".org".
2020-06-04 22:40:44 Lowercase assembler.
```

## [Release 2.1.0] 2020-06-04 12:44:07

Simplified and bugfixed `sym[]` (identifier) and `lval[]` (peephole) structures.
More flexible pointer depth.
Dropped preprocessor and pointers to functions.

```
2020-06-04 12:19:44 Lost+Found.
                    After syncing with `xtools-historic 1.4.0`
2020-06-02 15:59:42 Synced post-increments to pre-increments.
                    And other tweaks that generate better. 
                    More optimal coding.
2020-06-02 15:35:10 Rename `LTYPE=VARIABLE` and convert to `LTYPE=MEMORY`.
                    Drop `LEA` as it is no longer needed.
2020-06-02 15:09:34 Consistent use of LTYPE=ADDRESS and register variables.
                    `ADDRESS/EA_ADDR` and `VARIABLE/EA_IND` should be consistent.
2020-06-02 13:59:21 Better implementation of ARRAY.
                    Address of arrays have an extra pointer indirection.
2020-06-02 12:48:40 Drop support for pointers to function.
                    Feature mixes code&data which is alien to `untangle`.
                    Previous commit contains functionality.
                    Reinstating might not work now because `LPTR` gets a different meaning.
2020-06-02 12:32:40 Drop use of pointers to function.
                    This is a strong flowcontrol feature and hardly used.
                    The "(*f)()" notation clashes with "..., int *f)".
2020-06-01 00:47:25 Replaced TYPE=ARRAY with LTYPE=ADDRESS.
                    This allows arrays of pointers.
2020-05-31 12:36:57 Support for and convert to LTYPE=ADDRESS.
                    Merging constant/register logic.
                    Simplify `lval[]` by merging `LTYPE/LEA`.
                    Dropped LTYPE=EXPR. 
2020-05-31 00:13:52 Added `gencode_lval()` which handles dynamic AUTO_SP adjustments.
                    This leaves `gencode_M()` as direct code.
2020-05-30 22:34:31 Drop back-end preprocessor.
                    Replace expansion buffer with streaming text.
2020-05-30 12:30:46 Drop front-end preprocessor.
                    Preprocessor is an ancient language hack.
```

## Release 2.0.0 2020-05-29 22:22:15

Initial commit.

[Unreleased]: /RockingShip/xtools/compare/v2.2.0...HEAD
[2.2.0]: /RockingShip/xtools-historic/compare/v2.1.0...v2.2.0
[2.1.0]: /RockingShip/xtools-historic/compare/v2.0.0...v2.1.0