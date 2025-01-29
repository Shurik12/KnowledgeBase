#include <map>
#include <string>
#include <iostream>
 
#include <curl/curl.h>
 
using namespace std;
 
class Curl
{
protected:    // User declarations

    static std::string token;
    static char errorBuffer[CURL_ERROR_SIZE];
    static int writer(char *data, size_t size, size_t nmemb, std::string *buffer);
    static std::string easycurl(const std::string &url, bool post, const std::string &postparamstring);
    static std::string urlencode(const std::string &c);
    static std::string char2hex(char dec);    
 
public:        // User declarations
    
    Curl()= default;
    ~Curl()= default;
    friend class User;
    static std::string buffer;
    static std::string post(const std::string & url, std::map<std::string, std::string> & abbrevs);
    static std::string get(const std::string & url, std::map<std::string, std::string> & abbrevs);
    static std::string get(const std::string & url);
    static void setToken(const std::string & token);
};