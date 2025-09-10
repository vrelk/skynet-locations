#pragma once

#include <string>

namespace plugin {
    extern const std::string DATABASE_PATH;
    extern const std::string IMPORT_FOLDER_PATH;

    void InitializeDatabase();
    std::string SelectValue(const std::string& key);
    RE::BSFixedString GetQuestDescription(RE::StaticFunctionTag*, std::string quest_eid);
    RE::BSFixedString GetStageDescription(RE::StaticFunctionTag*, std::string quest_eid, int stage);
    RE::BSFixedString GetObjectiveDescription(RE::StaticFunctionTag*, std::string quest_eid, int objective);
    std::future<void> ImportDataFromFileAsync(const std::string& filename);
    std::future<void> ImportDataFromFolderAsync(const std::string& folderPath);
}  // namespace plugin
