#ifndef ERROR_HEADER
#define ERROR_HEADER

#define RETURN_ERROR(code, message) \
        return Error {code, __LINE__, __FILE__, __func__, message}

#define GET_ERROR(code, message) \
        Error {code, __LINE__, __FILE__, __func__, message}

enum Error_codes
{
    CORRECT =           0,
    NULL_POINTER =      1,
    MEM_ALLOC =         2,
    INCOR_PARAMS =      3,
    NEGATIVE_SIZE =     4,
    INCOR_STRING_SIZE = 5,
    FILE_OPEN_ERR     = 6,
    INCOR_POS =         7,
    INCOR_ZERO_ELEM =   8,
    INCOR_LAST_ELEM =   9,
    CYCLES =            10,
    ELEMS_MORE_SIZE =   11,
    PREV_OF_NEXT =      12,
    LIST_OVERFLOW =     13,
    POS_MORE_SIZE =     14,
    VARS_OVERFLOW =     15,
    SYNTAX_ERR =        16
};

struct Error
{
    Error_codes code;
    int         line;
    const char* file;
    const char* func;
    const char* message;
};

#endif //ERROR_HEADER
