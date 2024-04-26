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
    for (int i = 0; i < ir->num_functions; i++)
    {
        ir->functions[i]->name = funcs->funcs[i]->root->name;
        error = ir_fill_func (ir->functions[i], funcs->funcs[i]->root);
        PARSE_ERROR_WITHOUT_TREE(error);
    }

    RETURN_ERROR(CORRECT, "");
}

Error ir_fill_func (IR_Function* ir_func, const Node* node)
{
    Error error = {};

    ir_func->cur_if     = 0;
    ir_func->cur_while  = 0;

    error = ir_fill_func_args (ir_func, node);
    PARSE_ERROR_WITHOUT_TREE(error);

    error = ir_fill_func_cmds (ir_func, node->right);
    PARSE_ERROR_WITHOUT_TREE(error);

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
        error = ir_fill_func_call_args (ir_func, node->left);
        PARSE_ERROR_WITHOUT_TREE(error);
        IR_CMD_FUNC(ir_func, node->name)
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

            IR_CMD_POP_REG(ir_func, IR_TMP_REG1)
            ir_add_cmd (ir_func, IR_MOV, IR_VAR, "", ir_get_num_var(node->right->name), IR_REG, IR_TMP_REG1, 0);
            RETURN_ERROR(CORRECT, "");
        }

        if ((int) node->value == PRINT)
        {
            error = ir_fill_func_cmds (ir_func, node->right);
            PARSE_ERROR_WITHOUT_TREE(error);

            ir_add_cmd (ir_func, IR_FUNC, IR_NONE, IR_FUNC_PRINT_NAME, 0, IR_NONE, "", 0);
        }

        if ((int) node->value == INPUT_VAR)
        {
            ir_add_cmd (ir_func, IR_FUNC, IR_NONE, IR_FUNC_INPUT_NAME, 0, IR_NONE, "", 0);
            IR_CMD_POP_REG(ir_func, IR_TMP_REG1)
            ir_add_cmd (ir_func, IR_MOV, IR_VAR, "", ir_get_num_var (node->right->name), IR_REG, IR_TMP_REG1, 0);
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

Error ir_fill_func_call_args (IR_Function* ir_func, const Node* node)
{
    if (!node)
        RETURN_ERROR(CORRECT, "");

    Error error = {};

    error = ir_fill_func_call_args (ir_func, node->right);
    PARSE_ERROR_WITHOUT_TREE(error);

    error = ir_fill_func_cmds (ir_func, node->left);
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

void ir_add_cmd (IR_Function* func, IR_CommandType cmd_type,
                 IR_DataType type1, const char* name1, int val1,
                 IR_DataType type2, const char* name2, int val2)
{
    func->commands[func->cur_command].cmd_type = cmd_type;

    func->commands[func->cur_command].data_type1 = type1;
    strcpy (func->commands[func->cur_command].data_value1.name, name1);
    func->commands[func->cur_command].data_value1.value = val1;

    func->commands[func->cur_command].data_type2 = type2;
    strcpy (func->commands[func->cur_command].data_value2.name, name2);
    func->commands[func->cur_command].data_value2.value = val2;

    func->cur_command++;                                             
    func->num_commands++;
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
    printf ("Function - %s, num of commands - %d, num of vars - %d\n",
            ir_func->name, ir_func->num_commands, ir_func->num_vars);
    
    for (int i = 0; i < ir_func->num_commands; i++)
    {
        IR_Command cmd = ir_func->commands[i];
        printf ("#%d %s\n"
                "<%s %s %d> <%s %s %d>\n",
                i, IR_DUMP_CMD_TYPE_TABLE[cmd.cmd_type], 
                IR_DUMP_DATA_TYPE_TABLE[cmd.data_type1], cmd.data_value1.name, cmd.data_value1.value,
                IR_DUMP_DATA_TYPE_TABLE[cmd.data_type2], cmd.data_value2.name, cmd.data_value2.value);
    }

    printf (OFF_COL);
    RETURN_ERROR(CORRECT, "");
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