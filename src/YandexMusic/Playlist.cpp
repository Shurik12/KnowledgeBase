#include <YandexMusic/Playlist.h>
#include <YandexMusic/Request.h>
#include <rapidjson/document.h>
#include <set>
#include <utility>

namespace yandex_music {

    string Playlist::output_folder;

    Playlist::Playlist(
            string title_,
            int revision_,
            int trackCount_,
            string playlistUuid_,
            int kind_,
            string userId_)
            : title(std::move(title_)), revision(revision_), trackCount(trackCount_),
              playlistUuid(std::move(playlistUuid_)), kind(kind_), userId(std::move(userId_)) {}

    string Playlist::getTitle() const {
        return title;
    }

    void Playlist::setTitle(const string &title_) {
        title = title_;
    }

    int Playlist::getRevision() const {
        return revision;
    }

    void Playlist::setRevision(const int &revision_) {
        revision = revision_;
    }


    int Playlist::getTrackCount() const {
        return trackCount;
    }

    void Playlist::setTrackCount(const int &trackCount_) {
        trackCount = trackCount_;
    }

    string Playlist::getPlaylistUuid() const {
        return playlistUuid;
    }

    void Playlist::setPlaylistUuid(const string &playlistUuid_) {
        playlistUuid = playlistUuid_;
    }

    int Playlist::getKind() const {
        return kind;
    }

    void Playlist::setKind(const int &kind_) {
        kind = kind_;
    }

    void Playlist::setId(const string &id_) {
        userId = id_;
    }

    void Playlist::print() {
        cout << this->title << ": " << this->kind << "\n";
        logger->info(fmt::format("{}: {}", this->title, this->kind));
    }

    void Playlist::downloadPlaylist() {
        logger->info(fmt::format("Playlist to download: {}", title));
        string playlist_folder = output_folder + "/" + title;
        filesystem::create_directories(playlist_folder);
        getPlaylistTracks();

        string tracks_folder = playlist_folder + "/tracks";
        string lyrics_folder = playlist_folder + "/lyrics";

        filesystem::create_directories(tracks_folder);
        filesystem::create_directories(lyrics_folder);

        /// Get already loaded tracks (already on yandex disk and file system)
        if (tracks.empty()) {
            logger->warn(fmt::format("No tracks in playlist: {}", title));
            return;
        }

        logger->info(fmt::format("Count tracks in playlist {}: {}", title, tracks.size()));

        /// Get list directory
        set<string> loaded_tracks;
        string file_name;
        for (const auto &entry: fs::directory_iterator(tracks_folder)) {
            file_name = entry.path().generic_string();
            file_name = file_name.substr(file_name.find_last_of('/') + 1);
            logger->info(fmt::format("{}", file_name));
            loaded_tracks.emplace(file_name);
        }
        logger->info(fmt::format("Already loaded tracks in playlist {}: {}", title, loaded_tracks.size()));

        /// Download new tracks
        vector<Track> tracks_for_download;
        for (const Track &track: tracks) {
            auto track_artists = track.getArtists();
            string name = track_artists.empty() ? track.getTitle() : track_artists[0] + " - " + track.getTitle();
            if (name.size() > 100)
                name = name.substr(0, 100);
            name += ".mp3";
            if (auto search = loaded_tracks.find(name); search == loaded_tracks.end()) {
                tracks_for_download.emplace_back(track);
            }
        }
        logger->info(fmt::format("Tracks for download: {}", tracks_for_download.size()));
        User::downloadTracks(tracks_for_download, lyrics_folder, tracks_folder);
    }

    void Playlist::getPlaylistTracks() {
        Document document;
        Request request;
        string url{"users/" + userId + "/playlists/" + to_string(kind)};
        request.makeRequest(url, document);
        SizeType i, j;

        const Value &rTracks = document["result"]["tracks"];
        assert(rTracks.IsArray());
        for (i = 0; i < rTracks.Size(); ++i) // Uses SizeType instead of size_t
        {
            vector<string> processed_artists;
            vector<int> processed_albums;

            const Value &track = rTracks[i]["track"];

            /// Processing artists
            for (j = 0; j < track["artists"].Size(); ++j)
                for (auto &artist: track["artists"][j].GetObject())
                    if (string(artist.name.GetString()) == "name")
                        processed_artists.emplace_back(artist.value.GetString());

            /// Processing albums
            for (j = 0; j < track["albums"].Size(); ++j)
                for (auto &album: track["albums"][j].GetObject())
                    if (string(album.name.GetString()) == "id")
                        processed_albums.emplace_back(album.value.GetInt());

//        for (auto& it : track.GetObject())
//            cout << it.name.GetString() << "\n";

            Track processed_track(
                    track["id"].GetString(),
                    track["title"].GetString(),
                    processed_artists,
                    processed_albums,
                    track["available"].GetBool(),
                    track.HasMember("lyricsAvailable") ? track["lyricsAvailable"].GetBool() : false);


            if (!processed_artists.empty())
                artists.insert(processed_artists[0]);
            processed_artists.clear();
            tracks.emplace_back(processed_track);
        }
    }


    void Playlist::setOutput(const string &output_folder_) {
        Playlist::output_folder = output_folder_;
    }


    void Playlist::addTracksToPlaylist(const vector<Track> &tracks) {
        Document document;
        Request request;

        string url{"users/" + userId + "/playlists/" + to_string(kind) + "/change-relative"};
        map<string, string> body;
        body["revision"] = to_string(revision);
        body["diff"] = fmt::format(R"({{"diff":{{"op":"insert","at":{},"tracks":[{{"id":"{}","albumId":"{}"}}]}}}})", 0,
                                   tracks[0].getId(), tracks[0].getAlbums()[0]);
        request.makePostRequest(url, body, document);
    }

}