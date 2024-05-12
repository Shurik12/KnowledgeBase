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

	Request request {};
	User user {request.getUser()};
    string user_id = user.getId();
	vector<Playlist> playlists {user.getUserPlaylists()};
    for (Playlist playlist : playlists)
        playlist.print();
    Playlist playlist = playlists[5];
    playlist.downloadPlaylist();
//	vector<Track> tracks {playlist.getPlaylistTracks()};
//    Track track = tracks[2];
//	DownloadInfo download_info = track.getDownloadInfo();
//    string url = download_info.getDownloadInfoUrl();
//	track.downloadTrack(url);
	return 0;
}