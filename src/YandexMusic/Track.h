#pragma once

#include <string>
#include <vector>
#include <memory>
#include <optional>

#include <YandexMusic/DownloadInfo.h>
#include <YandexMusic/Supplement.h>
#include <Common/tinyxml2.h>

namespace YandexMusic
{
    class Track
    {
    public:
        Track() = default;

        Track(std::string id, std::string title,
              std::vector<std::string> artists,
              std::vector<int> albums,
              bool available = true,
              bool lyricsAvailable = false);

        // Getters
        [[nodiscard]] const std::string &id() const noexcept { return id_; }
        [[nodiscard]] const std::string &title() const noexcept { return title_; }
        [[nodiscard]] const std::vector<std::string> &artists() const noexcept { return artists_; }
        [[nodiscard]] const std::vector<int> &albums() const noexcept { return albums_; }
        [[nodiscard]] bool available() const noexcept { return available_; }
        [[nodiscard]] bool lyricsAvailable() const noexcept { return lyricsAvailable_; }

        [[nodiscard]] const std::optional<DownloadInfo> &downloadInfo() const noexcept { return downloadInfo_; }
        [[nodiscard]] const std::optional<Supplement> &supplement() const noexcept { return supplement_; }

        // Operations
        void fetchDownloadInfo();
        void fetchSupplement();
        void download(std::string_view lyricsDir, std::string_view tracksDir) const;

        static std::string generateDownloadUrl(const tinyxml2::XMLDocument &xmlResponse);

        void print() const;

    private:
        std::string id_;
        std::string title_;
        std::vector<std::string> artists_;
        std::vector<int> albums_;
        bool available_ = true;
        bool lyricsAvailable_ = false;

        std::optional<DownloadInfo> downloadInfo_;
        std::optional<Supplement> supplement_;
    };
} // namespace YandexMusic