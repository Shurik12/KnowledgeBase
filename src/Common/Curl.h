#include <map>
#include <string>
#include <iostream>
 
#include <curl/curl.h>
 
using namespace std;
 
class Curl
{

protected:    // User declarations

    static string token;
    static char errorBuffer[CURL_ERROR_SIZE];
    static int writer(char *data, size_t size, size_t nmemb, string *buffer);
    static string easycurl(const string &url, bool post, const string &postparamstring);
    static string urlencode(const string &c);
    static string char2hex(char dec);    
 
public:        // User declarations
    
    Curl()= default;
    ~Curl()= default;
    friend class User;
    static string buffer;
    static string post(const string & url, map<string, string> & abbrevs);
    static string get(const string & url, map<string, string> & abbrevs);
    static string get(const string & url);
    static void setToken(const string & token);
};