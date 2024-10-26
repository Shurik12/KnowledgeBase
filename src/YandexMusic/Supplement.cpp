#include <YandexMusic/Supplement.h>

namespace yandex_music {

    Supplement::Supplement(
            int id_,
            string lyrics_,
            string fullLyrics_,
            string textLanguage_,
            bool showTranslation_,
            bool hasRights_)
            : id(std::move(id_)), lyrics(std::move(lyrics_)), fullLyrics(std::move(fullLyrics_)),
              textLanguage(textLanguage_), showTranslation(std::move(showTranslation_)), hasRights(hasRights_) {}

    int Supplement::getId() const {
        return id;
    }

    string Supplement::getLyrics() const {
        return lyrics;
    }

    string Supplement::getFullLyrics() const {
        return fullLyrics;
    }

    void Supplement::setId(const int &value) {
        id = value;
    }

    void Supplement::setLyrics(const string &value) {
        lyrics = value;
    }

    void Supplement::setFullLyrics(const string &value) {
        fullLyrics = value;
    }

    void Supplement::setTextLanguage(const string &value) {
        textLanguage = value;
    }

    void Supplement::setShowTranslation(const bool &value) {
        showTranslation = value;
    }

    void Supplement::setHasRights(const bool &value) {
        hasRights = value;
    }

}