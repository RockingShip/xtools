@date 2020-06-04 16:52:41

# Rationale behind RISC design.

First steps towards a classic RISC architecture.

The effective address (EA) is the value of the immediate added with the contents of the indirect register.
The opcode determines how the effective address is applied.

The prior arithmetic instructions were register/register based.
Replacement arithmetic instructions with EA notation would look like "opcode Rdst,(Rsrc)".
Which implies that the EA is a value and not memory reference which makes this very inefficient.

To compensate and make more versatile, the instruction should also mention the data size.
If the size is zero then the EA is taken literally, 
otherwise it represents the memory location to read the given size of storage.

Currently the size is measured in bytes. 
This might change as untangle` is based on bits.   
 
First approach is the following instructions syntax and 32-bit word:
```  
    opcode+size regdest,immediate(regindirect)
    
    OPCODE.6 SIZE.2 REGDEST.4 IMMEDIATE.16 REGINDIRECT.4
```

# Conditionals

The RISC architecture does not have condition codes.
Conditional flow control needs to set a register followed by a conditional jump
that sets the next instruction to a given EA if the contents of the specified register is zero.

An unconditional jump would be `"j r0,<ea>"` as r0 is expected to be zero.

There are 6 support instruction to aid in the setting of the state register.

Code examples:

language   | code 
-----------+-----------
if(a)      | ld R,a
    block; | j  R,lbl
           | block;
           | lbl:

language   | code 
-----------+-----------
if (a > b) | ld R,a
    block  | gt R,b
           | j  R,lbl
           | block
           | lbl:
                
# Instruction set                

## Load/store

### Effective address

Calculate effective address:
    - Add immediate to contents of indirect register
    - r0 is writable but is expected to always contain zero.
    - If size is non-zero, load those many bytes from memory starting as EA  
      
### Load/store
    
ld.<size> <reg>,<ea>
    
    Load <ea> into register <reg>

st.<size> <reg>,<ea>
    
    Store contents <reg> to <ea>.
    NOTE: Size must be non-zero.
    NOTE: The concept of this instructions does not exist within `untangle`. (`pDATA` segment).

psh.<size> <ea>

    Push <ea> onto stack
    
pshr <ea>

    Push group of registers onto stack as described by the bits of <ea>
    
    "pshr 0x05" First pushes r0 followed by a push of r2.

popr <ea>

    Pops group registers from stack as described by the bits of <ea>
    
    "popr 0x05" First pops r2 followed by a pop of r0.

### Arithmetic
    
sub.<size> <reg>,<ea>
add.<size> <reg>,<ea>
mul.<size> <reg>,<ea>
div.<size> <reg>,<ea>
mod.<size> <reg>,<ea>
or.<size> <reg>,<ea>
and.<size> <reg>,<ea>
xor.<size> <reg>,<ea>
lsl.<size> <reg>,<ea>
lsr.<size> <reg>,<ea>
   
   Perform a requested arithmetic operation:

    <reg> = <reg> <operation> <ea>
    
not.<size> <reg>,<ea>
neg.<size> <reg>,<ea>
   
   Perform a requested arithmetic operation:

    <reg> = <operation> <ea>

### Flow control

j <reg>,<ea>

    If contents of <reg> is zero, set next instruction location to <ea>

jsb <ea>

    Call subroutine. 
    <reg> is reserved and should be zero
    
rsb <ea>

    Return from subroutine. 
    <ea> is the size of the return value.
    <reg> is reserved and should be zero

### Relational

teq.size <reg>,<ea>
tne.size <reg>,<ea>
tgt.size <reg>,<ea>
tle.size <reg>,<ea>
tge.size <reg>,<ea>
tlt.size <reg>,<ea>

    Perform "<reg> <condition> <ea>"
    
    If result is "true" set all bits of <reg> to "0" else set all bits to "1".

### Special

illegal

    Flag an error situation.
    <reg> and <ea> are advised to be zero otherwise anything goes.
    Note: <ea> is calculated and should be valid

svc <reg>,<ea>

    System call.
    <reg> is a memory location containing the arguments to the system call
    <ea> is the enumeration identifying the system call 
