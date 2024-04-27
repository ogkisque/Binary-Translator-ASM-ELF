#ifndef IR_CODEGEN_HEADER
#define IR_CODEGEN_HEADER

#include "read_ir.h"
#include "tree.h"

const int IR_TABLE_SIZE = 12;

struct Oper_to_ir
{
    Opers           type;
    IR_CommandType  ir_type;
};

const Oper_to_ir OPER_TO_IR_TABLE[] = {
        {ADD,           IR_ADD},
        {MUL,           IR_MUL},
        {SUB,           IR_SUB},
        {DIV,           IR_DIV},
        {ASSIGN,        IR_MOV},
        {RET,           IR_RET},
        {EQUAL,         IR_JNE},
        {NOT_EQUAL,     IR_JE},
        {MORE,          IR_JLE},
        {MORE_EQ,       IR_JL},
        {LESS,          IR_JGE},
        {LESS_EQ,       IR_JG}
};

const char* IR_DUMP_CMD_TYPE_TABLE[] = {
        "mov",   "add",   "sub",  "mul",
        "div",   "cmp",   "jmp",  "je",
        "jne",   "jg",    "jge",  "jl",
        "jle",   "func",  "push", "pop",
        "print", "input", "ret",  "cqo",
        "label"
};

const char* IR_DUMP_DATA_TYPE_TABLE[] = {
        "none",                 "register",             "memory",
        "variable",             "number",               "label_if",
        "label_while_start",    "label_while_stop",     "stack"
};

#define IR_CMD_NUM(func, num)                                                           \
        ir_add_cmd (func, IR_PUSH, IR_NUM, "", (int) num, IR_NONE, "", 0);                                          

#define IR_CMD_VAR(func, num_var)                                                       \
        ir_add_cmd (func, IR_PUSH, IR_MEM, "", (int) num_var * 8, IR_NONE, "", 0);                                                                                                     
                                   
#define IR_CMD_FUNC(func, name_func)                                                    \
        ir_add_cmd (func, IR_FUNC, IR_NONE, name_func, 0, IR_NONE, "", 0);  

#define IR_CMD_PUSH_REG(func, name_reg)                                                 \
        ir_add_cmd (func, IR_PUSH, IR_REG, name_reg, 0, IR_NONE, "", 0);                

#define IR_CMD_POP_REG(func, name_reg)                                                  \
        ir_add_cmd (func, IR_POP, IR_REG, name_reg, 0, IR_NONE, "", 0);                 

#define IR_CMD_CQO(func)                                                                \
        ir_add_cmd (func, IR_CQO, IR_NONE, "", 0, IR_NONE, "", 0); 

#define IR_CMD_CMP(func)                                                                \
        ir_add_cmd (func, IR_CMP, IR_REG, IR_TMP_REG1, 0, IR_REG, IR_TMP_REG2, 0); 

#endif //IR_CODEGEN_HEADER