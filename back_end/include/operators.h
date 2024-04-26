#ifndef OPERATORS_HEADER
#define OPERATORS_HEADER

#include "tree.h"

const size_t NUM_OPERS = 32;

struct Operator
{
    const char* name;
    Types       type;
    int         id;
    const char* name_to_print;
    const char* name_to_print_asm;
};

const Operator OPERATORS[] = {  {"+",               OPER, ADD,          "+",        "fadd"},
                                {"-",               OPER, SUB,          "-",        "fsub"},
                                {"*",               OPER, MUL,          "*",        "fmul"},
                                {"/",               OPER, DIV,          "/",        "fdiv"},
                                {"(",               OPER, LBRAC,        "(",        NULL},
                                {")",               OPER, RBRAC,        ")",        NULL},
                                {"sin",             OPER, SIN,          "sin",      NULL},
                                {"cos",             OPER, COS,          "cos",      NULL},
                                {"tg",              OPER, TG,           "tg",       NULL},
                                {"arctg",           OPER, ARCTG,        "arctg",    NULL},
                                {"arcsin",          OPER, ARCSIN,       "arcsin",   NULL},
                                {"arccos",          OPER, ARCCOS,       "arccos",   NULL},
                                {"^",               OPER, POW,          "^",        NULL},
                                {"sqrt",            OPER, SQRT,         "sqrt",     NULL},
                                {"ln",              OPER, LN,           "ln",       NULL},
                                {"aka",             OPER, ASSIGN,       "=",        NULL},
                                {";",               OPER, END_STR,      ";",        NULL},
                                {"choppa",          OPER, IF,           "if",       NULL},
                                {"smoke",           OPER, WHILE,        "while",    NULL},
                                {"lesss_go",        OPER, START_CODE,   "{",        NULL},
                                {"stop_that_shit",  OPER, END_CODE,     "}",        NULL},
                                {">",               OPER, MORE,         ">",        "ogt"},
                                {">=",              OPER, MORE_EQ,      ">=",       "oge"},
                                {"<",               OPER, LESS,         "<",        "olt"},
                                {"<=",              OPER, LESS_EQ,      "<=",       "ole"},
                                {"==",              OPER, EQUAL,        "==",       "oeq"},
                                {"!=",              OPER, NOT_EQUAL,    "!=",       "one"},
                                {"freestyle",       OPER, PRINT,        "print",    NULL},
                                {"houmi",           OPER, NEW_FUNC,     "def",      NULL},
                                {"glock",           OPER, RET,          "return",   NULL},
                                {",",               OPER, COMMA,        ",",        NULL},
                                {"megera",          OPER, INPUT_VAR,    "input",    NULL}};

#endif //OPERATORS_HEADER
