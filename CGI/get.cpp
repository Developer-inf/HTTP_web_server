#include <iostream>
#include <pqxx/pqxx>
#include <vector>
#include <string>
#include "../request.h"
#include "../config/db_config.h"

typedef std::vector<std::pair<std::string, std::string>> request_result;

request_result get_data(pqxx::connection &conn) {
    std::string query = "SELECT * FROM " + db_tablename + ";";
    pqxx::work worker(conn);
    pqxx::result res(worker.exec(query));
    request_result ret;
    ret.reserve(res.size() * sizeof(std::pair<std::string, std::string>));
    
    // for (auto row = res.begin(); row != res.end(); row++) {
    for (auto row : res) {
        ret.emplace_back(row[1].as<std::string>(), row[2].as<std::string>());
    }
    
    return ret;
    // return std::map<std::string, std::string>();
}

int GetCGI(Request &req, std::map<std::string, std::string> &keys_values) {
    try {
        const std::string conn_params = "dbname = " + db_dbname + " user = " + db_user + " password = " + db_password + 
                                        " host = " + db_hostaddr + " port = " + db_port;
        pqxx::connection conn(conn_params);
        
        
        if (conn.is_open()) {
            fprintf(stderr, "[SUCCESS] Database \"postgres\" successfully opened!\n");
        } else {
            throw "Cant open test db";
        }
        
        request_result keys_values = get_data(conn);
        // std::map<std::string, std::string> keys_values;
        
        std::string response;
        response.reserve(100);
        for (const auto &[key, val] : keys_values) {
            response += key + ';' + val + '\n';
        }
        response = response.substr(0, response.size() - 1);
        printf( "HTTP/1.1 200 OK\r\n"
                "Content-Type: text/plain\r\n"
                "Content-Length: %ld\r\n\r\n"
                "%s", response.size(), response.c_str());
        // printf("window.location.href = 'http://www.google.com';\r\n");
    } catch (char const *ex) {
        fprintf(stderr, "[ERROR] %s\n", ex);
        printf( "HTTP/1.1 500 Internal Server Error\r\n"
                "Connection: close\r\n");
        return 1;
    }
    
    return 0;
}