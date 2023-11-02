#include <iostream>
#include <map>
#include <pqxx/pqxx>
#include "../request.h"
#include "../config/db_config.h"

// #define TRY try {
// #define CATCH } catch (std::exception ex) { fprintf(stderr, "%s\n", ex.what()); return 1; }


void CheckValuesForExists(std::map<std::string, std::string> &values, pqxx::connection &conn) {
    std::string query = "SELECT * FROM " + db_tablename + " WHERE str='" + values["str"] + "' and num=" + values["num"] + ";";
    pqxx::work worker(conn);
    pqxx::result res(worker.exec(query));
    
    if (res.size() > 0) {
        throw "Record already exists";
    }
}

void InsertValuesInDB(std::map<std::string, std::string> &values, pqxx::connection &conn) {
    std::string query = "INSERT INTO " + db_tablename + " (str, num) VALUES ('" + values["str"] + "', " + values["num"] + ");";
    pqxx::work worker(conn);
    
    worker.exec(query);
    worker.commit();
}

int AddCGI(Request &req, std::map<std::string, std::string> &keys_values) {
    fprintf(stderr, "Keys: values\n");
    for (const auto &[key, val] : keys_values) {
        fprintf(stderr, "%s: %s\n", key.c_str(), val.c_str());
    }
    
    try {
        const std::string conn_params = "dbname = " + db_dbname + " user = " + db_user + " password = " + db_password + 
                                        " host = " + db_hostaddr + " port = " + db_port;
        pqxx::connection conn(conn_params);
        
        if (conn.is_open()) {
            fprintf(stderr, "[SUCCESS] Database \"test\" successfully opened!\n");
        } else {
            throw "Cant open test db";
        }
        
        // std::map<std::string, std::string> keys_values;
        // for (size_t i = 1; i < argc; i++) {
        //     std::string param(argv[i]);
        //     int idx = param.find('=');
        //     keys_values.emplace(param.substr(0, idx), param.substr(idx + 1));
        // }
        
        CheckValuesForExists(keys_values, conn);
        InsertValuesInDB(keys_values, conn);
        
        fprintf(stderr, "[SUCCESS] Record successfulyy added\n");
        
        keys_values.erase("path");
        // std::string set_cookie;
        // set_cookie.reserve(100);
        // for (const auto &[key, val] : keys_values) {
        //     set_cookie += "Set-Cookie: " + key + "=" + val + "\r\n";
        // }
        
        
        printf( "HTTP/1.1 200 OK\r\n"
                // "%s" // set cookie
                "Content-Type: text/html\r\n\r\n"
                "<script>window.location.href = \"http://localhost:42069/add-data\";</script>\r\n"
                );/*
                    ,set_cookie.c_str()); /**/
        // printf("window.location.href = 'http://www.google.com';\r\n");
    } catch (char const*ex) {
        fprintf(stderr, "[ERROR] %s\n", ex);
        printf( "HTTP/1.1 500 Internal Server Error\r\n"
                "Connection: close\r\n");
        return 1;
    }
    
    return 0;
}
