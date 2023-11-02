#if !defined(_REQUEST)
#define _REQUEST

#include <string>
#include <map>

using std::size_t;

void parse_first_line(std::string &method, std::string &path, std::string &version, std::string &req) {
    size_t fp = req.find(' ');
    size_t sp = req.find(' ', fp + 1);
    size_t r = req.find('\r');
    method = req.substr(0, fp);
    std::transform(method.begin(), method.end(), method.begin(), [](char ch) { return std::toupper(ch); });
    path = req.substr(fp + 1, sp - fp - 1);
    version = req.substr(sp + 1, r - sp - 1);
    req = req.substr(r + 2);
}

void parse_header(std::map<std::string, std::string> &header, std::map<std::string, std::string> &cookie, std::string &req) {
    std::string line = std::move(req.substr(0, req.find('\n')));
    while (line.size() - 1 > 0) {
        size_t pos = line.find(':');
        std::string key = std::move(line.substr(0, pos));
        if (line[pos + 1] == ' ') { pos++; }
        std::string val = std::move(line.substr(pos + 1, line.find('\r') - pos - 1));
        header.insert(std::make_pair(key, val));
        req = std::move(req.substr(line.size() + 1));
        line = std::move(req.substr(0, req.find('\n')));
    }
    if (!header.contains("Cookie")) {
        cookie.clear();
    } else {
        std::string key = "", val = "";
        std::string cookie_line = std::move(header["Cookie"]);
        int start = 0, eq = 0;
        for (int i = 0; i <= cookie_line.size(); i++) {
            if (cookie_line[i] == '=') {
                eq = i;
                key = cookie_line.substr(start, i - start);
            } else if (cookie_line[i] == ';' || i == cookie_line.size()) {
                val = cookie_line.substr(eq + 1, i - eq - 1);
                start = i + 2;
                cookie.insert(std::make_pair(key, val));
            }
        }
    }
}

class Request {
public:
    std::string method;
    std::string path;
    std::string version;
    std::map<std::string, std::string> header;
    std::map<std::string, std::string> cookie;
    std::string body;
    int socket_fd;
    
    Request(std::string &&req, int socket_fd);
    ~Request();
};

Request::Request(std::string &&req, int socket_fd) {
    this->socket_fd = socket_fd;
    size_t end_pos = req.find("\r\n");
    if (end_pos == std::string::npos) {
        fprintf(stderr, "Can't parse request\n");
        return;
    }
    
    parse_first_line(method, path, version, req);
    parse_header(header, cookie, req);
    try {
        body = req.substr(2);
    }
    catch(const std::exception& e) {
        printf("req: %s\n", req.c_str());
        printf("body: %s\n", body.c_str());
        // fprintf(stderr, "%s\n", e.what());
    }
}

Request::~Request() {
}


#endif // _REQUEST