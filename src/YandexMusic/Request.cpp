#include <YandexMusic/Request.h>
#include <Poco/MD5Engine.h>

#define SIGN_SALT "XGRlBW9FXlekgbPrRHuSiA"

void Request::processConfig()
{
    consoleLogger.debug(fmt::format("Processing configuration file '/home/alex/Git/KnowledgeBase/config.xml'."));
    XMLDocument xml_doc;
    XMLError eResult = xml_doc.LoadFile("/home/alex/Git/KnowledgeBase/config.xml");
    if (eResult != XML_SUCCESS)
        cout << "Error!\n";

    XMLNode * root = xml_doc.FirstChild();
    if (root == nullptr)
        cout << "Error!\n";

    auto token = root->FirstChildElement("token")->GetText();
    Curl::setToken(token);
}

User Request::getUser()
{
	string url {url_prefix};
	url += "account/settings";
	Curl::get(url);
    Document document;
    document.Parse(Curl::buffer.c_str());
	User user {to_string(document["result"]["uid"].GetInt())};
	return user;
}

void Request::makePostRequest(string & url_postfix, map<string, string> & body, Document & document)
{
    string url {url_prefix + url_postfix};
    auto res = Curl::post(url, body);
    document.Parse(Curl::buffer.c_str());
}

void Request::makeRequest(string & params, Document & document)
{
    string url {url_prefix + params};
    Curl::get(url);
    document.Parse(Curl::buffer.c_str());
}

void Request::makeRequest(const string & url, XMLDocument & xml_response)
{
    Curl::get(url);
    xml_response.Parse(Curl::buffer.c_str());

    /// Print XML to log file
    XMLPrinter printer;
    xml_response.Print( &printer );
    fileLogger.debug(fmt::format("\n{}", printer.CStr()));
    consoleLogger.debug(fmt::format("\n{}", printer.CStr()));
    /// Save XML to file
    /// xml_response.SaveFile( "tmp/foo.xml" );
}
