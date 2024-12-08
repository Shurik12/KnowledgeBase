#pragma once

#include <fstream>
#include <spdlog/spdlog.h>

#include <string>
#include <algorithm>
#include <set>
#include <YandexMusic/Playlist.h>
#include <Common/tinyxml2.h>
#include <rapidjson/document.h>

namespace yandex_music 
{
    class User 
    {
    public:

        explicit User(std::string id_);

        ~User() = default;

        [[nodiscard]] std::string getId() const;

        void getUserPlaylists();

        static void getTracks(std::vector<Track> &tracks);

        static std::vector<Track> getTracks(std::vector<std::string> &track_ids);

        void getLikeTracks();

        static void downloadTracks(std::vector<Track> &tracks, std::string &lyrics_dir, std::string &tracks_dir);

        void getTracksWithoutPlaylist();

        void printUserPlaylists();

        Playlist playlistObjectFromResponse(const rapidjson::Value &response);

        Playlist createPlaylist(const std::string &title);

        void changePlaylistName(const int &kind, const std::string &new_title);

        void deletePlaylist(const int &kind);

        Playlist getPlaylist(const int &kind);

        void downloadPlaylists(std::vector<Playlist> &playlists);

        std::vector<Playlist> playlists;
        std::vector<Track> like_tracks;

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