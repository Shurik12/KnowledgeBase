#include <Common/httplib.h>
using namespace httplib;

using namespace std;

std::string dump_headers(const Headers &headers)
{
    std::string s;

    for (const auto & x : headers)
        s += x.first + ": " + x.second + "\n";

    return s;
}

std::string log(const Request &req, const Response &res) {

    std::string s;
    s += "Request: ===================================\n";
    s += req.method + " " + req.version + " " + req.path;

    std::string query;
    for (auto it = req.params.begin(); it != req.params.end(); ++it)
    {
        const auto &x = *it;

        query += (it == req.params.begin() ? '?' : '&') + x.first + "=" + x.second;
    }
    s += query;
    s += dump_headers(req.headers);
    s += "Response: ----------------------------------\n";
    s += to_string(res.status) + res.version + "\n";
    s += dump_headers(res.headers);
    s += "\n";

    if (!res.body.empty()) { s += res.body; }

    s += "\n";

    return s;
}

int main()
{
    Server svr;

    Logger logger = [](const auto& req, const auto& res) {
        std::cout << log(req, res);
    };

    svr.set_logger(logger);

    svr.Get("/hi", [](const Request & /*req*/, Response &res)
    {
        res.set_content("Hello World!", "text/plain");
    });

    svr.Get("/", [&](const Request &req, Response &res) {
        res.set_file_content("../frontend/index.html");
    });

    svr.Get("/public/main.js", [&](const Request &req, Response &res) {
        res.set_file_content("../frontend/public/main.js");
    });

    svr.Get("/src/index.js", [&](const Request &req, Response &res) {
        res.set_file_content("../frontend/src/index.js");
    });


    svr.listen("0.0.0.0", 8081);

    return 0;
}
