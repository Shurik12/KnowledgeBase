#pragma once

#include <string>
#include <vector>
#include <set>
#include <filesystem>
#include <optional>

#include <spdlog/spdlog.h>
#include <rapidjson/document.h>
#include <YandexMusic/Playlist.h>

namespace YandexMusic
{
    class User
    {
    public:
        User() = default;
        explicit User(std::string id);

        // Getters
        [[nodiscard]] const std::string &id() const noexcept { return id_; }
        std::vector<Playlist> &playlists() noexcept { return playlists_; }
        [[nodiscard]] const std::vector<Track> &likedTracks() const noexcept { return likedTracks_; }
        [[nodiscard]] const std::vector<Track> &tracksWithoutPlaylists() const noexcept { return tracksWithoutPlaylists_; }

        // Playlist operations
        void fetchPlaylists();
        Playlist fetchPlaylist(int kind) const;
        Playlist createPlaylist(std::string_view title) const;
        void renamePlaylist(int kind, std::string_view newTitle) const;
        void deletePlaylist(int kind) const;

        // Track operations
        static std::vector<Track> fetchTracks(const std::vector<std::string> &trackIds);
        void fetchLikedTracks();
        void analyzeTracksWithoutPlaylists();

        // Download operations
        static void downloadTracks(const std::vector<Track> &tracks,
                                   std::string_view lyricsDir,
                                   std::string_view tracksDir);

        // Utility
        void printPlaylists() const;

        // Static configuration
        static void setLogDirectory(std::string_view path);

    private:
        static inline std::filesystem::path logDirectory_;

        std::string id_;
        std::vector<Playlist> playlists_;
        std::vector<Track> likedTracks_;
        std::vector<Track> tracksWithoutPlaylists_;

        std::shared_ptr<spdlog::logger> logger_ = spdlog::get("multi_sink");

        Playlist parsePlaylistResponse(const rapidjson::Value &response) const;
    };
} // namespace YandexMusic