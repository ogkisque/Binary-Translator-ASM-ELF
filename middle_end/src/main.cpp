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

const char*     INPUT_NAME              = "../front.txt";
const char*     OUTPUT_NAME             = "../middle.txt";

int main ()
{
    Functions funcs = {};
    ReadStr str = {};
    Error error = {};

    FILE* file = fopen (INPUT_NAME, "r");
    read_file (file, &str);
    fclose (file);
    error = read_trees (&funcs, &str);

    FILE* file_output = fopen (OUTPUT_NAME, "w");
    print_funcs (&funcs, file_output);
    fclose (file_output);

    funcs_dtor (&funcs);
    return 0;
}
