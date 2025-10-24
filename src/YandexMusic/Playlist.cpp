// Playlist.cpp
#include <YandexMusic/Playlist.h>
#include <YandexMusic/Request.h>
#include <rapidjson/document.h>

#include <algorithm>
#include <set>
#include <utility>

namespace YandexMusic
{
    Playlist::Playlist(std::string title, int revision, int trackCount,
                       std::string playlistUuid, int kind, std::string userId)
        : title_(std::move(title)), revision_(revision), trackCount_(trackCount), playlistUuid_(std::move(playlistUuid)), kind_(kind), userId_(std::move(userId))
    {
    }

    void Playlist::print() const
    {
        spdlog::info("{}: {}", title_, kind_);
    }

    void Playlist::download()
    {
        spdlog::info("Downloading playlist: {}", title_);

        const auto playlistDir = outputDirectory_ / title_;
        std::filesystem::create_directories(playlistDir);

        if (title_ != "Like")
        {
            fetchTracks();
        }

        if (tracks_.empty())
        {
            spdlog::warn("No tracks in playlist: {}", title_);
            return;
        }

        const auto tracksDir = playlistDir / "tracks";
        const auto lyricsDir = playlistDir / "lyrics";
        std::filesystem::create_directories(tracksDir);
        std::filesystem::create_directories(lyricsDir);

        // Get already downloaded tracks
        std::set<std::string> downloadedTracks;
        for (const auto &entry : std::filesystem::directory_iterator(tracksDir))
        {
            downloadedTracks.insert(entry.path().filename().string());
        }

        // Filter tracks that need to be downloaded
        std::vector<Track> tracksToDownload;
        for (const auto &track : tracks_)
        {
            std::string filename = track.artists().empty()
                                       ? track.title()
                                       : track.artists()[0] + " - " + track.title();

            if (filename.size() > 100)
            {
                filename.resize(100);
            }
            filename += ".mp3";

            if (downloadedTracks.count(filename) == 0)
            {
                tracksToDownload.push_back(track);
            }
        }

        spdlog::info("Tracks to download: {}", tracksToDownload.size());
        User::downloadTracks(tracksToDownload, lyricsDir.string(), tracksDir.string());
    }

    void Playlist::fetchTracks()
    {
        Request request;
        rapidjson::Document document;
        const std::string url = "users/" + userId_ + "/playlists/" + std::to_string(kind_);

        request.makeRequest(url, document);
        const auto &rTracks = document["result"]["tracks"];

        if (!rTracks.IsArray())
        {
            throw std::runtime_error("Invalid tracks array in response");
        }

        tracks_.clear();
        artists_.clear();

        for (const auto &trackItem : rTracks.GetArray())
        {
            const auto &track = trackItem["track"];

            // Process artists
            std::vector<std::string> trackArtists;
            for (const auto &artist : track["artists"].GetArray())
            {
                if (artist.HasMember("name"))
                {
                    trackArtists.push_back(artist["name"].GetString());
                }
            }

            // Process albums
            std::vector<int> trackAlbums;
            for (const auto &album : track["albums"].GetArray())
            {
                if (album.HasMember("id"))
                {
                    trackAlbums.push_back(album["id"].GetInt());
                }
            }

            Track newTrack(
                track["id"].GetString(),
                track["title"].GetString(),
                trackArtists,
                trackAlbums,
                track["available"].GetBool(),
                track.HasMember("lyricsAvailable") ? track["lyricsAvailable"].GetBool() : false);

            if (!trackArtists.empty())
            {
                artists_.insert(trackArtists[0]);
            }

            tracks_.push_back(std::move(newTrack));
        }
    }

    void Playlist::addTracks(const std::vector<Track> &tracks)
    {
        if (tracks.empty())
            return;

        Request request;
        rapidjson::Document document;
        const std::string url = "users/" + userId_ + "/playlists/" + std::to_string(kind_) + "/change-relative";

        std::string diff = R"({"diff":{"op":"insert","at":0,"tracks":[)";
        for (const auto &track : tracks)
        {
            diff += fmt::format(R"({{"id":"{}","albumId":"{}"}},)", track.id(), track.albums()[0]);
        }
        diff.pop_back(); // Remove trailing comma
        diff += R"(]}})";

        std::map<std::string, std::string> body{
            {"revision", std::to_string(revision_)},
            {"diff", diff}};

        request.makePostRequest(url, body, document);
    }

    void Playlist::removeTracks(const std::vector<Track> &tracks)
    {
        if (tracks.empty())
            return;

        Request request;
        rapidjson::Document document;
        const std::string url = "users/" + userId_ + "/playlists/" + std::to_string(kind_) + "/change-relative";

        std::string diff = fmt::format(R"({{"diff":{{"op":"delete","from":{},"to":{},"tracks":[)", 0, tracks.size());
        for (const auto &track : tracks)
        {
            diff += fmt::format(R"({{"id":"{}","albumId":"{}"}},)", track.id(), track.albums()[0]);
        }
        diff.pop_back(); // Remove trailing comma
        diff += R"(]}}}})";

        std::map<std::string, std::string> body{
            {"revision", std::to_string(revision_)},
            {"diff", diff}};

        request.makePostRequest(url, body, document);
    }

    void Playlist::setOutputDirectory(std::string_view path)
    {
        outputDirectory_ = std::filesystem::path{path};
    }
} // namespace YandexMusic