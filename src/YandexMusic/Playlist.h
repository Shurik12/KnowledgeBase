#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <Poco/Logger.h>
#include <fmt/format.h>
#include <YandexMusic/Track.h>
#include <filesystem>

namespace fs = std::filesystem;
using namespace std;

using Poco::Logger;

class Playlist
{

public:

	Playlist(
		string  title_,
		string  revision_,
		string  trackCount_,
		string  playlistUuid_,
		string  kind_,
        string  userId_);

	~Playlist()= default;
    void print();
	
	[[nodiscard]] string getTitle() const;
	[[nodiscard]] string getRevision() const;
	[[nodiscard]] string getTrackCount() const;
	[[nodiscard]] string getPlaylistUuid() const;
	[[nodiscard]] string getKind() const;
//    void deleteUserPlaylist();
    void downloadPlaylist();
    void downloadPlaylists(vector<Playlist> & playlists);
    vector<Track> getPlaylistTracks();

private:

	string title;
	string revision;
	string trackCount;
	string playlistUuid;
	string kind;
    string userId;
    string output {"/home/alex/Git/KnowledgeBase/cmake-build-debug/Music"};
    Logger& fileLogger = Logger::get("FileLogger");
    Logger& consoleLogger = Logger::get("ConsoleLogger");

};