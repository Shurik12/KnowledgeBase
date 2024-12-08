#pragma once

#include <spdlog/spdlog.h>
#include <string>
#include <iostream>
#include <vector>
#include <unordered_set>
#include <fmt/format.h>
#include <YandexMusic/Track.h>
#include <filesystem>

namespace yandex_music 
{
    class Playlist 
    {
    public:

        Playlist() = default;

        Playlist(
            std::string title_,
            int revision_,
            int trackCount_,
            std::string playlistUuid_,
            int kind_,
            std::string userId_);

        ~Playlist() = default;

        void print();

        [[nodiscard]] std::string getTitle() const;

        void setTitle(const std::string &title_);

        [[nodiscard]] int getRevision() const;

        void setRevision(const int &revision_);

        [[nodiscard]] int getTrackCount() const;

        void setTrackCount(const int &trackCount_);

        [[nodiscard]] std::string getPlaylistUuid() const;

        void setPlaylistUuid(const std::string &playlistUuid_);

        [[nodiscard]] int getKind() const;

        void setKind(const int &kind_);

        void setId(const std::string &id_);

//    void deleteUserPlaylist();
        void downloadPlaylist();

        void getPlaylistTracks();

        void addTracksToPlaylist(const std::vector<Track> &tracks);

        static void setOutput(const std::string &output_);

        std::vector<Track> tracks;
        std::unordered_set<std::string> artists;

    private:

        std::string title;
        int revision;
        int trackCount{};
        std::string playlistUuid;
        int kind{};
        std::string userId;

        static std::string output_folder;
        std::shared_ptr<spdlog::logger> logger = spdlog::get("multi_sink");
    };

}