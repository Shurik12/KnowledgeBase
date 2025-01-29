#include <hello_test.h>

// Demonstrate some basic assertions.
TEST(HelloTest, BasicAssertions)
{
	// Expect two strings not to be equal.
	EXPECT_STRNE("hello", "world");
	// Expect equality.
	EXPECT_EQ(7 * 6, 42);
}

TEST_F(YandexMusicTest, PrintUserPlaylists)
{
	user.getUserPlaylists();
	user.printUserPlaylists();
}

TEST_F(YandexMusicTest, TracksWithoutPlaylist)
{
	user.getTracksWithoutPlaylist();
}