#include <iostream>
#include <fmt/format.h>
#include <rapidjson/document.h>

#include <YandexMusic/Request.h>
#include <Databases/PostgreSQL.h>
#include <Common/httplib.h>

using namespace httplib;

std::string dump_headers(const Headers &headers)
{
    std::string s;

    for (const auto & x : headers)
        s += x.first + ": " + x.second + "\n";

    return s;
}

std::string log(const httplib::Request &req, const Response &res) {

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

    httplib::Logger logger = [](const auto& req, const auto& res) {
        std::cout << log(req, res);
    };

    svr.set_logger(logger);

    PostgreSQL::createTableUser();

    svr.Get("/hi", [](const httplib::Request & /*req*/, Response &res)
    {
        res.set_content("Hello World!", "text/plain");
    });

    svr.Get("/", [&](const httplib::Request &req, Response &res) {
        res.set_file_content("../frontend/index.html");
    });

    svr.Get("/public/main.js", [&](const httplib::Request &req, Response &res) {
        res.set_file_content("../frontend/public/main.js");
    });

    svr.Get("/src/index.js", [&](const httplib::Request &req, Response &res) {
        res.set_file_content("../frontend/src/index.js");
    });

    /// Match the request path against a regular expression
    /// and extract its captures
//    svr.Get(R"(/cli/(\d+))", [&](const Request& req, Response& res) {
//        auto numbers = req.matches[1];
//        res.set_content(numbers, "text/plain");
//    });

    /// Capture the second segment of the request path as "id" path param
    svr.Get("/cli/:command_id", [&](const httplib::Request& req, Response& res) 
    {
        auto command_id = std::stoi(req.path_params.at("command_id"));

        yandex_music::Request request {};
        yandex_music::Request::processConfig();
        yandex_music::User user {request.getUser()};
        string user_id = user.getId();

        switch (command_id)
        {
            /// Print playlists
            case 1:
                user.getUserPlaylists();
                user.printUserPlaylists();
                break;

                /// Get tracks without playlist
            case 2:
                user.getTracksWithoutPlaylist();
                break;

                /// Create, change and delete playlist
            case 3:
            {
                yandex_music::Playlist playlist {user.createPlaylist("Test3")};
                user.changePlaylistName(playlist.getKind(), "Test4");
                user.getLikeTracks();
                playlist.addTracksToPlaylist(user.like_tracks);
                user.deletePlaylist(playlist.getKind());
                break;
            }

                /// Print track
            case 4:
            {
                user.getUserPlaylists();
                auto playlist = user.playlists[4];
                playlist.getPlaylistTracks();
                auto track = playlist.tracks[0];
                track.print();
                break;
            }

                /// Download playlist
            case 5:
            {
                user.getUserPlaylists();
                auto playlist = user.playlists[7];
                playlist.downloadPlaylist();
                break;
            }

                /// Download all playlists
            case 6:
            {
                user.getUserPlaylists();
                for (auto playlist : user.playlists)
                    playlist.downloadPlaylist();
                break;
            }

            default:
                std::cout << "Nothing to do\n";
        }
        res.set_content(fmt::format("Done {}!", command_id), "text/plain");
    });

    svr.Get("/stop", [&](const Request& req, Response& res) {
        svr.stop();
    });

    svr.Post("/music/register", [&](const httplib::Request& req, Response& res) {
        rapidjson::Document document;
        document.Parse(req.body.c_str());

        // get document (json) object keys
        // for (auto& it : document.GetObject())
        //    std::cout << it.name.GetString() << "\n";

        std::string username = document["username"].GetString();
        std::string password = document["password"].GetString();
        std::string confirmation = document["confirmation"].GetString();
        std::string email = document["email"].GetString();

        int insertion = PostgreSQL::insertNewUser(username, email, password);
        if (insertion == 0)
            res.set_content(fmt::format("{\"username\": \"{}\", \"auth\": true, \"message\": \"\"}", username), "text/json");
        else
            res.set_content("{\"username\": \"\", \"auth\": false, \"message\": \"Username already taken.\"}", "text/json");
    });

    svr.Post("/music/login", [&](const httplib::Request& req, Response& res) {
        
        rapidjson::Document document;
        document.Parse(req.body.c_str());
        std::string username = document["username"].GetString();
        std::string password = document["password"].GetString();

        if (bool check = PostgreSQL::searchUser(username, password); check)
            res.set_content(
                fmt::format("{\"username\": \"{}\", \"auth\": true, \"message\": \"\"}", username), "text/json");
        else
            res.set_content(
                "{\"username\": \"\", \"auth\": false, \"message\": \"Invalid username and/or password.\"}", "text/json");
    });

    svr.Post("/music/logout", [&](const httplib::Request& req, Response& res) {
        res.set_content("{}", "text/json");
    });

//    svr.Get("/music/categories", [&](const Request &req, Response &res) {
//        return response(x);
//    });

//    int port = svr.bind_to_any_port("0.0.0.0");
//    std::cout << port;
//    svr.listen_after_bind();

    svr.listen("0.0.0.0", 8080);

    return 0;
}
