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


int main(int argc, char const *argv[])
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    
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
        
        ssize_t valread;
        char buffer[BUFFSIZE] = { 0 };
        // std::vector<char> buffer(BUFFSIZE, 0);
        // char* hello = "Hello from server";
        std::vector<char> vectorBuffer;
        vectorBuffer.reserve(BUFFSIZE);
        std::vector<std::string> allowed_urls = { "/sign-in", "/sign-in.js", "/css.css", "/sign-up-page", "/sign-up", "/sign-up.js", "/favicon.ico" };
        
        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 20'000;
        setsockopt(new_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
        
        do {
            valread = recv(new_socket, buffer, BUFFSIZE, 0);
            if (valread <= 0) break;
            vectorBuffer.insert(vectorBuffer.end(), buffer, buffer + valread);
        } while (valread > 0);
        
        if (vectorBuffer.size() < 1000) {
            fprintf(stderr, DELIMS "RECIEVED (%ld b):" DELIMS "\n\n%-*s\n", vectorBuffer.size(), static_cast<int>(vectorBuffer.size()), vectorBuffer.data());
        }
        else {
            fprintf(stderr, DELIMS "RECIEVED (%ld b):" DELIMS "\n\n%.*s ...\n", vectorBuffer.size(), 1000, vectorBuffer.data());
        }
        fflush(stderr);
        Request request = Request(std::move(vectorBuffer), new_socket);
        
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
