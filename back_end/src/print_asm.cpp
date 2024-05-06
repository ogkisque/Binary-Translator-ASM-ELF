#include "print_asm.h"

Error print_asm (IR_Struct* ir, FILE* file)
{
    if (!ir)
        RETURN_ERROR(NULL_POINTER, "Null pointer of ir struct");
    if (!file)
        RETURN_ERROR(NULL_POINTER, "Null pointer of file");

    Error error = {};

    error = print_asm_header (file);
    PARSE_ERROR_WITHOUT_TREE(error);

    for (int i = 0; i < ir->num_functions; i++)
    {
        error = print_asm_func (ir->functions[i], file);
        PARSE_ERROR_WITHOUT_TREE(error);
    }

    error = print_asm_section_data (file);
    PARSE_ERROR_WITHOUT_TREE(error);

    RETURN_ERROR(CORRECT, "");
}

Error print_asm_func (IR_Function* func, FILE* file)
{
    if (!func)
        RETURN_ERROR(NULL_POINTER, "Null pointer of ir func");

    Error error = {};
    int i = 0;
    int num_cmd = func->num_commands;

    if (strcmp (func->name, "main") == 0)
    {
        fprintf (file, "_start:\n");
        fprintf (file, "mov %s, RAM\n", IR_STACK_REG);
        i = 1;
        num_cmd -= 2;
    } 
    else
    {
        fprintf (file, "%s:\n", func->name);
    }

    for (; i < num_cmd; i++)
    {
        IR_Command cmd = func->commands[i];
        const char* name_cmd = IR_TO_ASM_CMD_TABLE[cmd.cmd_type];
        fprintf (file, "%s", name_cmd);

        error = print_asm_cmd_arg (cmd, file);
        PARSE_ERROR_WITHOUT_TREE(error);

        fprintf (file, "\n");
    }

    if (strcmp (func->name, "main") == 0)
    {
        fprintf (file, "mov rax, 60\n");
        fprintf (file, "syscall\n");
    } 

    fprintf (file, "\n");

    RETURN_ERROR(CORRECT, "");
}

Error print_asm_cmd_arg (IR_Command cmd, FILE* file)
{
    switch (cmd.data_type1)
    {
        case IR_NONE:
            if (cmd.cmd_type == IR_FUNC)
                fprintf (file, " %s", cmd.data_value1.name);
            break;
        case IR_REG:
            fprintf (file, " %s", cmd.data_value1.name);
            break;
        case IR_MEM:
            fprintf (file, " [%s + %d]", IR_STACK_REG, cmd.data_value1.value);
            break;
        case IR_NUM:
            fprintf (file, " %d", cmd.data_value1.value);
            break;
        case IR_LABEL_IF:
            fprintf (file, ".if%d", cmd.data_value1.value);
            if (cmd.cmd_type == IR_LABEL_CMD)
                fprintf (file, ":");
            break;
        case IR_LABEL_WHILE_START:
            fprintf (file, ".while_start%d", cmd.data_value1.value);
            if (cmd.cmd_type == IR_LABEL_CMD)
                fprintf (file, ":");
            break;
        case IR_LABEL_WHILE_STOP:
            fprintf (file, ".while_stop%d", cmd.data_value1.value);
            if (cmd.cmd_type == IR_LABEL_CMD)
                fprintf (file, ":");
            break;
        default:
            RETURN_ERROR(UNKNOWN_DATA, "Unknown data type");
    }

    switch (cmd.data_type2)
    {
        case IR_NONE:
            break;
        case IR_REG:
            fprintf (file, ", %s", cmd.data_value2.name);
            break;
        case IR_MEM:
            fprintf (file, ", [%s + %d]", IR_STACK_REG, cmd.data_value2.value);
            break;
        case IR_NUM:
            fprintf (file, ", %d", cmd.data_value2.value);
            break;
        case IR_LABEL_IF:
            fprintf (file, ".if%d", cmd.data_value2.value);
            break;
        case IR_LABEL_WHILE_START:
            fprintf (file, ".while_start%d", cmd.data_value2.value);
            break;
        case IR_LABEL_WHILE_STOP:
            fprintf (file, ".while_stop%d", cmd.data_value2.value);
            break;
        default:
            RETURN_ERROR(UNKNOWN_DATA, "Unknown data type");
    }

    RETURN_ERROR(CORRECT, "");
}

Error print_asm_header (FILE* file)
{
    fprintf (file, "global _start\n\n");
    fprintf (file, "section .text\n\n");
    fprintf (file, "%%INCLUDE \"%s\"\n\n", IO_LIB_NAME);

    RETURN_ERROR(CORRECT, "");
}

Error print_asm_section_data (FILE* file)
{
    fprintf (file, "section .data\n");
    fprintf (file, "RAM: dq %d dup 0\n", RAM_SIZE);
    fprintf (file, "BUFF: db %d dup 0\n", BUF_SIZE);

    RETURN_ERROR(CORRECT, "");
}