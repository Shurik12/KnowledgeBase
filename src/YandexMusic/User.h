#pragma once

#include <string>
#include <YandexMusic/Playlist.h>

using namespace std;

class User
{

public:

	explicit User(string  id_);
    ~User()= default;
    [[nodiscard]] string getId() const;
    vector<Playlist> getUserPlaylists();
    static void getTracks(vector<Track> & tracks);
    static void downloadTracks(vector<Track> & tracks, string & lyrics_dir, string & tracks_dir);

private:

	string id;
	string client_id;
	string client_secret;
	string password;
	string auth_token;

};
