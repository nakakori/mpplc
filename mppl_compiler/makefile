# Makefile for mpplc

# Redefine Macro
CC	= gcc
CFLAGS	= -Wall -Wextra
# Target
TRGTDIR	= ./bin
TARGET	= ${TRGTDIR}/mpplc
TESTTAR	= ${TRGTDIR}/dmpplc
# Source
SRCDIR	= ./src
SOURCE	= ${wildcard ${SRCDIR}/*.c}
# Objects
ROBJDIR = ./obj
OBJDIR	= ${ROBJDIR}/${SRCDIR}
OBJECTS	= ${addprefix ${OBJDIR}/, ${notdir ${SOURCE:.c=.o}}}

#Used Source
LIBSRC = ../cross_reference/src/symbol_table.c ../pretty_printer/src/ll_parse.c ../pretty_printer/src/syntax_tree.c ../token_count/src/scan.c ../token_count/src/token-list.c

.PHONY: all clean

all:	${TARGET}

debug:	${TESTTAR}

test: ${TEST}

${TARGET}:	${OBJECTS} ${LIBSRC}
	@if [ ! -d ${TRGTDIR} ]; \
		then echo "Create binary directory"; mkdir -p ${TRGTDIR}; \
		fi
	@echo "Building target $@"
	${CC} -o $@ $^
	@echo "Finished building target: $@"

${TESTTAR}:  ${SRCDIR}/*.c ${LIBSRC}
	@if [ ! -d ${TRGTDIR} ]; \
		then echo "Create binary directory"; mkdir -p ${TRGTDIR}; \
		fi
	@echo "Building target $@"
	${CC} -o $@ -g $^
	@echo "Finished building target: $@"

${OBJDIR}/%.o: ${SRCDIR}/%.c
	@if [ ! -d ${ROBJDIR} ]; \
		then echo "Create objects directory."; mkdir -p ${ROBJDIR}; \
		fi
	@if [ ! -d ${OBJDIR} ]; \
		then mkdir -p ${OBJDIR}; \
		fi
	${CC} ${CFLAGS} -o $@ -c $<

clean:
	${RM} ${OBJECTS} ${TCOBJCTS} ${TARGET} ${TESTTAR}
