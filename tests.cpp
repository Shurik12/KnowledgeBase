#include <gtest/gtest.h>
#include "Common/Config.h"
#include "Common/Logging.h"
#include <YandexMusic/Request.h>

int main(int argc, char **argv)
{
    // Get the config instance and load configuration
    auto &config = Common::Config::instance();
    if (!config.loadFromFile())
    {
        std::cerr << "Failed to load application configuration" << std::endl;
        return -1;
    }

    // Setup application environment
    if (!config.setupApplicationEnvironment())
    {
        std::cerr << "Failed to setup application environment" << std::endl;
        return -1;
    }

    // Initialize logging
    try
    {
        Common::initializeLogger();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Failed to initialize logger: " << e.what() << std::endl;
        return -1;
    }
    catch (...)
    {
        std::cerr << "Failed to initialize logger (unknown error)" << std::endl;
        return -1;
    }

    // Initialize Google Test
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}