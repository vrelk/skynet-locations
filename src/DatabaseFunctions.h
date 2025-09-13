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
    std::string GetQuestDescription(std::string quest_eid);
    std::string GetStageDescription(std::string quest_eid, int stage);
    std::string GetObjectiveDescription(std::string quest_eid, int objective);
    std::string GetSceneDescription(std::string scene_eid, int phase, bool exactMatch = false);

    std::future<void> ImportDataFromFileAsync(const std::string& filename);
    std::future<void> ImportDataFromFolderAsync(const std::string& folderPath);

    void AddPlaceholderCell(const std::string& mod_name, const std::string& form_id);
    void AddPlaceholderLocation(const std::string& mod_name, const std::string& editor_id);
    void AddPlaceholderWorldspace(const std::string& mod_name, const std::string& editor_id);
}  // namespace plugin::DatabaseFunctions
