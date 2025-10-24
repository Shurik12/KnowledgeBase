#include <iostream>
#include <Server/KnowledgeBaseServer.h>

using namespace httplib;

int main()
{
    try
    {
        KnowledgeBaseServer server;
        server.start();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Server error: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}
