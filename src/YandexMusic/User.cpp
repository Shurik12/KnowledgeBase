#include <YandexMusic/User.h>
#include <YandexMusic/Request.h>
#include <rapidjson/document.h>

#include <utility>

User::User(string  id_)
	: id(std::move(id_))
{}

string User::getId() const
{
	return id;
}

vector<Playlist> User::getUserPlaylists()
{
    Document document;
    Request request;
    string url {"users/"+id+"/playlists/list"};
    request.makeRequest(url, document);

    vector<Playlist> playlists {};
    const Value& result = document["result"];
    assert(result.IsArray());
    for (SizeType i = 0; i < result.Size(); i++) // Uses SizeType instead of size_t
    {
        Playlist playlist(
                result[i]["title"].GetString(),
                to_string(result[i]["revision"].GetInt()),
                to_string(result[i]["trackCount"].GetInt()),
                result[i]["playlistUuid"].GetString(),
                to_string(result[i]["kind"].GetInt()),
                id);
        playlists.push_back(playlist);
    }

    return playlists;
}

void User::getTracks(vector<Track> & tracks)
{
    size_t i;
    map<string, string> body;
    string value {"["};
    for (i = 0; i < tracks.size()-1; ++i)
    {
        value += tracks[i].getId() + ", ";
    }
    value += tracks[i].getId() + "]";
    body["track-ids"] = value;

    Request request;
    Document document;
    string url_prefix = "track";
    request.makePostRequest(url_prefix, body, document);
}

void User::downloadTracks(vector<Track> & tracks, string & lyrics_dir, string & tracks_dir)
{
    for (Track track : tracks)
        if (track.getAvailable())
            track.downloadTrack(lyrics_dir, tracks_dir);
}