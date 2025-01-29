#include <gtest/gtest.h>

#include <YandexMusic/Request.h>
#include <Common/Logging.h>

int main(int argc, char **argv)
{
	yandex_music::Request::processConfig();
	multi_sink_example(yandex_music::User::log_folder + "/multisink.txt");

	::testing::InitGoogleTest(&argc, argv);

	return RUN_ALL_TESTS();
}