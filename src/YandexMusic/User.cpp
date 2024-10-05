#include <YandexMusic/User.h>
#include <YandexMusic/Request.h>
#include <utility>

std::string User::log_folder;

User::User(string id_)
	: id(std::move(id_))
{}

string User::getId() const
{
	return id;
}

void User::setLog(const std::string & log_folder_)
{
    User::log_folder = log_folder_;
}

void User::printUserPlaylists()
{
    for (auto playlist : playlists)
        playlist.print();
}

Playlist User::getPlaylist(const int & kind)
{

    Document document;
    Request request;
    string url {"users/" + id + "/playlists/" + to_string(kind)};
    request.makeRequest(url, document);
    return playlistObjectFromResponse(document["result"]);
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
                result[i]["revision"].GetInt(),
                result[i]["trackCount"].GetInt(),
                result[i]["playlistUuid"].GetString(),
                result[i]["kind"].GetInt(),
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
    vector<int> processed_albums;

    for (i = 0; i < result.Size(); ++i) // Uses SizeType instead of size_t
    {
        /// Processing artists
        for (auto j = 0; j < result[i]["artists"].Size(); ++j)
            for (auto& artist : result[i]["artists"][j].GetObject())
                if (string(artist.name.GetString()) == "name")
                    processed_artists.emplace_back(artist.value.GetString());

        /// Processing albums
        for (auto j = 0; j < result[i]["albums"].Size(); ++j)
            for (auto& album : result[i]["albums"][j].GetObject())
                if (string(album.name.GetString()) == "id")
                    processed_albums.emplace_back(album.value.GetInt());

        Track processed_track(
                result[i]["id"].GetString(),
                result[i]["title"].GetString(),
                processed_artists,
                processed_albums,
                result[i]["available"].GetBool(),
                result[i]["lyricsAvailable"].GetBool());

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
    ofstream result;
    result.open (fmt::format("/home/alex/Git/KnowledgeBase/output/tracks_without_playlist_{}.txt", Poco::Timestamp().epochTime()));

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

//    consoleLogger.information(fmt::format("Count tracks in any playlist: {}", tracks_in_playlist.size()));
    result << fmt::format("Count tracks in any playlist: {}\n", tracks_in_playlist.size());

    getLikeTracks();

//    consoleLogger.information(fmt::format("All tracks: {}", like_tracks.size()));
    result << fmt::format("All tracks: {}\n", like_tracks.size());

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

//    consoleLogger.information(fmt::format("Count tracks out of playlist: {}", tracks_out_playlist.size()));
//    consoleLogger.information(fmt::format("Count tracks out of like: {}", tracks_out_like.size()));
//    consoleLogger.information(fmt::format("Tracks out of playlist:"));
    result << fmt::format("Count tracks out of playlist: {}\n", tracks_out_playlist.size());
    result << fmt::format("Count tracks out of like: {}\n", tracks_out_like.size());
    result << fmt::format("Tracks out of playlist:\n");

    for (const auto& track : tracks_out_playlist)
    {
        auto artists = track.getArtists();
        string name = artists.empty() ? track.getTitle() : artists[0] + " - " + track.getTitle();
//        consoleLogger.information(fmt::format("\t{}", name));
        result << fmt::format("\t{}\n", name);
    }
    result.close();
}


Playlist User::createPlaylist(const string & title)
{
    Document document;
    Request request;
    string url {"users/" + id + "/playlists/create"};
    map<string, string> body;
    body["visibility"] = "public";
    body["title"] = title;
    request.makePostRequest(url, body, document);

    /// Processing response
    return playlistObjectFromResponse(document["result"]);
}

void User::changePlaylistName(const int & kind, const string & new_title)
{
    Document document;
    Request request;
    string url {"users/" + id + "/playlists/" + to_string(kind) + "/name"};
    map<string, string> body;
    body["value"] = new_title;
    request.makePostRequest(url, body, document);

    /// Processing response
    const Value& result = document["result"];
    assert(result.IsObject());
    for (auto & key : result.GetObject())
    {
        if (string(key.name.GetString()) == "kind")
            cout << "Playlist kind: " << key.value.GetInt() << "\n";
        if (string(key.name.GetString()) == "title")
            cout << "Playlist title: " << key.value.GetString() << "\n";
    }
}

void User::deletePlaylist(const int & kind)
{
    Document document;
    Request request;
    string url {"users/" + id + "/playlists/" + to_string(kind) + "/delete"};
    map<string, string> body;
    request.makePostRequest(url, body, document);

    /// Processing response
    const Value& result = document["result"];
    assert(result.IsString());
    cout << "Delete playlist " + to_string(kind) << ": " << result.GetString() << "\n";
}

Playlist User::playlistObjectFromResponse(const Value & response)
{
    assert(response.IsObject());
    Playlist playlist;
    for (auto & key : response.GetObject())
    {
        if (string(key.name.GetString()) == "title")
            playlist.setTitle(key.value.GetString());
        if (string(key.name.GetString()) == "revision")
            playlist.setRevision(key.value.GetInt());
        if (string(key.name.GetString()) == "trackCount")
            playlist.setTrackCount(key.value.GetInt());
        if (string(key.name.GetString()) == "playlistUuid")
            playlist.setPlaylistUuid(key.value.GetString());
        if (string(key.name.GetString()) == "kind")
            playlist.setKind(key.value.GetInt());
        playlist.setId(id);
    }
    return playlist;
}

void User::downloadPlaylists(vector<Playlist> & playlists)
{
    for (Playlist playlist : playlists)
        playlist.downloadPlaylist();
}
