#include <YandexMusic/Request.h>

#include <fmt/format.h>
#include <Poco/Logger.h>
#include <Poco/Message.h>
#include <Poco/PatternFormatter.h>
#include <Poco/FormattingChannel.h>
#include <Poco/ConsoleChannel.h>
#include <Poco/FileChannel.h>

using namespace std;

using Poco::Logger;
using Poco::PatternFormatter;
using Poco::FormattingChannel;
using Poco::ConsoleChannel;
using Poco::FileChannel;
using Poco::Message;

int main()
{
/// Loggers initializations
///-------------------------------------------------------------------------------------------------------------------------------------------
    ///ToDo вынести в отдельную функцию создание логгера и доработать
    // set up two channel chains - one to the
    // console and the other one to a log file.
    auto * pFCConsole = new FormattingChannel(new PatternFormatter("%s: %p: %t"));
    pFCConsole->setChannel(new ConsoleChannel);
    pFCConsole->open();

    auto * pFCFile = new FormattingChannel(new PatternFormatter("%Y-%m-%d %H:%M:%S.%c %N [ %P ] : %s:%q:%t"));
    pFCFile->setChannel(new FileChannel("sample.log"));
    pFCFile->open();

    // create two Logger objects - one for
    // each channel chain.
    Logger & consoleLogger = Logger::create("ConsoleLogger", pFCConsole, Message::PRIO_DEBUG);
    Logger & fileLogger    = Logger::create("FileLogger", pFCFile, Message::PRIO_DEBUG);
    consoleLogger.information(fmt::format("\nStart program execution{}", "!"));
///-------------------------------------------------------------------------------------------------------------------------------------------

    int flag = 3;

	Request request {};
    request.processConfig();
	User user {request.getUser()};
    string user_id = user.getId();

    switch (flag)
    {
        case 1:
            user.getUserPlaylists();
            user.printUserPlaylists();
            break;
        case 2:
            user.getTracksWithoutPlaylist();
            break;
        case 3:
        {
            Playlist playlist {user.createPlaylist("Test3")};
            user.changePlaylistName(playlist.getKind(), "Test4");
            user.getLikeTracks();
            playlist.addTracksToPlaylist(user.like_tracks);
            user.deletePlaylist(playlist.getKind());
            break;
        }

        case 4:
        {
            user.getUserPlaylists();
            auto playlist = user.playlists[0];
            playlist.getPlaylistTracks();
            auto track = playlist.tracks[0];
            track.print();
        }

        default:
            cout << "Nothing to do\n";
    }
//    track.getSupplement();
//    cout << track.supplement->getLyrics() << "\n";

    return 0;
}