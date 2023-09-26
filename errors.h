#if !defined(_ERRORS)
#define _ERRORS


enum ERRORS {
    UNKNOWN_METHOD_ERROR = 2,
    UNKNOWN_PATH_ERROR,
    CANT_READ_BODY,
    FORK_ERROR,
    EXECVP_ERROR,
    SOCKET_ERROR,
    SETSOCKET_ERROR,
    BIND_ERROR,
    LISTEN_ERROR,
    ACCEPT_ERROR,
    IFSTREAM_ERROR
};


#endif // _ERRORS
