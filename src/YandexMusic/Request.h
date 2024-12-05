#include <Common/Curl.h>
#include <YandexMusic/Playlist.h>
#include <YandexMusic/Track.h>
#include <YandexMusic/DownloadInfo.h>
#include <YandexMusic/User.h>
#include <rapidjson/document.h>
#include <fmt/format.h>
#include <Common/tinyxml2.h>

#include <vector>

using namespace std;
using namespace rapidjson;
using namespace tinyxml2;

namespace yandex_music {

    class Request : public Curl {

    public:

        Request() = default;

        ~Request() = default;

        std::shared_ptr<spdlog::logger> logger = spdlog::get("multi_sink");

        User getUser();

        static void processConfig();

        void makeRequest(string &params, Document &document1);

        void makeRequest(const string &url, XMLDocument &xml_response);

        void makePostRequest(string &url_postfix, map<string, string> &body, Document &document);

        // vector<Track> getDislikedTracksIds(string user_id);
        // Playlist getUserPlaylist(string user_id, string kind);
        // Playlist createUserPlaylist(...);
        // Playlist changeUserPlaylistName(string user, string kind, string name);
        // Playlist changeUserPlaylist(string user, string kind);

    private:

        const string url_prefix = "https://api.music.yandex.net:443/";
    };

}