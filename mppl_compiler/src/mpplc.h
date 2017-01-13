#ifndef _MPPLC_H_
#define _MPPLC_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// #include "ll_parse.h"
#include "../../pretty_printer/src/ll_parse.h"

#define ERROR -1

#define NONE NULL
#define gr0 "gr0"
#define gr1 "gr1"
#define gr2 "gr2"
#define gr3 "gr3"
#define gr4 "gr4"
#define gr5 "gr5"
#define gr6 "gr6"
#define gr7 "gr7"

// #define OBJFALSE "0"
#define OBJTRUE "1"

#define LABEL_LEN 5
#define MPROGRAM 0
#define MVARIABLE 1
#define MSUBPROGRAM 2
#define MPARAMETER 3

struct REQUIRE_DATA{
    char *label;
    char *data;
    struct REQUIRE_DATA *next;
};

/* label */
extern char *create_label(void);
extern char *name_label(int type, char *name, char *subname);
extern void set_label(char *label);

/* set data requiring work area */
extern void set_required(char *label, char *str);

/* Assembler instructions */
extern void START(char *label, char *addr);
extern void END(void);
extern void DS(int wordnum);
extern void DC(char *constant);
/* Macro instructions */
extern void IN(char *input_area, char *len_area);
extern void OUT(char *output_area, char *len_area);
extern void RPUSH(void);
extern void RPOP(void);
/* Machine language instructions */
extern void LD_rr(char *r1, char *r2);
extern void LD_rm(char *r, char *addr, char *x);
extern void ST(char *r, char *addr, char *x);
extern void LAD(char *r, char *addr, char *x);
extern void ADDA_rr(char *r1, char *r2);
extern void ADDA_rm(char *r, char *addr, char *x);
extern void ADDL_rr(char *r1, char *r2);
extern void ADDL_rm(char *r, char *addr, char *x);
extern void SUBA_rr(char *r1, char *r2);
extern void SUBA_rm(char *r, char *addr, char *x);
extern void SUBL_rr(char *r1, char *r2);
extern void SUBL_rm(char *r, char *addr, char *x);
extern void MULA_rr(char *r1, char *r2);
extern void MULA_rm(char *r, char *addr, char *x);
extern void MULL_rr(char *r1, char *r2);
extern void MULL_rm(char *r, char *addr, char *x);
extern void DIVA_rr(char *r1, char *r2);
extern void DIVA_rm(char *r, char *addr, char *x);
extern void DIVL_rr(char *r1, char *r2);
extern void DIVL_rm(char *r, char *addr, char *x);
extern void AND_rr(char *r1, char *r2);
extern void AND_rm(char *r, char *addr, char *x);
extern void OR_rr(char *r1, char *r2);
extern void OR_rm(char *r, char *addr, char *x);
extern void XOR_rr(char *r1, char *r2);
extern void XOR_rm(char *r, char *addr, char *x);
extern void CPA_rr(char *r1, char *r2);
extern void CPA_rm(char *r, char *addr, char *x);
extern void CPL_rr(char *r1, char *r2);
extern void CPL_rm(char *r, char *addr, char *x);
extern void SLA(char *r, char *addr, char *x);
extern void SRA(char *r, char *addr, char *x);
extern void SLL(char *r, char *addr, char *x);
extern void SRL(char *r, char *addr, char *x);
extern void JPL(char *addr, char *x);
extern void JMI(char *addr, char *x);
extern void JNZ(char *addr, char *x);
extern void JZE(char *addr, char *x);
extern void JOV(char *addr, char *x);
extern void JUMP(char *addr, char *x);
extern void PUSH(char *addr, char *x);
extern void POP(char *r);
extern void CALL(char *addr, char *x);
extern void RET(void);
extern void SVC(char *addr, char *x);
extern void NOP(void);

/* lib */
extern char *itoa(int n);

#endif //_MPPLC_H_
