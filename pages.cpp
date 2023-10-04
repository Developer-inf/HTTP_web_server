#include <string>
#include <fstream>
#include <sys/socket.h>
#include <vector>
#include <string.h>
#include <unistd.h>
#include "request.h"
#include "errors.h"
#include "CGI/add.cpp"
#include "CGI/get.cpp"


// char **SplitPOSTBody(Request *r) {
//     char **ans = nullptr;
//     std::vector<int> args;
    
//     size_t i = 0, j = 0;
//     while (i < r->body.length()) {
//         if (r->body[i] == '&') {
//             args.emplace_back(i);
//         }
//         i++;
//     }
    
//     i = 1;
//     ans = new char *[args.size() + 3];
//     for (int pos : args) {
//         ans[i] = strdup(r->body.substr(j, pos - j).c_str());
//         j = pos + 1;
//         i++;
//     }
//     ans[i++] = strdup(r->body.substr(j).c_str());
//     ans[i] = NULL;
//     ans[0] = strdup(r->path.c_str());
    
//     return ans;
// }

std::map<std::string, std::string> SplitPOSTBody(Request *r) {
    std::map<std::string, std::string> ans;
    std::vector<int> args;
    
    for (size_t i = 0; i <= r->body.size(); i++) {
        if (r->body[i] == '&' || r->body[i] == '\0' || r->body[i] == '\n' || r->body[i] == '\r') {
            args.emplace_back(i);
        }
    }
    
    size_t eq_char = 0, key_start = 0;
    for (int and_char : args) {
        eq_char = r->body.find_first_of("=\0\r\n", key_start);
        printf("eq char: %d\n", eq_char);
        ans[r->body.substr(key_start, eq_char - key_start)] = r->body.substr(eq_char + 1, and_char - eq_char - 1);
        key_start = and_char + 1;
    }
    ans["path"] = r->path;
    
    return ans;
}

void ExecuteCGI(Request *r) {
    r->path = "./CGI/" + r->path.substr(1);
    
    std::map<std::string, std::string> post_body = SplitPOSTBody(r);
    dup2(r->socket_fd, STDOUT_FILENO);
    
    if (r->path == "./CGI/get.cgi") {
        GetCGI(*r, post_body);
    } else if (r->path == "./CGI/add.cgi") {
        AddCGI(*r, post_body);
    }
    
    exit(0);
}

std::string get_daytime()
{
    time_t rawtime;
    struct tm * timeinfo;
    char buffer[80];
    
    time (&rawtime);
    timeinfo = localtime(&rawtime);
    
    strftime(buffer, sizeof(buffer), "%a %b %d %H:%M:%S %Y", timeinfo);
    std::string time(buffer);
    
    return time;
}

std::string MakeHeader(std::string &&type, int body_length) {
    return  "HTTP/1.1 200 OK\r\n"
            "Date: " + get_daytime() + "\r\n"
            "Content-Type: " + type + "; charset=utf-8\r\n"
            "Content-Length: " + std::to_string(body_length) + "\r\n\n";
}

void render(int socket_fd, std::string &&filename, std::string &&header_type) {
    if (filename == "") {
        std::string resp = "HTTP/1.1 404 Not found\r\nDate" + get_daytime() + "\r\nConnection: close\r\n";
        send(socket_fd, resp.c_str(), resp.length(), 0);
        return;
    }
    
    std::string buffer = "";
    std::string response = "";
    std::ifstream f(filename, std::ios::in);
    
    if (f.bad()) {
        perror("Ifstream");
        exit(IFSTREAM_ERROR);
    }
    
    while (f.good()) {
        std::getline(f, buffer);
        response += buffer + '\n';
    }
    
    std::string header = MakeHeader(std::move(header_type), response.length());
    response = header + response;
    send(socket_fd, response.c_str(), response.length(), 0);
    fprintf(stdout, "SENT:\n\n%s\n\n", header.c_str());
}

void main_page(Request *r) {
    std::string header_type = "text/html";
    std::string filename = "html/main.html";
    render(r->socket_fd, std::move(filename), std::move(header_type));
}

void css_page(Request *r) {
    std::string header_type = "text/css";
    std::string filename = "css/css.css";
    render(r->socket_fd, std::move(filename), std::move(header_type));
}

void get_data_js_page(Request *r) {
    std::string header_type = "text/javascript";
    std::string filename = "js/get_data.js";
    render(r->socket_fd, std::move(filename), std::move(header_type));
}

void test_page(Request *r) {
    std::string header_type = "text/html";
    std::string filename = "html/another.html";
    render(r->socket_fd, std::move(filename), std::move(header_type));
}

void add_data_page(Request *r) {
    std::string header_type = "text/html";
    std::string filename = "html/add_data.html";
    render(r->socket_fd, std::move(filename), std::move(header_type));
}

void check_data_page(Request *r) {
    if (r->method == "POST") {
        r->path = "/get.cgi";
        ExecuteCGI(r);
    } else {
        std::string header_type = "text/html";
        std::string filename = "html/check_data.html";
        render(r->socket_fd, std::move(filename), std::move(header_type));
    }
}

void proceed_cgi_page(Request *r) {
    if (r->method == "POST") {
        ExecuteCGI(r);
    } else {
        std::string header_type = "text/html";
        std::string filename = "html/404_not_found.html";
        render(r->socket_fd, std::move(filename), std::move(header_type));
    }
}