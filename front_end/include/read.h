#ifndef READ_HEADER
#define READ_HEADER

#include <ctype.h>

#include "tree.h"
#include "error.h"
#include "dump.h"

#define PARSE_ERROR_STR(str, error)     \
        if (error.code != CORRECT)      \
        {                               \
            str_dump (str, error);      \
            return error;               \
        }

#define PARSE_ERROR_STR_NO_DUMP(error)          \
        if (error.code != CORRECT)              \
        {                                       \
            return error;                       \
        }

Error nodes_read            (Function* main, Functions* func, Tokens* tokens,  ReadStr* str);
Error get_funcs             (Functions* funcs, Tokens* tokens);
Error get_func_declaration  (Function* func, Tokens* tokens, Functions* funcs, bool* is_read);
Error get_args              (Node** node, Tokens* tokens, Elements* elems, Functions* funcs, bool is_declaration);
Error get_opers             (Node** node, Tokens* tokens, Elements* elems, Functions* funcs);
Error get_oper              (Node** node, Tokens* tokens, Elements* elems, Functions* funcs, bool* is_read);
Error get_if                (Node** node, Tokens* tokens, Elements* elems, Functions* funcs, bool* is_read);
Error get_while             (Node** node, Tokens* tokens, Elements* elems, Functions* funcs, bool* is_read);
Error get_assign            (Node** node, Tokens* tokens, Elements* elems, Functions* funcs, bool* is_read);
Error get_print_input       (Node** node, Tokens* tokens, Elements* elems, bool* is_read);
Error get_ret               (Node** node, Tokens* tokens, Elements* elems, bool* is_read);
Error get_logic_expression  (Node** node, Tokens* tokens, Elements* elems, Functions* funcs);
Error get_expression        (Node** node, Tokens* tokens, Elements* elems, Functions* funcs);
Error get_term              (Node** node, Tokens* tokens, Elements* elems, Functions* funcs);
Error get_prim_expr         (Node** node, Tokens* tokens, Elements* elems, Functions* funcs);
Error get_num               (Node** node, Tokens* tokens, bool* is_read);
Error get_var               (Node** node, Tokens* tokens, Elements* elems, bool* is_read);
Error get_func_call         (Node** node, Tokens* tokens, Elements* elems, Functions* funcs, bool* is_read);
Error get_math_oper         (Node** node, Tokens* tokens, Elements* elems, Functions* funcs, bool* is_read);
Error get_pow               (Node** node, Tokens* tokens, Elements* elems, Functions* funcs);
Error get_unar_min          (Node** node, Tokens* tokens, Elements* elems, Functions* funcs);
Error get_tokens            (Tokens* tokens, ReadStr* str);
Error get_token_num         (Tokens* tokens, ReadStr* str);
Error get_token_oper        (Tokens* tokens, ReadStr* str);
Error get_token_oper_long   (Tokens* tokens, ReadStr* str);
Error get_token_var_func    (Tokens* tokens, ReadStr* str);
void  skip_spaces           (ReadStr* str);

#endif //READ_HEADER
