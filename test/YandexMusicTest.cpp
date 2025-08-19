#include "YandexMusicTest.h"
#include <spdlog/spdlog.h>

TEST_F(YandexMusicTest, GetUserPlaylistsBasic)
{
    // Test basic functionality
    user.fetchPlaylists();

    // Verify we got some playlists
    EXPECT_FALSE(user.playlists().empty()) << "User should have at least some playlists";

    // Log some info for debugging
    spdlog::info("Found {} playlists for user {}", user.playlists().size(), user_id);
}

TEST_F(YandexMusicTest, PlaylistOperations)
{
    // Create a test playlist
    auto playlist = user.createPlaylist(test_playlist_name);

    // Verify it appears in the playlist list
    user.fetchPlaylists();
    bool found = false;
    for (const auto &pl : user.playlists())
    {
        if (pl.title() == test_playlist_name)
        {
            found = true;
            break;
        }
    }
    EXPECT_TRUE(found) << "Newly created playlist should be in the list";

    // Clean up
    user.deletePlaylist(playlist.kind());
}