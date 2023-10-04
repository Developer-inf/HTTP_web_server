#include <iostream>
#include <map>
#include <pqxx/pqxx>

// #define TRY try {
// #define CATCH } catch (std::exception ex) { fprintf(stderr, "%s\n", ex.what()); return 1; }


void CheckValuesForExists(std::map<std::string, std::string> &values, pqxx::connection &conn) {
    const std::string tablename("test");
    std::string query = "SELECT * FROM " + tablename + " WHERE str='" + values["str"] + "' and num=" + values["num"] + ";";
    pqxx::work worker(conn);
    pqxx::result res(worker.exec(query));
    
    if (res.size() > 0) {
        throw "Record already exists";
    }
}

void InsertValuesInDB(std::map<std::string, std::string> &values, pqxx::connection &conn) {
    const std::string tablename("test");
    std::string query = "INSERT INTO " + tablename + " (str, num) VALUES ('" + values["str"] + "', " + values["num"] + ");";
    pqxx::work worker(conn);
    
    worker.exec(query);
    worker.commit();
}

int AddCGI(Request &req, std::map<std::string, std::string> &keys_values)
{
    fprintf(stderr, "Keys: values\n");
    for (const auto &[key, val] : keys_values) {
        fprintf(stderr, "%s: %s\n", key.c_str(), val.c_str());
    }
    
    try {
        pqxx::connection conn("dbname = postgres user = paul password = postgres hostaddr = 127.0.0.1 port = 5432");
        
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
        
        std::string set_cookie;
        set_cookie.reserve(100);
        for (const auto &[key, val] : keys_values) {
            set_cookie += "Set-Cookie: " + key + "=" + val + "\r\n";
        }
        
        
        printf( "HTTP/1.1 200 OK\r\n"
                "%s" // set cookie
                "Content-Type: text/html\r\n\r\n"
                "<script>window.location.href = \"http://localhost:42069/add-data\";</script>\r\n",
                    set_cookie.c_str());
        // printf("window.location.href = 'http://www.google.com';\r\n");
    } catch (char const*ex) {
        fprintf(stderr, "[ERROR] %s\n", ex);
        printf( "HTTP/1.1 500 Internal Server Error\r\n"
                "Connection: close\r\n");
        return 1;
    }
    
    return 0;
}
