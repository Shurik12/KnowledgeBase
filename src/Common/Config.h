#pragma once

#include <string>
#include <filesystem>
#include <optional>
#include <memory>
#include "tinyxml2.h"

namespace Common
{

    class Config
    {
    public:
        // Delete copy/move operations for singleton-like behavior
        Config(const Config &) = delete;
        Config &operator=(const Config &) = delete;
        Config(Config &&) = delete;
        Config &operator=(Config &&) = delete;

        // Static accessor for singleton instance
        static Config &instance();

        // Configuration loading
        bool loadFromFile(const std::filesystem::path &configPath = "config.xml");
        bool setupApplicationEnvironment();

        // Getters with const correctness
        [[nodiscard]] const std::string &token() const noexcept { return token_; }
        [[nodiscard]] const std::filesystem::path &outputFolder() const noexcept { return output_folder_; }
        [[nodiscard]] const std::filesystem::path &logFolder() const noexcept { return log_folder_; }

        // Status checks
        [[nodiscard]] bool isConfigured() const noexcept { return configured_; }

    private:
        Config() = default;
        ~Config() = default;

        // Helper methods
        bool parseConfigFile(const std::filesystem::path &filename);
        bool createDirectories() const;

        // Configuration data
        std::string token_;
        std::filesystem::path output_folder_;
        std::filesystem::path log_folder_;
        bool configured_ = false;
    };

} // namespace Common