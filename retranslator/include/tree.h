#ifndef TREE_HEADER
#define TREE_HEADER

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "colors.h"
#include "error.h"

#define RETURN_ERROR_AND_DUMP(list, code, message)                                          \
        {                                                                                   \
            tree_dump (tree, Error {code, __LINE__, __FILE__, __func__, message});          \
            tree_graph_dump (tree, Error {code, __LINE__, __FILE__, __func__, message});    \
            return Error {code, __LINE__, __FILE__, __func__, message};                     \
        }

#define PARSE_ERROR(tree, error)            \
        if (error.code != CORRECT)          \
        {                                   \
            tree_dump (tree, error);        \
            tree_graph_dump (tree, error);  \
            return error;                   \
        }

#define PARSE_ERROR_WITHOUT_TREE(error)     \
        if (error.code != CORRECT)          \
        {                                   \
            return error;                   \
        }

#define TREE_CTOR(tree) \
        tree_ctor (tree, #tree, __FILE__, __func__, __LINE__)

#define _ADD(left, right)       create_node (OPER,  ADD,            NULL, left, right)
#define _MUL(left, right)       create_node (OPER,  MUL,            NULL, left, right)
#define _DIV(left, right)       create_node (OPER,  DIV,            NULL, left, right)
#define _SUB(left, right)       create_node (OPER,  SUB,            NULL, left, right)
#define _UNSUB(left, right)     create_node (OPER,  UNSUB,          NULL, left, right)
#define _COS(left, right)       create_node (FUNC,  COS,            NULL, left, right)
#define _SIN(left, right)       create_node (FUNC,  SIN,            NULL, left, right)
#define _SQRT(left, right)      create_node (FUNC,  SQRT,           NULL, left, right)
#define _POW(left, right)       create_node (FUNC,  POW,            NULL, left, right)
#define _LN(left, right)        create_node (FUNC,  LN,             NULL, left, right)
#define _TG(left, right)        create_node (FUNC,  TG,             NULL, left, right)
#define _ARCSIN(left, right)    create_node (FUNC,  ARCSIN,         NULL, left, right)
#define _ARCCOS(left, right)    create_node (FUNC,  ARCSIN,         NULL, left, right)
#define _ARCTG(left, right)     create_node (FUNC,  ARCTG,          NULL, left, right)
#define _NUM(value)             create_node (NUM,   value,          NULL, NULL, NULL)
#define _VAR(name)              create_node (VAR,   VAR_DEF_VAL,    name, NULL, NULL)
#define _FUNC(name)             create_node (FUNC,  FUNC_DEF_VAL,   name, NULL, NULL)
#define _LBRAC                  create_node (OPER,  LBRAC,          NULL, NULL, NULL)
#define _RBRAC                  create_node (OPER,  RBRAC,          NULL, NULL, NULL)
#define _END                    create_node (OPER,  END,            NULL, NULL, NULL)
#define _ASSIGN(left, right)    create_node (OPER,  ASSIGN,         NULL, left, right)
#define LVAL                    node->left->value
#define RVAL                    node->right->value
#define LTYP                    node->left->type
#define RTYP                    node->right->type
#define _LEFT                   node->left
#define _RIGHT                  node->right

const int       MAX_STR_SIZE    = 2000;
const int       MAX_TEXT_SIZE   = 200;
const int       MAX_SIZE        = 100;
const int       VAR_DEF_VAL     = 2;
const double    EPS             = 1e-3;
const int       MAX_NUM_ELEMS   = 300;
const double    OPER_DEF_VAL    = 2;
const double    FUNC_DEF_VAL    = 2;
const int       MAX_NUM_FUNCS   = 15;

enum Types
{
    NUM  = 1,
    OPER = 2,
    VAR  = 3,
    FUNC = 4
};

enum Opers
{
    ADD         =   1,
    MUL         =   2,
    SUB         =   3,
    DIV         =   4,
    LBRAC       =   5,
    RBRAC       =   6,
    END         =   7,
    UNSUB       =   8,
    COS         =   9,
    SIN         =   10,
    POW         =   11,
    SQRT        =   12,
    LN          =   13,
    TG          =   14,
    ARCSIN      =   15,
    ARCCOS      =   16,
    ARCTG       =   17,
    ASSIGN      =   18,
    END_STR     =   19,
    IF          =   20,
    WHILE       =   21,
    START_CODE  =   22,
    END_CODE    =   23,
    MORE        =   24,
    MORE_EQ     =   25,
    LESS        =   26,
    LESS_EQ     =   27,
    EQUAL       =   28,
    PRINT       =   29,
    NOT_EQUAL   =   30,
    NEW_FUNC    =   31,
    RET         =   32,
    COMMA       =   33,
    INPUT_VAR   =   34
};

struct Element
{
    Types           type;
    int             id;
    const char*     name;
    double          value;
};

struct Elements
{
    Element*    elems;
    size_t      num_elems;
    size_t      num_vars;
    size_t      num_funcs;
};

struct Node
{
    Types   type;
    double  value;
    char*   name;
    Node*   left;
    Node*   right;
};

struct Tree
{
    Node*       root;
    size_t      size;

    const char* name;
    const char* file;
    const char* func;
    int         line;
};

struct ReadStr
{
    char    str[MAX_STR_SIZE];
    size_t  pos;
    size_t  size;
};

struct Tokens
{
    Node** tokens;
    size_t size;
    size_t pos;
};

struct Function
{
    Node*       root;
    Elements    elems;
    size_t      offset;
    int         id;
};

struct Functions
{
    Function*   funcs[MAX_NUM_FUNCS];
    size_t      num_funcs;
};

Error   tree_ctor                   (Tree* tree, const char* name, const char* file, const char* func, int line);
Error   tree_dtor                   (Tree* tree);
Error   read_file                   (FILE* file, ReadStr* str);
Error   nodes_print                 (const Node* node, FILE* file);
void    val_to_str                  (const Node* node, char* str);
Error   new_node                    (Types type, double value, char* name, Node** adres);
Error   node_ctor                   (Types type, double value, char* name, Node* node);
void    nodes_dtor                  (Node* node);
Error   tree_verify                 (const Tree* tree);
bool    is_cycles                   (Node* node);
void    get_points                  (Node* node, Node** points, size_t pos);
int     comparator                  (const void* p1, const void* p2);
bool    is_zero                     (double x);
Node*   create_node                 (Types type, double value, char* name, Node* left, Node* right);
void    del_node                    (Node* node);
void    funcs_dtor                  (Functions* funcs);

#endif //TREE_HEADER
