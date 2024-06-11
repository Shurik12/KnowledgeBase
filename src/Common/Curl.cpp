#include <Common/Curl.h>
 
//have to re-declare static members at file level
char Curl::errorBuffer[CURL_ERROR_SIZE];
string Curl::buffer;
string Curl::token;

//---------------------------------------------------------------------------
string Curl::get(const string &url)
{
    bool use_post{false};
    string params;
    return easycurl(url, use_post, params);
}
//---------------------------------------------------------------------------
string Curl::get(const string &url, map<string, string> &m)
{
    bool use_post{false};
    string post_string;
 
    map<string, string>::iterator curr,end;
    for(curr = m.begin(), end = m.end(); curr != end; ++curr)
    {
        post_string += curr->first + "=" + Curl::urlencode(curr->second)+ "&";
    }
 
    return easycurl(url, use_post, post_string);
}
//---------------------------------------------------------------------------
string Curl::post(const string &url, map<string, string> &m)
{
    bool use_post{true};
    string post_string;
 
    map<string, string>::iterator curr,end;
    for(curr = m.begin(), end = m.end(); curr != end; curr++)
    {
        post_string+= curr->first + "=" + Curl::urlencode(curr->second)+ "&";
    }
 
    return easycurl(url, use_post, post_string);
}
//---------------------------------------------------------------------------
string Curl::easycurl(const string &url, bool post, const string & post_param_string)
{
 
    // Our curl objects
    buffer = "";
    errorBuffer[0] = 0;

    struct curl_slist *headers=nullptr; /* init to NULL is important */
    string authorization = "Authorization: OAuth " + token;
    headers = curl_slist_append(headers, authorization.c_str());
    CURL *curl;
    CURLcode result;
 
    // Create our curl handle
    curl = curl_easy_init();
 
    if (curl)
    {
      // Now set up all of the curl options
      curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);
      curl_easy_setopt(curl, CURLOPT_URL, url.c_str() );
      curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
      curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
      if (post)
      {
          curl_easy_setopt(curl, CURLOPT_POST, 1);
          curl_easy_setopt(curl, CURLOPT_POSTFIELDS,post_param_string.c_str());
      }
      curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2);
      curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/4.0 (compatible; MSIE 5.01; Windows NT 5.0)");
      curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);  // this line makes it work under https
      curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "cookies.txt");//read from
      curl_easy_setopt(curl, CURLOPT_COOKIEJAR, "cookies.txt"); //write to
 
      // Attempt to retrieve the remote page
      result = curl_easy_perform(curl);
 
      // Always cleanup
      curl_easy_cleanup(curl);
 
      // Did we succeed?
      if (result == CURLE_OK)
      {
          return buffer;
      }
      else
      {
          cerr << "error:" << errorBuffer <<endl;
          return "";
      }
    }
 
    return "";
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
int Curl::writer(char *data, size_t size, size_t nmemb, string *buffer)
{
  int result = 0;
  if (buffer != nullptr)
  {
    buffer->append(data, size * nmemb);
    result = size * nmemb;
  }
  return result;
}
//---------------------------------------------------------------------------
string Curl::urlencode(const string &c)
{
 
    string escaped="";
    int max = c.length();
    for(int i=0; i<max; i++)
    {
        if ( (48 <= c[i] && c[i] <= 57) ||//0-9
             (65 <= c[i] && c[i] <= 90) ||//abc...xyz
             (97 <= c[i] && c[i] <= 122) || //ABC...XYZ
             (c[i]=='~' || c[i]=='!' || c[i]=='*' || c[i]=='(' || c[i]==')' || c[i]=='\'') //~!*()'
        )
        {
            escaped.append( &c[i], 1);
        }
        else
        {
            escaped.append("%");
            escaped.append( char2hex(c[i]) );//converts char 255 to string "ff"
        }
    }
    return escaped;
}
//-----------------------------------------------------------------------------
string Curl::char2hex( char dec )
{
    char dig1 = (dec&0xF0)>>4;
    char dig2 = (dec&0x0F);
    if ( 0<= dig1 && dig1<= 9) dig1+=48;    //0,48inascii
    if (10<= dig1 && dig1<=15) dig1+=97-10; //a,97inascii
    if ( 0<= dig2 && dig2<= 9) dig2+=48;
    if (10<= dig2 && dig2<=15) dig2+=97-10;
 
    string r;
    r.append( &dig1, 1);
    r.append( &dig2, 1);
    return r;
}
//-----------------------------------------------------------------------------

void Curl::setToken(const string & token_)
{
    Curl::token = token_;
}
