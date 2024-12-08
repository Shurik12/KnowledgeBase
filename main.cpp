#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"

#include <YandexMusic/Request.h>
#include <Databases/Clickhouse.h>

using namespace yandex_music;

void multi_sink_example();
// create a logger with 2 targets, with different log levels and formats.
// The console will show only warnings or errors, while the file will log all.
void multi_sink_example()
{
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(spdlog::level::warn);
    console_sink->set_pattern("[%H:%M:%S %z] [thread %t] [%^%l%$] %v");

    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/multisink.txt", true);
    file_sink->set_level(spdlog::level::trace);
    file_sink->set_pattern("[%H:%M:%S %z] [thread %t] [%^%l%$] %v");

    std::vector<spdlog::sink_ptr> sinks{console_sink, file_sink};
    auto logger = std::make_shared<spdlog::logger>("multi_sink", sinks.begin(), sinks.end());
    spdlog::register_logger(logger);
}

int main()
{
    Clickhouse::createTableUser();
    multi_sink_example();

    int flag = 1;

    yandex_music::Request request {};
    yandex_music::Request::processConfig();
    yandex_music::User user {request.getUser()};
    string user_id = user.getId();

    switch (flag)
    {
        /// Print playlists
        case 1:
            user.getUserPlaylists();
            user.printUserPlaylists();
            break;

        /// Get tracks without playlist
        case 2:
            user.getTracksWithoutPlaylist();
            break;

        /// Create, change and delete playlist
        case 3:
        {
            yandex_music::Playlist playlist {user.createPlaylist("Test3")};
            user.changePlaylistName(playlist.getKind(), "Test4");
            user.getLikeTracks();
            playlist.addTracksToPlaylist(user.like_tracks);
            user.deletePlaylist(playlist.getKind());
            break;
        }

        /// Print track
        case 4:
        {
            user.getUserPlaylists();
            auto playlist = user.playlists[4];
            playlist.getPlaylistTracks();
            auto track = playlist.tracks[0];
            track.print();
            break;
        }

        /// Download playlist
        case 5:
        {
            user.getUserPlaylists();
            auto playlist = user.playlists[7];
            playlist.downloadPlaylist();
            break;
        }

        /// Download all playlists
        case 6:
        {
            user.getUserPlaylists();
            for (auto playlist : user.playlists)
                playlist.downloadPlaylist();
        }

        default:
            std::cout << "Done!\n";
            // fileLogger.information(fmt::format("Nothing to do\n"));
    }
//    track.getSupplement();
//    cout << track.supplement->getLyrics() << "\n";

    return 0;
}