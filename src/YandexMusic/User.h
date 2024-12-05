#pragma once

#include <fstream>
#include <rapidjson/document.h>
#include <spdlog/spdlog.h>

#include <string>
#include <unordered_map>
#include <algorithm>
#include <set>
#include <YandexMusic/Playlist.h>
#include <Common/tinyxml2.h>

using namespace std;
using namespace rapidjson;

namespace yandex_music {

    class User {

    public:

        explicit User(string id_);

        ~User() = default;

        [[nodiscard]] string getId() const;

        void getUserPlaylists();

        static void getTracks(vector<Track> &tracks);

        static vector<Track> getTracks(vector<string> &track_ids);

        void getLikeTracks();

        static void downloadTracks(vector<Track> &tracks, string &lyrics_dir, string &tracks_dir);

        void getTracksWithoutPlaylist();

        void printUserPlaylists();

        Playlist playlistObjectFromResponse(const Value &response);

        Playlist createPlaylist(const string &title);

        void changePlaylistName(const int &kind, const string &new_title);

        void deletePlaylist(const int &kind);

        Playlist getPlaylist(const int &kind);

        void downloadPlaylists(vector<Playlist> &playlists);

        vector<Playlist> playlists;
        vector<Track> like_tracks;

        std::shared_ptr<spdlog::logger> logger = spdlog::get("multi_sink");

        static void setLog(const std::string &log_folder_);

        static std::string log_folder;

    private:

        std::string id;
        std::string client_id;
        std::string client_secret;
        std::string password;
    };

}