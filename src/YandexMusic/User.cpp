#include <YandexMusic/User.h>
#include <YandexMusic/Request.h>
#include <rapidjson/document.h>

#include <utility>

User::User(string id_)
	: id(std::move(id_))
{}

string User::getId() const
{
	return id;
}

void User::getUserPlaylists()
{
    Document document;
    Request request;
    string url {"users/"+id+"/playlists/list"};
    request.makeRequest(url, document);

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
}

vector<Track> User::getTracks(vector<string> & track_ids)
{
    size_t i;
    map<string, string> body;
    string value;
    for (i = 0; i < track_ids.size()-1; ++i)
    {
        value += track_ids[i] + ",";
    }
    value += track_ids[i];
    body["track-ids"] = value;
//    body["with-positions"] = "false";

    Request request;
    Document document;
    string url_prefix = "tracks";
    request.makePostRequest(url_prefix, body, document);

    const Value& result = document["result"];
    assert(result.IsArray());

    vector<Track> tracks;
    vector<string> processed_artists;

    for (i = 0; i < result.Size(); ++i) // Uses SizeType instead of size_t
    {
        for (auto j = 0; j < result[i]["artists"].Size(); ++j)
            for (auto& artist : result[i]["artists"][j].GetObject())
                if (string(artist.name.GetString()) == "name")
                    processed_artists.emplace_back(artist.value.GetString());

        Track processed_track(
                result[i]["id"].GetString(),
                result[i]["title"].GetString(),
                processed_artists,
                result[i]["available"].GetBool());

        processed_artists.clear();

        tracks.emplace_back(processed_track);
//        for (auto& it : result[i].GetObject())
//            cout << string(it.name.GetString()) << "\n";
    }
    return tracks;
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

void User::getLikeTracks()
{
    Document document;
    Request request;
    string url {"users/" + id + "/likes/tracks"};
    request.makeRequest(url, document);
    SizeType i, j;

    const Value& tracks = document["result"]["library"]["tracks"];

    //    for (auto& it : rTracks.GetObject())
    //        cout << string(it.name.GetString()) << "\n";

    assert(tracks.IsArray());

    vector<string> track_ids;
    for (i = 0; i < tracks.Size(); ++i) // Uses SizeType instead of size_t
    {
        vector<string> processed_artists {};
        track_ids.emplace_back(tracks[i]["id"].GetString());
    }
    like_tracks = getTracks(track_ids);
}

void User::downloadTracks(vector<Track> & tracks, string & lyrics_dir, string & tracks_dir)
{
    for (Track track : tracks)
        if (track.getAvailable())
            track.downloadTrack(lyrics_dir, tracks_dir);
}

void User::getTracksWithoutPlaylist()
{
    if (playlists.empty())
        getUserPlaylists();

    vector<Track> tracks_in_playlist;
    vector<Track> tracks_out_playlist;
    vector<Track> tracks_out_like;

    for (auto playlist : playlists)
    {
        playlist.getPlaylistTracks();
        for (const auto& track : playlist.tracks)
            tracks_in_playlist.emplace_back(track);
    }
    consoleLogger.information(fmt::format("Count tracks in any playlist: {}", tracks_in_playlist.size()));

    getLikeTracks();
    consoleLogger.information(fmt::format("All tracks: {}", like_tracks.size()));

    sort(like_tracks.begin(), like_tracks.end(),[](auto& a, auto& b) { return a.getId() == b.getId(); });
    sort(tracks_in_playlist.begin(), tracks_in_playlist.end(),[](auto& a, auto& b) { return a.getId() == b.getId(); });

    set_difference(like_tracks.begin(), like_tracks.end(),
                   tracks_in_playlist.begin(), tracks_in_playlist.end(),
                   inserter(tracks_out_playlist, tracks_out_playlist.begin()),
                   [](auto& a, auto& b) { return a.getId() < b.getId(); });

    set_difference(tracks_in_playlist.begin(), tracks_in_playlist.end(),
                   like_tracks.begin(), like_tracks.end(),
                   inserter(tracks_out_like, tracks_out_like.begin()),
                   [](auto& a, auto& b) { return a.getId() < b.getId(); });

    consoleLogger.information(fmt::format("Count tracks out of playlist: {}", tracks_out_playlist.size()));
    consoleLogger.information(fmt::format("Count tracks out of like: {}", tracks_out_like.size()));

    consoleLogger.information(fmt::format("Tracks out of playlist:"));
    for (const auto& track : tracks_out_playlist)
    {
        auto artists = track.getArtists();
        string name = artists.empty() ? track.getTitle() : artists[0] + " - " + track.getTitle();
        consoleLogger.information(fmt::format("\t{}", name));
    }
}

