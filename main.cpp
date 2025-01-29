#include <YandexMusic/Request.h>
#include <Databases/Clickhouse.h>
#include <Common/Logging.h>

using namespace yandex_music;

int main()
{
    Clickhouse::createTableUser();

    int flag = 2;

    yandex_music::Request request {};
    yandex_music::Request::processConfig();

    multi_sink_example(yandex_music::User::log_folder + "/multisink.txt");

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
            break;
        }

        /// Download playlist Like
        case 7:
        {
            user.getLikeTracks();
            yandex_music::Playlist playlist ("Like", 0, 0, "", 0, user_id);
            playlist.tracks = user.like_tracks;
            playlist.downloadPlaylist();
            break;
        }

        /// Print playlist artists
        case 8:
        {
            user.getUserPlaylists();
            for (auto playlist: user.playlists)
            {
                playlist.print();
                playlist.getPlaylistTracks();
                for (auto artist : playlist.artists)
                    std::cout << "\t" << artist << "\n";
            }
        }

        default:
            std::cout << "Done!\n";
            // fileLogger.information(fmt::format("Nothing to do\n"));
    }
//    track.getSupplement();
//    cout << track.supplement->getLyrics() << "\n";

    return 0;
}