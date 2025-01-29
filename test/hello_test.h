#include <gtest/gtest.h>
#include <Common/Logging.h>
#include <YandexMusic/Request.h>

class YandexMusicTest : public testing::Test
{
public:
	// Creates a counter that starts at 0.
	YandexMusicTest()
	{
		yandex_music::Request request{};
		user = request.getUser();
		user_id = user.getId();
	}
	~YandexMusicTest() = default;

	yandex_music::User user;
	std::string user_id;
};
