#include "print.h"
#include "operators.h"

void print_funcs (const Functions* funcs, FILE* file)
{
    for (size_t i = 0; i < funcs->num_funcs; i++)
        print_func (funcs->funcs[i]->root, file);
}

void print_func (const Node* node, FILE* file)
{
    if (strcmp (node->name, "main") != 0)
    {
        fprintf (file, "houmi %s (", node->name);
        print_args_declaration (node->left, file);
        fprintf (file, ")\nlesss_go\n");
        print_commands (node->right, file);
        fprintf (file, "stop_that_shit\n\n");
    }
    else
    {
        print_commands (node->right, file);
    }
}

void print_args_declaration (const Node* node, FILE* file)
{
    if (!node)
        return;

    if (node->right)
        fprintf (file, "%s, ", node->left->name);
    else
        fprintf (file, "%s", node->left->name);

    print_args_declaration (node->right, file);
}

void print_commands (const Node* node, FILE* file)
{
    if (!node)
        return;

    print_command (node->left, file);
    if ((int) node->left->value != IF && (int) node->left->value != WHILE)
        fprintf (file, ";\n");
    print_commands (node->right, file);
}

void print_command (const Node* node, FILE* file)
{
    if (node->type == NUM)
    {
        fprintf (file, "%lg", node->value);
        return;
    }

    if (node->type == FUNC)
    {
        fprintf (file, "func%d (", (int) node->value);
        print_args_call (node->left, file);
        fprintf (file, ")");
        return;
    }

    if (node->type == VAR)
    {
        fprintf (file, "var%d", (int) node->value);
        return;
    }

    for (size_t i = 0; i < NUM_OPERS; i++)
    {
        if ((int) node->value == OPERATORS[i].id)
        {
            if ((int) node->value == IF || (int) node->value == WHILE)
            {
                fprintf (file, "%s (", OPERATORS[i].name);
                print_command (node->left, file);
                fprintf (file, ")\nlesss_go\n");
                print_commands (node->right, file);
                fprintf (file, "stop_that_shit\n");
            }
            else if ((int) node->value == PRINT || (int) node->value == INPUT_VAR || (int) node->value == RET)
            {
                fprintf (file, "%s (", OPERATORS[i].name);
                print_command (node->right, file);
                fprintf (file, ")");
            }
            else if ((int) node->value == ASSIGN)
            {
                print_command (node->right, file);
                fprintf (file, " aka ");
                print_command (node->left, file);
            }
            else
            {
                if (node->left)
                {
                    fprintf (file, "(");
                    print_command (node->left, file);
                    fprintf (file, ")");
                }
                fprintf (file, " %s ", OPERATORS[i].name);
                if (node->right)
                {
                    fprintf (file, "(");
                    print_command (node->right, file);
                    fprintf (file, ")");
                }
            }
            return;
        }
    }
}

void print_args_call (const Node* node, FILE* file)
{
    if (!node)
        return;

    print_command (node->left, file);
    if (node->right)
        fprintf (file, ", ");

    print_args_call (node->right, file);
}
