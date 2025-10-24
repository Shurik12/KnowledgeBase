#include <YandexMusic/User.h>
#include <YandexMusic/Request.h>

#include <algorithm>
#include <utility>
#include <fmt/ranges.h>

namespace YandexMusic
{
    User::User(std::string id) : id_(std::move(id)) {}

    void User::fetchPlaylists()
    {
        Request request;
        rapidjson::Document document;
        const std::string url = "users/" + id_ + "/playlists/list";

        request.makeRequest(url, document);
        const auto &result = document["result"];

        if (!result.IsArray())
        {
            throw std::runtime_error("Invalid playlists array in response");
        }

        playlists_.clear();
        for (const auto &playlist : result.GetArray())
        {
            playlists_.emplace_back(
                playlist["title"].GetString(),
                playlist["revision"].GetInt(),
                playlist["trackCount"].GetInt(),
                playlist["playlistUuid"].GetString(),
                playlist["kind"].GetInt(),
                id_);
        }
    }

    Playlist User::fetchPlaylist(int kind) const
    {
        Request request;
        rapidjson::Document document;
        const std::string url = "users/" + id_ + "/playlists/" + std::to_string(kind);

        request.makeRequest(url, document);
        return parsePlaylistResponse(document["result"]);
    }

    std::vector<Track> User::fetchTracks(const std::vector<std::string> &trackIds)
    {
        if (trackIds.empty())
            return {};

        Request request;
        rapidjson::Document document;
        std::map<std::string, std::string> body{
            {"track-ids", fmt::format("{}", fmt::join(trackIds, ","))}};

        request.makePostRequest("tracks", body, document);
        const auto &result = document["result"];

        if (!result.IsArray())
        {
            throw std::runtime_error("Invalid tracks array in response");
        }

        std::vector<Track> tracks;
        for (const auto &track : result.GetArray())
        {
            // Process artists
            std::vector<std::string> artists;
            for (const auto &artist : track["artists"].GetArray())
            {
                if (artist.HasMember("name"))
                {
                    artists.push_back(artist["name"].GetString());
                }
            }

            // Process albums
            std::vector<int> albums;
            for (const auto &album : track["albums"].GetArray())
            {
                if (album.HasMember("id"))
                {
                    albums.push_back(album["id"].GetInt());
                }
            }

            tracks.emplace_back(
                track["id"].GetString(),
                track["title"].GetString(),
                artists,
                albums,
                track["available"].GetBool(),
                track.HasMember("lyricsAvailable") ? track["lyricsAvailable"].GetBool() : false);
        }

        return tracks;
    }

    void User::fetchLikedTracks()
    {
        Request request;
        rapidjson::Document document;
        const std::string url = "users/" + id_ + "/likes/tracks";

        request.makeRequest(url, document);
        const auto &tracks = document["result"]["library"]["tracks"];

        if (!tracks.IsArray())
        {
            throw std::runtime_error("Invalid liked tracks array in response");
        }

        std::vector<std::string> trackIds;
        for (const auto &track : tracks.GetArray())
        {
            trackIds.push_back(track["id"].GetString());
        }

        likedTracks_ = fetchTracks(trackIds);
    }

    void User::analyzeTracksWithoutPlaylists()
    {
        if (playlists_.empty())
        {
            fetchPlaylists();
        }

        std::vector<Track> allPlaylistTracks;
        for (auto &playlist : playlists_)
        {
            playlist.fetchTracks();
            allPlaylistTracks.insert(allPlaylistTracks.end(),
                                     playlist.tracks().begin(),
                                     playlist.tracks().end());
        }

        fetchLikedTracks();

        // Sort for set operations
        std::sort(likedTracks_.begin(), likedTracks_.end(),
                  [](const auto &a, const auto &b)
                  { return a.id() < b.id(); });

        std::sort(allPlaylistTracks.begin(), allPlaylistTracks.end(),
                  [](const auto &a, const auto &b)
                  { return a.id() < b.id(); });

        // Find tracks in liked but not in any playlist
        tracksWithoutPlaylists_.clear();
        std::set_difference(
            likedTracks_.begin(), likedTracks_.end(),
            allPlaylistTracks.begin(), allPlaylistTracks.end(),
            std::back_inserter(tracksWithoutPlaylists_),
            [](const auto &a, const auto &b)
            { return a.id() < b.id(); });

        spdlog::info("Tracks without playlists: {}", tracksWithoutPlaylists_.size());
    }

    Playlist User::createPlaylist(std::string_view title) const
    {
        Request request;
        rapidjson::Document document;
        const std::string url = "users/" + id_ + "/playlists/create";

        std::map<std::string, std::string> body{
            {"visibility", "public"},
            {"title", std::string{title}}};

        request.makePostRequest(url, body, document);
        return parsePlaylistResponse(document["result"]);
    }

    void User::renamePlaylist(int kind, std::string_view newTitle) const
    {
        Request request;
        rapidjson::Document document;
        const std::string url = "users/" + id_ + "/playlists/" + std::to_string(kind) + "/name";

        std::map<std::string, std::string> body{
            {"value", std::string{newTitle}}};

        request.makePostRequest(url, body, document);
    }

    void User::deletePlaylist(int kind) const
    {
        Request request;
        rapidjson::Document document;
        const std::string url = "users/" + id_ + "/playlists/" + std::to_string(kind) + "/delete";

        std::map<std::string, std::string> body {};
        request.makePostRequest(url, body, document);
    }

    void User::downloadTracks(const std::vector<Track> &tracks,
                              std::string_view lyricsDir,
                              std::string_view tracksDir)
    {
        for (const auto &track : tracks)
        {
            if (track.available())
            {
                track.download(lyricsDir, tracksDir);
            }
        }
    }

    void User::printPlaylists() const
    {
        spdlog::info("User playlists (title: kind)");
        for (const auto &playlist : playlists_)
        {
            playlist.print();
        }
    }

    Playlist User::parsePlaylistResponse(const rapidjson::Value &response) const
    {
        if (!response.IsObject())
        {
            throw std::runtime_error("Invalid playlist response");
        }

        Playlist playlist;
        playlist.setUserId(id_);

        for (const auto &field : response.GetObject())
        {
            const std::string_view name{field.name.GetString()};

            if (name == "title")
            {
                playlist.setTitle(field.value.GetString());
            }
            else if (name == "revision")
            {
                playlist.setRevision(field.value.GetInt());
            }
            else if (name == "trackCount")
            {
                playlist.setTrackCount(field.value.GetInt());
            }
            else if (name == "playlistUuid")
            {
                playlist.setPlaylistUuid(field.value.GetString());
            }
            else if (name == "kind")
            {
                playlist.setKind(field.value.GetInt());
            }
        }

        return playlist;
    }
} // namespace YandexMusic