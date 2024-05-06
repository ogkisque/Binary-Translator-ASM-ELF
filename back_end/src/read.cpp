#include "read.h"
#include "operators.h"

Error read_trees (Functions* funcs, ReadStr* str)
{
    Error error = {};
    bool is_read = true;
    printf (OFF_COL);

    funcs->funcs[0] = (Function*) calloc (1, sizeof (Function));
    error = read_tree (funcs->funcs[0], str, &is_read);
    PARSE_ERROR_STR(str, error);
    funcs->num_funcs = 1;

    for (size_t i = 1; is_read; i++)
    {
        funcs->funcs[i] = (Function*) calloc (1, sizeof (Function));
        error = read_tree (funcs->funcs[i], str, &is_read);
        PARSE_ERROR_STR(str, error);

        if (is_read)
            funcs->num_funcs++;
    }

    RETURN_ERROR(CORRECT, "");
}

Error read_tree (Function* func, ReadStr* str, bool* is_read)
{
    skip_spaces (str);
    if (str->str[str->pos] == '\0')
    {
        *is_read = false;
        RETURN_ERROR(CORRECT, "");
    }

    *is_read = true;
    Error error = {};

    error = read_name_func (func, str, is_read);
    PARSE_ERROR_STR(str, error);
    if (!(*is_read))
        RETURN_ERROR(CORRECT, "");

    if (str->str[str->pos] != '(')
        RETURN_ERROR(SYNTAX_ERR, "No '(' after function");
    str->pos++;

    error = read_args (&(func->root->left), str);
    PARSE_ERROR_STR(str, error);

    skip_spaces (str);
    if (str->str[str->pos] != ')')
        RETURN_ERROR(SYNTAX_ERR, "No ')' after function");
    str->pos++;

    skip_spaces (str);
    if (str->str[str->pos] != '(')
        RETURN_ERROR(SYNTAX_ERR, "No '(' after function");
    str->pos++;

    error = read_commands (&(func->root->right), str);
    PARSE_ERROR_STR(str, error);

    skip_spaces (str);
    if (str->str[str->pos] != ')')
        RETURN_ERROR(SYNTAX_ERR, "No ')' after function");
    str->pos++;

    RETURN_ERROR(CORRECT, "");
}

Error read_commands (Node** node, ReadStr* str)
{
    skip_spaces (str);
    if (str->str[str->pos] == ')')
    {
        (*node) = NULL;
        RETURN_ERROR(CORRECT, "");
    }

    Error error = {};
    char text[MAX_STR_SIZE] = "";
    get_command (str, text);

    if (read_num (node, text))
        RETURN_ERROR(CORRECT, "");

    for (size_t i = 0; i < NUM_OPERS; i++)
    {
        if (strcmp (text, OPERATORS[i].name_to_print) == 0)
        {
            (*node) = create_node (OPERATORS[i].type, OPERATORS[i].id, NULL, NULL, NULL);

            skip_spaces (str);
            if (str->str[str->pos] != '(')
                RETURN_ERROR(SYNTAX_ERR, "No '(' after operator");
            str->pos++;

            error = read_commands (&((*node)->left), str);
            PARSE_ERROR_STR(str, error);

            skip_spaces (str);
            if (str->str[str->pos] != ')')
                RETURN_ERROR(SYNTAX_ERR, "No ')' after '('");
            str->pos++;

            skip_spaces (str);
            if (str->str[str->pos] != '(')
                RETURN_ERROR(SYNTAX_ERR, "No '(' after operator");
            str->pos++;

            error = read_commands (&((*node)->right), str);
            PARSE_ERROR_STR(str, error);

            skip_spaces (str);
            if (str->str[str->pos] != ')')
                RETURN_ERROR(SYNTAX_ERR, "No ')' after '('");
            str->pos++;

            RETURN_ERROR(CORRECT, "");
        }
    }

    if (text[0] == 'v')
    {
        (*node) = _VAR(text);
        (*node)->value = get_id (text);
        RETURN_ERROR(CORRECT, "");
    }

    if (text[0] == 'f')
    {
        error = read_func (node, str, text);
        PARSE_ERROR_STR(str, error);
        RETURN_ERROR(CORRECT, "");
    }

    printf ("text = <%s>\n", text);

    RETURN_ERROR(SYNTAX_ERR, "Unknown element");
}

Error read_name_func (Function* func, ReadStr* str, bool* is_read)
{
    skip_spaces (str);

    if (str->pos >= str->size)
    {
        *is_read = false;
        RETURN_ERROR(CORRECT, "");
    }

    char text[MAX_TEXT_SIZE] = "";
    for (int i = 0; str->str[str->pos] != '(' && str->str[str->pos] != ' '; i++, str->pos++)
        text[i] = str->str[str->pos];
    skip_spaces (str);

    func->root = _FUNC(text);
    func->id = get_id (text);

    *is_read = true;
    RETURN_ERROR(CORRECT, "");
}

Error read_args (Node** node, ReadStr* str)
{
    skip_spaces (str);
    if (str->str[str->pos] == ')')
    {
        (*node) = NULL;
        RETURN_ERROR(CORRECT, "");
    }
    Error error = {};

    if (str->str[str->pos] == ',')
    {
        (*node) = create_node (OPER, COMMA, NULL, NULL, NULL);
        str->pos++;

        skip_spaces (str);
        if (str->str[str->pos] != '(')
            RETURN_ERROR(SYNTAX_ERR, "No '(' after ','");

        str->pos++;
        error = read_args (&((*node)->left), str);
        PARSE_ERROR_STR(str, error)

        skip_spaces (str);
        if (str->str[str->pos] != ')')
            RETURN_ERROR(SYNTAX_ERR, "No ')' after '('");
        str->pos++;

        skip_spaces (str);
        if (str->str[str->pos] != '(')
            RETURN_ERROR(SYNTAX_ERR, "No '(' after ','");

        str->pos++;
        error = read_args (&((*node)->right), str);
        PARSE_ERROR_STR(str, error)

        skip_spaces (str);
        if (str->str[str->pos] != ')')
            RETURN_ERROR(SYNTAX_ERR, "No ')' after '('");
        str->pos++;
    }
    else
    {
        char text[MAX_TEXT_SIZE] = "";
        for (int i = 0; str->str[str->pos] != ' '; i++, str->pos++)
            text[i] = str->str[str->pos];

        (*node) = _VAR(text);
        (*node)->value = (double) get_id (text);
    }

    RETURN_ERROR(CORRECT, "");
}

int get_id (char* text)
{
    int i = 0;
    for (; text[i] < '0' || text[i] > '9'; i++) ;

    int id = 0;
    for (; '0' <= text[i] && text[i] <= '9'; i++)
        id = id * 10 + (text[i] - '0');

    return id;
}

void get_command (ReadStr* str, char* text)
{
    for (int i = 0; str->str[str->pos] != ' ' && str->str[str->pos] != '(' && str->str[str->pos] != ')'; i++, str->pos++)
        text[i] = str->str[str->pos];
}

void skip_spaces (ReadStr* str)
{
    while (str->str[str->pos] == ' ' || str->str[str->pos] == '\n' || str->str[str->pos] == '\t')
        str->pos++;
}

Error read_func (Node** node, ReadStr* str, char* text)
{
    Error error = {};
    (*node) = _FUNC(text);
    (*node)->value = get_id (text);

    skip_spaces (str);
    if (str->str[str->pos] != '(')
        RETURN_ERROR(SYNTAX_ERR, "No '(' after function");
    str->pos++;

    error = read_commands (&((*node)->left), str);
    PARSE_ERROR_STR(str, error);

    skip_spaces (str);
    if (str->str[str->pos] != ')')
        RETURN_ERROR(SYNTAX_ERR, "No ')' after '('");
    str->pos++;

    skip_spaces (str);
    if (str->str[str->pos] != '(')
        RETURN_ERROR(SYNTAX_ERR, "No '(' after function");
    str->pos++;

    skip_spaces (str);
    if (str->str[str->pos] != ')')
        RETURN_ERROR(SYNTAX_ERR, "No ')' after '('");
    str->pos++;

    RETURN_ERROR(CORRECT, "");
}

bool read_num (Node** node, char* text)
{
    int mul = 1;
    if (text[0] == '-')
    {
        text++;
        mul = -1;
    }

    if ('0' > *text || *text > '9')
        return false;

    int id1 = get_id (text);
    double id = (double) id1;

    for (; '0' <= *text && *text <= '9'; text++) ;

    if (*text == '.')
    {
        text++;
        int id2 = get_id (text);

        int count = 0;
        for (; '0' <= *text && *text <= '9'; text++, count++) ;
        int st = 1;
        for (int i = 0; i < count; i++)
            st *= 10;

        id += (double) id2 / st;
    }

    (*node) = _NUM(id * mul);
    return true;
}
