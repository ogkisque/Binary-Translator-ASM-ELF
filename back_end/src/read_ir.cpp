#include "read_ir.h"
#include "ir_codegen.h"

Error ir_ctor (IR_Struct* ir, Functions* funcs)
{
    if (!ir)
        RETURN_ERROR(NULL_POINTER, "Null pointer of ir struct");
    if (!funcs)
        RETURN_ERROR(NULL_POINTER, "Null pointer of funcs struct");
    
    ir->num_functions   = funcs->num_funcs;
    ir->functions       = (IR_Function**) calloc (funcs->num_funcs, sizeof (IR_Function*));
    if (!(ir->functions))
        RETURN_ERROR(MEM_ALLOC, "Error with allocation memory for ir struct");

    for (int i = 0; i < funcs->num_funcs; i++)
    {
        ir->functions[i] = (IR_Function*) calloc (1, sizeof (IR_Function));
        ir->functions[i]->commands = (IR_Command*) calloc (IR_MAX_NUM_COMMANDS, sizeof (IR_Command));
        if (!(ir->functions[i]->commands))
            RETURN_ERROR(MEM_ALLOC, "Error with allocation memory for ir function struct");

        ir->functions[i]->num_vars  = 0;
    }

    RETURN_ERROR(CORRECT, "");
}

Error ir_dtor (IR_Struct* ir)
{
    if (!ir)
        RETURN_ERROR(NULL_POINTER, "Null pointer of ir struct");

    for (int i = 0; i < ir->num_functions; i++)
    {
        free (ir->functions[i]->commands);
        free (ir->functions[i]);
    }

    free (ir->functions);

    RETURN_ERROR(CORRECT, "");
}

Error ir_fill (IR_Struct* ir, Functions* funcs)
{
    if (!ir)
        RETURN_ERROR(NULL_POINTER, "Null pointer of ir struct");
    if (!funcs)
        RETURN_ERROR(NULL_POINTER, "Null pointer of funcs struct");
    if (!(ir->functions))
        RETURN_ERROR(NULL_POINTER, "Null pointer of funcs in ir struct. Do ctor");

    Error error = {};
    ir->functions[0]->offset = 0;
    for (int i = 0; i < ir->num_functions; i++)
    {
        if (i > 0)
            ir->functions[i]->offset = ir->functions[i-1]->offset + ir->functions[i-1]->num_opcodes;
        ir->functions[i]->name = funcs->funcs[i]->root->name;

        error = ir_fill_func (ir->functions[i], funcs->funcs[i]->root);
        PARSE_ERROR_WITHOUT_TREE(error);
    }

    error = ir_fill_adrs (ir);

    RETURN_ERROR(CORRECT, "");
}

Error ir_fill_func (IR_Function* ir_func, const Node* node)
{
    Error error = {};

    ir_func->cur_if     = 0;
    ir_func->cur_while  = 0;

    if (strcmp (ir_func->name, "main") == 0)
    {
        error = ir_add_mov_ram (ir_func);
        PARSE_ERROR_WITHOUT_TREE(error);
    }

    error = ir_fill_func_args (ir_func, node);
    PARSE_ERROR_WITHOUT_TREE(error);

    error = ir_fill_func_cmds (ir_func, node->right);
    PARSE_ERROR_WITHOUT_TREE(error);

    if (strcmp (ir_func->name, "main") == 0)
    {
        error = ir_add_syscall (ir_func);
        PARSE_ERROR_WITHOUT_TREE(error);
    }

    ir_func->num_opcodes = ir_func->commands[ir_func->cur_command - 1].offset +
                           ir_func->commands[ir_func->cur_command - 1].num_bytes;

    RETURN_ERROR(CORRECT, "");
}

Error ir_fill_func_args (IR_Function* ir_func, const Node* node)
{
    if (!node)
        RETURN_ERROR(CORRECT, "");

    Error error = {};

    if (node->type == VAR)
        ir_add_var (ir_func, node->name);
    
    error = ir_fill_func_args (ir_func, node->left);
    PARSE_ERROR_WITHOUT_TREE(error);
    error = ir_fill_func_args (ir_func, node->right);
    PARSE_ERROR_WITHOUT_TREE(error);

    RETURN_ERROR(CORRECT, "");
}

Error ir_fill_func_cmds (IR_Function* ir_func, const Node* node)
{
    if (!node)
        RETURN_ERROR(CORRECT, "");

    Error error = {};

    if (node->type == OPER && (int) node->value == END_STR)
    {
        error = ir_fill_func_cmds (ir_func, node->left);
        PARSE_ERROR_WITHOUT_TREE(error);
        error = ir_fill_func_cmds (ir_func, node->right);
        PARSE_ERROR_WITHOUT_TREE(error);
        RETURN_ERROR(CORRECT, "");
    }

    if (node->type == NUM)
    {
        IR_CMD_NUM(ir_func, node->value)
    }
    else if (node->type == VAR)
    {
        IR_CMD_VAR(ir_func, ir_get_num_var(node->name))
    }
    else if (node->type == FUNC)
    {
        error = ir_push_func_call_args (ir_func, node->left, 0);
        PARSE_ERROR_WITHOUT_TREE(error);

        ir_add_cmd (ir_func, IR_ADD, IR_REG, IR_STACK_REG, 0, IR_NUM, "", 8 * ir_func->num_vars);
        IR_CMD_FUNC(ir_func, node->name)
        ir_add_cmd (ir_func, IR_SUB, IR_REG, IR_STACK_REG, 0, IR_NUM, "", 8 * ir_func->num_vars);
    }
    else //OPER
    {
        if ((int) node->value == IF)
        {
            error = ir_add_cmd_if (ir_func, node);
            PARSE_ERROR_WITHOUT_TREE(error);
            RETURN_ERROR(CORRECT, "");
        }

        if ((int) node->value == WHILE)
        {
            error = ir_add_cmd_while (ir_func, node);
            PARSE_ERROR_WITHOUT_TREE(error);
            RETURN_ERROR(CORRECT, "");
        }

        if ((int) node->value == ASSIGN)
        {
            error = ir_fill_func_cmds (ir_func, node->left);
            PARSE_ERROR_WITHOUT_TREE(error);

            ir_add_cmd (ir_func, IR_POP, IR_MEM, "", ir_get_num_var(node->right->name) * 8, IR_NONE, "", 0);
            RETURN_ERROR(CORRECT, "");
        }

        if ((int) node->value == PRINT)
        {
            error = ir_fill_func_cmds (ir_func, node->right);
            PARSE_ERROR_WITHOUT_TREE(error);

            ir_add_cmd (ir_func, IR_POP, IR_MEM, "", 8 * ir_func->num_vars, IR_NONE, "", 0);

            ir_add_cmd (ir_func, IR_ADD, IR_REG, IR_STACK_REG, 0, IR_NUM, "", 8 * ir_func->num_vars);
            ir_add_cmd (ir_func, IR_FUNC, IR_NONE, IR_FUNC_PRINT_NAME, 0, IR_NONE, "", 0);
            ir_add_cmd (ir_func, IR_SUB, IR_REG, IR_STACK_REG, 0, IR_NUM, "", 8 * ir_func->num_vars);
        }

        if ((int) node->value == INPUT_VAR)
        {
            ir_add_cmd (ir_func, IR_FUNC, IR_NONE, IR_FUNC_INPUT_NAME, 0, IR_NONE, "", 0);
            ir_add_cmd (ir_func, IR_POP, IR_MEM, "", 8 * ir_get_num_var (node->right->name), IR_NONE, "", 0);
        }

        for (int i = 0; i < IR_TABLE_SIZE; i++)
        {
            if (OPER_TO_IR_TABLE[i].type  == (int) node->value)
            {
                error = ir_fill_func_cmds (ir_func, node->left);
                PARSE_ERROR_WITHOUT_TREE(error);
                error = ir_fill_func_cmds (ir_func, node->right);
                PARSE_ERROR_WITHOUT_TREE(error);

                error = ir_add_cmd_oper (ir_func, OPER_TO_IR_TABLE[i].ir_type);
                PARSE_ERROR_WITHOUT_TREE(error);
            }
        }
    }

    RETURN_ERROR(CORRECT, "");
}

Error ir_push_func_call_args (IR_Function* ir_func, const Node* node, int num_var)
{
    if (!node)
        RETURN_ERROR(CORRECT, "");

    Error error = {};

    error = ir_fill_func_cmds (ir_func, node->left);
    PARSE_ERROR_WITHOUT_TREE(error);

    ir_add_cmd (ir_func, IR_POP, IR_MEM, "", 8 * ir_func->num_vars + 8 * num_var, IR_NONE,  "", 0);

    error = ir_push_func_call_args (ir_func, node->right, num_var + 1);
    PARSE_ERROR_WITHOUT_TREE(error);

    RETURN_ERROR(CORRECT, "");
}

Error ir_add_cmd_oper (IR_Function* func, IR_CommandType cmd_type)
{
    if (cmd_type == IR_ADD || cmd_type == IR_SUB || cmd_type == IR_MUL)
    {
        IR_CMD_POP_REG(func, IR_TMP_REG2)
        IR_CMD_POP_REG(func, IR_TMP_REG1)
        ir_add_cmd (func, cmd_type, IR_REG, IR_TMP_REG1, 0, IR_REG, IR_TMP_REG2, 0);
        IR_CMD_PUSH_REG(func, IR_TMP_REG1)
    }
    else if (cmd_type == IR_DIV)
    {
        IR_CMD_POP_REG(func, IR_TMP_REG1)
        IR_CMD_POP_REG(func, "rax")
        IR_CMD_CQO(func)
        ir_add_cmd (func, cmd_type, IR_REG, IR_TMP_REG1, 0, IR_NONE, "", 0);
        IR_CMD_PUSH_REG(func, "rax")
    }
    else if (cmd_type == IR_RET)
    {
        IR_CMD_POP_REG(func, IR_TMP_REG1)

        IR_CMD_POP_REG(func, IR_TMP_REG2)
        IR_CMD_PUSH_REG(func, IR_TMP_REG1)
        IR_CMD_PUSH_REG(func, IR_TMP_REG2)

        ir_add_cmd (func, IR_RET, IR_NONE, "", 0, IR_NONE, "", 0);
        RETURN_ERROR(CORRECT, "");
    }
    else
    {
        RETURN_ERROR(UNKNOWN_OPER, "Unknown operation");
    }

    RETURN_ERROR(CORRECT, "");
}

Error ir_add_cmd_if (IR_Function* func, const Node* node)
{
    if (!node)
        RETURN_ERROR(NULL_POINTER, "Null pointer of 'if' node");
    
    Error error = {};
    error = ir_fill_func_cmds (func, node->left->left);
    PARSE_ERROR_WITHOUT_TREE(error);
    error = ir_fill_func_cmds (func, node->left->right);
    PARSE_ERROR_WITHOUT_TREE(error);

    IR_CMD_POP_REG(func, IR_TMP_REG2)
    IR_CMD_POP_REG(func, IR_TMP_REG1)

    IR_CMD_CMP(func)

    int cond = (int) node->left->value;
    for (int i = 0; i < IR_TABLE_SIZE; i++)
    {
        if (OPER_TO_IR_TABLE[i].type  == cond)
        {
            int cur_if = func->cur_if;
            ir_add_cmd (func, OPER_TO_IR_TABLE[i].ir_type, IR_LABEL_IF, "", cur_if, IR_NONE, "", 0);
            func->cur_if++;

            error = ir_fill_func_cmds (func, node->right);
            PARSE_ERROR_WITHOUT_TREE(error);

            ir_add_cmd (func, IR_LABEL_CMD, IR_LABEL_IF, "", cur_if, IR_NONE, "", 0);
            RETURN_ERROR(CORRECT, "");
        }
    }

    RETURN_ERROR(CORRECT, "");
}

Error ir_add_cmd_while (IR_Function* func, const Node* node)
{
    if (!node)
        RETURN_ERROR(NULL_POINTER, "Null pointer of 'if' node");
    
    Error error = {};

    int cur_while = func->cur_while;
    ir_add_cmd (func, IR_LABEL_CMD, IR_LABEL_WHILE_START, "", cur_while, IR_NONE, "", 0);

    error = ir_fill_func_cmds (func, node->left->left);
    PARSE_ERROR_WITHOUT_TREE(error);
    error = ir_fill_func_cmds (func, node->left->right);
    PARSE_ERROR_WITHOUT_TREE(error);

    IR_CMD_POP_REG(func, IR_TMP_REG2)
    IR_CMD_POP_REG(func, IR_TMP_REG1)

    IR_CMD_CMP(func)

    int cond = (int) node->left->value;
    for (int i = 0; i < IR_TABLE_SIZE; i++)
    {
        if (OPER_TO_IR_TABLE[i].type  == cond)
        {
            ir_add_cmd (func, OPER_TO_IR_TABLE[i].ir_type, IR_LABEL_WHILE_STOP, "", cur_while, IR_NONE, "", 0);
            func->cur_while++;

            error = ir_fill_func_cmds (func, node->right);
            PARSE_ERROR_WITHOUT_TREE(error);

            ir_add_cmd (func, IR_JMP, IR_LABEL_WHILE_START, "", cur_while, IR_NONE, "", 0);
            ir_add_cmd (func, IR_LABEL_CMD, IR_LABEL_WHILE_STOP, "", cur_while, IR_NONE, "", 0);
            RETURN_ERROR(CORRECT, "");
        }
    }

    RETURN_ERROR(CORRECT, "");
}

Error ir_add_cmd (IR_Function* func, IR_CommandType cmd_type,
                 IR_DataType type1, const char* name1, int val1,
                 IR_DataType type2, const char* name2, int val2)
{
    Error error = {};
    func->commands[func->cur_command].cmd_type = cmd_type;

    func->commands[func->cur_command].data_type1 = type1;
    strcpy (func->commands[func->cur_command].data_value1.name, name1);
    func->commands[func->cur_command].data_value1.value = val1;

    func->commands[func->cur_command].data_type2 = type2;
    strcpy (func->commands[func->cur_command].data_value2.name, name2);
    func->commands[func->cur_command].data_value2.value = val2;
    
    if (cmd_type != IR_LABEL_CMD)
    {
        error = ir_add_cmd_opcode (func);
        PARSE_ERROR_WITHOUT_TREE(error);
    }
    else
    {
        if (func->cur_command != 0)
            func->commands[func->cur_command].offset = func->commands[func->cur_command - 1].offset + 
                                                       func->commands[func->cur_command - 1].num_bytes;
        else
            func->commands[func->cur_command].offset = 0;

        func->labels[func->num_labels].type = func->commands[func->cur_command].data_type1;
        func->labels[func->num_labels].num = func->commands[func->cur_command].data_value1.value;
        func->labels[func->num_labels].offset = func->commands[func->cur_command].offset;
        func->num_labels++;
    }

    func->cur_command++;                                             
    func->num_commands++;

    RETURN_ERROR(CORRECT, "");
}

Error ir_add_cmd_opcode (IR_Function* func)
{
    if (!func)
        RETURN_ERROR(NULL_POINTER, "Null pointer of ir func");

    IR_Command* cmd = &(func->commands[func->cur_command]);
    for (int i = 0; i < IR_OPCODES_TABLE_SIZE; i++)
    {
        IR_CmdOpcode opcode_struct = IR_TO_OPCODE_TABLE[i];
        if (cmd->cmd_type == opcode_struct.cmd_type)
        {
            if (opcode_struct.num_opcodes == 1)
            {
                cmd->num_bytes = opcode_struct.opcodes[0].opcode_size;
                memcpy (cmd->opcode, opcode_struct.opcodes[0].opcode, cmd->num_bytes);
            }
            else
            {
                for (int j = 0; j < opcode_struct.num_opcodes; j++)
                {
                    if (opcode_struct.opcodes[j].data_type1 == cmd->data_type1 &&
                        strcmp (opcode_struct.opcodes[j].data_name1, cmd->data_value1.name) == 0)
                    {
                        cmd->num_bytes = opcode_struct.opcodes[j].opcode_size;
                        memcpy (cmd->opcode, opcode_struct.opcodes[j].opcode, cmd->num_bytes);

                        if (cmd->data_type1 == IR_NUM || cmd->data_type1 == IR_MEM)
                            get_hex_num (cmd, cmd->data_value1.value);
                        else if (cmd->data_type2 == IR_NUM)
                            get_hex_num (cmd, cmd->data_value2.value);
                    }
                }
            }

            if (func->cur_command == 0)
                cmd->offset = 0;
            else
                cmd->offset = func->commands[func->cur_command - 1].offset +
                              func->commands[func->cur_command - 1].num_bytes;
            
            RETURN_ERROR(CORRECT, "");
        }
    }

    RETURN_ERROR(UNKNOWN_CMD, "Unknown cmd for opcodes");
}

Error ir_fill_adrs (IR_Struct* ir)
{
    if (!ir)
        RETURN_ERROR(NULL_POINTER, "Null pointer of ir struct");
    Error error = {};

    for (int i = 0; i < ir->num_functions; i++)
    {
        IR_Function* func = ir->functions[i];
        for (int j = 0; j < func->num_commands; j++)
        {
            IR_Command* cmd = &(func->commands[j]);
            if (cmd->cmd_type == IR_JMP || cmd->cmd_type == IR_JE ||
                cmd->cmd_type == IR_JNE || cmd->cmd_type == IR_JG ||
                cmd->cmd_type == IR_JGE || cmd->cmd_type == IR_JL ||
                cmd->cmd_type == IR_JLE)
            {
                error = ir_fill_jmp_adr (func, j);
                PARSE_ERROR_WITHOUT_TREE(error);
            }
            else if (cmd->cmd_type == IR_FUNC)
            {
                error = ir_fill_call_adr (ir, func, j);
                PARSE_ERROR_WITHOUT_TREE(error);
            }
        }
    }

    RETURN_ERROR(CORRECT, "");
}

Error ir_fill_jmp_adr (IR_Function* func, int curr_cmd)
{
    if (!func)
        RETURN_ERROR(NULL_POINTER, "Null pointer of ir func");
    
    IR_Command* cmd = &(func->commands[curr_cmd]);
    IR_DataType label_type = cmd->data_type1;
    int label_num = cmd->data_value1.value;
    int offset = -1;

    for (int i = 0; i < func->num_labels; i++)
        if (label_type == func->labels[i].type && label_num == func->labels[i].num)
            offset = func->labels[i].offset;

    if (offset == -1)
        RETURN_ERROR(UNKNOWN_LABEL, "Jump on unknown label");

    int jump = offset - func->commands[curr_cmd + 1].offset;

    get_hex_num (cmd, jump);
    RETURN_ERROR(CORRECT, "");
}

Error ir_fill_call_adr (IR_Struct* ir, IR_Function* func, int curr_cmd)
{
    if (!ir)
        RETURN_ERROR(NULL_POINTER, "Null pointer of ir struct");
    if (!func)
        RETURN_ERROR(NULL_POINTER, "Null pointer of ir func");

    IR_Command* cmd = &(func->commands[curr_cmd]);
    char* name_label = cmd->data_value1.name;
    int offset = -1;

    if (strcmp (name_label, "my_input") == 0 || strcmp (name_label, "my_print") == 0)
        RETURN_ERROR(CORRECT, "");

    for (int i = 0; i < ir->num_functions; i++)
        if (strcmp (ir->functions[i]->name, name_label) == 0)
            offset = ir->functions[i]->offset;

    if (offset == -1)
        RETURN_ERROR(UNKNOWN_LABEL, "Call unknown func");

    int jump = 0;
    if (offset <= func->offset)
        jump = offset - (func->offset + func->commands[curr_cmd + 1].offset);
    else
        jump = offset - (func->offset + cmd->offset);

    get_hex_num (cmd, jump);
    RETURN_ERROR(CORRECT, "");
}

Error ir_add_mov_ram (IR_Function* func)
{
    IR_Command* cmd = &(func->commands[0]);

    cmd->cmd_type           = IR_MOV_RAM;
    cmd->data_type1         = IR_NONE;
    cmd->data_type2         = IR_NONE;
    cmd->offset             = 0;
    cmd->num_bytes          = 10;
    unsigned char code[5]   = {0x49, 0xbf, 0x00, 0x20, 0x40};
    memcpy (cmd->opcode, code, 5);

    func->cur_command++;
    func->num_commands++;

    RETURN_ERROR(CORRECT, "");
}

Error ir_add_syscall (IR_Function* func)
{
    IR_Command* cmd = &(func->commands[func->cur_command]);

    cmd->cmd_type           = IR_MOV_RAX;
    cmd->data_type1         = IR_NONE;
    cmd->data_type2         = IR_NONE;
    cmd->offset             = func->commands[func->cur_command - 1].offset +
                              func->commands[func->cur_command - 1].num_bytes;
    cmd->num_bytes          = 7;
    unsigned char code[7]   = {0x48, 0xc7, 0xc0, 0x3c, 0x00, 0x00, 0x00};
    memcpy (cmd->opcode, code, 7);

    func->cur_command++;
    func->num_commands++;

    IR_Command* cmd1 = &(func->commands[func->cur_command]);

    cmd1->cmd_type           = IR_SYSCALL;
    cmd1->data_type1         = IR_NONE;
    cmd1->data_type2         = IR_NONE;
    cmd1->offset             = func->commands[func->cur_command - 1].offset +
                               func->commands[func->cur_command - 1].num_bytes;
    cmd1->num_bytes          = 2;
    unsigned char code1[2]   = {0x0f, 0x05};
    memcpy (cmd1->opcode, code1, 2);

    func->cur_command++;
    func->num_commands++;

    RETURN_ERROR(CORRECT, "");
}

Error ir_dump (IR_Struct* ir)
{
    if (!ir)
        RETURN_ERROR(NULL_POINTER, "Null pointer of ir struct");

    Error error = {};

    printf (GREEN_COL);
    printf ("Num of functions - %d\n", ir->num_functions);
    for (int i = 0; i < ir->num_functions; i++)
    {
        printf (YELLOW_COL);
        printf ("-----------------------------------------------------\n");
        error  = ir_func_dump (ir->functions[i]);
        PARSE_ERROR_WITHOUT_TREE(error);
        printf (YELLOW_COL);
        printf ("-----------------------------------------------------\n");
    }
    printf (OFF_COL);
    printf ("End\n");

    RETURN_ERROR(CORRECT, "");
}

Error ir_func_dump (IR_Function* ir_func)
{
    if (!ir_func)
        RETURN_ERROR(NULL_POINTER, "Null pointerr of ir func struct");
    if (!(ir_func->commands))
        RETURN_ERROR(NULL_POINTER, "Null pointerr of ir commands in func struct");
    
    printf (BLUE_COL);
    printf ("Function - %s, num of commands - %d, num of vars - %d\n"
            "offset - %d, num of opcodes - %d\n",
            ir_func->name, ir_func->num_commands, ir_func->num_vars,
            ir_func->offset, ir_func->num_opcodes);
    
    for (int i = 0; i < ir_func->num_commands; i++)
    {
        IR_Command cmd = ir_func->commands[i];
        printf ("#%d %s\n"
                "<%s %s %d> <%s %s %d>\n"
                "offset: %d\nnum opcodes: %d\n",
                i, IR_DUMP_CMD_TYPE_TABLE[cmd.cmd_type], 
                IR_DUMP_DATA_TYPE_TABLE[cmd.data_type1], cmd.data_value1.name, cmd.data_value1.value,
                IR_DUMP_DATA_TYPE_TABLE[cmd.data_type2], cmd.data_value2.name, cmd.data_value2.value,
                cmd.offset, cmd.num_bytes);
        for (int j = 0; j < cmd.num_bytes; j++)
            printf ("0x%02hhx ", cmd.opcode[j]);
        printf ("\n\n");
    }

    printf (OFF_COL);
    RETURN_ERROR(CORRECT, "");
}

int get_num_not_zero_bytes (unsigned char opcode[])
{
    int i = 0;
    for (i = 0; opcode[i] != 0; i++) ;
    return i;
}

void get_hex_num (IR_Command* cmd, int num)
{
    unsigned char hex_num[4] = "";
    for (int i = 0; i < 4; i++)
    {
        if (num == 0)
            break;
        
        unsigned char x = num & 0xff;
        num = num >> 8;
        hex_num[i] = x;
    }

    memcpy (cmd->opcode + cmd->num_bytes - 4, hex_num, 4);
}

int ir_get_num_var (const char* name)
{
    int num_var = 0;
    sscanf (name + 3, "%d", &num_var);
    return num_var;
}

void ir_add_var (IR_Function* ir_func, const char* name)
{
    int num_var = ir_get_num_var (name);

    if (ir_func->vars[num_var] == 0)
    {
        ir_func->vars[num_var] = 1;
        ir_func->num_vars++;
    }
}