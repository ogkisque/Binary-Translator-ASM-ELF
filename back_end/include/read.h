#ifndef READ_HEADER
#define READ_HEADER

#include "tree.h"
#include "dump.h"
#include "error.h"

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

Error   read_trees      (Functions* funcs, ReadStr* str);
Error   read_tree       (Function* func, ReadStr* str, bool* is_read);
Error   read_commands   (Node** node, ReadStr* str);
Error   read_name_func  (Function* func, ReadStr* str, bool* is_read);
Error   read_args       (Node** node, ReadStr* str);
int     get_id          (char* text);
void    get_command     (ReadStr* str, char* text);
void    skip_spaces     (ReadStr* str);
bool    read_num        (Node** node, char* text);
Error   read_func       (Node** node, ReadStr* str, char* text);

#endif //READ_HEADER
