#pragma once

#include <string>
#include <vector>
#include <memory>

#include <YandexMusic/DownloadInfo.h>
#include <YandexMusic/Supplement.h>
#include <Common/tinyxml2.h>

using namespace std;
using namespace tinyxml2;

class Track
{
public:
	Track(
		string  id_,
		string  title_,
		vector<string> artists_,
        vector<int> albums_,
        bool available_,
        bool lyrics_available_);

	~Track()= default;

    /// Get private members of class
	[[nodiscard]] string getId() const;
	[[nodiscard]] string getTitle() const;
	[[nodiscard]] vector<string> getArtists() const;
    [[nodiscard]] vector<int> getAlbums() const;
    [[nodiscard]] bool getAvailable() const;
    [[nodiscard]] bool getLyricsAvailable() const;

    void getDownloadInfo();
    void getSupplement();

    void downloadTrack(string & lyrics_dir, string & tracks_dir);

    static void getSign(string & download_url, const XMLDocument & xml_response);

    shared_ptr<DownloadInfo> download_info;
    shared_ptr<Supplement> supplement;
    void print();

private:
	string id;
	string title;
	vector<string> artists;
    vector<int> albums;
    bool available = true;
    bool lyrics_available = false;
};
