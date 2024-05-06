#ifndef PRINT_ASM_HEADER
#define PRINT_ASM_HEADER

#include "read_ir.h"
#include "error.h"

const int           RAM_SIZE    = 500;
const int           BUF_SIZE    = 100;
const char* const   IO_LIB_NAME = "my_io_lib.asm";

const char* const IR_TO_ASM_CMD_TABLE[] = {
        "mov",   "add",   "sub",  "imul",
        "idiv",  "cmp",   "jmp ",  "je ",
        "jne ",   "jg ",    "jge ",  "jl ",
        "jle ",   "call",  "push QWORD", "pop QWORD",
        "print", "input", "ret",  "cqo",
        ""
};

Error print_asm                 (IR_Struct* ir, FILE* file);
Error print_asm_func            (IR_Function* func, FILE* file);
Error print_asm_cmd_arg         (IR_Command cmd, FILE* file);
Error print_asm_header          (FILE* file);
Error print_asm_section_data    (FILE* file);

#endif //PRINT_ASM_HEADER