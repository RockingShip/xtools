# XTools

X-C-Compiler/Assembler/Linker/Archiver.

An experimental toolchain for supporting the [untangle](/RockingShip/untangle) fractal engine.
It is based on the remastered 1991 school project [XTools-historic](/RockingShip/xtools-historic).

Known issues:
 - Pointer arithmetic. The only working variant is "&arr[ofs]"`.
 - Initializers.

## About the project

In my early education days of computer science there was a "PSO" (practical system design) assignment which required the 
design, implementation and testing of a emulated CPU and MMU unit. 
Personally not challenging so to up the ante and in agreement with school staff I received permission to
extend the testing by implementing a turing-complete compiler/assembler/linker/archiver toolchain.
Modifications to the incomplete instruction set were also permitted as long as they were backwards compatible. 
The biggest challenge was fitting the toolchain in 64k memory with most instructions being 5-6 bytes.


```xcc``` was heavily inspired and partly based on  ```Small-C``` and Digital-Equipment's TOPS-20 ```"REL blocks"```.
[```Small-C```](https://en.wikipedia.org/wiki/Small-C) is a minimalistic ```C``` compiler for resource-limited environments and 
[```"REL blocks"```](https://www.livingcomputers.org/UI/UserDocs/TOPS-20-v7-1/3_LINK_Reference_Manual.pdf) (chapter 1 and appendix A) which are basically a mix of code and post-fix operators to evaluate and resolve symbol references.

### ```xcc``` language features:

 - Preprocessor ```#include```, ```#define``` and ```#asm```
 - No structures (use arrays with named offsets)
 - No ```typedef```
 - Datatypes ```char```, ```int``` and "(single) pointer to"
 - Signed ints only
 - ```"sizeof(int)==sizeof(char*)"``` (allows for de-referencing of ints)
 - No heap or ```malloc()``` (pointer stack based variables might be an alternative)
 - Peephole optimiser

### `"-Dint=long"`

`xtools` needs to be compilable and runnable without modifications both with a native compiler (`gcc`) and the sandboxed self.
`xcc` assumes and is designed for architectures where `"sizeof(int)>=sizeof(char*))"` because pointers are stores in ints.

The only problem on architectures where this differs (like x86-64) are function calls.
Variables will be passed as 64 bits values, however constants as 32 bits.

Back in the day all calling arguments would be pushed on the stack, constants would therefore unsync the stack. 
Modern calling standards pass the first 6 arguments through registers, so the first six arguments are 'safe'.

However, because they are in registers, the pushed constant will be in the lower 32 bits, the upper 32 set to zero.
This will break the signedness of arguments.   

The phenomena is best explained with this code snippet:
```
    long main(argc, argv)
    {
        test(+1L, -1L, +1, -1);
        return 0;
    }

    test(a, b, c, d)
    long a,b,c,d;
    {
        printf("%lx %lx %lx %lx\n", a, b, c, d);

        if (a > 0) printf("a=%lx\n", a);
        if (b > 0) printf("b=%lx\n", b);
        if (c > 0) printf("c=%lx\n", c);
        if (d > 0) printf("d=%lx\n", d);
    }
```

 - The "`main`" is placed before "`test()`" so the compiler cannot implicitly protoize `"test()"`.
 - Traditional C function declarations. (Yes, `main(argc,argv)` is a valid declaration).
 - Four function arguments, two for 32/64 bits and two for set/clear sign bit.
 - `if` statements that test sign bit. 
 
Output after compiling with `"gcc -traditional-cpp -fno-inline"`:

````
    1 ffffffffffffffff 1 ffffffff
    a=1
    c=1
    d=ffffffff
````

The second arguments shows a sign extended 64 bit number, the forth only filled with lower 32 bits.

Solution for this issue is to manually sign extend function arguments where `SBIT` is the sign bit of your choosing.
For native this is 31 and for xtools 15.
```
  arg |= -(arg & (1 << SBIT));
```  

Revised source for `"test()"`

```
test(a, b, c, d)
long a,b,c,d;
{
        a |= -(a & (1<<31));
        b |= -(b & (1<<31));
        c |= -(c & (1<<31));
        d |= -(d & (1<<31));

        printf("%lx %lx %lx %lx\n", a, b, c, d);

        if (a > 0) printf("a=%lx\n", a);
        if (b > 0) printf("b=%lx\n", b);
        if (c > 0) printf("c=%lx\n", c);
        if (d > 0) printf("d=%lx\n", d);
}
```

Revised output:
```
    1 ffffffffffffffff 1 ffffffffffffffff
```
  
  
  
### `REL` language

`REL` objects are used to construct assembler output and executables.
It is a conceptually different approach of both traditional `a.out` and modern `ELF`. 

`a.out/ELF` both see code/data as binary blobs with fixup tables. 
These tables contain instructions on how to fix parts of these blobs that are position/location dependent.

`REL` blocks can be seen as expression trees where the nodes are opcodes pointing to other nodes, symbol, references or binary blobs.
The result after "evaluating" the tree is the executable image.

For example, The binary represention of a call to subroutine would be the binary value for the opcode "CALL" followed by some addressing mode representing the address of the function.
Say the opcode is `0x12 0x44` and the function name resolving to a 32 bits value.

In `a.out/ELF` this would be represented by `"0x12 0x44 0x00 0x00 0x00 0x00"` and a fixup that the 32 bits value of the label be added at offset+2.

With `REL` it would look like:
```
   <data 0x12 0x44> <push addrof-label> <pop 32-bits>`
```

`REL` also allows complex expressions of symbols and values (because of the postfix `REL` operators) but also conditional linking like:
```
   <blob for some code>
   <blob code to prepare, call and handle result of some function> 
   <blob code when function does not exist>
   <push result of "test if-label-exists">
   <pop result and 2 blobs and push the one for condition "TRUE"`>
   ...
```

`REL` trees contain a mix of multi-sized data blobs and instructions on how, if and where to place and use them.
It also allows for post-processing where blobs can be merged when conditionals fold to constants.
This feature is essential when integrating with the `untangle` engine.

### Project layout

```
    <root>
    +-- pso/     # school assigned emulator
    +-- source/  # xcc,xasm,xlnk,xar 
    +-- lib/     # X-library 
    \-- xemu.c   # native emulator
```

`xcc` assumes 16-bit/MSB architecture.
The project was originally developed using the 16-bits Zortech C/C++ development environment.

The function `disp_opc()` in `xemu.c` is an opcode cheat sheet.

## Getting Started

These instructions will get the project up and running on your local machine for development and testing purposes.

### Prerequisites

The school assignment is written in traditional ```C++``` and ```xcc``` written in minimalistic ```C```.

The programs `xcc/xasm/xlnk/xar` assume that ```"sizeof(int)==sizeof(char*)"```. 
Pointers are stored in ints which need need some creativity to work with 64-bit architecture. 

This requires the compiler flags `"-Dint=long"`, possibly `"-fno-inline"` and some magic. 

### Installing

The makefile supports a number of targets:

```
    make         # Build all the native programs
    make stage1  # Same as 'make'
    make stage2  # Build 'xtools/xlib` with emulated stage1 
    make stage3  # Build 'xtools/xlib` with emulated stage2 and compare result with stage2
```

After stage 2 the following files should be present:
```
    xcc.img     X-C-Compiler
    xasm.img    X-Asselmber
    xlnk.img    X-Linker
    xar.img     X-Archiver
    xlib.xa     X-Library to be used with "xlnk ... -l xlib"
    xemu        Native emulator
```

The try:

```
    ./xemu xcc cattle.c
    ./xemu xasm cattle
    ./xemu xlnk cattle -l xlib
    ./xemu cattle
```

Included example is my 1996 IOCCC submission: [Bulls and Cows](https://en.wikipedia.org/wiki/Bulls_and_Cows),
where you think of a secret number and the program tries to guess.
It's originally a one-liner, extra whitespace included for readability.
Anything can be programmed into a for-loop.

Compile test program. 
With gcc you would issue: "gcc cattle.c -traditional-cpp"

```
    # compile
    ./xemu xcc cattle
    # assemble
    ./xemu xasm cattle
    # link
    ./xemu xlnk cattle -l xlib
```

Run test program. 

```
    # run (think of secret number 0291)
    ./xemu cattle
    1123  # program displays first guess
    02    # you answer with "02". No digits at correct position, '2' and '1' present
    7912  # program displays second guess
    03    # you answer with "03". No digits at correct position, '2', '9' and '1' present
    9237  # program displays third guess
    11    # you answer with "11". The '2' is correct, '1' is present
    0291  # program displays fourth guess
    40    # you answer with "40". The guess is correct
```

## Versioning

We use [SemVer](http://semver.org/) for versioning.

## License

This project is licensed under the GPLv3 License - see the [LICENSE.md](LICENSE.md) file for details

## Acknowledgments

* Ron Cain, Jim E. Hendrix and many others for Small-C. Hat tip for placing in public domain!
* Digital Equipment for amazingly life path inspiring hardware and software
