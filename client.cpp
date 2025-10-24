#include <fmt/format.h>
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>

#include <Common/httplib.h>

void printParseError(const rapidjson::Document &doc)
{
    if (doc.HasParseError())
    {
        std::cout << "JSON parse error at offset " << doc.GetErrorOffset()
                  << ": " << rapidjson::GetParseError_En(doc.GetParseError()) << std::endl;
    }
}

void processPlaylistsResponse(const std::string &json_response)
{
    rapidjson::Document document;
    document.Parse(json_response.c_str());

    if (document.HasParseError())
    {
        printParseError(document);
        return;
    }

    if (document.HasMember("playlists") && document["playlists"].IsArray())
    {
        const auto &playlists = document["playlists"];
        std::cout << "\n=== User Playlists (" << playlists.Size() << " found) ===" << std::endl;
        for (rapidjson::SizeType i = 0; i < playlists.Size(); i++)
        {
            const auto &playlist = playlists[i];
            std::cout << "Title: " << playlist["title"].GetString()
                      << " | Kind: " << playlist["kind"].GetInt()
                      << " | Tracks: " << playlist["trackCount"].GetInt()
                      << " | Revision: " << playlist["revision"].GetInt()
                      << std::endl;
        }
        std::cout << "=================================" << std::endl;
    }
    else if (document.HasMember("error"))
    {
        std::cout << "Error: " << document["error"].GetString() << std::endl;
    }
}

void processTracksResponse(const std::string &json_response)
{
    rapidjson::Document document;
    document.Parse(json_response.c_str());

    if (document.HasParseError())
    {
        printParseError(document);
        return;
    }

    if (document.HasMember("tracks_without_playlists") && document["tracks_without_playlists"].IsArray())
    {
        const auto &tracks = document["tracks_without_playlists"];
        std::cout << "\n=== Tracks Without Playlists (" << tracks.Size() << " found) ===" << std::endl;
        for (rapidjson::SizeType i = 0; i < tracks.Size(); i++)
        {
            const auto &track = tracks[i];
            std::cout << "Title: " << track["title"].GetString();

            if (track.HasMember("artists") && track["artists"].IsArray())
            {
                const auto &artists = track["artists"];
                std::cout << " | Artists: ";
                for (rapidjson::SizeType j = 0; j < artists.Size(); j++)
                {
                    if (j > 0)
                        std::cout << ", ";
                    std::cout << artists[j].GetString();
                }
            }

            std::cout << " | ID: " << track["id"].GetString()
                      << " | Available: " << (track["available"].GetBool() ? "Yes" : "No")
                      << std::endl;
        }
        std::cout << "===========================================" << std::endl;
    }
    else if (document.HasMember("error"))
    {
        std::cout << "Error: " << document["error"].GetString() << std::endl;
    }
}

void processTrackResponse(const std::string &json_response)
{
    rapidjson::Document document;
    document.Parse(json_response.c_str());

    if (document.HasParseError())
    {
        printParseError(document);
        return;
    }

    if (document.HasMember("track"))
    {
        const auto &track = document["track"];
        std::cout << "\n=== Track Details ===" << std::endl;
        std::cout << "Title: " << track["title"].GetString() << std::endl;
        std::cout << "ID: " << track["id"].GetString() << std::endl;
        std::cout << "Available: " << (track["available"].GetBool() ? "Yes" : "No") << std::endl;

        if (track.HasMember("artists") && track["artists"].IsArray())
        {
            const auto &artists = track["artists"];
            std::cout << "Artists: ";
            for (rapidjson::SizeType i = 0; i < artists.Size(); i++)
            {
                if (i > 0)
                    std::cout << ", ";
                std::cout << artists[i].GetString();
            }
            std::cout << std::endl;
        }
        std::cout << "====================" << std::endl;
    }
    else if (document.HasMember("error"))
    {
        std::cout << "Error: " << document["error"].GetString() << std::endl;
    }
}

void processGenericResponse(const std::string &json_response)
{
    rapidjson::Document document;
    document.Parse(json_response.c_str());

    if (document.HasParseError())
    {
        printParseError(document);
        return;
    }

    if (document.HasMember("status") && document.HasMember("message"))
    {
        std::cout << "Status: " << document["status"].GetString() << std::endl;
        std::cout << "Message: " << document["message"].GetString() << std::endl;
    }
    else if (document.HasMember("error"))
    {
        std::cout << "Error: " << document["error"].GetString() << std::endl;
    }
    else
    {
        std::cout << "Unknown response format" << std::endl;
        std::cout << "Raw response: " << json_response << std::endl;
    }
}

int main(void)
{
    std::string s;

    /// Command line interface
    std::cout << "Menu:\n";
    std::cout << "\t1. Print playlists\n";
    std::cout << "\t2. Get tracks without playlist\n";
    std::cout << "\t3. Create, change and delete playlist\n";
    std::cout << "\t4. Print track\n";
    std::cout << "\t5. Download playlist\n";
    std::cout << "\t6. Download all playlists\n";
    std::cout << "\t7. Download Like tracks as playlist\n";
    std::cout << "\t8. Add tracks to playlist\n";
    std::cout << "\t9. Remove all tracks from playlist\n";

    httplib::Client cli("localhost", 8080);

    while (true)
    {
        std::cout << "\nInput the number of one of the options above (or 'q' to quit): ";
        std::cin >> s;

        if (s == "q" || s == "Q")
        {
            break;
        }

        if (auto res = cli.Get(fmt::format("/cli/{}", s)))
        {
            if (res->status == httplib::StatusCode::OK_200)
            {
                int command_id = std::stoi(s);

                // Process response based on command type
                switch (command_id)
                {
                case 1:
                    processPlaylistsResponse(res->body);
                    break;
                case 2:
                    processTracksResponse(res->body);
                    break;
                case 4:
                    processTrackResponse(res->body);
                    break;
                default:
                    processGenericResponse(res->body);
                    break;
                }
            }
            else
            {
                std::cout << "HTTP Error: " << res->status << std::endl;
                std::cout << "Response: " << res->body << std::endl;
            }
        }
        else
        {
            auto err = res.error();
            std::cout << "HTTP error: " << httplib::to_string(err) << std::endl;
            break;
        }
    }

    return 0;
}