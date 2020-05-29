ACLOCAL_AMFLAGS = -I m4
EXTRA_DIST = LICENSE.txt CHANGELOG.md README.md .gitignore autogen.sh cattle.c ostest.c

AM_CFLAGS =  -Dint=long -fno-inline -w
CLEANFILES = *.xs *.xo *.xa *.xs3 *.xo3 *.img *.map

# Directory order
SUBDIRS = lib .

.PRECIOUS: %.xs %.xo %.xs3 %.xo3

#
# STAGE 1
# Native binaries
#

bin_PROGRAMS = xemu xcc xasm xlnk xar

xemu_SOURCES = xemu.c
xemu_CFLAGS =

xcc_SOURCES = xcc.c getversion.c
xasm_SOURCES = xasm.c getversion.c
xlnk_SOURCES = xlnk.c getversion.c
xar_SOURCES = xar.c getversion.c

ostest.img : ostest.xo lib/main.xo xlnk
	./xlnk -m ostest.map -o $@ ostest.xo lib/main.xo

test-ostest : ostest.img
	./xemu ostest.img

#
# STAGE 2 *PRIMARY TARGET*
# XTools compiled with native
#

%.xs: %.c
	./xcc -S $@ $<

%.xo: %.xs
	./xasm -c $@ $<

%.img: %.xo
	./xlnk -o $@ $^ -l xlib

xcc.img : xcc.xo getversion.xo
	./xlnk -m xcc.map -o $@ $^ -l xlib.xa

xasm.img : xasm.xo getversion.xo
	./xlnk -m xasm.map -o $@ $^ -l xlib.xa

xlnk.img : xlnk.xo getversion.xo
	./xlnk -m xlnk.map -o $@ $^ -l xlib.xa

xar.img : xar.xo getversion.xo
	./xlnk -m xar.map -o $@ $^ -l xlib.xa

#
# STAGE 3
# XTools compiled and compared with stage-2
#

%.xs3: %.c
	./xemu xcc -S $@ $<
	cmp $*.xs3 $*.xs

%.xo3: %.xs3
	./xemu xasm -c $@ $<
	cmp $*.xo3 $*.xo

stage3-xcc.img : xcc.xo3 getversion.xo3
	./xemu xlnk -m xcc.map -o $@ $^ -l xlib.xa
	cmp stage3-xcc.img xcc.img

stage3-xasm.img : xasm.xo3 getversion.xo3
	./xemu xlnk -m xasm.map -o $@ $^ -l xlib.xa
	cmp stage3-xasm.img xasm.img

stage3-xlnk.img : xlnk.xo3 getversion.xo3
	./xemu xlnk -m xlnk.map -o $@ $^ -l xlib.xa
	cmp stage3-xlnk.img xlnk.img

stage3-xar.img : xar.xo3 getversion.xo3
	./xemu xlnk -m xar.map -o $@ $^ -l xlib.xa
	cmp stage3-xar.img xar.img

#
# TOP_LEVEL
#


all-local:
	@echo 
	@echo "stage 1 has been built"
	@echo "xcc does not support the -I include path"
	@echo "run \"make stage2\" to build XTools/XLibrary in this directory"
	@echo "run \"make stage3\" to verify stage 2 can build itself and produce identical output"
	@echo 

stage1-local : xemu xcc xasm xlnk xar
	@echo "Stage-1 XTools complete"

stage2-local: xcc.img xasm.img xlnk.img xar.img
	@echo 
	@echo "The following files should be present:"
	@echo "  xcc.img   X-C-Compiler"
	@echo "  xasm.img  X-Asselmber"
	@echo "  xlnk.img  X-Linker"
	@echo "  xar.img   X-Archiver"
	@echo "  xlib.xa   X-Library to be used with \"xlnk ... -l xlib\""
	@echo "  xemu      Native emulator"
	@echo 
	@echo "Try:"
	@echo " ./xemu xcc  cattle.c"
	@echo " ./xemu xasm cattle"
	@echo " ./xemu xlnk cattle -l xlib"
	@echo " ./xemu cattle"
	@echo 

stage3-local: stage3-xcc.img stage3-xasm.img stage3-xlnk.img stage3-xar.img
	@echo 
	@echo "Compile/run the demo 'cattle.c'."
	@echo "What input the demo is waiting for is described in 'README.md'"
	@echo 
	@echo "  make cattle.img"
	@echo "  ./xemu cattle"
	@echo 
