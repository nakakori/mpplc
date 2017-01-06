#include "mpplc.h"

static FILE *fp;
static int number;

static int init_mpplc(char *filename);
static void end_compile(void);
static void release_labels(void);

static void error_obj(void);
static void eovf_obj(void);
static void e0div_obj(void);
static void erov_obj(void);

static void write_obj(void);
static void writechar_obj(void);
static void writestr_obj(void);
static void bovcheck_obj(void);
static void writeint_obj(void);
static void writebool_obj(void);
static void writeline_obj(void);
static void flush_obj(void);

static void read_obj(void);
static void readchar_obj(void);
static void readint_obj(void);
static void readline_obj(void);

int main(int argc, char *argv[]) {
    if(argc < 2){
        printf("File nam id not given.\n");
        return ERROR;
    }

    if(init_mpplc(argv[1]) == ERROR){
        printf("error initilize\n");
        return ERROR;
    }

    // if(parse_program() < 0){
    //     return ERROR;
    // }

    set_label(create_label());
    set_label(create_label());
    set_label(create_label());
    set_label(create_label());

    set_label(name_label(MPROGRAM, "sample", NONE));
    set_label(name_label(MVARIABLE, "var", NONE));
    set_label(name_label(MSUBPROGRAM, "sum", NONE));
    set_label(name_label(MPARAMETER, "n", "sum"));

    end_compile();

    printf("finish compile\n");

    return 0;
}

int init_mpplc(char *filename){
    char *file, *p;
    char extension[] = ".csl";
    int i;

    if((file = (char *)malloc(strlen(filename)+1)) == NULL){
        printf("can not malloc in init_mpplc\n");
        return ERROR;
    }

    strcpy(file, filename);
    if((p = strstr(file, ".mpl")) == NULL){
        printf("this file is not mppl file\n");
        return ERROR;
    }
    for(i = 0; i < 4; i++){
        p[i] = extension[i];
    }
    if((fp = fopen(file, "w")) == NULL){
        return ERROR;
    }

    number = 0;

    return 0;//init_ll_parse(filename);
}

void end_compile(void){
    error_obj();
    write_obj();
    read_obj();
    // end_ll_parse();
}

char *create_label(void){
    char *label;
    int i, num;

    if((label = (char *)malloc(LABEL_LEN+1)) == NULL){
        printf("can not malloc in create_label\n");
        return NULL;
    }
    num = ++number;
    label[0] = 'L';
    for(i = LABEL_LEN-1; i != 0; i--){
        int tmp = num % 10;
        num = num / 10;
        label[i] = '0' + tmp;
    }
    label[LABEL_LEN] = '\0';

    return label;
}

char *name_label(int type, char *name, char *subname){
    char *p;
    if(type == MPROGRAM){
        if((p = (char *)malloc(2+strlen(name)+1)) == NULL){
            printf("can not maloc in name_label\n");
            return NULL;
        }
        p[0] = p[1] = '$';
        strcpy(p+2,name);
    }else if(type == MVARIABLE || type == MSUBPROGRAM){
        if((p = (char *)malloc(1+strlen(name)+1)) == NULL){
            printf("can not maloc in name_label\n");
            return NULL;
        }
        p[0] = '$';
        strcpy(p+1,name);
    }else if(type == MPARAMETER){
        if((p = (char *)malloc(1+strlen(name)+1+strlen(subname)+1)) == NULL){
            printf("can not maloc in name_label\n");
            return NULL;
        }
        p[0] = '$';
        strcpy(p+1,name);
        p[1+strlen(name)] = '%';
        strcpy(p+1+strlen(name)+1, subname);
    }else{ // never pass
        return NULL;
    }

    return p;
}

void set_label(char *label){
    fprintf(fp, "%s\n", label);
}

/* Assembler instructions */
void START(char *addr){
    fprintf(fp, "\t%s\t", "START");
    if(addr != NONE){
        fprintf(fp, "%s\n", addr);
    }else{
        fprintf(fp, "\n");
    }
}

void END(void){
    fprintf(fp, "\t%s\n", "END");
}

void DS(int wordnum){
    fprintf(fp, "\t%s\t%d\n", "DS", wordnum);
}

void DC(char *constant){
    fprintf(fp, "\t%s\t%s\n", "DC", constant);
}

/* Macro instructions */
void IN(char *input_area, char *len_area){
    fprintf(fp, "\t%s\t%s, %s\n", "IN", input_area, len_area);
}

void OUT(char *output_area, char *len_area){
    fprintf(fp, "\t%s\t%s, %s\n", "OUT", output_area, len_area);
}

void RPUSH(void){
    fprintf(fp, "\t%s\n", "RPUSH");
}
void RPOP(void){
    fprintf(fp, "\t%s\n", "RPOP");
}

/* Machine language instructions */
void LD_rr(char *r1, char *r2){
    fprintf(fp, "\t%s\t%s, %s\n", "LD", r1, r2);
}

void LD_rm(char *r, char *addr, char *x){
    fprintf(fp, "\t%s\t%s, %s", "LD", r, addr);
    if(x != NONE){
        fprintf(fp, ", %s\n", x);
    }else{
        fprintf(fp, "\n");
    }
}

void ST(char *r, char *addr, char *x){
    fprintf(fp, "\t%s\t%s, %s", "ST", r, addr);
    if(x != NONE){
        fprintf(fp, ", %s\n", x);
    }else{
        fprintf(fp, "\n");
    }
}

void LAD(char *r, char *addr, char *x){
    fprintf(fp, "\t%s\t%s, %s", "LAD", r, addr);
    if(x != NONE){
        fprintf(fp, ", %s\n", x);
    }else{
        fprintf(fp, "\n");
    }
}

void ADDA_rr(char *r1, char *r2){
    fprintf(fp, "\t%s\t%s, %s\n", "ADDA", r1, r2);
}

void ADDA_rm(char *r, char *addr, char *x){
    fprintf(fp, "\t%s\t%s, %s", "ADDA", r, addr);
    if(x != NONE){
        fprintf(fp, ", %s\n", x);
    }else{
        fprintf(fp, "\n");
    }
}

void ADDL_rr(char *r1, char *r2){
    fprintf(fp, "\t%s\t%s, %s\n", "ADDL", r1, r2);
}

void ADDL_rm(char *r, char *addr, char *x){
    fprintf(fp, "\t%s\t%s, %s", "ADDL", r, addr);
    if(x != NONE){
        fprintf(fp, ", %s\n", x);
    }else{
        fprintf(fp, "\n");
    }
}

void SUBA_rr(char *r1, char *r2){
    fprintf(fp, "\t%s\t%s, %s\n", "SUBA", r1, r2);
}

void SUBA_rm(char *r, char *addr, char *x){
    fprintf(fp, "\t%s\t%s, %s", "SUBA", r, addr);
    if(x != NONE){
        fprintf(fp, ", %s\n", x);
    }else{
        fprintf(fp, "\n");
    }
}

void SUBL_rr(char *r1, char *r2){
    fprintf(fp, "\t%s\t%s, %s\n", "SUBL", r1, r2);
}

void SUBL_rm(char *r, char *addr, char *x){
    fprintf(fp, "\t%s\t%s, %s", "SUBL", r, addr);
    if(x != NONE){
        fprintf(fp, ", %s\n", x);
    }else{
        fprintf(fp, "\n");
    }
}

void MULA_rr(char *r1, char *r2){
    fprintf(fp, "\t%s\t%s, %s\n", "MULA", r1, r2);
}

void MULA_rm(char *r, char *addr, char *x){
    fprintf(fp, "\t%s\t%s, %s", "MULA", r, addr);
    if(x != NONE){
        fprintf(fp, ", %s\n", x);
    }else{
        fprintf(fp, "\n");
    }
}

void MULL_rr(char *r1, char *r2){
    fprintf(fp, "\t%s\t%s, %s\n", "MULL", r1, r2);
}

void MULL_rm(char *r, char *addr, char *x){
    fprintf(fp, "\t%s\t%s, %s", "MULL", r, addr);
    if(x != NONE){
        fprintf(fp, ", %s\n", x);
    }else{
        fprintf(fp, "\n");
    }
}

void DIVA_rr(char *r1, char *r2){
    fprintf(fp, "\t%s\t%s, %s\n", "DIVA", r1, r2);
}

void DIVA_rm(char *r, char *addr, char *x){
    fprintf(fp, "\t%s\t%s, %s", "DIVA", r, addr);
    if(x != NONE){
        fprintf(fp, ", %s\n", x);
    }else{
        fprintf(fp, "\n");
    }
}

void DIVL_rr(char *r1, char *r2){
    fprintf(fp, "\t%s\t%s, %s\n", "DIVL", r1, r2);
}

void DIVL_rm(char *r, char *addr, char *x){
    fprintf(fp, "\t%s\t%s, %s", "DIVL", r, addr);
    if(x != NONE){
        fprintf(fp, ", %s\n", x);
    }else{
        fprintf(fp, "\n");
    }
}

void AND_rr(char *r1, char *r2){
    fprintf(fp, "\t%s\t%s, %s\n", "AND", r1, r2);
}

void AND_rm(char *r, char *addr, char *x){
    fprintf(fp, "\t%s\t%s, %s", "AND", r, addr);
    if(x != NONE){
        fprintf(fp, ", %s\n", x);
    }else{
        fprintf(fp, "\n");
    }
}

void OR_rr(char *r1, char *r2){
    fprintf(fp, "\t%s\t%s, %s\n", "OR", r1, r2);
}

void OR_rm(char *r, char *addr, char *x){
    fprintf(fp, "\t%s\t%s, %s", "OR", r, addr);
    if(x != NONE){
        fprintf(fp, ", %s\n", x);
    }else{
        fprintf(fp, "\n");
    }
}

void XOR_rr(char *r1, char *r2){
    fprintf(fp, "\t%s\t%s, %s\n", "XOR", r1, r2);
}

void XOR_rm(char *r, char *addr, char *x){
    fprintf(fp, "\t%s\t%s, %s", "XOR", r, addr);
    if(x != NONE){
        fprintf(fp, ", %s\n", x);
    }else{
        fprintf(fp, "\n");
    }
}

void CPA_rr(char *r1, char *r2){
    fprintf(fp, "\t%s\t%s, %s\n", "CPA", r1, r2);
}

void CPA_rm(char *r, char *addr, char *x){
    fprintf(fp, "\t%s\t%s, %s", "CPA", r, addr);
    if(x != NONE){
        fprintf(fp, ", %s\n", x);
    }else{
        fprintf(fp, "\n");
    }
}

void CPL_rr(char *r1, char *r2){
    fprintf(fp, "\t%s\t%s, %s\n", "CPL", r1, r2);
}

void CPL_rm(char *r, char *addr, char *x){
    fprintf(fp, "\t%s\t%s, %s", "CPL", r, addr);
    if(x != NONE){
        fprintf(fp, ", %s\n", x);
    }else{
        fprintf(fp, "\n");
    }
}

void SLA(char *r, char *addr, char *x){
    fprintf(fp, "\t%s\t%s, %s", "SLA", r, addr);
    if(x != NONE){
        fprintf(fp, ", %s\n", x);
    }else{
        fprintf(fp, "\n");
    }
}

void SRA(char *r, char *addr, char *x){
    fprintf(fp, "\t%s\t%s, %s", "SRA", r, addr);
    if(x != NONE){
        fprintf(fp, ", %s\n", x);
    }else{
        fprintf(fp, "\n");
    }
}

void SLL(char *r, char *addr, char *x){
    fprintf(fp, "\t%s\t%s, %s", "SLL", r, addr);
    if(x != NONE){
        fprintf(fp, ", %s\n", x);
    }else{
        fprintf(fp, "\n");
    }
}

void SRL(char *r, char *addr, char *x){
    fprintf(fp, "\t%s\t%s, %s", "SRL", r, addr);
    if(x != NONE){
        fprintf(fp, ", %s\n", x);
    }else{
        fprintf(fp, "\n");
    }
}

void JPL(char *addr, char *x){
    fprintf(fp, "\t%s\t%s", "JPL", addr);
    if(x != NONE){
        fprintf(fp, ", %s\n", x);
    }else{
        fprintf(fp, "\n");
    }
}

void JMI(char *addr, char *x){
    fprintf(fp, "\t%s\t%s", "JMI", addr);
    if(x != NONE){
        fprintf(fp, ", %s\n", x);
    }else{
        fprintf(fp, "\n");
    }
}

void JNZ(char *addr, char *x){
    fprintf(fp, "\t%s\t%s", "JNZ", addr);
    if(x != NONE){
        fprintf(fp, ", %s\n", x);
    }else{
        fprintf(fp, "\n");
    }
}

void JZE(char *addr, char *x){
    fprintf(fp, "\t%s\t%s", "JZE", addr);
    if(x != NONE){
        fprintf(fp, ", %s\n", x);
    }else{
        fprintf(fp, "\n");
    }
}

void JOV(char *addr, char *x){
    fprintf(fp, "\t%s\t%s", "JOV", addr);
    if(x != NONE){
        fprintf(fp, ", %s\n", x);
    }else{
        fprintf(fp, "\n");
    }
}

void JUMP(char *addr, char *x){
    fprintf(fp, "\t%s\t%s", "JUMP", addr);
    if(x != NONE){
        fprintf(fp, ", %s\n", x);
    }else{
        fprintf(fp, "\n");
    }
}

void PUSH(char *addr, char *x){
    fprintf(fp, "\t%s\t%s", "PUSH", addr);
    if(x != NONE){
        fprintf(fp, ", %s\n", x);
    }else{
        fprintf(fp, "\n");
    }
}

void POP(char *r){
    fprintf(fp, "\t%s\t%s\n", "POP", r);
}

void CALL(char *addr, char *x){
    fprintf(fp, "\t%s\t%s", "CALL", addr);
    if(x != NONE){
        fprintf(fp, ", %s\n", x);
    }else{
        fprintf(fp, "\n");
    }
}

void RET(void){
    fprintf(fp, "\t%s\n", "RET");
}

void SVC(char *addr, char *x){
    fprintf(fp, "\t%s\t%s", "SVC", addr);
    if(x != NONE){
        fprintf(fp, ", %s\n", x);
    }else{
        fprintf(fp, "\n");
    }
}

void NOP(void){
    fprintf(fp, "\t%s\n", "NOP");
}

/* Run time error */
void error_obj(void){
    eovf_obj();
    e0div_obj();
    erov_obj();
}
/* Run time error : overflow */
void eovf_obj(void){
    set_label("EOVF");
    CALL("WRITELINE", NONE);
    LAD(gr1, "EOVF1", NONE);
    LD_rr(gr2, gr0);
    CALL("WRITESTR", NONE);
    CALL("WRITELINE", NONE);
    SVC("1", NONE);
    set_label("EOVF1");
    DC("'***** Run-Time Error : Overflow *****'");
}
/* Run time error : zero divide */
void e0div_obj(void){
    set_label("E0DIV");
    JNZ("EOVF", NONE);
    CALL("WRITELINE", NONE);
    LAD(gr1, "E0DIV1", NONE);
    LD_rr(gr2, gr0);
    CALL("WRITESTR", NONE);
    CALL("WRITELINE", NONE);
    SVC("2", NONE);
    set_label("E0DIV1");
    DC("'***** Run-Time Error : Zero-Divide *****'");
}
/* Run time error : range over in array index */
void erov_obj(void){
    CALL("WRITELINE", NONE);
    LAD(gr1, "EROV1", NONE);
    LD_rr(gr2, gr0);
    CALL("WRITESTR", NONE);
    CALL("WRITELINE", NONE);
    SVC("3", NONE);
    set_label("EROV1");
    DC("'***** Run-Time Error : Range-Over in Array Index *****'");
}

/* write object code */
void write_obj(void){
    writechar_obj();
    writestr_obj();
    bovcheck_obj();
    writeint_obj();
    writebool_obj();
    writeline_obj();
    flush_obj();
}
/* write character object code */
void writechar_obj(void){
    set_label("WRITECHAR");
    RPUSH();
    LD_rm(gr6, "SPACE", NONE);
    LD_rm(gr7, "OBUFSIZE", NONE);

    set_label("WC1");
    SUBA_rm(gr2, "ONE", NONE);
    JZE("WC2", NONE);
    JMI("WC2", NONE);
    ST(gr6, "OBUF", gr7);
    CALL("BOVFCHECK", NONE);
    JUMP("WC1", NONE);

    set_label("WC2");
    ST(gr1, "OBUF", gr7);
    CALL("BOVFCHECK", NONE);
    ST(gr7, "OBUFSIZE", NONE);
    RPOP();
    RET();
}
/* write string object code */
void writestr_obj(void){
    set_label("WRITESTR");
    RPUSH();
    LD_rr(gr6, gr1);

    set_label("WS1");
    LD_rm(gr4, "0", gr6);
    JZE("WS2", NONE);
    ADDA_rm(gr6, "ONE", NONE);
    SUBA_rm(gr2, "ONE", NONE);
    JUMP("WS1", NONE);

    set_label("WS2");
    LD_rm(gr7, "OBUFSIZE", NONE);
    LD_rm(gr5, "SPACE", NONE);

    set_label("WS3");
    SUBA_rm(gr2, "ONE", NONE);
    JMI("WS4", NONE);
    ST(gr5, "OBUF", gr7);
    CALL("BOVFCHECK", NONE);
    JUMP("WS3", NONE);

    set_label("WS4");
    LD_rm(gr4, "0", gr1);
    JZE("WS5", NONE);
    ST(gr4, "OBUF", gr7);
    ADDA_rm(gr1, "ONE", NONE);
    CALL("BOVFCHECK", NONE);
    JUMP("WS4", NONE);

    set_label("WS5");
    ST(gr7, "OBUFSIZE", NONE);
    RPOP();
    RET();
}
/* buffer overflow check routine object code */
void bovcheck_obj(void){
    set_label("BOVFCHECK");
    ADDA_rm(gr7, "ONE", NONE);
    CPA_rm(gr7, "BOVFLEVEL", NONE);
    JMI("BOVF1", NONE);
    CALL("WRITELINE", NONE);
    LD_rm(gr7, "OBUFSIZE", NONE);

    set_label("BOVF1");
    RET();

    set_label("BOVFLEVEL");
    DC("256");
}
/* write integer object code */
void writeint_obj(void){
    set_label("WRITEINT");
    RPUSH();
    LD_rr(gr7, gr0);
    CPA_rr(gr1, gr0);
    JPL("WI1", NONE);
    JZE("WI1", NONE);
    LD_rr(gr4, gr0);
    SUBA_rr(gr4, gr1);
    CPA_rr(gr4, gr1);
    JZE("WI6", NONE);
    LD_rr(gr1, gr4);
    LD_rm(gr7, "ONE", NONE);

    set_label("WI1");
    LD_rm(gr6, "SIX", NONE);
    ST(gr0, "INTBUF", gr6);
    SUBA_rm(gr6, "ONE", NONE);
    CPA_rr(gr1, gr0);
    JNZ("WI2", NONE);
    LD_rm(gr4, "ZERO", NONE);
    ST(gr4, "INTBUF", gr6);
    JUMP("WI5", NONE);

    set_label("WI2");
    CPA_rr(gr1, gr0);
    JZE("WI3", NONE);
    LD_rr(gr5, gr1);
    DIVA_rm(gr1, "TEN", NONE);
    LD_rr(gr4, gr1);
    MULA_rm(gr4, "TEN", NONE);
    SUBA_rr(gr5, gr4);
    ADDA_rm(gr5, "ZERO", NONE);
    ST(gr5, "INTBUF", gr6);
    SUBA_rm(gr6, "ONE", NONE);
    JUMP("WI2", NONE);

    set_label("WI3");
    CPA_rr(gr7, gr0);
    JZE("WI4", NONE);
    LD_rm(gr4, "MINUS", NONE);
    ST(gr4, "INTBUF", gr6);
    JUMP("WI5", NONE);

    set_label("WI4");
    ADDA_rm(gr6, "ONE", NONE);

    set_label("WI5");
    LAD(gr1, "INTBUF", gr6);
    CALL("WRITESTR", NONE);
    RPOP();
    RET();

    set_label("WI6");
    LAD(gr1, "MMINT", NONE);
    CALL("WRITESTR", NONE);
    RPOP();
    RET();

    set_label("MMINT");
    DC("'-32768'");
}
/* write boolean object code */
void writebool_obj(void){
    set_label("WRITEBOOL");
    RPUSH();
    CPA_rr(gr1, gr0);
    JZE("WB1", NONE);
    LAD(gr1, "WBTRE", NONE);
    JUMP("WB2", NONE);

    set_label("WB1");
    LAD(gr1, "WBFALSE", NONE);

    set_label("WB2");
    CALL("WRITESTR", NONE);
    RPOP();
    RET();

    set_label("WBTRUE");
    DC("'TRUE'");

    set_label("WBFALSE");
    DC("'FALSE'");
}
/* write new line object code */
void writeline_obj(void){
    set_label("WRITELINE");
    RPUSH();
    LD_rm(gr7, "OBUFSIZE", NONE);
    LD_rm(gr6, "NEWLINE", NONE);
    ST(gr6, "OBUF", gr7);
    ADDA_rm(gr7, "ONE", NONE);
    ST(gr7, "OBUFSIZE", NONE);
    OUT("OBUF", "OBUFSIZE");
    ST(gr0, "OBUFSIZE", NONE);
    RPOP();
    RET();
}
/* flush object code */
void flush_obj(void){
    set_label("FLUSH");
    RPUSH();
    LD_rm(gr7, "OBUFSIZE", NONE);
    JZE("FL1", NONE);
    CALL("WRITELINE", NONE);

    set_label("FL1");
    RPOP();
    RET();
}

/* read object code */
void read_obj(void){
    readchar_obj();
    readint_obj();
    readline_obj();
}
/* read character object code */
void readchar_obj(void){
    set_label("READCHAR");
    RPUSH();
    LD_rm(gr5, "RPBBUF", NONE);
    JZE("RC0", NONE);
    ST(gr5, "0", gr1);
    ST(gr0, "RPBBUF", NONE);
    JUMP("RC3", NONE);

    set_label("RC0");
    LD_rm(gr7, "INP", NONE);
    LD_rm(gr6, "IBUFSIZE", NONE);
    JNZ("RC1", NONE);
    IN("IBUF", "IBUFSIZE");
    LD_rr(gr7, gr0);

    set_label("RC1");
    CPA_rm(gr7, "IBUFSIZE", NONE);
    JNZ("RC2", NONE);
    LD_rm(gr5, "NEWLINE", NONE);
    ST(gr5, "0", gr1);
    ST(gr0, "IBUFSIZE", NONE);
    ST(gr0, "INP", NONE);
    JUMP("RC3", NONE);

    set_label("RC2");
    LD_rm(gr5, "IBUF", gr7);
    ADDA_rm(gr7, "ONE", NONE);
    ST(gr5, "0", gr1);
    ST(gr7, "INP", NONE);

    set_label("RC3");
    RPOP();
    RET();
}
/* read integer object code */
void readint_obj(void){
    set_label("READINT");
    RPUSH();

    set_label("RI1");
    CALL("READCHAR", NONE);
    LD_rm(gr7, "0", gr1);
    CPA_rm(gr7, "SPACE", NONE);
    JZE("RI1", NONE);
    CPA_rm(gr7, "TAB", NONE);
    JZE("RI1", NONE);
    CPA_rm(gr7, "NEWLINE", NONE);
    JZE("RI1", NONE);
    LD_rm(gr5, "ONE", NONE);
    CPA_rm(gr7, "MINUS", NONE);
    JNZ("RI2", NONE);
    LD_rr(gr5, gr0);
    CALL("READCHAR", NONE);
    LD_rm(gr7, "0", gr1);

    set_label("RI2");
    LD_rr(gr6, gr0);

    set_label("RI3");
    CPA_rm(gr7, "ZERO", NONE);
    JMI("RI4", NONE);
    CPA_rm(gr7, "NINE", NONE);
    JPL("RI4", NONE);
    MULA_rm(gr6, "TEN", NONE);
    ADDA_rr(gr6, gr7);
    SUBA_rm(gr6, "ZERO", NONE);
    CALL("READCHAR", NONE);
    LD_rm(gr7, "0", gr1);
    JUMP("RI3", NONE);

    set_label("RI4");
    ST(gr7, "RPBBUF", NONE);
    ST(gr6, "0", gr1);
    CPA_rr(gr5, gr0);
    JNZ("RI5", NONE);
    SUBA_rr(gr5, gr6);
    ST(gr5, "0", gr1);

    set_label("RI5");
    RPOP();
    RET();
}
/* read newline object code */
void readline_obj(void){
    set_label("READLINE");
    ST(gr0, "IBUFSIZE", NONE);
    ST(gr0, "INP", NONE);
    ST(gr0, "RPBBUF", NONE);
    RET();
}

/* use constant label */
void use_label_obj(){
    /* integer value label */
    set_label("ONE");
    DC("1");
    set_label("SIX");
    DC("6");
    set_label("TEN");
    DC("10");

    /* character label */
    set_label("SPACE");
    DC("#0020");
    set_label("MINUS");
    DC("#002D");
    set_label("TAB");
    DC("#0009");
    set_label("ZERO");
    DC("#0030");
    set_label("NINE");
    DC("#0039");
    set_label("NEWLINE");
    DC("#000A");

    /* other */
    set_label("INTBUF");
    DS(8);
    set_label("OBUFSIZE");
    DC("0");
    set_label("IBUFSIZE");
    DC("0");
    set_label("INP");
    DC("0");
    set_label("OBUF");
    DS(257);
    set_label("IBUF");
    DS(257);
    set_label("RPBBUF");
    DC("0");
}
