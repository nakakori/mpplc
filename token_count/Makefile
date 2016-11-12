# Makefile for tc

# Redefine Macro
CC	= gcc
CFLAGS	= -Wall -Wextra
# Target
TRGTDIR	= ./bin
TARGET	= ${TRGTDIR}/tc
# Source
SRCDIR	= ./src
SOURCE	= ${wildcard ${SRCDIR}/*.c}
# Objects
ROBJDIR = ./obj
OBJDIR	= ${ROBJDIR}/${SRCDIR}
OBJECTS	= ${addprefix ${OBJDIR}/, ${notdir ${SOURCE:.c=.o}}}

.PHONY: all clean

all:	${TARGET}

test:
	@make -f TestMakefile

${TARGET}:	${OBJECTS}
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

clean:
	${RM} ${OBJECTS} ${TARGET}
