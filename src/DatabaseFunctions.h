#pragma once

#include <string>
#include "DataTypes.h"

namespace plugin::DatabaseFunctions {
    extern const std::string DATABASE_PATH;
    extern const std::string IMPORT_FOLDER_PATH;

    std::future<void> UpdateLoadOrderAsync();
    void InitializeDatabase();
    plugin::DataTypes::LocationLookupResult GetCellDescription(const std::string& form_id);
    plugin::DataTypes::LocationLookupResult GetLocationDescription(const std::string& editor_id);
    plugin::DataTypes::LocationLookupResult GetWorldspaceDescription(const std::string& editor_id);
    RE::BSFixedString GetQuestDescription(RE::StaticFunctionTag*, std::string quest_eid);
    RE::BSFixedString GetStageDescription(RE::StaticFunctionTag*, std::string quest_eid, int stage);
    RE::BSFixedString GetObjectiveDescription(RE::StaticFunctionTag*, std::string quest_eid, int objective);
    RE::BSFixedString GetSceneDescription(RE::StaticFunctionTag*, std::string scene_eid, int phase, bool exactMatch = false);

    std::future<void> ImportDataFromFileAsync(const std::string& filename);
    std::future<void> ImportDataFromFolderAsync(const std::string& folderPath);

    void AddPlaceholderCell(const std::string& mod_name, const std::string& form_id);
    void AddPlaceholderLocation(const std::string& mod_name, const std::string& editor_id);
    void AddPlaceholderWorldspace(const std::string& mod_name, const std::string& editor_id);
}  // namespace plugin::DatabaseFunctions
