#include <YandexMusic/Request.h>

#define SIGN_SALT "XGRlBW9FXlekgbPrRHuSiA"

namespace yandex_music 
{
    bool Request::processConfig()
    {
        tinyxml2::XMLDocument xml_doc;
        tinyxml2::XMLError eResult = xml_doc.LoadFile("config.xml");

        if (eResult != tinyxml2::XML_SUCCESS)
            return false;

        tinyxml2::XMLNode *root = xml_doc.FirstChild();
        if (root == nullptr)
            return false;

        auto token = root->FirstChildElement("token")->GetText();
        auto output_folder = root->FirstChildElement("output_folder")->GetText();
        auto log_folder = root->FirstChildElement("log_folder")->GetText();

        std::filesystem::create_directories(output_folder);
        std::filesystem::create_directories(log_folder);

        Curl::setToken(token);
        Playlist::setOutput(output_folder);
        User::setLog(log_folder);

        return true;
    }

    User Request::getUser() 
    {
        std::string url{url_prefix};
        url += "account/settings";
        Curl::get(url);
        rapidjson::Document document;
        document.Parse(Curl::buffer.c_str());
        User user{to_string(document["result"]["uid"].GetInt())};
        return user;
    }

    void Request::makePostRequest(
        string &url_postfix, 
        std::map<std::string, 
        std::string> &body, 
        rapidjson::Document &document) 
    {
        std::string url{url_prefix + url_postfix};
        auto res = Curl::post(url, body);
        document.Parse(Curl::buffer.c_str());
    }

    void Request::makeRequest(string &params, rapidjson::Document &document) 
    {
        string url{url_prefix + params};
        Curl::get(url);
        document.Parse(Curl::buffer.c_str());
    }

    void Request::makeRequest(const std::string &url, tinyxml2::XMLDocument &xml_response) 
    {
        Curl::get(url);
        xml_response.Parse(Curl::buffer.c_str());

        /// Print XML to log file
        tinyxml2::XMLPrinter printer;
        xml_response.Print(&printer);
        logger->debug(fmt::format("\n{}", printer.CStr()));
        /// Save XML to file
        /// xml_response.SaveFile( "tmp/foo.xml" );
    }
}