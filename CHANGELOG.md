# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/en/1.0.0/)
and this project adheres to [Semantic Versioning](http://semver.org/spec/v2.0.0.html).

## [Unreleased]

```
2020-06-05 20:27:27 Re-enumerate opcodes and relocate 0x6? block to 0x3?.
                    Free block to free a large rance for risc opcodes.
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

[Unreleased]: /RockingShip/xtools/compare/v2.1.0...HEAD
[2.1.0]: /RockingShip/xtools-historic/compare/v2.0.0...v2.1.0