#include <iostream>
#include <pqxx/pqxx>
#include <map>
#include <string>


std::map<std::string, std::string> get_data(pqxx::connection &conn) {
    const std::string tablename("test");
    std::string query = "SELECT * FROM " + tablename + ";";
    pqxx::work worker(conn);
    pqxx::result res(worker.exec(query));
    std::map<std::string, std::string> ret;
    
    // for (auto row = res.begin(); row != res.end(); row++) {
    for (auto row : res) {
        ret.emplace(row[1].as<std::string>(), row[2].as<std::string>());
    }
    
    return ret;
    // return std::map<std::string, std::string>();
}

int GetCGI(Request &req, std::map<std::string, std::string> &keys_values)
{
    try {
        pqxx::connection conn("dbname = postgres user = paul password = postgres hostaddr = 127.0.0.1 port = 5432");
        
        if (conn.is_open()) {
            fprintf(stderr, "[SUCCESS] Database \"test\" successfully opened!\n");
        } else {
            throw "Cant open test db";
        }
        
        std::map<std::string, std::string> keys_values = get_data(conn);
        // std::map<std::string, std::string> keys_values;
        
        std::string json("{");
        json.reserve(100);
        for (const auto &[key, val] : keys_values) {
            json += "\"" + key + "\": \"" + val + "\",";
        }
        json[json.size() - 1] = '}';
        // json += "}";
        
        printf( "HTTP/1.1 200 OK\r\n"
                "Content-Type: application/json\r\n\r\n"
                "%s\r\n", json.c_str());
        // printf("window.location.href = 'http://www.google.com';\r\n");
    } catch (char const*ex) {
        fprintf(stderr, "[ERROR] %s\n", ex);
        printf( "HTTP/1.1 500 Internal Server Error\r\n"
                "Connection: close\r\n");
        return 1;
    }
    
    return 0;
}
