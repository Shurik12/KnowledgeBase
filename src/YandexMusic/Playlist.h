#pragma once

#include <spdlog/spdlog.h>
#include <string>
#include <iostream>
#include <vector>
#include <unordered_set>
#include <fmt/format.h>
#include <YandexMusic/Track.h>
#include <filesystem>

namespace fs = std::filesystem;
using namespace std;

namespace yandex_music {

    class Playlist {

    public:

        Playlist() = default;

        Playlist(
                string title_,
                int revision_,
                int trackCount_,
                string playlistUuid_,
                int kind_,
                string userId_);

        ~Playlist() = default;

        void print();

        [[nodiscard]] string getTitle() const;

        void setTitle(const string &title_);

        [[nodiscard]] int getRevision() const;

        void setRevision(const int &revision_);

        [[nodiscard]] int getTrackCount() const;

        void setTrackCount(const int &trackCount_);

        [[nodiscard]] string getPlaylistUuid() const;

        void setPlaylistUuid(const string &playlistUuid_);

        [[nodiscard]] int getKind() const;

        void setKind(const int &kind_);

        void setId(const string &id_);

//    void deleteUserPlaylist();
        void downloadPlaylist();

        void getPlaylistTracks();

        void addTracksToPlaylist(const vector<Track> &tracks);

        static void setOutput(const string &output_);

        vector<Track> tracks;
        unordered_set<string> artists;

    private:

        string title;
        int revision;
        int trackCount{};
        string playlistUuid;
        int kind{};
        string userId;

        static string output_folder;
        std::shared_ptr<spdlog::logger> logger = spdlog::get("multi_sink");
    };

}