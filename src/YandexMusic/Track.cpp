#include <YandexMusic/Track.h>
#include <YandexMusic/Request.h>
#include <Common/md5.h>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <utility>

#include <fmt/ranges.h>
namespace YandexMusic
{

    namespace
    {
        constexpr std::string_view SIGN_SALT = "XGRlBW9FXlekgbPrRHuSiA";
    }

    Track::Track(std::string id, std::string title,
                 std::vector<std::string> artists,
                 std::vector<int> albums,
                 bool available,
                 bool lyricsAvailable)
        : id_(std::move(id)), title_(std::move(title)), artists_(std::move(artists)), albums_(std::move(albums)), available_(available), lyricsAvailable_(lyricsAvailable)
    {
    }

    void Track::fetchDownloadInfo()
    {
        Request request;
        rapidjson::Document document;
        const std::string url = "tracks/" + id_ + "/download-info";

        request.makeRequest(url, document);
        const auto &result = document["result"];

        if (!result.IsArray())
        {
            throw std::runtime_error("Invalid response format for download info");
        }

        for (const auto &item : result.GetArray())
        {
            if (std::string_view{item["codec"].GetString()} == "mp3")
            {
                downloadInfo_.emplace(
                    item["downloadInfoUrl"].GetString(),
                    std::to_string(item["bitrateInKbps"].GetInt()));
                break;
            }
        }
    }

    void Track::fetchSupplement()
    {
        if (!lyricsAvailable_)
            return;

        Request request;
        rapidjson::Document document;
        const std::string url = "tracks/" + id_ + "/supplement";

        request.makeRequest(url, document);
        const auto &result = document["result"];

        if (!result.IsObject())
        {
            throw std::runtime_error("Invalid response format for supplement");
        }

        const auto lyricsIt = result.FindMember("lyrics");
        if (lyricsIt == result.MemberEnd())
            return;

        const auto &lyricsObj = lyricsIt->value;
        Supplement supp;

        for (const auto &field : lyricsObj.GetObject())
        {
            const std::string_view name{field.name.GetString()};

            if (name == "id")
            {
                supp.setId(field.value.GetInt());
            }
            else if (name == "lyrics")
            {
                supp.setLyrics(field.value.GetString());
            }
            else if (name == "fullLyrics")
            {
                supp.setFullLyrics(field.value.GetString());
            }
            else if (name == "textLanguage")
            {
                supp.setTextLanguage(field.value.GetString());
            }
            else if (name == "showTranslation")
            {
                supp.setShowTranslation(field.value.GetBool());
            }
            else if (name == "hasRights")
            {
                supp.setHasRights(field.value.GetBool());
            }
        }

        supplement_.emplace(std::move(supp));
    }

    std::string Track::generateDownloadUrl(const tinyxml2::XMLDocument &xmlResponse)
    {
        const auto *downloadInfo = xmlResponse.FirstChildElement("download-info");
        if (!downloadInfo)
        {
            throw std::runtime_error("Invalid XML response - missing download-info");
        }

        const std::string host = downloadInfo->FirstChildElement("host")->GetText();
        const std::string path = downloadInfo->FirstChildElement("path")->GetText();
        const std::string ts = downloadInfo->FirstChildElement("ts")->GetText();
        const std::string s = downloadInfo->FirstChildElement("s")->GetText();

        MD5 md5;
        const std::string text = std::string{SIGN_SALT} + path.substr(1) + s;
        md5.add(text.c_str(), text.size());

        return fmt::format("https://{}/get-mp3/{}/{}{}", host, md5.getHash(), ts, path);
    }

    void Track::download(std::string_view lyricsDir, std::string_view tracksDir) const
    {
        if (!available_ || !downloadInfo_)
            return;

        // Generate filename
        std::string filename = artists_.empty() ? title_ : artists_[0] + " - " + title_;
        if (filename.size() > 100)
        {
            filename.resize(100);
        }
        filename += ".mp3";
        std::replace(filename.begin(), filename.end(), '/', '-');

        // Download track
        tinyxml2::XMLDocument xmlResponse;
        Request request;
        request.makeRequest(downloadInfo_->downloadInfoUrl(), xmlResponse);

        const std::string downloadUrl = generateDownloadUrl(xmlResponse);
        Curl::get(downloadUrl);

        // Save track
        const std::string trackPath = std::string{tracksDir} + "/" + filename;
        std::ofstream trackFile(trackPath, std::ios::binary);
        trackFile.write(Curl::buffer.data(), Curl::buffer.size());

        // Save lyrics if available
        if (lyricsAvailable_ && supplement_)
        {
            const std::string lyricsPath = std::string{lyricsDir} + "/" + filename + ".txt";
            std::ofstream lyricsFile(lyricsPath);
            lyricsFile << supplement_->fullLyrics();
        }
    }

    void Track::print() const
    {
        fmt::print("Id: {}\n", id_);
        fmt::print("Title: {}\n", title_);
        fmt::print("Artists: {}\n", fmt::join(artists_, ", "));
        fmt::print("Albums: {}\n", fmt::join(albums_, ", "));
        fmt::print("Available: {}\n", available_);
    }

} // namespace YandexMusic