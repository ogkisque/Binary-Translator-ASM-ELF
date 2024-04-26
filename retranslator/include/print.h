#ifndef PRINT_HEADER
#define PRINT_HEADER

#include "tree.h"
#include "error.h"

void    print_funcs                 (const Functions* funcs, FILE* file);
void    print_func                  (const Node* node, FILE* file);
void    print_args_declaration      (const Node* node, FILE* file);
void    print_commands              (const Node* node, FILE* file);
void    print_command               (const Node* node, FILE* file);
void    print_args_call             (const Node* node, FILE* file);

#endif //PRINT_HEADER
