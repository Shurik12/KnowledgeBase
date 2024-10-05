#pragma once

#include <string>
#include <vector>
#include <iostream>
 
using namespace std;

class Supplement
{

public:

	Supplement()= default;

    Supplement(
            int id_,
            string  lyrics_,
            string fullLyrics_,
            string textLanguage_,
            bool showTranslation_,
            bool hasRights_);

	~Supplement()= default;

    /// Get private members of class
    [[nodiscard]] int getId() const;
    [[nodiscard]] string getLyrics() const;
    [[nodiscard]] string getFullLyrics() const;

    /// Set private members of class
    void setId(const int& value);
    void setLyrics(const string& value);
    void setFullLyrics(const string& value);
    void setTextLanguage(const string& value);
    void setShowTranslation(const bool& value);
    void setHasRights(const bool& value);

private:

    int id{};
    string lyrics;
    string fullLyrics;
    string textLanguage;
    bool showTranslation{};
    bool hasRights{};

};