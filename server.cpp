#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
// #include <string.h>
#include <string>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <utility>
#include <iomanip>
#include <algorithm>
#include <map>
#include <vector>
#include "urls.cpp"
#include "request.h"
#include "errors.h"


#define PORT 42069


// Request *ParseRequest(std::string &&request, int socket_fd) {
//     static Request r;
//     r.socket_fd = socket_fd;
//     r.method = request.substr(0, request.find(' '));
//     std::transform(r.method.begin(), r.method.end(), r.method.begin(), ::toupper);
//     r.path = request.substr(
//                 r.method.length() + 1, 
//                 request.find(' ', r.method.length() + 1) - r.method.length() - 1
//             );
    
//     int idx = 0;
    
//     if ((idx = request.find("Referer")) > 0) {
//         idx = request.find(' ', idx) + 1;
//         r.referer = request.substr(idx, request.find('\n', idx) - idx);
//         // printf("\n\nREFERER:\t%s\n\n", r.referer.c_str());
//     } else { r.referer = ""; }
    
//     idx = request.find("\r\n\r\n");
//     if (idx == std::string::npos) {
//         idx = request.find("\r\n\n");
//         if (idx == std::string::npos) {
//             r.body = "";
//         } else {
//             r.body = request.substr(idx + 3);
//         }
//     } else {
//         r.body = request.substr(idx + 4);
//     }
    
//     return &r;
// }


int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = { 0 };
    // char* hello = "Hello from server";
    std::string response;
    
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(SOCKET_ERROR);
    }
    
    // Forcefully attaching socket to the port
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(SETSOCKET_ERROR);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    
    // Forcefully attaching socket to the port
    int bind_res = bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    if (bind_res < 0) {
        perror("bind failed");
        exit(BIND_ERROR);
    }
    
    if (listen(server_fd, 5) < 0) {
        perror("listen");
        exit(LISTEN_ERROR);
    }
    
    fprintf(stderr, "Server started successfully\n");
    
    while (true) {
        new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
        if (new_socket < 0) {
            perror("accept");
            exit(ACCEPT_ERROR);
        }
        
        int pid = fork();
        
        if (pid < 0) {
            perror("Fork\n");
            exit(FORK_ERROR);
        } else if (pid > 0) {
            close(new_socket);
            continue;
        }
        
        std::fill_n(buffer, sizeof(buffer), 0);
        valread = read(new_socket, buffer, 1024);
        fprintf(stderr, DELIMS "RECIEVED:" DELIMS "\n\n%s\n", buffer);
        
        Request request = Request(std::string(buffer), new_socket);
        // MakeResponse(request);
        UrlToPage(&request);
        
        // closing the connected socket
        close(new_socket);
        exit(0);
    }
    
    // closing the listening socket
    shutdown(server_fd, SHUT_RDWR);
    
    return 0;
}
