#if !defined(_DB_CONFIG)
#define _DB_CONFIG

#include <string>

const std::string db_dbname =    "postgres";
const std::string db_user =      "postgres";
const std::string db_password =  "123";
// const std::string db_hostaddr =  "127.0.0.1";
const std::string db_hostaddr =  "db";
const std::string db_port =      "5432";
const std::string db_tablename = "record";
const std::string db_users_tablename = "person";
constexpr int PORT = 42069;
constexpr int BUFFSIZE = 4096;

#endif // _DB_CONFIG
