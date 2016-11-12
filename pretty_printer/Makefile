# Makefile for pp

# Redefine Macro
CC	= gcc
CFLAGS	= -Wall -Wextra
# Target
TRGTDIR	= ./bin
TARGET	= ${TRGTDIR}/pp
# Source
SRCDIR	= ./src
SOURCE	= ${wildcard ${SRCDIR}/*.c}
# Objects
ROBJDIR = ./obj
OBJDIR	= ${ROBJDIR}/${SRCDIR}
OBJECTS	= ${addprefix ${OBJDIR}/, ${notdir ${SOURCE:.c=.o}}}

#Used Project
TCSRCDIR= ../token_count/src
TCSOURCE= ${wildcard ${TCSRCDIR}/*.c}
TCOBJDIR= ${ROBJDIR}/tmp
TCOBJCTS= ${addprefix ${TCOBJDIR}/, ${notdir ${TCSOURCE:.c=.o}}}

.PHONY: all clean

all:	${TARGET}

test:
	@make -f TestMakefile

${TARGET}:	${OBJECTS} ${TCOBJCTS}
	@if [ ! -d ${TRGTDIR} ]; \
		then echo "Create binary directory"; mkdir -p ${TRGTDIR}; \
		fi
	@echo "Building target $@"
	${CC} -o $@ $^
	@echo "Finished building target: $@"

${OBJDIR}/%.o: ${SRCDIR}/%.c
	@if [ ! -d ${ROBJDIR} ]; \
		then echo "Create objects directory."; mkdir -p ${ROBJDIR}; \
		fi
	@if [ ! -d ${OBJDIR} ]; \
		then mkdir -p ${OBJDIR}; \
		fi
	${CC} ${CFLAGS} -o $@ -c $<

${TCOBJDIR}/%.o: ${TCSRCDIR}/%.c
	@if [ ! -d ${ROBJDIR} ]; \
		then echo "Create objects directory."; mkdir -p ${ROBJDIR}; \
		fi
	@if [ ! -d ${TCOBJDIR} ]; \
		then mkdir -p ${TCOBJDIR}; \
		fi
	${CC} ${CFLAGS} -o $@ -c $<

clean:
	${RM} ${OBJECTS} ${TARGET}
