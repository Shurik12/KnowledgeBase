#include <YandexMusic/Track.h>
#include <YandexMusic/Request.h>
#include <Poco/MD5Engine.h>
#include <Poco/DigestStream.h>

#include <memory>
#include <utility>

#define SIGN_SALT "XGRlBW9FXlekgbPrRHuSiA"

Track::Track(
	string  id_,
	string  title_,
    vector<string> artists_,
    bool available_)
	: id(std::move(id_))
	, title(std::move(title_))
	, artists(std::move(artists_))
    , available(available_)
    , supplement()
{}


string Track::getTitle() const
{
	return title;
}


string Track::getId() const
{
	return id;
}

bool Track::getAvailable() const
{
    return available;
}

vector<string> Track::getArtists() const
{
    return artists;
}

void Track::getSign(string & download_url, const XMLDocument & xml_response)
{
    /// Parse XML
    string host, path, ts, s;
    host = xml_response.FirstChildElement( "download-info" )->FirstChildElement( "host" )->GetText();
    path = xml_response.FirstChildElement( "download-info" )->FirstChildElement( "path" )->GetText();
    ts = xml_response.FirstChildElement( "download-info" )->FirstChildElement( "ts" )->GetText();
    s = xml_response.FirstChildElement( "download-info" )->FirstChildElement( "s" )->GetText();

    /// Get md5 hash
    Poco::MD5Engine md5;
    Poco::DigestOutputStream ds(md5);
    ds << SIGN_SALT << path.substr(1) << s;
    ds.close();

    string sign = Poco::DigestEngine::digestToHex(md5.digest());

    /// Return value
    download_url = "https://" + host + "/" + "get-mp3/" + sign + "/" + ts + path;
}


void Track::downloadTrack(string & lyrics_dir, string & tracks_dir)
{
    /// Get track file name on fs
    string name = artists.empty() ? title : artists[0] + " - " + title;
    name = name.size()>100 ? name.substr(0, 100)+".mp3" : name + ".mp3";

    /// Get track download info
    DownloadInfo download_info = getDownloadInfo();
    string url = download_info.getDownloadInfoUrl();
    string bitrate = download_info.getBitrateInKbps();
    cout << bitrate << "\n";

    XMLDocument xml_response;
    Request request;
    request.makeRequest(url, xml_response);

    string download_url;
    getSign(download_url, xml_response);

    /// Download track
    Curl::get(download_url);
    FILE * fp;
    fp = fopen((tracks_dir + "/" + name).data() , "wb" );
    fwrite(Curl::buffer.data(), Curl::buffer.size(), 1, fp);
    fclose(fp);
}

DownloadInfo Track::getDownloadInfo()
{
    DownloadInfo download_info;
    Document document;
    Request request;
    string url {"tracks/"+id+"/download-info"};
    request.makeRequest(url, document);
    const Value& result = document["result"];
    assert(result.IsArray());
    for (SizeType i = 0; i < result.Size(); i++) // Uses SizeType instead of size_t
    {
        string k = result[i]["codec"].GetString();
        if ( k == "mp3")
        {
            download_info.setDownloadInfoUrl(result[i]["downloadInfoUrl"].GetString());
            download_info.setBitrateInKbps(to_string(result[i]["bitrateInKbps"].GetInt()));
        }
    }
    return download_info;
}

void Track::getSupplement()
{
//    Supplement supplement;
    Document document;
    Request request;
    string url {"tracks/"+id+"/supplement"};
    request.makeRequest(url, document);
    const Value& result = document["result"];
    assert(result.IsObject());

    supplement = make_shared<Supplement>();

    /// Keys {id: string, lyrics: object}
    for (auto& key : result.GetObject())
    {
        if (string(key.name.GetString()) == "lyrics")
            for (auto& key1 : key.value.GetObject())
            {
                if (string(key1.name.GetString()) == "id")
                    supplement->setId(key1.value.GetInt());

                if (string(key1.name.GetString()) == "lyrics")
                    supplement->setLyrics(key1.value.GetString());

                if (string(key1.name.GetString()) == "fullLyrics")
                    supplement->setFullLyrics(key1.value.GetString());

                if (string(key1.name.GetString()) == "textLanguage")
                    supplement->setTextLanguage(key1.value.GetString());

                if (string(key1.name.GetString()) == "showTranslation")
                    supplement->setShowTranslation(key1.value.GetBool());

                if (string(key1.name.GetString()) == "hasRights")
                    supplement->setHasRights(key1.value.GetBool());
            }
    }
}

