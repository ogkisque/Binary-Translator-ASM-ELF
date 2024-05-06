#ifndef IR_CODEGEN_HEADER
#define IR_CODEGEN_HEADER

#include "read_ir.h"
#include "tree.h"

const int IR_TABLE_SIZE = 12;
const int IR_OPCODES_TABLE_SIZE = 17;

struct Oper_to_ir
{
    Opers           type;
    IR_CommandType  ir_type;
};

struct IR_DataOpcode
{
        IR_DataType     data_type1;
        const char*     data_name1;
        int             data_val1;
        IR_DataType     data_type2;
        const char*     data_name2;
        int             data_val2;
        unsigned char   opcode[20];
        int             opcode_size;
};

struct IR_CmdOpcode
{
        IR_CommandType          cmd_type;
        IR_DataOpcode           opcodes[10];
        int                     num_opcodes;
};

const IR_CmdOpcode IR_TO_OPCODE_TABLE[] = {
        {IR_POP,  {{IR_REG, "r10",   0, IR_NONE, "",    0, {0x41, 0x5a},                2},     // pop r10
                   {IR_REG, "r11",   0, IR_NONE, "",    0, {0x41, 0x5b},                2},     // pop r11
                   {IR_REG, "rax",   0, IR_NONE, "",    0, {0x58},                      1},     // pop rax
                   {IR_MEM, "",      0, IR_NONE, "",    0, {0x41, 0x8f, 0x87},          7}      // pop [r15]   
                  }, 4},                                   
        {IR_PUSH, {{IR_REG, "r10",   0, IR_NONE, "",    0, {0x41, 0x52},                2},     // push r10      
                   {IR_REG, "r11",   0, IR_NONE, "",    0, {0x41, 0x53},                2},     // push r11
                   {IR_REG, "rax",   0, IR_NONE, "",    0, {0x50},                      1},     // push rax
                   {IR_MEM, "",      0, IR_NONE, "",    0, {0x41, 0xff, 0xb7},          7},     // push [r15]
                   {IR_NUM, "",      0, IR_NONE, "",    0, {0x68},                      5}      // push num
                  }, 5},   
        {IR_MUL,  {{IR_REG, "r10",   0, IR_REG,  "r11", 0, {0x4d, 0x0f, 0xaf, 0xd3},    4}      // imul r10, r11
                  }, 1},  
        {IR_DIV,  {{IR_REG, "r10",   0, IR_NONE, "",    0, {0x49, 0xf7, 0xfa},          3}      // idiv r10
                  }, 1},  
        {IR_ADD,  {{IR_REG, "r15",   0, IR_NUM,  "",    0, {0x49, 0x81, 0xc7},          7},     // add r15, 0
                   {IR_REG, "r10",   0, IR_REG,  "r11", 0, {0x4d, 0x01, 0xda},          3}      // add r10, r11
                  }, 2},   
        {IR_SUB,  {{IR_REG, "r15",   0, IR_NUM,  "",    0, {0x49, 0x81, 0xef},          7},     // sub r15, 0
                   {IR_REG, "r10",   0, IR_REG,  "r11", 0, {0x4d, 0x29, 0xda},          3}      // sub r10, r11
                  }, 2},                           
        {IR_CMP,  {{IR_REG, "r10",   0, IR_REG,  "r11", 0, {0x4d, 0x39, 0xda},          3}      // cmp r10, r11
                  }, 1},                           
        {IR_JMP,  {{IR_NONE, "",     0, IR_NONE, "",    0, {0xe9},                      5}      // jmp label
                  }, 1},                           
        {IR_JE,   {{IR_NONE, "",     0, IR_NONE, "",    0, {0x0f, 0x84},                6}      // je label
                  }, 1},                           
        {IR_JNE,  {{IR_NONE, "",     0, IR_NONE, "",    0, {0x0f, 0x85},                6}      // jne label
                  }, 1},                           
        {IR_JG,   {{IR_NONE, "",     0, IR_NONE, "",    0, {0x0f, 0x8f},                6}      // jg label
                  }, 1},                           
        {IR_JGE,  {{IR_NONE, "",     0, IR_NONE, "",    0, {0x0f, 0x8d},                6}      // jge label
                  }, 1},                           
        {IR_JL,   {{IR_NONE, "",     0, IR_NONE, "",    0, {0x0f, 0x8c},                6}      // jl label
                  }, 1},                           
        {IR_JLE,  {{IR_NONE, "",     0, IR_NONE, "",    0, {0x0f, 0x8e},                6}      // jle label
                  }, 1},                           
        {IR_RET,  {{IR_NONE, "",     0, IR_NONE, "",    0, {0xc3},                      1}      // ret
                  }, 1},                           
        {IR_FUNC, {{IR_NONE, "",     0, IR_NONE, "",    0, {0xe8},                      5}      // call func
                  }, 1},                           
        {IR_CQO,  {{IR_NONE, "",     0, IR_NONE, "",    0, {0x48, 0x99},                2}      // cqo
                  }, 1},
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
        "mov",          "add",         "sub",          "mul",
        "div",          "cmp",         "jmp",          "je",
        "jne",          "jg",          "jge",          "jl",
        "jle",          "func",        "push",         "pop",
        "print",        "input",       "ret",          "cqo",
        "label",        "mov RAM",     "mov rax",      "syscall"
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