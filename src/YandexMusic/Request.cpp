#include <YandexMusic/Request.h>

#define SIGN_SALT "XGRlBW9FXlekgbPrRHuSiA"

namespace YandexMusic {
    User Request::getUser() {
        std::string url{url_prefix};
        url += "account/settings";
        Curl::get(url);
        rapidjson::Document document;
        document.Parse(Curl::buffer.c_str());
        User user{std::to_string(document["result"]["uid"].GetInt())};
        return user;
    }

    void Request::makePostRequest(
        const std::string &url_postfix, 
        std::map<std::string, std::string> &body, 
        rapidjson::Document &document) 
    {
        std::string url{url_prefix + url_postfix};
        auto res = Curl::post(url, body);
        document.Parse(Curl::buffer.c_str());
    }

    void Request::makeRequest(const std::string &params, rapidjson::Document &document) 
    {
        std::string url{url_prefix + params};
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