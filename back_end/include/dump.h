#ifndef DUMP_HEADER
#define DUMP_HEADER

#include "tree.h"

void    tree_dump                       (const Tree* tree, Error error);
void    tree_graph_dump                 (const Tree* tree, Error error);
void    tree_graph_dump_without_error   (const Node* node);
void    nodes_graph_dump                (const Node* node, size_t counter);
void    error_graph_dump                (const Tree* tree, Error error);
void    print_error                     (Error error);
void    str_dump                        (const ReadStr* str, Error error);

#endif //DUMP_HEADER
