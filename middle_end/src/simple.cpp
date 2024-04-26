#include "simple.h"

void simple (Node* node)
{
    bool is_change1 = true;
    bool is_change2 = true;
    while (is_change1 || is_change2)
    {
        is_change1 = false;
        simple_nums (node, &is_change1);

        is_change2 = false;
        simple_vars (node, &is_change2);
    }
}

void simple_nums (Node* node, bool* is_change)
{
    if (!node)
        return;

    if (node->type != OPER)
        return;

    simple_nums (node->left, is_change);
    simple_nums (node->right, is_change);

    simple_nums_oper (node, is_change);
}

void simple_nums_oper (Node* node, bool* is_change)
{
    if (!node || !node->right)
        return;

    bool need_simple = false;
    if (RTYP == NUM)
    {
        if (node->left)
            if (LTYP == NUM)
                need_simple = true;
            else
                need_simple = false;
        else
            need_simple = true;
    }
    else
    {
        need_simple = false;
    }

    if (need_simple)
    {
        *is_change = true;
        node->type = NUM;
        switch ((int) node->value)
        {
            case ADD:
                node->value = LVAL + RVAL;
                break;
            case MUL:
                node->value = LVAL * RVAL;
                break;
            case SUB:
                node->value = LVAL - RVAL;
                break;
            case DIV:
                node->value = LVAL / RVAL;
                break;
            case COS:
                node->value = cos (RVAL);
                break;
            case SIN:
                node->value = sin (RVAL);
                break;
            case SQRT:
                node->value = sqrt (RVAL);
                break;
            case LN:
                node->value = log (RVAL);
                break;
            case POW:
                node->value = pow (LVAL, RVAL);
                break;
            case ARCSIN:
                node->value = asin (RVAL);
                break;
            case ARCCOS:
                node->value = acos (RVAL);
                break;
            case ARCTG:
                node->value = atan (RVAL);
                break;
            case TG:
                node->value = tan (RVAL);
                break;
            default:
                break;
        }

        nodes_dtor (node->left);
        nodes_dtor (node->right);
        node->left = NULL;
        node->right = NULL;
    }
}

void simple_vars (Node* node, bool* is_change)
{
    if (!node)
        return;

    if (node->type != OPER)
        return;

    simple_vars (node->left, is_change);
    simple_vars (node->right, is_change);

    if (mul_zero (node) || div_zero (node))
    {
        node->type = NUM;
        node->value = 0;
        *is_change = true;

        nodes_dtor (node->left);
        nodes_dtor (node->right);

        node->left = NULL;
        node->right = NULL;
        return;
    }

    Node* new_simple_node = NULL;
    if (mul_one (node, &new_simple_node) || add_sub_zero (node, &new_simple_node) || div_one (node, &new_simple_node) || pow_one (node, &new_simple_node))
    {
        if (new_simple_node == _RIGHT)
            del_node (_LEFT);
        else
            del_node (_RIGHT);

        node->type = new_simple_node->type;
        node->value = new_simple_node->value;
        node->right = new_simple_node->right;
        node->left = new_simple_node->left;
        node->name = strdup (new_simple_node->name);
        *is_change = true;

        del_node (new_simple_node);
        return;
    }

    if (pow_zero (node) || one_pow (node))
    {
        node->type = NUM;
        node->value = 1;
        *is_change = true;

        nodes_dtor (node->left);
        nodes_dtor (node->right);

        node->left = NULL;
        node->right = NULL;
        return;
    }
}

bool pow_zero (Node* node)
{
    return (((int) node->value == POW) &&
            (RTYP == NUM) && (is_zero (RVAL)));
}

bool one_pow (Node* node)
{
    return (((int) node->value == POW) &&
            (LTYP == NUM) && (is_zero (LVAL - 1)));
}

bool pow_one (Node* node, Node** new_simple_node)
{
    if (((int) node->value == POW) && (RTYP == NUM) && (is_zero (RVAL - 1)))
    {
        *new_simple_node = _LEFT;
        return true;
    }
    return false;
}

bool mul_zero (Node* node)
{
    return (((int) node->value == MUL &&
            ((is_zero (LVAL) && LTYP == NUM) ||
            (is_zero (RVAL) && RTYP == NUM))));
}

bool mul_one (Node* node, Node** new_simple_node)
{
    if ((int) node->value == MUL)
    {
        if (is_zero (LVAL - 1) && LTYP == NUM)
        {
            *new_simple_node = _RIGHT;
            return true;
        }
        else if (is_zero (RVAL - 1) && RTYP == NUM)
        {
            *new_simple_node = _LEFT;
            return true;
        }
    }
    return false;
}

bool add_sub_zero (Node* node, Node** new_simple_node)
{
    if (((int) node->value == ADD || (int) node->value == SUB))
    {
        if (is_zero (LVAL) && LTYP == NUM && (int) node->value != SUB)
        {
            *new_simple_node = _RIGHT;
            return true;
        }
        else if (is_zero (RVAL) && RTYP == NUM)
        {
            *new_simple_node = _LEFT;
            return true;
        }
    }
    return false;
}

bool div_one (Node* node, Node** new_simple_node)
{
    if ((int) node->value == DIV && RTYP == NUM && is_zero (RVAL - 1))
    {
        *new_simple_node = _LEFT;
        return true;
    }
    return false;
}

bool div_zero (Node* node)
{
    return ((((int) node->value == DIV) &&
            (LTYP == NUM && is_zero (LVAL))));
}
