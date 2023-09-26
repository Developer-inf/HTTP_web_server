#ifndef _REQUEST
#define _REQUEST

#include <string>

struct Request {
    std::string method;
    std::string path;
    std::string body;
    std::string referer;
    int socket_fd;
};

#endif // _REQUEST