// Playlist.h
#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include <filesystem>
#include <optional>

#include <spdlog/spdlog.h>
#include <fmt/format.h>
#include <YandexMusic/Track.h>

namespace YandexMusic
{
    class Playlist
    {
    public:
        Playlist() = default;

        Playlist(std::string title, int revision, int trackCount,
                 std::string playlistUuid, int kind, std::string userId);

        // Getters
        [[nodiscard]] const std::string &title() const noexcept { return title_; }
        [[nodiscard]] int revision() const noexcept { return revision_; }
        [[nodiscard]] int trackCount() const noexcept { return trackCount_; }
        [[nodiscard]] const std::string &playlistUuid() const noexcept { return playlistUuid_; }
        [[nodiscard]] int kind() const noexcept { return kind_; }
        [[nodiscard]] const std::string &userId() const noexcept { return userId_; }

        [[nodiscard]] const std::vector<Track> &tracks() const noexcept { return tracks_; }
        [[nodiscard]] const std::unordered_set<std::string> &artists() const noexcept { return artists_; }

        // Setters
        void setTitle(std::string title) { title_ = std::move(title); }
        void setRevision(int revision) noexcept { revision_ = revision; }
        void setTrackCount(int count) noexcept { trackCount_ = count; }
        void setPlaylistUuid(std::string uuid) { playlistUuid_ = std::move(uuid); }
        void setKind(int kind) noexcept { kind_ = kind; }
        void setUserId(std::string id) { userId_ = std::move(id); }
        void setTracks(const std::vector<Track>& tracks) {
            tracks_.clear();
            artists_.clear();
            
            tracks_ = tracks;
            
            for (const auto& track : tracks_) {
                const auto& trackArtists = track.artists();
                if (!trackArtists.empty()) {
                    artists_.insert(trackArtists[0]);
                }
            }
            
            trackCount_ = static_cast<int>(tracks_.size());
        }

        // Operations
        void print() const;
        void download();
        void fetchTracks();
        void addTracks(const std::vector<Track> &tracks);
        void removeTracks(const std::vector<Track> &tracks);

        // Static methods
        static void setOutputDirectory(std::string_view path);

    private:
        static inline std::filesystem::path outputDirectory_;

        std::string title_;
        int revision_ = 0;
        int trackCount_ = 0;
        std::string playlistUuid_;
        int kind_ = 0;
        std::string userId_;

        std::vector<Track> tracks_;
        std::unordered_set<std::string> artists_;

        std::shared_ptr<spdlog::logger> logger_ = spdlog::get("multi_sink");
    };
} // namespace YandexMusic