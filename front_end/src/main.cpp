#include "dump.h"
#include "tree.h"
#include "read.h"

#define VERIFY(tree)                            \
        error = tree_verify (&tree);            \
        if (error.code != CORRECT)              \
        {                                       \
            tree_dump (&tree, error);           \
            tree_graph_dump (&tree, error);     \
            return error.code;                  \
        }

const char*     INPUT_NAME              = "../input.txt";
const char*     OUTPUT_NAME             = "../front.txt";

int main ()
{
    Function main = {};
    Functions funcs = {};
    ReadStr str = {};
    Tokens tokens = {};
    Error error = {};

    FILE* file = fopen (INPUT_NAME, "r");
    read_file (file, &str);
    fclose (file);
    error = nodes_read (&main, &funcs, &tokens, &str);

    FILE* file_output = fopen (OUTPUT_NAME, "w");
    print_funcs (&funcs, &main, file_output);
    fclose (file_output);

    funcs_dtor (&funcs);
    func_dtor (&main);
    tokens_dtor (&tokens);

    return 0;
}
