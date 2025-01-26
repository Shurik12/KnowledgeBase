#include <YandexMusic/User.h>
#include <YandexMusic/Request.h>
#include <utility>

namespace yandex_music 
{
    std::string User::log_folder;

    User::User(std::string id_)
        : id(std::move(id_)) 
    {}

    std::string User::getId() const 
    {
        return id;
    }

    void User::setLog(const std::string &log_folder_) 
    {
        User::log_folder = log_folder_;
    }

    void User::printUserPlaylists() 
    {
        for (auto playlist: playlists)
            playlist.print();
    }

    Playlist User::getPlaylist(const int &kind) 
    {
        rapidjson::Document document;
        Request request;
        std::string url{"users/" + id + "/playlists/" + to_string(kind)};
        request.makeRequest(url, document);
        return playlistObjectFromResponse(document["result"]);
    }

    void User::getUserPlaylists() 
    {
        rapidjson::Document document;
        Request request;
        std::string url{"users/" + id + "/playlists/list"};
        request.makeRequest(url, document);

        const rapidjson::Value &result = document["result"];
        assert(result.IsArray());
        for (rapidjson::SizeType i = 0; i < result.Size(); i++) // Uses rapidjson::SizeType instead of size_t
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

    std::vector<Track> User::getTracks(std::vector<std::string> &track_ids) 
    {
        size_t i;
        std::map<std::string, std::string> body;
        std::string value;
        for (i = 0; i < track_ids.size() - 1; ++i)
            value += track_ids[i] + ",";
        value += track_ids[i];
        body["track-ids"] = value;
        //  body["with-positions"] = "false";

        Request request;
        rapidjson::Document document;
        std::string url_prefix = "tracks";
        request.makePostRequest(url_prefix, body, document);

        const rapidjson::Value &result = document["result"];
        assert(result.IsArray());

        std::vector<Track> tracks;
        std::vector<std::string> processed_artists;
        std::vector<int> processed_albums;

        for (i = 0; i < result.Size(); ++i) // Uses rapidjson::SizeType instead of size_t
        {
            /// Processing artists
            for (auto j = 0; j < result[i]["artists"].Size(); ++j)
                for (auto &artist: result[i]["artists"][j].GetObject())
                    if (std::string(artist.name.GetString()) == "name")
                        processed_artists.emplace_back(artist.value.GetString());

            /// Processing albums
            for (auto j = 0; j < result[i]["albums"].Size(); ++j)
                for (auto &album: result[i]["albums"][j].GetObject())
                    if (std::string(album.name.GetString()) == "id")
                        processed_albums.emplace_back(album.value.GetInt());

            Track processed_track(
                result[i]["id"].GetString(),
                result[i]["title"].GetString(),
                processed_artists,
                processed_albums,
                result[i]["available"].GetBool(),
                result[i].HasMember("lyricsAvailable") ? result[i]["lyricsAvailable"].GetBool() : false);

            processed_artists.clear();
            processed_albums.clear();

            tracks.emplace_back(processed_track);
//          for (auto& it : result[i].GetObject())
//              cout << std::string(it.name.GetString()) << "\n";
        }
        return tracks;
    }

    void User::getTracks(std::vector<Track> &tracks) 
    {
        size_t i;
        std::map<std::string, std::string> body;
        std::string value{"["};
        for (i = 0; i < tracks.size() - 1; ++i)
            value += tracks[i].getId() + ", ";
        value += tracks[i].getId() + "]";
        body["track-ids"] = value;

        Request request;
        rapidjson::Document document;
        std::string url_prefix = "track";
        request.makePostRequest(url_prefix, body, document);
    }

    void User::getLikeTracks() 
    {
        rapidjson::Document document;
        Request request;
        std::string url{"users/" + id + "/likes/tracks"};
        request.makeRequest(url, document);
        rapidjson::SizeType i, j;

        const rapidjson::Value &tracks = document["result"]["library"]["tracks"];

        //    for (auto& it : rTracks.GetObject())
        //        cout << std::string(it.name.GetString()) << "\n";

        assert(tracks.IsArray());

        std::vector<std::string> track_ids;
        for (i = 0; i < tracks.Size(); ++i) // Uses rapidjson::SizeType instead of size_t
        {
            std::vector<std::string> processed_artists{};
            track_ids.emplace_back(tracks[i]["id"].GetString());
        }
        like_tracks = getTracks(track_ids);
    }

    void User::downloadTracks(std::vector<Track> &tracks, std::string &lyrics_dir, std::string &tracks_dir) 
    {
        for (Track track: tracks)
            if (track.getAvailable())
                track.downloadTrack(lyrics_dir, tracks_dir);
    }

    void User::getTracksWithoutPlaylist() 
    {
        if (playlists.empty())
            getUserPlaylists();

        std::vector<Track> tracks_in_playlist;
        // std::vector<Track> tracks_out_playlist;
        std::vector<Track> tracks_out_like;

        for (auto playlist: playlists) 
        {
            playlist.getPlaylistTracks();
            for (const auto &track: playlist.tracks)
                tracks_in_playlist.emplace_back(track);
        }

        logger->info(fmt::format("Count tracks in any playlist: {}", tracks_in_playlist.size()));

        getLikeTracks();

        logger->info(fmt::format("All tracks: {}", like_tracks.size()));

        sort(
            like_tracks.begin(), like_tracks.end(), 
            [](auto &a, auto &b) { return a.getId() < b.getId(); });
        sort(
            tracks_in_playlist.begin(), tracks_in_playlist.end(),
            [](auto &a, auto &b) { return a.getId() < b.getId(); });

        set_difference(
            like_tracks.begin(), like_tracks.end(),
            tracks_in_playlist.begin(), tracks_in_playlist.end(),
            inserter(tracks_out_playlist, tracks_out_playlist.begin()),
            [](auto &a, auto &b) { return a.getId() < b.getId(); });

        set_difference(
            tracks_in_playlist.begin(), tracks_in_playlist.end(),
            like_tracks.begin(), like_tracks.end(),
            inserter(tracks_out_like, tracks_out_like.begin()),
            [](auto &a, auto &b) { return a.getId() < b.getId(); });

        logger->info(fmt::format("Count tracks out of playlist: {}", tracks_out_playlist.size()));
        logger->info(fmt::format("Count tracks out of like: {}", tracks_out_like.size()));
        logger->info(fmt::format("Tracks out of playlist:"));

        for (const auto &track: tracks_out_playlist) 
        {
            auto artists = track.getArtists();
            std::string name = artists.empty() ? track.getTitle() : artists[0] + " - " + track.getTitle();
            logger->info(fmt::format("\t{}", name));
        }
    }

    Playlist User::createPlaylist(const std::string &title) 
    {
        rapidjson::Document document;
        Request request;
        std::string url{"users/" + id + "/playlists/create"};
        std::map<std::string, std::string> body;
        body["visibility"] = "public";
        body["title"] = title;
        request.makePostRequest(url, body, document);

        /// Processing response
        return playlistObjectFromResponse(document["result"]);
    }

    void User::changePlaylistName(const int &kind, const std::string &new_title) 
    {
        rapidjson::Document document;
        Request request;
        std::string url{"users/" + id + "/playlists/" + to_string(kind) + "/name"};
        std::map<std::string, std::string> body;
        body["value"] = new_title;
        request.makePostRequest(url, body, document);

        /// Processing response
        const rapidjson::Value &result = document["result"];
        assert(result.IsObject());
        for (auto &key: result.GetObject()) 
        {
            if (std::string(key.name.GetString()) == "kind")
                cout << "Playlist kind: " << key.value.GetInt() << "\n";
            if (std::string(key.name.GetString()) == "title")
                cout << "Playlist title: " << key.value.GetString() << "\n";
        }
    }

    void User::deletePlaylist(const int &kind) 
    {
        rapidjson::Document document;
        Request request;
        std::string url{"users/" + id + "/playlists/" + to_string(kind) + "/delete"};
        std::map<std::string, std::string> body;
        request.makePostRequest(url, body, document);

        /// Processing response
        const rapidjson::Value &result = document["result"];
        assert(result.IsString());
        cout << "Delete playlist " + to_string(kind) << ": " << result.GetString() << "\n";
    }

    Playlist User::playlistObjectFromResponse(const rapidjson::Value &response) 
    {
        assert(response.IsObject());
        Playlist playlist;
        for (auto &key: response.GetObject()) 
        {
            if (std::string(key.name.GetString()) == "title")
                playlist.setTitle(key.value.GetString());
            if (std::string(key.name.GetString()) == "revision")
                playlist.setRevision(key.value.GetInt());
            if (std::string(key.name.GetString()) == "trackCount")
                playlist.setTrackCount(key.value.GetInt());
            if (std::string(key.name.GetString()) == "playlistUuid")
                playlist.setPlaylistUuid(key.value.GetString());
            if (std::string(key.name.GetString()) == "kind")
                playlist.setKind(key.value.GetInt());
            playlist.setId(id);
        }
        return playlist;
    }

    void User::downloadPlaylists(std::vector<Playlist> &playlists) 
    {
        for (Playlist playlist: playlists)
            playlist.downloadPlaylist();
    }

}