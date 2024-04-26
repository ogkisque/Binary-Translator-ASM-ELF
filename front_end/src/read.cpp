#include "read.h"
#include "operators.h"

Error nodes_read (Function* main, Functions* funcs, Tokens* tokens, ReadStr* str)
{
    elems_ctor (&(main->elems));
    tokens->pos = 0;

    Error error = get_tokens (tokens, str);
    PARSE_ERROR_STR(str, error);

    funcs->num_funcs = 0;
    error = get_funcs (funcs, tokens);
    PARSE_ERROR_STR(str, error);

    error = get_opers (&(main->root), tokens, &(main->elems), funcs);
    PARSE_ERROR_STR(str, error);

    main->offset = 0;
    RETURN_ERROR(CORRECT, "");
}

Error get_funcs (Functions* funcs, Tokens* tokens)
{
    Error error = {};
    bool is_read = true;
    for (size_t i = 0; tokens->tokens[tokens->pos]->type == OPER && (int) tokens->tokens[tokens->pos]->value == NEW_FUNC; i++)
    {
        funcs->num_funcs++;
        elems_ctor (&(funcs->funcs[i].elems));
        funcs->funcs[i].id = (int) i;

        error = get_func_declaration (&(funcs->funcs[i]), tokens, funcs, &is_read);
        PARSE_ERROR_STR_NO_DUMP(error);

        funcs->funcs[i].offset = 0;
        
        if (!is_read)
            RETURN_ERROR(SYNTAX_ERR, "The number of declared functions is less than the number of called functions");
    }

    RETURN_ERROR(CORRECT, "");
}

Error get_func_declaration (Function* func, Tokens* tokens, Functions* funcs, bool* is_read)
{
    Error error = {};
    if (tokens->tokens[tokens->pos]->type == OPER && (int) tokens->tokens[tokens->pos]->value == NEW_FUNC)
    {
        *is_read = true;
    }
    else
    {
        *is_read = false;
        RETURN_ERROR(CORRECT, "");
    }

    tokens->pos++;
    if (tokens->tokens[tokens->pos]->type != FUNC)
        RETURN_ERROR(SYNTAX_ERR, "No name of functions after 'houmi'");
    func->root = tokens->tokens[tokens->pos];
    func->root->value = (double) funcs->num_funcs - 1;
    tokens->pos++;

    if (!(tokens->tokens[tokens->pos]->type == OPER && (int) tokens->tokens[tokens->pos]->value == LBRAC))
        RETURN_ERROR(SYNTAX_ERR, "No '(' after name of function");
    tokens->pos++;

    error = get_args (&(func->root->left), tokens, &(func->elems), funcs, true);
    PARSE_ERROR_STR_NO_DUMP(error);

    if (!(tokens->tokens[tokens->pos]->type == OPER && (int) tokens->tokens[tokens->pos]->value == RBRAC))
        RETURN_ERROR(SYNTAX_ERR, "No ')' after name of function");
    tokens->pos++;

    if (!(tokens->tokens[tokens->pos]->type == OPER && (int) tokens->tokens[tokens->pos]->value == START_CODE))
        RETURN_ERROR(SYNTAX_ERR, "No 'lesss_go' before function body");
    tokens->pos++;

    error = get_opers (&(func->root->right), tokens, &(func->elems), funcs);
    PARSE_ERROR_STR_NO_DUMP(error);

    if (!(tokens->tokens[tokens->pos]->type == OPER && (int) tokens->tokens[tokens->pos]->value == END_CODE))
        RETURN_ERROR(SYNTAX_ERR, "No 'stop_that_shit' after function body");
    tokens->pos++;

    RETURN_ERROR(CORRECT, "");
}

Error get_args (Node** node, Tokens* tokens, Elements* elems, Functions* funcs, bool is_declaration)
{
    Node* node_left = NULL;
    bool is_read = false;
    Error error = {};
    if (is_declaration)
        error = get_var (&node_left, tokens, elems, &is_read);
    else
        error = get_expression (&node_left, tokens, elems, funcs);

    PARSE_ERROR_STR_NO_DUMP(error);

    if (tokens->tokens[tokens->pos]->type == OPER &&
        (int) tokens->tokens[tokens->pos]->value == COMMA)
    {
        *node = tokens->tokens[tokens->pos];
        (*node)->left = node_left;
        tokens->pos++;
        get_args (&((*node)->right), tokens, elems, funcs, is_declaration);
    }
    else
    {
        *node = create_node (OPER, COMMA, NULL, node_left, NULL);
    }

    RETURN_ERROR(CORRECT, "");
}

Error get_opers (Node** node, Tokens* tokens, Elements* elems, Functions* funcs)
{
    Error error = {};
    bool is_read = true;
    Node* node_left = NULL;

    error = get_oper (&node_left, tokens, elems, funcs, &is_read);
    PARSE_ERROR_STR_NO_DUMP(error);
    if (!is_read)
        RETURN_ERROR(CORRECT, "");

    if (tokens->tokens[tokens->pos - 1]->type == OPER &&
        (int) tokens->tokens[tokens->pos - 1]->value == END_STR)
    {
        *node = tokens->tokens[tokens->pos - 1];
        (*node)->left = node_left;
    }
    else
    {
        *node = create_node (OPER, END_STR, NULL, node_left, NULL);
    }

    get_opers (&((*node)->right), tokens, elems, funcs);

    RETURN_ERROR(CORRECT, "");
}

Error get_oper (Node** node, Tokens* tokens, Elements* elems, Functions* funcs, bool* is_read)
{
    Error error = {};

    error = get_if (node, tokens, elems, funcs, is_read);
    PARSE_ERROR_STR_NO_DUMP(error);
    if (*is_read)
        RETURN_ERROR(CORRECT, "");

    error = get_while (node, tokens, elems, funcs, is_read);
    PARSE_ERROR_STR_NO_DUMP(error);
    if (*is_read)
        RETURN_ERROR(CORRECT, "");

    error = get_print_input (node, tokens, elems, is_read);
    PARSE_ERROR_STR_NO_DUMP(error);
    if (*is_read)
        RETURN_ERROR(CORRECT, "");

    error = get_ret (node, tokens, elems, is_read);
    PARSE_ERROR_STR_NO_DUMP(error);
    if (*is_read)
        RETURN_ERROR(CORRECT, "");

    error = get_assign (node, tokens, elems, funcs, is_read);
    PARSE_ERROR_STR_NO_DUMP(error);
    if (*is_read)
        RETURN_ERROR(CORRECT, "");

    RETURN_ERROR(CORRECT, "");
}

Error get_if (Node** node, Tokens* tokens, Elements* elems, Functions* funcs, bool* is_read)
{
    Error error = {};
    Node* node_left = NULL;
    Node* node_right = NULL;

    if (tokens->tokens[tokens->pos]->type != OPER || (int) tokens->tokens[tokens->pos]->value != IF)
    {
        *is_read = false;
        RETURN_ERROR(CORRECT, "");
    }

    *node = tokens->tokens[tokens->pos];
    tokens->pos++;

    if (tokens->tokens[tokens->pos]->type != OPER || (int) tokens->tokens[tokens->pos]->value != LBRAC)
        RETURN_ERROR(SYNTAX_ERR, "No '(' after 'choppa'");

    error = get_logic_expression (&node_left, tokens, elems, funcs);
    PARSE_ERROR_STR_NO_DUMP(error);

    if (tokens->tokens[tokens->pos]->type != OPER || (int) tokens->tokens[tokens->pos]->value != START_CODE)
        RETURN_ERROR(SYNTAX_ERR, "No 'lesss_go' after ')'");

    tokens->pos++;
    error = get_opers (&node_right, tokens, elems, funcs);
    PARSE_ERROR_STR_NO_DUMP(error);

    if (tokens->tokens[tokens->pos]->type != OPER || (int) tokens->tokens[tokens->pos]->value != END_CODE)
        RETURN_ERROR(SYNTAX_ERR, "No 'stop_that_shit' after 'lesss_go'");
    tokens->pos++;

    (*node)->left = node_left;
    (*node)->right = node_right;
    *is_read = true;

    RETURN_ERROR(CORRECT, "");
}

Error get_while (Node** node, Tokens* tokens, Elements* elems, Functions* funcs, bool* is_read)
{
    Error error = {};
    Node* node_left = NULL;
    Node* node_right = NULL;

    if (tokens->tokens[tokens->pos]->type != OPER || (int) tokens->tokens[tokens->pos]->value != WHILE)
    {
        *is_read = false;
        RETURN_ERROR(CORRECT, "");
    }

    *node = tokens->tokens[tokens->pos];
    tokens->pos++;

    if (tokens->tokens[tokens->pos]->type != OPER || (int) tokens->tokens[tokens->pos]->value != LBRAC)
        RETURN_ERROR(SYNTAX_ERR, "No '(' after 'smoke'");

    error = get_logic_expression (&node_left, tokens, elems, funcs);
    PARSE_ERROR_STR_NO_DUMP(error);

    if (tokens->tokens[tokens->pos]->type != OPER || (int) tokens->tokens[tokens->pos]->value != START_CODE)
        RETURN_ERROR(SYNTAX_ERR, "No 'lesss_go' after ')'");

    tokens->pos++;
    error = get_opers (&node_right, tokens, elems, funcs);
    PARSE_ERROR_STR_NO_DUMP(error);

    if (tokens->tokens[tokens->pos]->type != OPER || (int) tokens->tokens[tokens->pos]->value != END_CODE)
        RETURN_ERROR(SYNTAX_ERR, "No 'stop_that_shit' after 'lesss_go'");
    tokens->pos++;

    (*node)->left = node_left;
    (*node)->right = node_right;
    *is_read = true;
    RETURN_ERROR(CORRECT, "");
}

Error get_assign (Node** node, Tokens* tokens, Elements* elems, Functions* funcs, bool* is_read)
{
    Error error = {};
    Node* node_left = NULL;
    Node* node_right = NULL;
    *is_read = false;

    error = get_var (&node_left, tokens, elems, is_read);
    PARSE_ERROR_STR_NO_DUMP(error);
    if (!(*is_read))
        RETURN_ERROR(CORRECT, "");

    if (tokens->tokens[tokens->pos]->type != OPER || (int) tokens->tokens[tokens->pos]->value != ASSIGN)
        RETURN_ERROR(SYNTAX_ERR, "No 'aka' after variable");

    *node = tokens->tokens[tokens->pos];
    tokens->pos++;

    error = get_expression (&node_right, tokens, elems, funcs);
    PARSE_ERROR_STR_NO_DUMP(error);

    if (tokens->tokens[tokens->pos]->type != OPER || (int) tokens->tokens[tokens->pos]->value != END_STR)
        RETURN_ERROR(SYNTAX_ERR, "No ';' after command");

    tokens->pos++;

    (*node)->left = node_right;
    (*node)->right = node_left;
    *is_read = true;

    RETURN_ERROR(CORRECT, "");
}

Error get_print_input (Node** node, Tokens* tokens, Elements* elems, bool* is_read)
{
    Error error = {};
    Node* node_right = NULL;

    if (!(tokens->tokens[tokens->pos]->type == OPER &&
         ((int) tokens->tokens[tokens->pos]->value == PRINT || (int) tokens->tokens[tokens->pos]->value == INPUT_VAR)))
    {
        *is_read = false;
        RETURN_ERROR(CORRECT, "");
    }

    *node = tokens->tokens[tokens->pos];
    tokens->pos++;

    if (tokens->tokens[tokens->pos]->type != OPER || (int) tokens->tokens[tokens->pos]->value != LBRAC)
        RETURN_ERROR(SYNTAX_ERR, "No '(' after 'freestyle' or 'megera'");
    tokens->pos++;

    error = get_var (&node_right, tokens, elems, is_read);
    PARSE_ERROR_STR_NO_DUMP(error);
    if (!(*is_read))
        RETURN_ERROR(SYNTAX_ERR, "No variable after 'freestyle' or 'megera'");

    if (tokens->tokens[tokens->pos]->type != OPER || (int) tokens->tokens[tokens->pos]->value != RBRAC)
        RETURN_ERROR(SYNTAX_ERR, "No ')' after '('");
    tokens->pos++;

    if (tokens->tokens[tokens->pos]->type != OPER || (int) tokens->tokens[tokens->pos]->value != END_STR)
        RETURN_ERROR(SYNTAX_ERR, "No ';' after command");
    tokens->pos++;

    (*node)->left = NULL;
    (*node)->right = node_right;
    *is_read = true;

    RETURN_ERROR(CORRECT, "");
}

Error get_ret (Node** node, Tokens* tokens, Elements* elems, bool* is_read)
{
    Error error = {};
    Node* node_right = NULL;

    if (tokens->tokens[tokens->pos]->type != OPER || (int) tokens->tokens[tokens->pos]->value != RET)
    {
        *is_read = false;
        RETURN_ERROR(CORRECT, "");
    }

    *node = tokens->tokens[tokens->pos];
    tokens->pos++;

    if (tokens->tokens[tokens->pos]->type != OPER || (int) tokens->tokens[tokens->pos]->value != LBRAC)
        RETURN_ERROR(SYNTAX_ERR, "No '(' after 'glock'");
    tokens->pos++;

    error = get_var (&node_right, tokens, elems, is_read);
    PARSE_ERROR_STR_NO_DUMP(error);
    if (!(*is_read))
        RETURN_ERROR(SYNTAX_ERR, "No variable after 'glock'");

    if (tokens->tokens[tokens->pos]->type != OPER || (int) tokens->tokens[tokens->pos]->value != RBRAC)
        RETURN_ERROR(SYNTAX_ERR, "No ')' after '('");
    tokens->pos++;

    if (tokens->tokens[tokens->pos]->type != OPER || (int) tokens->tokens[tokens->pos]->value != END_STR)
        RETURN_ERROR(SYNTAX_ERR, "No ';' after command");
    tokens->pos++;

    (*node)->left = NULL;
    (*node)->right = node_right;
    *is_read = true;

    RETURN_ERROR(CORRECT, "");
}

Error get_logic_expression (Node** node, Tokens* tokens, Elements* elems, Functions* funcs)
{
    Node* node_left = NULL;
    Node* node_right = NULL;
    if (tokens->tokens[tokens->pos]->type != OPER || (int) tokens->tokens[tokens->pos]->value != LBRAC)
        RETURN_ERROR(SYNTAX_ERR, "No '(' before condition");
    tokens->pos++;

    Error error = get_expression (&node_left, tokens, elems, funcs);
    PARSE_ERROR_STR_NO_DUMP(error);

    int val = (int) tokens->tokens[tokens->pos]->value;
    if (!(tokens->tokens[tokens->pos]->type == OPER &&
         (val == MORE || val == MORE_EQ || val == LESS || val == LESS_EQ || val == EQUAL || val == NOT_EQUAL)))
        RETURN_ERROR(SYNTAX_ERR, "No logical operator in condition");

    *node = tokens->tokens[tokens->pos];
    tokens->pos++;

    error = get_expression (&node_right, tokens, elems, funcs);
    PARSE_ERROR_STR_NO_DUMP(error);

    if (tokens->tokens[tokens->pos]->type != OPER || (int) tokens->tokens[tokens->pos]->value != RBRAC)
        RETURN_ERROR(SYNTAX_ERR, "No ')' before condition");
    tokens->pos++;

    (*node)->left = node_left;
    (*node)->right = node_right;

    RETURN_ERROR(CORRECT, "");
}

Error get_expression (Node** node, Tokens* tokens, Elements* elems, Functions* funcs)
{
    Node* node_left = NULL;
    Node* node_right = NULL;
    Error error = get_term (&node_left, tokens, elems, funcs);
    PARSE_ERROR_STR_NO_DUMP(error);
    *node = node_left;

    while (tokens->tokens[tokens->pos]->type == OPER &&
          ((int) tokens->tokens[tokens->pos]->value == ADD || (int) tokens->tokens[tokens->pos]->value == SUB))
    {
        *node = tokens->tokens[tokens->pos];
        tokens->pos++;
        error = get_term (&node_right, tokens, elems, funcs);
        PARSE_ERROR_STR_NO_DUMP(error);

        (*node)->left = node_left;
        (*node)->right = node_right;

        node_left = *node;
    }
    RETURN_ERROR(CORRECT, "");
}

Error get_term (Node** node, Tokens* tokens, Elements* elems, Functions* funcs)
{
    Node* node_left = NULL;
    Node* node_right = NULL;
    Error error = get_pow (&node_left, tokens, elems, funcs);
    PARSE_ERROR_STR_NO_DUMP(error);
    *node = node_left;

    while (tokens->tokens[tokens->pos]->type == OPER &&
          ((int) tokens->tokens[tokens->pos]->value == MUL || (int) tokens->tokens[tokens->pos]->value == DIV))
    {
        *node = tokens->tokens[tokens->pos];
        tokens->pos++;
        error = get_pow (&node_right, tokens, elems, funcs);
        PARSE_ERROR_STR_NO_DUMP(error);

        (*node)->left = node_left;
        (*node)->right = node_right;

        node_left = *node;
    }

    RETURN_ERROR(CORRECT, "");
}

Error get_pow (Node** node, Tokens* tokens, Elements* elems, Functions* funcs)
{
    Node* node_left = NULL;
    Node* node_right = NULL;
    Error error = get_unar_min (&node_left, tokens, elems, funcs);
    PARSE_ERROR_STR_NO_DUMP(error);
    *node = node_left;

    while (tokens->tokens[tokens->pos]->type == OPER &&
           (int) tokens->tokens[tokens->pos]->value == POW)
    {
        *node = tokens->tokens[tokens->pos];
        tokens->pos++;
        error = get_unar_min (&node_right, tokens, elems, funcs);
        PARSE_ERROR_STR_NO_DUMP(error);

        (*node)->left = node_left;
        (*node)->right = node_right;

        node_left = *node;
    }

    RETURN_ERROR(CORRECT, "");
}

Error get_unar_min (Node** node, Tokens* tokens, Elements* elems, Functions* funcs)
{
    Error error = {};
    if (tokens->tokens[tokens->pos]->type == OPER &&
        (int) tokens->tokens[tokens->pos]->value == UNSUB)
    {
        tokens->pos++;
        Node* node_right = NULL;
        error = get_prim_expr (&node_right, tokens, elems, funcs);
        PARSE_ERROR_STR_NO_DUMP(error);

        *node = _MUL(_NUM(-1), node_right);
    }
    else
    {
        error = get_prim_expr (node, tokens, elems, funcs);
        PARSE_ERROR_STR_NO_DUMP(error);
    }

    RETURN_ERROR(CORRECT, "");
}

Error get_prim_expr (Node** node, Tokens* tokens, Elements* elems, Functions* funcs)
{
    Error error = {};
    if (tokens->tokens[tokens->pos]->type == OPER &&
        (int) tokens->tokens[tokens->pos]->value == LBRAC)
    {
        tokens->pos++;
        error = get_expression (node, tokens, elems, funcs);
        if (tokens->tokens[tokens->pos]->type != OPER ||
            (int) tokens->tokens[tokens->pos]->value != RBRAC)
            RETURN_ERROR(SYNTAX_ERR, "Error in reading (). No ')' after '('");

        PARSE_ERROR_STR_NO_DUMP(error);
        tokens->pos++;
        RETURN_ERROR(CORRECT, "");
    }
    else
    {
        bool is_read = false;
        error = get_num (node, tokens, &is_read);
        PARSE_ERROR_STR_NO_DUMP(error);
        if (is_read)
            return error;

        error = get_math_oper (node, tokens, elems, funcs, &is_read);
        PARSE_ERROR_STR_NO_DUMP(error);
        if (is_read)
            return error;

        error = get_func_call (node, tokens, elems, funcs, &is_read);
        PARSE_ERROR_STR_NO_DUMP(error);
        if (is_read)
            return error;

        error = get_var (node, tokens, elems, &is_read);
        PARSE_ERROR_STR_NO_DUMP(error);
        if (!is_read)
            RETURN_ERROR(SYNTAX_ERR, "Unknown symbol");

        return error;
    }
}

Error get_num (Node** node, Tokens* tokens, bool* is_read)
{
    if (tokens->tokens[tokens->pos]->type == NUM)
    {
        *node = tokens->tokens[tokens->pos];
        tokens->pos++;
        *is_read = true;
    }
    else
    {
        *is_read = false;
    }
    RETURN_ERROR(CORRECT, "");
}

Error get_math_oper (Node** node, Tokens* tokens, Elements* elems, Functions* funcs, bool* is_read)
{
    if (tokens->tokens[tokens->pos]->type == OPER)
    {
        *is_read = true;
        *node = tokens->tokens[tokens->pos];
        tokens->pos++;
        return get_prim_expr (&((*node)->right), tokens, elems, funcs);
    }
    else
    {
        *is_read = false;
        RETURN_ERROR(CORRECT, "");
    }
}

Error get_var (Node** node, Tokens* tokens, Elements* elems, bool* is_read)
{
    if (tokens->tokens[tokens->pos]->type != VAR)
    {
        *is_read = false;
        RETURN_ERROR(CORRECT, "");
    }

    *node = tokens->tokens[tokens->pos];
    tokens->pos++;
    *is_read = true;

    if (!found_var (elems, (*node)->name))
        return append_var (elems, (*node)->name, VAR_DEF_VAL);

    RETURN_ERROR(CORRECT, "");
}

Error get_func_call (Node** node, Tokens* tokens, Elements* elems, Functions* funcs, bool* is_read)
{
    if (tokens->tokens[tokens->pos]->type != FUNC)
    {
        *is_read = false;
        RETURN_ERROR(CORRECT, "");
    }

    *node = tokens->tokens[tokens->pos];
    tokens->pos++;

    int id = found_func (funcs, (*node)->name);
    if (id == -1)
        RETURN_ERROR(SYNTAX_ERR, "Unknown function");

    (*node)->value = id;

    if (!(tokens->tokens[tokens->pos]->type == OPER && (int) tokens->tokens[tokens->pos]->value == LBRAC))
        RETURN_ERROR(SYNTAX_ERR, "No '(' after name of function");
    tokens->pos++;

    Error error = get_args (&((*node)->left), tokens, elems, funcs, false);
    PARSE_ERROR_STR_NO_DUMP(error);

    if (!(tokens->tokens[tokens->pos]->type == OPER && (int) tokens->tokens[tokens->pos]->value == RBRAC))
        RETURN_ERROR(SYNTAX_ERR, "No ')' after name of function");
    tokens->pos++;

    *is_read = true;

    RETURN_ERROR(CORRECT, "");
}

Error get_tokens (Tokens* tokens, ReadStr* str)
{
    tokens->size = 0;
    tokens->tokens = (Node**) calloc (str->size + 1, sizeof (Node*));

    Error error = {};
    size_t pos0 = str->pos;
    
    while (str->str[str->pos] != '\0')
    {
        skip_spaces (str);
        pos0 = str->pos;
        error = get_token_num (tokens, str);
        PARSE_ERROR_STR(str, error);
        if (pos0 != str->pos) continue;

        skip_spaces (str);
        pos0 = str->pos;
        error = get_token_oper_long (tokens, str);
        PARSE_ERROR_STR(str, error);
        if (pos0 != str->pos) continue;

        skip_spaces (str);
        pos0 = str->pos;
        error = get_token_oper (tokens, str);
        PARSE_ERROR_STR(str, error);
        if (pos0 != str->pos) continue;

        skip_spaces (str);
        error = get_token_var_func (tokens, str);
        PARSE_ERROR_STR(str, error);
    }
    tokens->tokens[tokens->size] = _END;
    tokens->size++;

    tokens->tokens = (Node**) realloc (tokens->tokens, (tokens->size + 1) * sizeof (Node*));
    RETURN_ERROR(CORRECT, "");
}

Error get_token_num (Tokens* tokens, ReadStr* str)
{
    int val1 = 0;
    size_t pos0 = str->pos;
    while ('0' <= str->str[str->pos] && str->str[str->pos] <= '9')
    {
        val1 = val1 * 10 + (str->str[str->pos] - '0');
        str->pos++;
    }

    int val2 = 0;
    size_t count = 0;
    double val = (double) val1;
    if (str->str[str->pos] == '.')
    {
        str->pos++;
        while ('0' <= str->str[str->pos] && str->str[str->pos] <= '9')
        {
            val2 = val2 * 10 + (str->str[str->pos] - '0');
            str->pos++;
            count++;
        }

        if (count == 0)
            RETURN_ERROR(SYNTAX_ERR, "No numbers after '.'");

        int st = 1;
        for (size_t i = 0; i < count; i++)
            st *= 10;

        val += (double) val2 / (double) st;
    }

    if (pos0 != str->pos)
    {
        tokens->tokens[tokens->size] = _NUM(val);
        tokens->size++;
    }
    RETURN_ERROR(CORRECT, "");
}

Error get_token_oper (Tokens* tokens, ReadStr* str)
{
    char text[2] = "";
    text[0] = str->str[str->pos];

    for (size_t i = 0; i < NUM_OPERS; i++)
    {
        if (strcmp (text, OPERATORS[i].name) == 0)
        {
            int val = OPERATORS[i].id;
            if (OPERATORS[i].type == OPER && OPERATORS[i].id == SUB)
            {
                if (tokens->size == 0)
                {
                    tokens->tokens[0] = _UNSUB(NULL, NULL);
                    tokens->size++;
                    str->pos++;
                    RETURN_ERROR(CORRECT, "");
                }

                Types type_prev = tokens->tokens[tokens->size - 1]->type;
                int val_prev = (int) tokens->tokens[tokens->size - 1]->value;
                if (type_prev == NUM || type_prev == VAR)
                    val = SUB;
                else if (type_prev == OPER)
                    if (val_prev == RBRAC)
                        val = SUB;
                    else if (val_prev == ADD || val_prev == MUL || val_prev == SUB || val_prev == DIV || val_prev == LBRAC || val_prev == ASSIGN)
                        val = UNSUB;
                    else
                        RETURN_ERROR(SYNTAX_ERR, "Unary minus after unary minus");
                else
                    RETURN_ERROR(SYNTAX_ERR, "No bracket after function");
            }

            tokens->tokens[tokens->size] = create_node (OPERATORS[i].type, val, NULL, NULL, NULL);
            tokens->size++;
            str->pos++;
            RETURN_ERROR(CORRECT, "");
        }
    }

    RETURN_ERROR(CORRECT, "");
}

Error get_token_oper_long (Tokens* tokens, ReadStr* str)
{
    char text[MAX_SIZE] = "";
    size_t count = 0;

    while (('a' <= str->str[str->pos] && str->str[str->pos] <= 'z') ||
            str->str[str->pos] == '_' || str->str[str->pos] == '>' ||
            str->str[str->pos] == '<' || str->str[str->pos] == '=' || str->str[str->pos] == '!')
    {
        text[count] = str->str[str->pos];
        count++;
        str->pos++;
    }

    for (size_t i = 0; i < NUM_OPERS; i++)
    {
        if (strcmp (text, OPERATORS[i].name) == 0)
        {
            tokens->tokens[tokens->size] = create_node (OPERATORS[i].type, OPERATORS[i].id, NULL, NULL, NULL);
            tokens->size++;
            RETURN_ERROR(CORRECT, "");
        }
    }

    for (size_t i = 0; i < count; i++)
        str->pos--;

    RETURN_ERROR(CORRECT, "");
}

Error get_token_var_func (Tokens* tokens, ReadStr* str)
{
    char name[MAX_SIZE] = "";
    size_t count = 0;

    if ('0' <= str->str[str->pos] && str->str[str->pos] <= '9')
        RETURN_ERROR(CORRECT, "");

    while (('a' <= tolower(str->str[str->pos]) && tolower(str->str[str->pos]) <= 'z') ||
           ('0' <= str->str[str->pos] && str->str[str->pos] <= '9') ||
           (str->str[str->pos] == '_'))
    {
        name[count] = str->str[str->pos];
        count++;
        str->pos++;
    }

    if (count != 0)
    {
        skip_spaces (str);

        if (tokens->size == 0)
        {
            tokens->tokens[tokens->size] = _VAR(name);
        }
        else
        {
            if ((tokens->tokens[tokens->size - 1]->type == OPER && (int) tokens->tokens[tokens->size - 1]->value == NEW_FUNC) ||
                (str->str[str->pos] == '('))
                tokens->tokens[tokens->size] = _FUNC(name);
            else
                tokens->tokens[tokens->size] = _VAR(name);
        }

        tokens->size++;
    }

    RETURN_ERROR(CORRECT, "");
}

void skip_spaces (ReadStr* str)
{
    while (isspace (str->str[str->pos]))
        str->pos++;
}
