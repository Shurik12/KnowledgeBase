#pragma once

#include <map>
#include <set>
#include <string>
#include <filesystem>
#include <functional>
#include <memory>

#include <Common/httplib.h>
#include <YandexMusic/User.h>
#include <Databases/PostgreSQL.h>

class KnowledgeBaseServer
{
public:
    explicit KnowledgeBaseServer();
    ~KnowledgeBaseServer();

    KnowledgeBaseServer(const KnowledgeBaseServer &) = delete;
    KnowledgeBaseServer &operator=(const KnowledgeBaseServer &) = delete;
    KnowledgeBaseServer(KnowledgeBaseServer &&) = delete;
    KnowledgeBaseServer &operator=(KnowledgeBaseServer &&) = delete;

    void initialize();
    void start();
    void stop() noexcept;

private:
    httplib::Server svr_;
    YandexMusic::User user_;
    PostgreSQL postgres_;
    std::map<int, std::set<std::string>> playlists_map_;
    std::string user_id_;
    std::filesystem::path static_files_root_ = "../frontend";
    std::filesystem::path log_folder_;

    void loadConfiguration();
    void initializeDatabase();
    void initializeLogging();
    void setupRoutes();
    void loadPlaylistsMap();

    // Route handlers
    void handleRoot(const httplib::Request &req, httplib::Response &res);
    void handleStaticFile(std::string_view path, const httplib::Request &req, httplib::Response &res);
    void handleMediaFile(std::string_view filename, const httplib::Request &req, httplib::Response &res);
    void handleCliCommand(int command_id, httplib::Response &res);
    void handleRegister(const rapidjson::Document &document, httplib::Response &res);
    void handleLogin(const rapidjson::Document &document, httplib::Response &res);
    void handleLogout(httplib::Response &res);
    void handleCategory(std::string_view category, httplib::Response &res);
    void handleCategories(httplib::Response &res);

    // Helper functions
    static void validateJsonDocument(const rapidjson::Document &doc);
    static std::string buildCategoriesJson(const std::vector<std::string> &categories);
};