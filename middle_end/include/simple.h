#ifndef SIMPLE_HEADER
#define SIMPLE_HEADER

#include "tree.h"

void simple             (Node* node);
void simple_nums        (Node* node, bool* is_change);
void simple_nums_oper   (Node* node, bool* is_change);
void simple_vars        (Node* node, bool* is_change);
bool pow_zero           (Node* node);
bool one_pow            (Node* node);
bool pow_one            (Node* node, Node** new_simple_node);
bool mul_zero           (Node* node);
bool mul_one            (Node* node, Node** new_simple_node);
bool add_sub_zero       (Node* node, Node** new_simple_node);
bool div_one            (Node* node, Node** new_simple_node);
bool div_zero           (Node* node);

#endif //SIMPLE_HEADER
