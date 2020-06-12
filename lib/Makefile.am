.PRECIOUS: %.xs %.xo %.xs3 %.xo3

EXTRA_DIST = $(MODULES)

CLEANFILES = *.xs *.xo *.xa *.xs3 *.xo3 stamp-stage2 stamp-stage3

MODULES = \
	fgets.c \
	fput0x.c \
	fputc.c \
	fputd.c \
	fputs.c \
	gets.c \
	isalnum.c \
	isalpha.c \
	iscntrl.c \
	isdigit.c \
	isgraph.c \
	islower.c \
	isprint.c \
	ispunct.c \
	isspace.c \
	isupper.c \
	isxdigit.c \
	main.c \
	puts.c \
	strcat.c \
	strchr.c \
	strcmp.c \
	strcpy.c \
	strlen.c \
	strncat.c \
	strncmp.c \
	strncpy.c \
	strrchr.c \
	tolower.c \
	toupper.c

#
# STAGE 2 *PRIMARY TARGET*
# XLibrary compiled and constructed with native
#

%.xs: %.c
	../xcc -S $@ $<

%.xo: %.xs
	../xasm -c $@ $<
	../xar a ../xlib.xa $@

stamp-stage2: $(subst .c,.xo,$(MODULES))
	touch $@

stage2-local: stamp-stage2
	@echo "Stage-2 XLibrary complete"

#
# STAGE 3 
# XLibrary compiled, constructed and compares with stage-2
#

%.xs3: %.c
	../xemu ../xcc -S $@ $<
	cmp $*.xs3 $*.xs

%.xo3: %.xs3
	../xemu ../xasm -c $@ $<
	cmp $*.xo3 $*.xo
	../xemu ../xar a stage3-xlib.xa $@

stamp-stage3: $(subst .c,.xo3,$(MODULES))
	cmp stage3-xlib.xa ../xlib.xa
	touch $@

stage3-local: stamp-stage3
	@echo "Stage-3 XLibrary complete"

stage3-clean :
	rm *.xs3 *.xo3 stage3-xlib.xa
