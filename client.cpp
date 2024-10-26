#include <fmt/format.h>

#include <Common/httplib.h>
using namespace httplib;

using namespace std;

int main(void)
{
    std::string s;

    /// Command line interface
    std::cout << "Menu:\n";
    std::cout << "\t1. Print playlists\n";
    std::cout << "\t2. Get tracks without playlist\n";
    std::cout << "\t3. Create, change and delete playlist\n";
    std::cout << "\t4. Print track\n";
    std::cout << "\t5. Download playlist\n";
    std::cout << "\t6. Download all playlists\n";

    httplib::Client cli("localhost", 8080);

    while (true)
    {
        std::cout << "Input the number of one of the options above: ";
        std::cin >> s;
        if (auto res = cli.Get(fmt::format("/cli/{}", s)))
        {
            if (res->status == StatusCode::OK_200)
            {
                std::cout << res->body << std::endl;
            }
        }
        else
        {
            auto err = res.error();
            std::cout << "HTTP error: " << httplib::to_string(err) << std::endl;
            break;
        }
    }
}