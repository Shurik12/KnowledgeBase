#include "Common/Config.h"
#include "Common/Curl.h"
#include "YandexMusic/Playlist.h"
#include "YandexMusic/User.h"
#include <spdlog/spdlog.h>

namespace Common
{

    Config &Config::instance()
    {
        static Config instance;
        return instance;
    }

    bool Config::loadFromFile(const std::filesystem::path &configPath)
    {
        try
        {
            if (!parseConfigFile(configPath))
            {
                spdlog::error("Failed to parse config file: {}", configPath.string());
                return false;
            }

            if (!createDirectories())
            {
                spdlog::error("Failed to create configuration directories");
                return false;
            }

            configured_ = true;
            return true;
        }
        catch (const std::exception &e)
        {
            spdlog::error("Configuration error: {}", e.what());
            return false;
        }
    }

    bool Config::setupApplicationEnvironment()
    {
        if (!configured_)
        {
            spdlog::error("Configuration not loaded");
            return false;
        }

        try
        {
            Curl::setToken(token_);
            YandexMusic::Playlist::setOutputDirectory(output_folder_.string());
            return true;
        }
        catch (const std::exception &e)
        {
            spdlog::error("Failed to setup application environment: {}", e.what());
            return false;
        }
    }

    bool Config::parseConfigFile(const std::filesystem::path &filename)
    {
        tinyxml2::XMLDocument xml_doc;
        if (xml_doc.LoadFile(filename.string().c_str()) != tinyxml2::XML_SUCCESS)
        {
            spdlog::error("Failed to load config file: {}", filename.string());
            return false;
        }

        const tinyxml2::XMLNode *root = xml_doc.FirstChild();
        if (!root)
        {
            spdlog::error("Empty config file: {}", filename.string());
            return false;
        }

        const auto *token_elem = root->FirstChildElement("token");
        const auto *output_elem = root->FirstChildElement("output_folder");
        const auto *log_elem = root->FirstChildElement("log_folder");

        if (!token_elem || !output_elem || !log_elem)
        {
            spdlog::error("Missing required elements in config file");
            return false;
        }

        token_ = token_elem->GetText() ? token_elem->GetText() : "";
        output_folder_ = output_elem->GetText() ? output_elem->GetText() : "";
        log_folder_ = log_elem->GetText() ? log_elem->GetText() : "";

        if (token_.empty() || output_folder_.empty() || log_folder_.empty())
        {
            spdlog::error("Empty configuration values detected");
            return false;
        }

        return true;
    }

    bool Config::createDirectories() const
    {
        try
        {
            std::filesystem::create_directories(output_folder_);
            std::filesystem::create_directories(log_folder_);
            return true;
        }
        catch (const std::filesystem::filesystem_error &e)
        {
            spdlog::error("Filesystem error creating directories: {}", e.what());
            return false;
        }
    }

} // namespace Common