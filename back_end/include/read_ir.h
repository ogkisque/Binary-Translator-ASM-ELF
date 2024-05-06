#ifndef READ_IR_HEADER
#define READ_IR_HEADER

#include <stdlib.h>

#include "tree.h"
#include "error.h"
#include "colors.h"

const int           IR_MAX_NUM_COMMANDS     = 1000;
const int           IR_MAX_NUM_VARS         = 100;
const char* const   IR_STACK_REG            = "r15";
const char* const   IR_TMP_REG1             = "r10";
const char* const   IR_TMP_REG2             = "r11";
const char* const   IR_FUNC_PRINT_NAME      = "my_print";
const char* const   IR_FUNC_INPUT_NAME      = "my_input";
const int           IR_MAX_CMD_OPCODE_SIZE  = 20;
const int           IR_MAX_NUM_LABELS       = 100;
const int           IR_MAX_LABEL_NAME_SIZE  = 20;

enum IR_CommandType
{
    IR_MOV              = 0,
    IR_ADD              = 1,
    IR_SUB              = 2,
    IR_MUL              = 3,
    IR_DIV              = 4,
    IR_CMP              = 5,
    IR_JMP              = 6,
    IR_JE               = 7,
    IR_JNE              = 8,
    IR_JG               = 9,
    IR_JGE              = 10,
    IR_JL               = 11,
    IR_JLE              = 12,
    IR_FUNC             = 13,
    IR_PUSH             = 14,
    IR_POP              = 15,
    IR_PRINT            = 16,
    IR_INPUT            = 17,
    IR_RET              = 18,
    IR_CQO              = 19,
    IR_LABEL_CMD        = 20,
    IR_MOV_RAM          = 21,
    IR_MOV_RAX          = 22,
    IR_SYSCALL          = 23
};

enum IR_DataType
{
    IR_NONE                 = 0,
    IR_REG                  = 1,
    IR_MEM                  = 2,
    IR_VAR                  = 3,
    IR_NUM                  = 4,
    IR_LABEL_IF             = 5,
    IR_LABEL_WHILE_START    = 6,
    IR_LABEL_WHILE_STOP     = 7,
    IR_STACK                = 8
};

struct IR_LabelOffset
{
    IR_DataType type;
    int         num;
    int         offset;
};


struct IR_DataValue
{
    char  name[10];
    int   value;
};

struct IR_Command
{
    IR_CommandType  cmd_type;
    IR_DataType     data_type1;
    IR_DataValue    data_value1;
    IR_DataType     data_type2;
    IR_DataValue    data_value2;

    unsigned char   opcode[IR_MAX_CMD_OPCODE_SIZE];
    int             num_bytes;
    int             offset;
};

struct IR_Function
{
    IR_Command*     commands;
    size_t          num_commands;   
    char*           name;
    int             vars[IR_MAX_NUM_VARS];
    int             num_vars;
    int             cur_command;
    int             cur_if;
    int             cur_while;

    int             offset;
    int             num_opcodes;
    IR_LabelOffset  labels[IR_MAX_NUM_LABELS];
    int             num_labels;
};

struct IR_Struct
{
    IR_Function**   functions;
    size_t          num_functions;
};

Error   ir_ctor                 (IR_Struct* ir, Functions* funcs);
Error   ir_dtor                 (IR_Struct* ir);
Error   ir_fill                 (IR_Struct* ir, Functions* funcs);
Error   ir_fill_func            (IR_Function* ir_func, const Node* node);
Error   ir_fill_func_args       (IR_Function* ir_func, const Node* node);
Error   ir_fill_func_cmds       (IR_Function* ir_func, const Node* node);
Error   ir_push_func_call_args  (IR_Function* ir_func, const Node* node, int num_var);
Error   ir_add_cmd_oper         (IR_Function* func, IR_CommandType cmd_type);
Error   ir_add_cmd_if           (IR_Function* func, const Node* node);
Error   ir_add_cmd_while        (IR_Function* func, const Node* node);
Error   ir_add_cmd              (IR_Function* func, IR_CommandType cmd_type,
                                 IR_DataType type1, const char* name1, int val1,
                                 IR_DataType type2, const char* name2, int val2);
Error   ir_add_cmd_opcode       (IR_Function* func);
Error   ir_fill_adrs            (IR_Struct* ir);
Error   ir_fill_jmp_adr         (IR_Function* func, int curr_cmd);
Error   ir_fill_call_adr        (IR_Struct* ir, IR_Function* func, int curr_cmd);
Error   ir_add_mov_ram          (IR_Function* func);
Error   ir_add_syscall          (IR_Function* func);

Error   ir_dump                 (IR_Struct* ir);
Error   ir_func_dump            (IR_Function* ir_func);

int     get_num_not_zero_bytes  (char opcode[]);
void    get_hex_num             (IR_Command* cmd, int num);
int     ir_get_num_var          (const char* name);
void    ir_add_var              (IR_Function* ir_func, const char* name);

#endif //READ_IR_HEADER