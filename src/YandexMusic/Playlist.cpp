#include <YandexMusic/Playlist.h>
#include <YandexMusic/Request.h>
#include <rapidjson/document.h>
#include <set>
#include <utility>

Playlist::Playlist(
	string  title_,
	string  revision_,
	string  trackCount_,
	string  playlistUuid_,
	string  kind_,
    string  userId_)
	: title(std::move(title_))
	, revision(std::move(revision_))
	, trackCount(std::move(trackCount_))
	, playlistUuid(std::move(playlistUuid_))
	, kind(std::move(kind_))
    , userId(std::move(userId_))
{}

string Playlist::getTitle() const
{
	return title;
}

string Playlist::getRevision() const
{
	return revision;
}


string Playlist::getTrackCount() const
{
	return trackCount;
}


string Playlist::getPlaylistUuid() const
{
	return playlistUuid;
}


string Playlist::getKind() const
{
	return kind;
}

void Playlist::print()
{
    cout << this->title << ": " << this->kind << "\n";
}

void Playlist::downloadPlaylist()
{
    consoleLogger.information(fmt::format("Playlist to download: {}", title));
    string playlist_directory = output + "/" + title;
    getPlaylistTracks();

    string tracks_directory = playlist_directory + "/tracks";
    string lyrics_directory = playlist_directory + "/lyrics";

    /// Get list directory
    set<string> list_directories;
    for (const auto & entry : fs::directory_iterator(output))
    {
        consoleLogger.information(fmt::format("Directory: {}", entry.path().generic_string()));
        list_directories.emplace(entry.path().generic_string());
    }

    /// Create playlist directory if not exists
    if (auto search = list_directories.find(title); search == list_directories.end())
    {
        fs::create_directory(playlist_directory);
        fs::create_directory(tracks_directory);
        fs::create_directory(lyrics_directory);
    }

    /// Get already loaded tracks (already on yandex disk and file system)
    if (tracks.empty())
    {
        consoleLogger.warning(fmt::format("No tracks in playlist: {}", title));
        return;
    }

    consoleLogger.information(fmt::format("Count tracks in playlist {}: {}", title, tracks.size()));

    /// Get list directory
    set<string> loaded_tracks;
    string file_name;
    for (const auto & entry : fs::directory_iterator(tracks_directory))
    {
        file_name = entry.path().generic_string();
        file_name = file_name.substr(file_name.find_last_of('/')+1);
        consoleLogger.information(fmt::format("{}", file_name));
        loaded_tracks.emplace(file_name);
    }
    consoleLogger.information(fmt::format("Already loaded tracks in playlist {}: {}", title, loaded_tracks.size()));

    /// Download new tracks
    vector<Track> tracks_for_download;
    for (const Track& track : tracks)
    {
        auto artists = track.getArtists();
        string name = artists.empty() ? track.getTitle() : artists[0] + " - " + track.getTitle();
        name = name.size()>100 ? name.substr(0, 100)+".mp3" : name + ".mp3";
        if (auto search = loaded_tracks.find(name); search == loaded_tracks.end())
        {
            tracks_for_download.emplace_back(track);
        }
    }
    consoleLogger.information(fmt::format("Tracks for download: {}", tracks_for_download.size()));
    User::downloadTracks(tracks_for_download, lyrics_directory, tracks_directory);
}

void Playlist::downloadPlaylists(vector<Playlist> & playlists)
{
    for (Playlist playlist : playlists)
        downloadPlaylist();
}

void Playlist::getPlaylistTracks()
{
    Document document;
    Request request;
    string url {"users/"+userId+"/playlists/"+kind};
    request.makeRequest(url, document);
    SizeType i, j;

    const Value& rTracks = document["result"]["tracks"];
    assert(rTracks.IsArray());
    for (i = 0; i < rTracks.Size(); ++i) // Uses SizeType instead of size_t
    {
        vector<string> processed_artists;
        const Value& track = rTracks[i]["track"];
        for (j = 0; j < track["artists"].Size(); ++j)
            for (auto& artist : track["artists"][j].GetObject())
                if (string(artist.name.GetString()) == "name")
                    processed_artists.emplace_back(artist.value.GetString());

        Track processed_track(
                track["id"].GetString(),
                track["title"].GetString(),
                processed_artists,
                track["available"].GetBool());

        processed_artists.clear();
        tracks.emplace_back(processed_track);
    }
}
