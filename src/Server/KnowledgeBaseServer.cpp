#include <filesystem>
#include <fstream>
#include <vector>
#include <utility>

#include <spdlog/spdlog.h>
#include <fmt/format.h>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/error/en.h>

#include <Server/KnowledgeBaseServer.h>
#include <Common/Config.h>
#include <Common/Logging.h>
#include <YandexMusic/Request.h>

namespace fs = std::filesystem;

KnowledgeBaseServer::KnowledgeBaseServer()
	: postgres_(
		  std::getenv("DB_NAME"),
		  std::getenv("DB_USER"),
		  std::getenv("DB_PASSWORD"),
		  std::getenv("DB_HOST"),
		  std::getenv("DB_PORT"))
{
	initialize();
	YandexMusic::Request request;
	user_ = YandexMusic::User(request.getUser());
	user_id_ = user_.id();
}

KnowledgeBaseServer::~KnowledgeBaseServer()
{
	try
	{
		stop();
	}
	catch (...)
	{
		spdlog::error("Exception during server shutdown");
	}
}

void KnowledgeBaseServer::initialize()
{
	loadConfiguration();
	initializeLogging();
	loadPlaylistsMap();
	initializeDatabase();
	setupRoutes();
}

void KnowledgeBaseServer::start()
{
	svr_.set_logger([](const auto &req, const auto &res)
					{ Common::log_request_response(req, res); });

	spdlog::info("Starting server on 0.0.0.0:8080");
	if (!svr_.listen("0.0.0.0", 8080))
	{
		throw std::runtime_error("Failed to start server");
	}
}

void KnowledgeBaseServer::stop() noexcept
{
	try
	{
		svr_.stop();
	}
	catch (const std::exception &e)
	{
		spdlog::error("Error stopping server: {}", e.what());
	}
}

void KnowledgeBaseServer::loadConfiguration()
{
	// Get the config instance
	auto &config = Common::Config::instance();

	// Load configuration from file
	if (!config.loadFromFile())
	{
		throw std::runtime_error("Failed to load application configuration");
	}

	// Setup application environment
	if (!config.setupApplicationEnvironment())
	{
		throw std::runtime_error("Failed to setup application environment");
	}

	// Get the configured paths
	log_folder_ = config.logFolder();
	static_files_root_ = "../frontend";
}

void KnowledgeBaseServer::initializeLogging()
{
	try
	{
		Common::multi_sink_example((log_folder_ / "multisink.log").string());
	}
	catch (const std::exception &e)
	{
		throw std::runtime_error(fmt::format("Failed to initialize logger: {}", e.what()));
	}
}

void KnowledgeBaseServer::loadPlaylistsMap()
{
	const fs::path playlists_map_path = "playlists_map.json";
	if (!fs::exists(playlists_map_path))
	{
		spdlog::error("Playlists map file not found: {}", playlists_map_path.string());
		return;
	}

	std::ifstream ifs{playlists_map_path};
	rapidjson::IStreamWrapper isw{ifs};
	rapidjson::Document document;
	document.ParseStream(isw);

	if (document.HasParseError())
	{
		spdlog::error("JSON parse error: {}", rapidjson::GetParseError_En(document.GetParseError()));
		return;
	}

	if (!document.IsObject())
	{
		spdlog::error("Invalid playlists_map.json format - expected object");
		return;
	}

	for (const auto &playlist_entry : document.GetObject())
	{
		if (!playlist_entry.value.IsObject())
			continue;

		int kind = 0;
		std::set<std::string> authors;

		for (const auto &field : playlist_entry.value.GetObject())
		{
			const std::string_view name{field.name.GetString()};

			if (name == "kind" && field.value.IsInt())
			{
				kind = field.value.GetInt();
			}
			else if (name == "authors" && field.value.IsArray())
			{
				for (const auto &author : field.value.GetArray())
				{
					if (author.IsString())
					{
						authors.insert(author.GetString());
					}
				}
			}
		}

		if (kind != 0)
		{
			playlists_map_.emplace(kind, std::move(authors));
		}
	}
}

void KnowledgeBaseServer::initializeDatabase()
{
	try
	{
		postgres_.createTables();
		spdlog::info("Database initialized successfully");
	}
	catch (const std::exception &e)
	{
		spdlog::critical("Database initialization failed: {}", e.what());
		throw;
	}
}

void KnowledgeBaseServer::setupRoutes()
{
	// Basic routes
	svr_.Get("/hi", [](const httplib::Request &, httplib::Response &res)
			 { res.set_content("Hello World!", "text/plain"); });

	svr_.Get("/", [this](const httplib::Request &req, httplib::Response &res)
			 { handleRoot(req, res); });

	// Static files
	svr_.Get("/public/main.js", [this](const httplib::Request &req, httplib::Response &res)
			 { handleStaticFile("public/main.js", req, res); });

	svr_.Get("/public/reactPlayerFilePlayer.js", [this](const httplib::Request &req, httplib::Response &res)
			 { handleStaticFile("public/reactPlayerFilePlayer.js", req, res); });

	svr_.Get("/src/index.js", [this](const httplib::Request &req, httplib::Response &res)
			 { handleStaticFile("src/index.js", req, res); });

	// Media files
	svr_.Get("/mediafiles/:filename", [this](const httplib::Request &req, httplib::Response &res)
			 { handleMediaFile(req.path_params.at("filename"), req, res); });

	// CLI commands
	svr_.Get("/cli/:command_id", [this](const httplib::Request &req, httplib::Response &res)
			 {
        try {
            const int command_id = std::stoi(req.path_params.at("command_id"));
            handleCliCommand(command_id, res);
        } catch (const std::exception& e) {
            res.status = 400;
            res.set_content(fmt::format("Invalid command_id: {}", e.what()), "text/plain");
        } });

	svr_.Get("/music/", [this](const httplib::Request &, httplib::Response &res)
			 { handleMusicRoot(res); });

	// Authentication
	svr_.Post("/music/register", [this](const httplib::Request &req, httplib::Response &res)
			  {
        rapidjson::Document document;
        document.Parse(req.body.c_str());

        if (document.HasParseError()) {
            res.status = 400;
            res.set_content("{\"error\": \"Invalid JSON\"}", "application/json");
            return;
        }

        handleRegister(document, res); });

	svr_.Post("/music/login", [this](const httplib::Request &req, httplib::Response &res)
			  {
        rapidjson::Document document;
        document.Parse(req.body.c_str());

        if (document.HasParseError()) {
            res.status = 400;
            res.set_content("{\"error\": \"Invalid JSON\"}", "application/json");
            return;
        }

        handleLogin(document, res); });

	svr_.Post("/music/logout", [this](const httplib::Request &, httplib::Response &res)
			  { handleLogout(res); });

	// Music categories
	svr_.Get("/music/category/:category", [this](const httplib::Request &req, httplib::Response &res)
			 { handleCategory(req.path_params.at("category"), res); });

	svr_.Get("/music/categories", [this](const httplib::Request &, httplib::Response &res)
			 { handleCategories(res); });

	// Stop endpoint
	svr_.Get("/stop", [this](const httplib::Request &, httplib::Response &)
			 { stop(); });

	// Catch-all route for client-side routing - MUST BE LAST
	svr_.Get(".*", [this](const httplib::Request &req, httplib::Response &res)
			 { handleClientSideRouting(req, res); });
}

void KnowledgeBaseServer::handleRoot(const httplib::Request &req, httplib::Response &res)
{
	handleStaticFile("index.html", req, res);
}

void KnowledgeBaseServer::handleStaticFile(std::string_view path, const httplib::Request & /*req*/, httplib::Response &res)
{
	const auto full_path = static_files_root_ / path;

	if (!fs::exists(full_path))
	{
		spdlog::error("File not found: {}", full_path.string());
		res.status = 404;
		return;
	}

	res.set_file_content(full_path.string());
}

void KnowledgeBaseServer::handleMediaFile(std::string_view filename, const httplib::Request &, httplib::Response &res)
{
	const auto path = static_files_root_ / "mediafiles" / filename;

	std::ifstream file(path, std::ios::binary | std::ios::ate);
	if (!file)
	{
		spdlog::error("Media file not found: {}", path.string());
		res.status = 404;
		return;
	}

	const auto size = file.tellg();
	file.seekg(0, std::ios::beg);

	std::vector<char> buffer(size);
	if (file.read(buffer.data(), size))
	{
		res.set_content(buffer.data(), size, "audio/mpeg");
	}
	else
	{
		spdlog::error("Failed to read file: {}", path.string());
		res.status = 500;
	}
}

void KnowledgeBaseServer::handleCliCommand(int command_id, httplib::Response &res)
{
	std::vector<YandexMusic::Track> add_tracks;
	YandexMusic::Playlist playlist;

	switch (command_id)
	{
	case 1: // Print playlists
		user_.fetchPlaylists();
		user_.printPlaylists();
		break;
	case 2: // Get tracks without playlist
		user_.analyzeTracksWithoutPlaylists();
		break;
	case 3: // Create, change and delete playlist
		playlist = user_.createPlaylist("Test3");
		user_.renamePlaylist(playlist.kind(), "Test4");
		user_.fetchLikedTracks();
		playlist.addTracks(user_.likedTracks());
		user_.deletePlaylist(playlist.kind());
		break;
	case 4: // Print track
		user_.fetchPlaylists();
		playlist = user_.playlists()[4];
		playlist.fetchTracks();
		playlist.tracks()[0].print();
		break;
	case 5: // Download playlist
		user_.fetchPlaylists();
		user_.playlists()[7].download();
		break;
	case 6: // Download all playlists
		user_.fetchPlaylists();
		for (auto &pl : user_.playlists())
		{
			pl.download();
		}
		break;
	case 7: // Download playlist Like
		user_.fetchLikedTracks();
		playlist = YandexMusic::Playlist("Like", 0, 0, "", 0, user_id_);
		playlist.setTracks(user_.likedTracks());
		playlist.download();
		break;
	case 8: // Add tracks to playlist
		if (!playlists_map_.empty())
		{
			user_.analyzeTracksWithoutPlaylists();
			for (const auto &[kind, authors] : playlists_map_)
			{
				playlist = user_.fetchPlaylist(kind);
				for (const auto &track : user_.tracksWithoutPlaylists())
				{
					if (const auto &artists = track.artists(); !artists.empty() && authors.count(artists[0]) > 0)
					{
						add_tracks.push_back(track);
					}
				}
				playlist.addTracks(add_tracks);
				add_tracks.clear();
			}
		}
		break;
	case 9: // Remove all tracks from playlist
		if (!playlists_map_.empty())
		{
			for (const auto &[kind, _] : playlists_map_)
			{
				playlist = user_.fetchPlaylist(kind);
				playlist.fetchTracks();
				if (!playlist.tracks().empty())
				{
					playlist.removeTracks(playlist.tracks());
				}
			}
		}
		break;
	default:
		spdlog::info("No action for command_id {}", command_id);
	}

	res.set_content(fmt::format("Done {}!", command_id), "text/plain");
}

void KnowledgeBaseServer::validateJsonDocument(const rapidjson::Document &doc)
{
	if (!doc.IsObject())
	{
		throw std::runtime_error("Expected JSON object");
	}
}

void KnowledgeBaseServer::handleRegister(const rapidjson::Document &document, httplib::Response &res)
{
	try
	{
		validateJsonDocument(document);

		if (!document.HasMember("username") || !document["username"].IsString() ||
			!document.HasMember("password") || !document["password"].IsString() ||
			!document.HasMember("confirmation") || !document["confirmation"].IsString() ||
			!document.HasMember("email") || !document["email"].IsString())
		{
			throw std::runtime_error("Missing required fields");
		}

		const std::string username = document["username"].GetString();
		const std::string password = document["password"].GetString();
		const std::string confirmation = document["confirmation"].GetString();
		const std::string email = document["email"].GetString();

		if (password != confirmation)
		{
			res.set_content(
				R"({"username": "", "auth": false, "message": "Passwords do not match."})",
				"application/json");
			return;
		}

		const int insertion = postgres_.insertNewUser(username, email, password);
		if (insertion == 0)
		{
			res.set_content(
				fmt::format(R"({{"username": "{}", "auth": true, "message": ""}})", username),
				"application/json");
		}
		else
		{
			res.set_content(
				R"({"username": "", "auth": false, "message": "Username already taken."})",
				"application/json");
		}
	}
	catch (const std::exception &e)
	{
		res.status = 400;
		res.set_content(
			fmt::format(R"({{"error": "{}"}})", e.what()),
			"application/json");
	}
}

void KnowledgeBaseServer::handleLogin(const rapidjson::Document &document, httplib::Response &res)
{
	try
	{
		validateJsonDocument(document);

		if (!document.HasMember("username") || !document["username"].IsString() ||
			!document.HasMember("password") || !document["password"].IsString())
		{
			throw std::runtime_error("Missing required fields");
		}

		const std::string username = document["username"].GetString();
		const std::string password = document["password"].GetString();

		if (postgres_.searchUser(username, password))
		{
			res.set_content(
				fmt::format(R"({{"username": "{}", "auth": true, "message": ""}})", username),
				"application/json");
		}
		else
		{
			res.set_content(
				R"({"username": "", "auth": false, "message": "Invalid username and/or password."})",
				"application/json");
		}
	}
	catch (const std::exception &e)
	{
		res.status = 400;
		res.set_content(
			fmt::format(R"({{"error": "{}"}})", e.what()),
			"application/json");
	}
}

void KnowledgeBaseServer::handleLogout(httplib::Response &res)
{
	res.set_content("{}", "application/json");
}

void KnowledgeBaseServer::handleCategory(std::string_view category, httplib::Response &res)
{
	spdlog::info("Requested category: {}", category);

	try
	{
		std::string categoryStr(category);
		std::string tracks;

		// Check if the category exists and get tracks
		postgres_.getCategoryTracks(categoryStr, tracks);
		if (category.empty())
		{
			spdlog::warn("Category not found or empty: {}", category);

			// Return empty tracks array instead of failing
			res.set_content(R"({"tracks": []})", "application/json");
			return;
		}

		// Validate that tracks contains valid JSON
		rapidjson::Document doc;
		doc.Parse(tracks.c_str());

		if (doc.HasParseError())
		{
			spdlog::error("Invalid JSON from database for category: {}", category);
			res.status = 500;
			res.set_content(
				fmt::format(R"({{"error": "Server error processing category {}", "tracks": []}})", category),
				"application/json");
			return;
		}

		// Success - return the tracks
		res.set_content(tracks, "application/json");
	}
	catch (const std::exception &e)
	{
		spdlog::error("Exception in handleCategory for {}: {}", category, e.what());
		res.status = 500;
		res.set_content(
			fmt::format(R"({{"error": "Server error: {}", "tracks": []}})", e.what()),
			"application/json");
	}
}

std::string KnowledgeBaseServer::buildCategoriesJson(const std::vector<std::string> &categories)
{
	std::string content = R"({"categories": [)";
	for (const auto &category : categories)
	{
		content += fmt::format(R"({{"name": "{}"}},)", category);
	}
	if (!categories.empty())
	{
		content.pop_back(); // Remove trailing comma
	}
	content += "]}";
	return content;
}

void KnowledgeBaseServer::handleCategories(httplib::Response &res)
{
	try
	{
		std::vector<std::string> categories;
		postgres_.getCategories(categories);

		res.set_content(buildCategoriesJson(categories), "application/json");
	}
	catch (const std::exception &e)
	{
		spdlog::error("Exception in handleCategories: {}", e.what());
		res.status = 500;
		res.set_content(R"({"categories": []})", "application/json");
	}
}

void KnowledgeBaseServer::handleMusicRoot(httplib::Response &res)
{
	try
	{
		spdlog::info("Handling /music/ endpoint");

		// Check for session cookie or authentication token
		// This is a simplified example - you'd need proper session management

		// For now, check if there's an active session
		// You should implement proper session validation here

		std::string response;

		// Example: Check if user is authenticated via session
		// if (isUserAuthenticated(req)) {
		//   std::string username = getAuthenticatedUsername();
		//   response = fmt::format(R"({{"username": "{}", "auth": true, "message": ""}})", username);
		// } else {
		response = R"({"username": "", "auth": false, "message": ""})";
		// }

		// Set proper cache control headers
		res.set_header("Cache-Control", "no-cache, no-store, must-revalidate");
		res.set_header("Pragma", "no-cache");
		res.set_header("Expires", "0");

		res.set_content(response, "application/json");
	}
	catch (const std::exception &e)
	{
		spdlog::error("Exception in handleMusicRoot: {}", e.what());
		res.status = 500;
		res.set_content(R"({"username": "", "auth": false, "message": "Server error"})", "application/json");
	}
}

void KnowledgeBaseServer::handleClientSideRouting(const httplib::Request &req, httplib::Response &res)
{
	// List of API endpoints that should not serve index.html
	static const std::set<std::string> apiEndpoints = {
		"/music/", "/music/register", "/music/login", "/music/logout",
		"/music/category/", "/music/categories", "/music/shurik_music",
		"/mediafiles/", "/public/", "/src/", "/cli/"};

	// Check if this is an API endpoint
	for (const auto &endpoint : apiEndpoints)
	{
		if (req.path.find(endpoint) == 0)
		{
			// This is an API endpoint, return 404 for unknown API routes
			res.status = 404;
			res.set_content("Not Found", "text/plain");
			return;
		}
	}

	// Serve index.html for client-side routing
	const auto full_path = static_files_root_ / "index.html";

	if (!fs::exists(full_path))
	{
		spdlog::error("Index file not found: {}", full_path.string());
		res.status = 404;
		res.set_content("Page not found", "text/plain");
		return;
	}

	res.set_file_content(full_path.string());
}