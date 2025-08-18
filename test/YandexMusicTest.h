#pragma once

#include <gtest/gtest.h>
#include <Common/Logging.h>
#include <Common/Config.h>
#include <YandexMusic/Request.h>
#include <YandexMusic/Playlist.h>
#include <YandexMusic/Track.h>

class YandexMusicTest : public testing::Test
{
protected:
    void SetUp() override
    {
        try
        {
            // Initialize configuration using new interface
            auto &config = Common::Config::instance();
            if (!config.loadFromFile())
            {
                FAIL() << "Failed to load application configuration";
            }
            if (!config.setupApplicationEnvironment())
            {
                FAIL() << "Failed to setup application environment";
            }

            YandexMusic::Request request{};
            user = request.getUser();
            user_id = user.id();

            // Initialize test data
            test_playlist_name = "Test Playlist " + std::to_string(rand() % 1000);
        }
        catch (const std::exception &e)
        {
            FAIL() << "Setup failed: " << e.what();
        }
    }

    void TearDown() override
    {
        // Cleanup if needed
    }

    // Helper method to count tracks in all playlists
    size_t countAllPlaylistTracks()
    {
        size_t total = 0;
        for (const auto &playlist : user.playlists())
        {
            total += playlist.trackCount();
        }
        return total;
    }

    YandexMusic::User user;
    std::string user_id;
    std::string test_playlist_name;
};