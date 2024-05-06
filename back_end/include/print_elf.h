#ifndef PRINT_ELF_HEADER
#define PRINT_ELF_HEADER

#include <elf.h>
#include "read_ir.h"
#include "error.h"

const int           ELF_NUM_CMDS        = 0x1000 * 7;
const int           ELF_NUM_LIB_CMDS    = 270;
const int           ELF_NUM_INPUT_CMDS  = 136;
const int           ELF_NUM_PRINT_CMDS  = 134;
const int           ELF_ALIGNMENT       = 0x1000;
const int           ELF_START_ADR       = 0x400000;
const char* const   ELF_LIB_FILE_NAME   = "../bin_lib";

struct ELF_cmds
{
    unsigned char*      cmds;
    int                 num_cmds;
    int                 cur_cmd;
};

Error print_elf                     (IR_Struct* ir, ELF_cmds* elf, FILE* file);
Error elf_ctor                      (ELF_cmds* elf, IR_Struct* ir);
Error elf_dtor                      (ELF_cmds* elf);
Error elf_add_lib                   (ELF_cmds* elf);
Error elf_add_call_lib_adrs         (IR_Struct* ir);
Error elf_add_header                (ELF_cmds* elf, int main_offset);
Error elf_add_program_header_table  (ELF_cmds* elf);
Error elf_add_text_section          (ELF_cmds* elf, IR_Struct* ir);

#endif // PRINT_ELF_HEADER