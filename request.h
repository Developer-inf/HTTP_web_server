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
    std::vector<char> body;
    int socket_fd;
    
    Request(std::vector<char> &&req, int socket_fd);
    ~Request();
};

Request::Request(std::vector<char> &&req, int socket_fd) {
    this->socket_fd = socket_fd;
    
    const char *headerEnd = "\r\n\r\n";
    auto bodyDelim = std::search(req.begin(), req.end(), headerEnd, headerEnd + 4);
    if (bodyDelim != req.end()) {
        body.insert(body.end(), bodyDelim + 4, req.end());
        req.erase(bodyDelim, req.end());
    }
    
    std::string reqStr(req.data());
    parse_first_line(method, path, version, reqStr);
    parse_header(header, cookie, reqStr);
}

Request::~Request() {
}


#endif // _REQUEST