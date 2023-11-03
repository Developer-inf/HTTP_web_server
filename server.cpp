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
#define BUFFSIZE 4096


int main(int argc, char const *argv[])
{
    int server_fd, new_socket;
    ssize_t valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[BUFFSIZE] = { 0 };
    // char* hello = "Hello from server";
    std::string response;
    std::string string_buffer;
    string_buffer.reserve(BUFFSIZE);
    std::vector<std::string> allowed_urls = { "/sign-in", "/sign-in.js", "/css.css", "/sign-up-page", "/sign-up", "/sign-up.js", "/favicon.ico" };
    
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
        
        string_buffer = "";
        do {
            std::fill_n(buffer, sizeof(buffer), 0);
            valread = read(new_socket, buffer, BUFFSIZE);
            string_buffer += buffer;
        } while (valread >= sizeof(buffer));
        
        fprintf(stderr, DELIMS "RECIEVED:" DELIMS "\n\n%s\n", string_buffer.c_str());
        Request request = Request(std::move(string_buffer), new_socket);
        
        if (request.cookie.empty() && std::find(allowed_urls.begin(), allowed_urls.end(), request.path) == allowed_urls.end()) {
            render(request.socket_fd, "html/login.html", "text/html");
            return 0;
        }
        
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
