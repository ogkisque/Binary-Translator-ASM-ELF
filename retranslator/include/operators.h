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
};

const Operator OPERATORS[] = {  {"+",               OPER, ADD,          "+"},
                                {"-",               OPER, SUB,          "-"},
                                {"*",               OPER, MUL,          "*"},
                                {"/",               OPER, DIV,          "/"},
                                {"(",               OPER, LBRAC,        "("},
                                {")",               OPER, RBRAC,        ")"},
                                {"sin",             OPER, SIN,          "sin"},
                                {"cos",             OPER, COS,          "cos"},
                                {"tg",              OPER, TG,           "tg"},
                                {"arctg",           OPER, ARCTG,        "arctg"},
                                {"arcsin",          OPER, ARCSIN,       "arcsin"},
                                {"arccos",          OPER, ARCCOS,       "arccos"},
                                {"^",               OPER, POW,          "^"},
                                {"sqrt",            OPER, SQRT,         "sqrt"},
                                {"ln",              OPER, LN,           "ln"},
                                {"aka",             OPER, ASSIGN,       "="},
                                {";",               OPER, END_STR,      ";"},
                                {"choppa",          OPER, IF,           "if"},
                                {"smoke",           OPER, WHILE,        "while"},
                                {"lesss_go",        OPER, START_CODE,   "{"},
                                {"stop_that_shit",  OPER, END_CODE,     "}"},
                                {">",               OPER, MORE,         ">"},
                                {">=",              OPER, MORE_EQ,      ">="},
                                {"<",               OPER, LESS,         "<"},
                                {"<=",              OPER, LESS_EQ,      "<="},
                                {"==",              OPER, EQUAL,        "=="},
                                {"!=",              OPER, NOT_EQUAL,    "!="},
                                {"freestyle",       OPER, PRINT,        "print"},
                                {"houmi",           OPER, NEW_FUNC,     "def"},
                                {"glock",           OPER, RET,          "return"},
                                {",",               OPER, COMMA,        ","},
                                {"megera",          OPER, INPUT_VAR,    "input"}};

#endif //OPERATORS_HEADER
