#include <iostream>
#include <fstream>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"

#include <fmt/format.h>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/ostreamwrapper.h>

#include <YandexMusic/Request.h>
#include <Databases/PostgreSQL.h>
#include <Common/httplib.h>
#include <Common/Logging.h>

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
    s += "METHOD: " + req.method + "\n";
    s += "VERSION: " + req.version + "\n";
    s += "PATH: " + req.path + "\n";

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

    if (!res.body.empty()) { s += res.body + "\n"; }

    s += "\n";

    return s;
}

int main()
{
    yandex_music::Request request {};
    if (!yandex_music::Request::processConfig())
        std::cout << "Error: bad configuration file!\n";
    multi_sink_example(yandex_music::User::log_folder + "/multisink.log");

    yandex_music::User user {request.getUser()};
    std::string user_id = user.getId();

    // Processing json playlists_map.json config-------------------------------
    int kind = 0;
    std::map<int, std::set<std::string>> playlists_map;

    std::ifstream ifs {"playlists_map.json"};
    if ( !ifs.is_open() )
    {
        std::cerr << "Could not open file for reading!\n";
    }
    else
    {   
        rapidjson::IStreamWrapper isw { ifs };
        rapidjson::Document document {};
        document.ParseStream( isw );
        assert(document.IsObject());
        for (auto & itr: document.GetObject())
        {
            assert(itr.value.IsObject());
            for (auto & itr1 : itr.value.GetObject())
            {
                if (std::string(itr1.name.GetString()) == "kind")
                {
                    kind = itr1.value.GetInt();
                    playlists_map[kind] = {};
                }

                if (std::string(itr1.name.GetString()) == "authors")
                {
                    assert(itr1.value.IsArray());
                    for (auto & itr2 : itr1.value.GetArray())
                        playlists_map[kind].insert(itr2.GetString());
                }
            }
        }
    }
    //-------------------------------------------------------------------------

    Server svr;

    httplib::Logger logger = [](const auto& req, const auto& res) {
        std::cout << log(req, res);
    };

    svr.set_logger(logger);

    // Todo: create method initializeTables()
    PostgreSQL::createTables();

    // Todo: create method fillTables()
    // PostgreSQL::fillTables();

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

    svr.Get("/public/reactPlayerFilePlayer.js", [&](const httplib::Request &req, Response &res) {
        res.set_file_content("../frontend/public/reactPlayerFilePlayer.js");
    });

    svr.Get("/mediafiles/:filename", [&](const httplib::Request &req, Response &res) {
        // auto filename = std::stoi(req.path_params.at("filename"));
        std::string filename = "2.mp3";
        ifstream file ("../frontend/mediafiles/2.mp3", ios::binary);
        if (file.is_open())
        {
            auto size = file.tellg();
            auto memblock = new char [size];
            file.seekg (0, ios::beg);
            file.read (memblock, size);
            file.close();

            res.set_content(memblock,  "audio/mpeg");

            delete[] memblock;
        }
        else cout << "Unable to open file\n";
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
    svr.Get("/cli/:command_id", [&](const httplib::Request& req, Response& res) {
        auto command_id = std::stoi(req.path_params.at("command_id"));

        std::vector<yandex_music::Track> add_tracks {};
        yandex_music::Playlist playlist {};
        int i=0;

        switch (command_id)
        {
            /// Print playlists
            case 1:
            {
                user.getUserPlaylists();
                user.printUserPlaylists();
                break;   
            }

            /// Get tracks without playlist
            case 2:
            {
                user.getTracksWithoutPlaylist();
                break;
            }

            /// Create, change and delete playlist
            case 3:
            {
                playlist = user.createPlaylist("Test3");
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

            /// Download playlist Like
            case 7:
            {
                user.getLikeTracks();
                playlist = yandex_music::Playlist("Like", 0, 0, "", 0, user_id);
                playlist.tracks = user.like_tracks;
                playlist.downloadPlaylist();
                break;
            }

            /// Add tracks to playlist
            case 8:
            {
                if (!playlists_map.empty()) 
                {
                    user.getTracksWithoutPlaylist();
                    for (const auto& kv_pair : playlists_map)
                    {
                        playlist = user.getPlaylist(kv_pair.first); 
                        for (auto track : user.tracks_out_playlist)
                            if (auto search = kv_pair.second.find(track.getArtists()[0]); search != kv_pair.second.end())
                                add_tracks.push_back(track);
                        playlist.addTracksToPlaylist(add_tracks);
                        add_tracks.clear();
                    }
                }
                break;
            }
            
            /// Remove all tracks from playlist
            case 9:
            {
                if (!playlists_map.empty()) 
                    for (const auto& kv_pair : playlists_map)
                    {
                        playlist = user.getPlaylist(kv_pair.first);
                        playlist.getPlaylistTracks();
                        if (playlist.tracks.size() > 0)
                            playlist.deleteTracksFromPlaylist(playlist.tracks);
                    }
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

    // svr.Get("/users/:id", [&](const Request& req, Response& res) {
    //     auto user_id = req.path_params.at("id");
    //     res.set_content(user_id, "text/plain");
    // });

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

    svr.Post("/music/logout", [&](const httplib::Request& req, httplib::Response& res) {
        res.set_content("{}", "text/json");
    });

   svr.Get("/music/category/:category", [&](const httplib::Request& req, httplib::Response& res) {
        std::string tracks;
        std::string category = req.path_params.at("category");
        std::cout << category << "\n";
        PostgreSQL::getCategoryTracks(category, tracks);
        res.set_content(tracks, "text/json");
   });

   svr.Get("/music/categories", [&](const httplib::Request& req, httplib::Response& res) {
        std::vector<std::string> categories;
        PostgreSQL::getCategories(categories);
        std::string content = "{\"categories\": [";
        for (auto & category : categories)
            content += "{\"name\": \"" + category + "\"},";
        content.pop_back();
        content += "]}";
        res.set_content(content, "text/json");
   });

   svr.Get("/mediafiles/:file", [&](const httplib::Request& req, httplib::Response& res) {
        res.set_content({}, "text/json");
   });

//    int port = svr.bind_to_any_port("0.0.0.0");
//    std::cout << port;
//    svr.listen_after_bind();

    svr.listen("0.0.0.0", 8080);

    return 0;
}
