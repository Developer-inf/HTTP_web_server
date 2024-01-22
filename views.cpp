#include <iostream>
#include <string>
#include <fstream>
#include <sys/socket.h>
#include <vector>
#include <string.h>
#include <unistd.h>
#include <pqxx/pqxx>
#include <filesystem>
#include "request.h"
#include "errors.h"
#include "CGI/add.cpp"
#include "CGI/get.cpp"

#define DELIMS "--------------------"

namespace fs = std::filesystem;

void escaped_print(std::string &&str) {
    for (size_t i = 0; i < str.size(); i++) {
        u_char c = str[i];
        switch (c)
        {
            case '\\':
                printf("\\\\");
                break;
            case '\n':
                printf("\\n");
                break;
            case '\r':
                printf("\\r");
                break;
            case '\t':
                printf("\\t");
                break;
            case '\0':
                printf("\\0");
                break;
            
            default:
                if (isprint(c)) {
                    putchar(c);
                } else {
                    printf("\\x%X", c);
                }
                break;
        }
    }
    printf("\n");
}
void escaped_print(std::vector<char> &&str) {
    for (size_t i = 0; i < str.size(); i++) {
        u_char c = str[i];
        switch (c)
        {
            case '\\':
                printf("\\\\");
                break;
            case '\n':
                printf("\\n");
                break;
            case '\r':
                printf("\\r");
                break;
            case '\t':
                printf("\\t");
                break;
            case '\0':
                printf("\\0");
                break;
            
            default:
                if (isprint(c)) {
                    putchar(c);
                } else {
                    printf("\\x%X", c);
                }
                break;
        }
    }
    printf("\n");
}

std::map<std::string, std::string> SplitPOSTBody(Request *r) {
    std::map<std::string, std::string> ans;
    std::vector<std::vector<char>::iterator> argsIters;
    
    for (auto b = r->body.begin(); b != r->body.end(); b++) {
        if (*b == '&' || *b == '\0' || *b == '\n' || *b == '\r') {
            argsIters.emplace_back(b);
        }
    }
    
    std::vector<char>::iterator eqIter, startIter = r->body.begin();
    for (auto chIter = r->body.begin(); chIter != r->body.end() && *chIter != '\0'; chIter++) {
        if (*chIter == '=') {
            eqIter = chIter;
        }
        else if (*chIter == '\r' || *chIter == '\n' || *chIter == '\0') {
            std::string key = std::move(std::string(startIter, eqIter));
            std::string val = std::move(std::string(eqIter + 1, chIter));
            ans[key] = val;
            
            while (*chIter == '\r' || *chIter == '\n' || *chIter == '\0') {
                chIter++;
            }
            startIter = chIter;
        }
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

inline std::string MakeHeader(std::string &&type, int body_length) {
    return  "HTTP/1.1 200 OK\r\n"
            "Date: " + get_daytime() + "\r\n"
            "Content-Type: " + type + "; charset=utf-8\r\n"
            "Content-Length: " + std::to_string(body_length) + "\r\n\r\n";
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

void get_files(Request *r) {
    std::string resp = "", filename = "", person_id = "", file = "";
    int id_start = 0;
    
    std::string path = "./files";
    for (const auto & entry : fs::directory_iterator(path)) {
        filename = entry.path().string();
        filename = filename.substr(filename.find_last_of('/') + 1);
        id_start = filename.find_last_of('.');
        person_id = filename.substr(id_start + 1);
        if (person_id != r->cookie["person_id"]) continue;
        file = filename.substr(0, id_start);
        resp += file + ':' + std::to_string(entry.file_size()) + '\n';
    }
    
    resp = MakeHeader("text/plain", resp.size() - 1) + resp;
    send(r->socket_fd, resp.c_str(), resp.size() - 1, 0);
}

void load_file(Request *r) {
    std::string endHeader = "\r\n\r\n";
    
    auto endHeaderIter = std::search(r->body.begin(), r->body.end(), endHeader.begin(), endHeader.end());
    auto endBoundaryIter = std::search(r->body.begin(), r->body.end(), endHeader.begin(), endHeader.begin() + 2);
    auto endContentDisposition = std::search(endBoundaryIter + 2, endHeaderIter, endHeader.begin(), endHeader.begin() + 2);
    // find boundary
    std::vector<char> boundary(r->body.begin(), endBoundaryIter);
    
    // find filename
    std::string name = "filename=\"";
    auto nameStart = std::search(endBoundaryIter + 2, endContentDisposition, name.begin(), name.end()) + name.size();
    auto nameEnd = std::find(nameStart, endContentDisposition, '"');
    std::string filename(nameStart, nameEnd);
    filename = "files/" + filename + '.' + r->cookie["person_id"];
    
    // get binary data
    auto endDataIter = std::search(endHeaderIter, r->body.end(), boundary.begin(), boundary.end()) - 2;
    std::vector<char> fileData(endHeaderIter + 4, endDataIter);
    
    // write to file
    std::ofstream file;
    // if (std::ifstream(filename).good()) {
    //     file.open(filename, std::ios::app);
    // }
    // else {
    //     file.open(filename);
    // }
    file.open(filename);
    
    file.write(fileData.data(), fileData.size() * sizeof(char));
    file.close();
    
    std::string resp =  "HTTP/1.1 200 Ok\r\n";
                        // "Content-Type: text/html\r\n\r\n"
                        // "<script>window.location.href = \"http://localhost:42069/load-file-page\";</script>\r\n";
    send(r->socket_fd, resp.c_str(), resp.size(), 0);
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

std::pair<std::string, std::string> get_login_password(std::string &body) {
    int delim = body.find('&');
    
    int eq = body.find_first_of('=', 0);
    std::string login = body.substr(eq + 1, delim - eq - 1);
    
    eq = body.find_first_of('=', delim);
    std::string password = body.substr(eq + 1, body.size() - eq - 1);
    
    return std::make_pair(login, password);
}

void sign_in(Request *r) {
    try {
        const std::string conn_params = "dbname = " + db_dbname + " user = " + db_user + " password = " + db_password + 
                                        " host = " + db_hostaddr + " port = " + db_port;
        pqxx::connection conn(conn_params);
        std::string bodyStr = std::string(r->body.begin(), r->body.end());
        auto [login, pass] = get_login_password(bodyStr);
        std::string query = "SELECT * FROM " + db_users_tablename + " WHERE login = '" + login + "' and password = '" + pass + "';";
        pqxx::work worker(conn);
        pqxx::result res(worker.exec(query));
        if (res.size() < 1) {
            fprintf(stderr, "QUERY: %s\n", query.c_str());
            throw("User doesn't exist!");
        }
        std::string person_id = res.begin()[0].as<std::string>();
        // std::string person_id = std::to_string(rand());
        std::string ans = "HTTP/1.1 200 Ok\r\nSet-Cookie: person_id=" + person_id + "; Max-Age=3000\r\n"
                "Content-Type: text/html\r\n\r\n"
                "<script>window.location.href = \"http://localhost:42069/\";</script>\r\n";
        send(r->socket_fd, ans.c_str(), ans.size(), 0);
    } catch (char const*ex) {
        fprintf(stderr, "[ERROR] %s\n", ex);
        std::string ans =   "HTTP/1.1 500 Internal Server Error\r\n"
                            "Content-Type: text/html\r\n\r\n"
                            "<script>window.location.href = \"http://localhost:42069/\";</script>\r\n";
        send(r->socket_fd, ans.c_str(), ans.size(), 0);
        return;
    }
}

void sign_up(Request *r) {
    try {
        const std::string conn_params = "dbname = " + db_dbname + " user = " + db_user + " password = " + db_password + 
                                        " host = " + db_hostaddr + " port = " + db_port;
        pqxx::connection conn(conn_params);
        std::string bodyStr = std::string(r->body.begin(), r->body.end());
        auto [login, pass] = get_login_password(bodyStr);
        std::string query = "INSERT INTO " + db_users_tablename + " (login, password) VALUES ('" + login + "', '" + pass + "');";
        pqxx::work worker(conn);
        worker.exec(query);
        
        query = "SELECT person_id FROM " + db_users_tablename + " WHERE login = '" + login + "' and password = '" + pass + "' LIMIT 1;";
        pqxx::result res(worker.exec(query));
        std::string person_id = res.begin()[0].as<std::string>();
        worker.commit();
        
        std::string ans = "HTTP/1.1 200 Ok\r\nSet-Cookie: person_id=" + person_id + "; Max-Age=3000\r\n"
                "Content-Type: text/html\r\n\r\n"
                "<script>window.location.href = \"http://localhost:42069/\";</script>\r\n";
        send(r->socket_fd, ans.c_str(), ans.size(), 0);
    } catch (char const*ex) {
        fprintf(stderr, "[ERROR] %s\n", ex);
        std::string ans =   "HTTP/1.1 500 Internal Server Error\r\n"
                            "Content-Type: text/html\r\n\r\n"
                            "<script>window.location.href = \"http://localhost:42069/\";</script>\r\n";
        send(r->socket_fd, ans.c_str(), ans.size(), 0);
        return;
    }
}

void exec_command(Request *r) {
    std::string params = r->path.substr(6);
    std::vector<std::string> args;
    int beg = 0;
    for (size_t i = 0; i <= params.size(); i++) {
        if (params[i] == '&' || i == params.size()) {
            args.emplace_back(params.substr(beg, i - beg));
            beg = i + 1;
        }
    }
    char **out = new char*[args.size() + 1];
    printf("PARAMS: %s\n", params.c_str());
    for (size_t i = 0; i < args.size(); i++) {
        if (args[i].find("%3") != std::string::npos) {
            args[i].replace(args[i].find("%3"), 6, ">>");
        }
        out[i] = strdup(args[i].c_str());
    }
    out[args.size()] = NULL;
    send(r->socket_fd, "HTTP/1.1 200 Ok", 16, 0);
    
    execvp(out[0], out);
    perror("Execv");
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