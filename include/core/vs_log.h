#pragma once

#include <spdlog/sinks/ostream_sink.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <algorithm>
#include <map>
#include <memory>
#include <sstream>
#include <filesystem>
#include <string>

struct VSLog
{
    enum class Category
    {
        Core,
        Shader,
        Generation,
    };

    using Level = spdlog::level::level_enum;

    static void init(std::ostringstream& logStream)
    {
        // TODO spdlog 1.5 will do that for us
        std::filesystem::create_directory("logs");

        auto file_sink =
            std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/voxelscape.log", true);

        auto ostream_sink = std::make_shared<spdlog::sinks::ostream_sink_mt>(logStream);
        const std::vector<spdlog::sink_ptr> sinks = {ostream_sink, file_sink};

        // const auto longestCategoryName = std::max_element(
        //     categoryNames.begin(), categoryNames.end(), [](const auto& a, const auto& b) {
        //         return a.second.size() < b.second.size();
        //     });

        // const auto formatPattern = std::string("[%Y-%m-%d %H:%M:%S:%e] [%=") +
        //                            std::to_string(longestCategoryName->second.size()) +
        //                            std::string("n] %v");

        for (const auto& categoryNamePair : categoryNames)
        {
            auto logger =
                std::make_unique<spdlog::logger>(categoryNamePair.second, begin(sinks), end(sinks));
            loggers.insert({categoryNamePair.first, std::move(logger)});
        };
    };

    template <typename... Args>
    static void Log(Category category, Level level, const char* fmt, const Args&... args)
    {
        auto* logger = loggers.at(category).get();
        logger->log(level, fmt, args...);
    };

private:
    inline static const std::map<Category, std::string> categoryNames = {
        {Category::Core, "Core"},
        {Category::Shader, "Shader"},
        {Category::Generation, "Generation"}};

    inline static std::map<Category, std::unique_ptr<spdlog::logger>> loggers = {};
};