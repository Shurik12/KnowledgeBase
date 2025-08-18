#include <Common/Curl.h>
#include <Common/tinyxml2.h>

#include <YandexMusic/Playlist.h>
#include <YandexMusic/Track.h>
#include <YandexMusic/DownloadInfo.h>
#include <YandexMusic/User.h>

#include <fmt/format.h>
#include <rapidjson/document.h>

#include <vector>

namespace YandexMusic 
{
    class Request : public Curl 
    {
    public:

        Request() = default;

        ~Request() = default;

        std::shared_ptr<spdlog::logger> logger = spdlog::get("multi_sink");

        User getUser();

        void makeRequest(const std::string &params, rapidjson::Document &document1);

        void makeRequest(const std::string &url, tinyxml2::XMLDocument &xml_response);

        void makePostRequest(
            const std::string &url_postfix, 
            std::map<std::string, std::string> &body, 
            rapidjson::Document &document);

        // std::vector<Track> getDislikedTracksIds(string user_id);
        // Playlist getUserPlaylist(string user_id, string kind);
        // Playlist createUserPlaylist(...);
        // Playlist changeUserPlaylistName(string user, string kind, string name);
        // Playlist changeUserPlaylist(string user, string kind);

    private:
        const std::string url_prefix = "https://api.music.yandex.net:443/";
    };
}