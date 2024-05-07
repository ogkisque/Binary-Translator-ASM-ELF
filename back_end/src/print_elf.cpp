#include "print_elf.h"

Error print_elf (IR_Struct* ir, ELF_cmds* elf, FILE* file)
{
    if (!ir)
        RETURN_ERROR(NULL_POINTER, "Null pointer of ir struct");
    if (!elf)
        RETURN_ERROR(NULL_POINTER, "Null pointer of elf struct");

    Error error = {};

    elf->cur_cmd = ELF_ALIGNMENT;
    error = elf_add_lib (elf);
    PARSE_ERROR_WITHOUT_TREE(error);

    error = elf_add_call_lib_adrs (ir);
    PARSE_ERROR_WITHOUT_TREE(error);
 
    int main_offset = ir->functions[ir->num_functions - 1]->offset;

    error = elf_add_header (elf, main_offset);
    PARSE_ERROR_WITHOUT_TREE(error);

    error = elf_add_program_header_table (elf);
    PARSE_ERROR_WITHOUT_TREE(error);

    error = elf_add_text_section (elf, ir);
    PARSE_ERROR_WITHOUT_TREE(error);

    fwrite (elf->cmds, elf->num_cmds, sizeof (unsigned char), file);

    RETURN_ERROR(CORRECT, "");
}

Error elf_ctor (ELF_cmds* elf, IR_Struct* ir)
{
    if (!elf)
        RETURN_ERROR(NULL_POINTER, "Null pointer of elf struct");
    if (!ir)
        RETURN_ERROR(NULL_POINTER, "Null pointer of ir struct");

    elf->cmds = (unsigned char*) calloc (ELF_NUM_CMDS, sizeof (unsigned char));
    if (!(elf->cmds))
        RETURN_ERROR(MEM_ALLOC, "Error of allocation memory for elf cmds");

    for (int i = 0; i < ELF_NUM_CMDS; i++)
        elf->cmds[i] = 0x00;
    
    elf->cur_cmd = 0;
    elf->num_cmds = ELF_NUM_CMDS;

    RETURN_ERROR(CORRECT, "");
}

Error elf_dtor (ELF_cmds* elf)
{
    if (!elf)
        RETURN_ERROR(NULL_POINTER, "Null pointer of elf struct");
    
    free (elf->cmds);
    RETURN_ERROR(CORRECT, "");
}

Error elf_add_lib (ELF_cmds* elf)
{
    if (!elf)
        RETURN_ERROR(NULL_POINTER, "Null pointer of elf struct");

    FILE* file = fopen (ELF_LIB_FILE_NAME, "rb");
    size_t num_cmds = fread (elf->cmds + elf->cur_cmd, sizeof (unsigned char),
                             ELF_NUM_LIB_CMDS, file);

    elf->cur_cmd  += num_cmds;
    elf->num_cmds += num_cmds;

    fclose (file);
    RETURN_ERROR(CORRECT, "");
}

Error elf_add_call_lib_adrs (IR_Struct* ir)
{
    if (!ir)
        RETURN_ERROR(NULL_POINTER, "Null pointer of ir struct");

    for (int i = 0; i < ir->num_functions; i++)
    {
        IR_Function* func = ir->functions[i];
        func->offset += ELF_NUM_LIB_CMDS;
        for (int j = 0; j < func->num_commands; j++)
        {
            IR_Command* cmd = &(func->commands[j]);
            if (cmd->cmd_type == IR_FUNC)
            {
                int jump = 0;
                int offset = cmd->offset + cmd->num_bytes + func->offset;
                if (strcmp (cmd->data_value1.name, "my_input") == 0)
                    jump = -offset;
                else if (strcmp (cmd->data_value1.name, "my_print") == 0)
                    jump = -offset + ELF_NUM_INPUT_CMDS;
                else
                    continue;

                get_hex_num (cmd, jump);
            }
        }
    }

    RETURN_ERROR(CORRECT, "");
}

Error elf_add_text_section (ELF_cmds* elf, IR_Struct* ir)
{
    if (!elf)
        RETURN_ERROR(NULL_POINTER, "Null pointer of elf struct");
    if (!ir)
        RETURN_ERROR(NULL_POINTER, "Null pointer of ir struct");

    elf->cur_cmd = ELF_ALIGNMENT + ELF_NUM_LIB_CMDS;
    for (int i = 0; i < ir->num_functions; i++)
    {
        IR_Function* func = ir->functions[i];
        for (int j = 0; j < func->num_commands; j++)
        {
            IR_Command* cmd = &(func->commands[j]);
            for (int k = 0; k < cmd->num_bytes; k++)
            {
                elf->cmds[elf->cur_cmd] = cmd->opcode[k];
                elf->cur_cmd++;
            }
        }
    }

    RETURN_ERROR(CORRECT, "");
}

Error elf_add_header (ELF_cmds* elf, int main_offset)
{
    Elf64_Ehdr elf_header = {};
    elf_header.e_ident[EI_MAG0]        = ELFMAG0;
    elf_header.e_ident[EI_MAG1]        = ELFMAG1;              
    elf_header.e_ident[EI_MAG2]        = ELFMAG2;
    elf_header.e_ident[EI_MAG3]        = ELFMAG3;
    elf_header.e_ident[EI_CLASS]       = ELFCLASS64;
    elf_header.e_ident[EI_DATA]        = ELFDATA2LSB;
    elf_header.e_ident[EI_VERSION]     = EV_CURRENT;
    elf_header.e_ident[EI_OSABI]       = ELFOSABI_NONE;  
    elf_header.e_ident[EI_ABIVERSION]  = 0x00;  
    elf_header.e_ident[EI_PAD]         = ET_NONE;
    
    elf_header.e_type       = ET_EXEC;
    elf_header.e_machine    = EM_X86_64;
    elf_header.e_version    = EI_VERSION;                                   // дублирует из e_ident
    elf_header.e_entry      = ELF_START_ADR + ELF_ALIGNMENT + main_offset;  // абсолютный виртуальный адрес начала выполнения программы
    elf_header.e_phoff      = sizeof(Elf64_Ehdr);                           // смещение от файла к program header table
    elf_header.e_shoff      = 0;                                            // смещение от начала файла к section header table
    elf_header.e_flags      = 0;
    elf_header.e_ehsize     = sizeof(Elf64_Ehdr);                           // размер заголовка ELF
    elf_header.e_phentsize  = sizeof(Elf64_Phdr);
    elf_header.e_phnum      = 2;                                            // количество заголовков программы .text и .data
    elf_header.e_shentsize  = 0;
    elf_header.e_shnum      = 0;
    elf_header.e_shstrndx   = 0;

    memcpy (elf->cmds, &elf_header, sizeof(elf_header));

    RETURN_ERROR(CORRECT, "");
}

Error elf_add_program_header_table (ELF_cmds* elf)
{
    Elf64_Phdr text_prog_header  = {};

    text_prog_header.p_type      = PT_LOAD;
    text_prog_header.p_offset    = ELF_ALIGNMENT;                   // смещение от начала файла
    text_prog_header.p_vaddr     = ELF_START_ADR + ELF_ALIGNMENT;   // виртуальный адрес
    text_prog_header.p_paddr     = 0;                               // физический адрес
    text_prog_header.p_filesz    = ELF_ALIGNMENT;                   // количество байтов в образе файла сегмента
    text_prog_header.p_memsz     = ELF_ALIGNMENT;                   // количество байтов в памяти образа сегмента
    text_prog_header.p_flags     = PF_R | PF_X;                     // PF_R = READ - 0x04, PF_W = WRITE - 0x02, PF_X = EXEC - 0x01
    text_prog_header.p_align     = ELF_ALIGNMENT;                   // выравнивание

    memcpy(elf->cmds + sizeof (Elf64_Ehdr), &text_prog_header, sizeof(text_prog_header));

    Elf64_Phdr data_prog_header  = {};

    data_prog_header.p_type      = PT_LOAD;
    data_prog_header.p_offset    = 2 * ELF_ALIGNMENT;                   // смещение от начала файла
    data_prog_header.p_vaddr     = ELF_START_ADR + 2 * ELF_ALIGNMENT;   // виртуальный адрес
    data_prog_header.p_paddr     = 0;                                   // физический адрес
    data_prog_header.p_filesz    = 5 * ELF_ALIGNMENT;                   // количество байтов в образе файла сегмента
    data_prog_header.p_memsz     = 5 * ELF_ALIGNMENT;                   // количество байтов в памяти образа сегмента
    data_prog_header.p_flags     = PF_R | PF_X | PF_W;                  // PF_R = READ - 0x04, PF_W = WRITE - 0x02, PF_X = EXEC - 0x01
    data_prog_header.p_align     = ELF_ALIGNMENT;                       // выравнивание

    memcpy(elf->cmds + sizeof (Elf64_Ehdr) + sizeof (Elf64_Phdr), &data_prog_header, sizeof(data_prog_header));
    
    RETURN_ERROR(CORRECT, "");
}