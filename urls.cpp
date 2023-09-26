#include <string>
#include "pages.cpp"
#include "request.h"

void UrlToPage(Request *r) {
    if      (r->path == "/")            { main_page(r); }
    else if (r->path == "/css.css")     { css_page(r); }
    else if (r->path == "/test")        { test_page(r); }
    else if (r->path == "/add-data")    { add_data_page(r); }
    else if (r->path.ends_with(".cgi")) { proceed_cgi_page(r); }
    else    {
        fprintf(stderr, "Unknown path: \"%s\"\n", r->path.c_str());
        render(r->socket_fd, "", "");
    }
}