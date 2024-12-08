#pragma once

#include <string>
#include <vector>
#include <memory>

#include <YandexMusic/DownloadInfo.h>
#include <YandexMusic/Supplement.h>
#include <Common/tinyxml2.h>

namespace yandex_music 
{
    class Track 
    {
    public:
        Track(
                std::string id_,
                std::string title_,
                std::vector<std::string> artists_,
                std::vector<int> albums_,
                bool available_,
                bool lyrics_available_);

        ~Track() = default;

        /// Get private members of class
        [[nodiscard]] std::string getId() const;

        [[nodiscard]] std::string getTitle() const;

        [[nodiscard]] std::vector<std::string> getArtists() const;

        [[nodiscard]] std::vector<int> getAlbums() const;

        [[nodiscard]] bool getAvailable() const;

        [[nodiscard]] bool getLyricsAvailable() const;

        void getDownloadInfo();

        void getSupplement();

        void downloadTrack(std::string &lyrics_dir, std::string &tracks_dir);

        static void getSign(std::string &download_url, const tinyxml2::XMLDocument &xml_response);

        std::shared_ptr<DownloadInfo> download_info;
        std::shared_ptr<Supplement> supplement;

        void print();

    private:
        std::string id;
        std::string title;
        std::vector<std::string> artists;
        std::vector<int> albums;
        bool available = true;
        bool lyrics_available = false;
    };
}