#pragma once

#include <string>
#include <optional>

namespace YandexMusic
{

    class Supplement
    {
    public:
        Supplement() = default;

        Supplement(int id, std::string lyrics, std::string fullLyrics,
                   std::string textLanguage, bool showTranslation, bool hasRights)
            : id_(id), lyrics_(std::move(lyrics)), fullLyrics_(std::move(fullLyrics)), textLanguage_(std::move(textLanguage)), showTranslation_(showTranslation), hasRights_(hasRights)
        {
        }

        // Getters
        [[nodiscard]] int id() const noexcept { return id_; }
        [[nodiscard]] const std::string &lyrics() const noexcept { return lyrics_; }
        [[nodiscard]] const std::string &fullLyrics() const noexcept { return fullLyrics_; }
        [[nodiscard]] const std::string &textLanguage() const noexcept { return textLanguage_; }
        [[nodiscard]] bool showTranslation() const noexcept { return showTranslation_; }
        [[nodiscard]] bool hasRights() const noexcept { return hasRights_; }

        // Setters
        void setId(int id) noexcept { id_ = id; }
        void setLyrics(std::string lyrics) { lyrics_ = std::move(lyrics); }
        void setFullLyrics(std::string fullLyrics) { fullLyrics_ = std::move(fullLyrics); }
        void setTextLanguage(std::string textLanguage) { textLanguage_ = std::move(textLanguage); }
        void setShowTranslation(bool show) noexcept { showTranslation_ = show; }
        void setHasRights(bool has) noexcept { hasRights_ = has; }

    private:
        int id_ = 0;
        std::string lyrics_;
        std::string fullLyrics_;
        std::string textLanguage_;
        bool showTranslation_ = false;
        bool hasRights_ = false;
    };

} // namespace YandexMusic