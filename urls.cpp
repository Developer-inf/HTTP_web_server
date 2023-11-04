#include <string>
#include "views.cpp"
#include "request.h"

void UrlToPage(Request *r) {
    if      (r->path == "/")                { render(r->socket_fd, "html/main.html",        "text/html"); }
    else if (r->path == "/css.css")         { render(r->socket_fd, "css/css.css",           "text/css"); }
    else if (r->path == "/get_data.js")     { render(r->socket_fd, "js/get_data.js",        "text/javascript"); }
    else if (r->path == "/test")            { render(r->socket_fd, "html/another.html",     "text/html"); }
    else if (r->path == "/add-data")        { render(r->socket_fd, "html/add_data.html",    "text/html"); }
    else if (r->path == "/check-files")     { render(r->socket_fd, "html/check_files.html", "text/html"); }
    else if (r->path == "/get_files.js")    { render(r->socket_fd, "js/get_files.js",       "text/javascript"); }
    else if (r->path == "/load-file-page")  { render(r->socket_fd, "html/load_file.html",   "text/html"); }
    else if (r->path == "/send-file-script"){ render(r->socket_fd, "js/send_file.js",       "text/javascript"); }
    else if (r->path == "/sign-in.js")      { render(r->socket_fd, "js/sign_in.js",         "text/javascript"); }
    else if (r->path == "/sign-up.js")      { render(r->socket_fd, "js/sign_up.js",         "text/javascript"); }
    else if (r->path == "/sign-up-page")    { render(r->socket_fd, "html/register.html",    "text/html"); }
    else if (r->path == "/delete-cookie.js"){ render(r->socket_fd, "js/delete_cookie.js",   "text/javascript"); }
    else if (r->path == "/get-files")       { get_files(r); }
    else if (r->path == "/load-file")       { load_file(r); }
    else if (r->path == "/check-data")      { check_data_page(r); }
    else if (r->path == "/sign-in")         { sign_in(r); }
    else if (r->path == "/sign-up")         { sign_up(r); }
    else if (r->path.starts_with("/exec?")) { exec_command(r); }
    else if (r->path.ends_with(".cgi"))     { proceed_cgi_page(r); }
    else    {
        fprintf(stderr, "Unknown path: \"%s\"\n", r->path.c_str());
        render(r->socket_fd, "", "");
    }
}