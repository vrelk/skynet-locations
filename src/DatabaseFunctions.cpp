#include <SQLiteCpp/SQLiteCpp.h>
#include <SQLiteCpp/VariadicBind.h>
#include <filesystem>
#include "DatabaseFunctions.h"
#include <future>  // Required for async operations
#include "JSON.hpp"
#include "DataTypes.h"

/*
{
    "cells": {
        "Skyrim.esm": {
            "0x1A9B2": {
                "name": "Whiterun",
                "description": "Something here"
            }
        }
    },
    "worldspaces": {
        "Skyrim.esm": {
            "Tamriel": {
                "name": "Tamriel",
                "description": "Something here"
            }
        }
    },
    "locations":{
        "Skyrim.esm": {
            "WhiterunLocation": {
                "name": "Whiterun",
                "description": "Something here"
            }
        }
    },
	"quests": {
		"MQ101": {
			"name": "Unbound",
			"description": "Something here",
			"stages": {
				"10": "Some description for stage 10."
            },
			"objectives": {
				"10": "Objective description for objective 10."
            }
		}
    }
}
*/

// Using SQLiteStudio to draft the schema
// need to create a cells and worldspaces table.
// determine best way to key them. FormID or EditorID, or both?

namespace plugin::DatabaseFunctions {
    const std::string DATABASE_PATH = "Data/SKSE/Plugins/vrelk/VrelkDb.db";
    const std::string IMPORT_FOLDER_PATH = "Data/SKSE/Plugins/vrelk/import";

    // MARK: - UpdateLoadOrderAsync
    std::future<void> UpdateLoadOrderAsync() {
        return std::async(std::launch::async, []() {
            try {
                SQLite::Database db(DATABASE_PATH, SQLite::OPEN_READWRITE);

                auto dataHandler = RE::TESDataHandler::GetSingleton();
                if (!dataHandler) {
                    // Handle null case
                }

                SQLite::Statement query(db, R"(
                    INSERT OR REPLACE INTO load_order (priority, mod_name, is_light)
                    VALUES (?, ?, ?)
                )");

                int sequentialIndex = 0;
                auto& files = dataHandler->files;

                for (auto it = files.begin(); it != files.end(); ++it) {
                    RE::TESFile* mod = *it;
                    if (!mod) {
                        continue;
                    }

                    std::string_view nameView = mod->GetFilename();
                    std::string name(nameView);  // Convert to std::string
                    bool isLight = mod->IsLight();

                    query.bind(1, sequentialIndex++);
                    query.bind(2, name);
                    query.bind(3, isLight ? 1 : 0);
                    query.exec();
                    query.reset();  // Reset for next iteration
                }

                logger::info("Load order updated successfully.");
            } catch (const std::exception& e) {
                logger::error("Failed to execute update load order: {}", e.what());
            }
        });
    }

    // MARK: - InitializeDatabase
    void InitializeDatabase() {
        try {
            // Ensure the database directory exists
            std::filesystem::create_directories(std::filesystem::path(DATABASE_PATH).parent_path());

            // Open or create the database
            SQLite::Database db(DATABASE_PATH, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);

            // Enable foreign key constraints
            db.exec("PRAGMA foreign_keys = ON;");

            // Create the load_order table
            db.exec(R"(
                CREATE TABLE IF NOT EXISTS load_order (
                    priority INTEGER        PRIMARY KEY
                                            NOT NULL,
                    mod_name TEXT           NOT NULL,
                    is_light INTEGER (1, 0) NOT NULL
                                            CHECK (is_light IN (0, 1) )
                                            DEFAULT (0)
                );
            )");

            // Create the description_cell table
            db.exec(R"(
                CREATE TABLE description_cell (
                    mod_name    TEXT     NOT NULL
                                        COLLATE NOCASE
                                        CHECK (mod_name = TRIM(mod_name) ) 
                                        DEFAULT ('Skyrim.esm'),
                    form_id     TEXT (8) NOT NULL
                                        CHECK (form_id LIKE '0x%' AND
                                                LENGTH(form_id) BETWEEN 3 AND 8) 
                                        COLLATE NOCASE,
                    name        TEXT,
                    description TEXT,
                    notes       TEXT,
                    PRIMARY KEY (
                        mod_name,
                        form_id
                    )
                );
            )");

            // Create the description_location table
            db.exec(R"(
                CREATE TABLE IF NOT EXISTS description_location (
                    mod_name    TEXT     NOT NULL
                                        COLLATE NOCASE
                                        CHECK (mod_name = TRIM(mod_name) )
                                        DEFAULT ('Skyrim.esm'),
                    editor_id    TEXT NOT NULL
                                    COLLATE NOCASE,
                    name         TEXT,
                    description  TEXT,
                    notes        TEXT,
                    PRIMARY KEY (
                        mod_name,
                        editor_id
                    )
                );
            )");

            // Create the description_worldspace table
            db.exec(R"(
                CREATE TABLE IF NOT EXISTS description_worldspace (
                    mod_name    TEXT     NOT NULL
                                        COLLATE NOCASE
                                        CHECK (mod_name = TRIM(mod_name) )
                                        DEFAULT ('Skyrim.esm'),
                    editor_id    TEXT NOT NULL
                                    COLLATE NOCASE,
                    name         TEXT,
                    description  TEXT,
                    notes        TEXT,
                    PRIMARY KEY (
                        mod_name,
                        editor_id
                    )
                );
            )");

            // Create the quests table
            db.exec(R"(
                CREATE TABLE IF NOT EXISTS quests (
                    quest_eid   TEXT NOT NULL
                                    COLLATE NOCASE,
                    name        TEXT NULL,
                    description TEXT NULL,
                    notes       TEXT NULL,
                    PRIMARY KEY (
                        quest_eid
                    )
                );
            )");

            // Create the stages table with a foreign key constraint
            db.exec(R"(
                CREATE TABLE IF NOT EXISTS stages (
                    quest_eid   TEXT NOT NULL
                                    COLLATE NOCASE,
                    stage       INT  NOT NULL,
                    description TEXT NULL,
                    notes       TEXT NULL,
                    PRIMARY KEY (
                        quest_eid,
                        stage
                    ),
                    FOREIGN KEY (
                        quest_eid
                    )
                    REFERENCES quests (quest_eid) ON DELETE RESTRICT
                                                ON UPDATE CASCADE
                );
            )");

            // Create the objectives table with a foreign key constraint
            db.exec(R"(
                CREATE TABLE IF NOT EXISTS objectives (
                    quest_eid   TEXT NOT NULL
                                    COLLATE NOCASE,
                    objective   INT  NOT NULL,
                    description TEXT NULL,
                    notes       TEXT NULL,
                    PRIMARY KEY (
                        quest_eid,
                        objective
                    ),
                    FOREIGN KEY (
                        quest_eid
                    )
                    REFERENCES quests (quest_eid) ON DELETE RESTRICT
                                                ON UPDATE CASCADE
                );
            )");

            // Create the scenes table
            db.exec(R"(
                CREATE TABLE scenes (
                    scene_eid   TEXT NOT NULL
                                    COLLATE NOCASE,
                    phase       INT  NOT NULL,
                    description TEXT NULL,
                    notes       TEXT NULL,
                    PRIMARY KEY (
                        scene_eid,
                        phase
                    )
                );
            )");

            // Clear existing load order data
            db.exec(R"(DELETE FROM load_order;)");

            // Asynchronously update the load order
            auto future = UpdateLoadOrderAsync();
            // Optionally wait for completion
            //future.get();

            logger::info("Database initialized successfully.");
        } catch (const std::exception& e) {
            logger::error("Failed to initialize database: {}", e.what());
        }
    }

    /**
     * @brief Retrieves the name and description of a cell based on the provided form ID.
     *
     * @param form_id The form ID of the cell as a string.
     * @return types::LocationLookupResult A structure containing the lookup result.
     */
    // MARK: - GetCellDescription
    plugin::DataTypes::LocationLookupResult GetCellDescription(const std::string& form_id) {
        try {
            SQLite::Database db(DATABASE_PATH, SQLite::OPEN_READWRITE);

            SQLite::Statement query(db, R"(
                SELECT 
                    dc.name,
                    dc.description
                FROM description_cell dc
                INNER JOIN load_order lo ON dc.mod_name = lo.mod_name
                WHERE dc.form_id = ?
                ORDER BY lo.priority ASC
                LIMIT 1;
            )");

            query.bind(1, form_id);

            if (query.executeStep()) {
                // Retrieve the name and description from the query result
                std::string name = query.getColumn(0).isNull() ? "" : query.getColumn(0).getString();
                std::string description = query.getColumn(1).isNull() ? "" : query.getColumn(1).getString();

                return {.found = true, .name = name, .description = description};
            } else {
                logger::warn("No value found for form_id: {}", form_id);

                return {.found = false};
            }
        } catch (const std::exception& e) {
            logger::error("Failed to execute query for form_id {}: {}", form_id, e.what());

            return {.found = false};
        }
    }

    /**
     * @brief Retrieves the name and description of a location based on the provided editor ID.
     *
     * @param editor_id The editor ID of the location as a string.
     * @return types::LocationLookupResult A structure containing the lookup result.
     */
    // MARK: - GetLocationDescription
    plugin::DataTypes::LocationLookupResult GetLocationDescription(const std::string& editor_id) {
        try {
            SQLite::Database db(DATABASE_PATH, SQLite::OPEN_READWRITE);

            SQLite::Statement query(db, R"(
                SELECT 
                    dc.name,
                    dc.description
                FROM description_location dc
                INNER JOIN load_order lo ON dc.mod_name = lo.mod_name
                WHERE dc.editor_id = ?
                ORDER BY lo.priority ASC
                LIMIT 1;
            )");

            query.bind(1, editor_id);

            if (query.executeStep()) {
                // Retrieve the name and description from the query result
                std::string name = query.getColumn(0).isNull() ? "" : query.getColumn(0).getString();
                std::string description = query.getColumn(1).isNull() ? "" : query.getColumn(1).getString();

                return {.found = true, .name = name, .description = description};
            } else {
                logger::warn("No value found for editor_id: {}", editor_id);

                return {.found = false};
            }
        } catch (const std::exception& e) {
            logger::error("Failed to execute query for editor_id {}: {}", editor_id, e.what());

            return {.found = false};
        }
    }

    /**
     * @brief Retrieves the name and description of a worldspace based on the provided editor ID.
     *
     * @param editor_id The editor ID of the worldspace as a string.
     * @return types::LocationLookupResult A structure containing the lookup result.
     */
    // MARK: - GetWorldspaceDescription
    plugin::DataTypes::LocationLookupResult GetWorldspaceDescription(const std::string& editor_id) {
        try {
            SQLite::Database db(DATABASE_PATH, SQLite::OPEN_READWRITE);

            SQLite::Statement query(db, R"(
                SELECT 
                    dc.name,
                    dc.description
                FROM description_worldspace dc
                INNER JOIN load_order lo ON dc.mod_name = lo.mod_name
                WHERE dc.editor_id = ?
                ORDER BY lo.priority ASC
                LIMIT 1;
            )");

            query.bind(1, editor_id);

            if (query.executeStep()) {
                // Retrieve the name and description from the query result
                std::string name = query.getColumn(0).isNull() ? "" : query.getColumn(0).getString();
                std::string description = query.getColumn(1).isNull() ? "" : query.getColumn(1).getString();

                return {.found = true, .name = name, .description = description};
            } else {
                logger::warn("No value found for editor_id: {}", editor_id);

                return {.found = false};
            }
        } catch (const std::exception& e) {
            logger::error("Failed to execute query for editor_id {}: {}", editor_id, e.what());

            return {.found = false};
        }
    }

    // MARK: - GetQuestDescription
    RE::BSFixedString GetQuestDescription(RE::StaticFunctionTag*, std::string quest_eid) {
        try {
            SQLite::Database db(DATABASE_PATH, SQLite::OPEN_READWRITE);

            SQLite::Statement query(db, "SELECT description FROM quests WHERE quest_eid = ?");
            query.bind(1, quest_eid);

            if (query.executeStep()) {
                return query.getColumn(0).getString();
            } else {
                logger::warn("No value found for key: {}", quest_eid);

                SQLite::Statement iquery(db, "INSERT OR IGNORE INTO quests (quest_eid) VALUES (?)");
                iquery.bind(1, quest_eid);
                iquery.exec();

                return "";
            }
        } catch (const std::exception& e) {
            logger::error("Failed to execute select query: {}", e.what());
            return "";
        }
    }

    // MARK: - GetStageDescription
    RE::BSFixedString GetStageDescription(RE::StaticFunctionTag*, std::string quest_eid, int stage) {
        try {
            SQLite::Database db(DATABASE_PATH, SQLite::OPEN_READWRITE);

            SQLite::Statement query(db, "SELECT description FROM stages WHERE quest_eid = ? AND stage = ?");
            query.bind(1, quest_eid);
            query.bind(2, stage);

            if (query.executeStep()) {
                return query.getColumn(0).getString();
            } else {
                logger::warn("No value found for key: {}", quest_eid);

                SQLite::Statement iquery(db, "INSERT OR IGNORE INTO stages (quest_eid, stage) VALUES (?, ?)");
                iquery.bind(1, quest_eid);
                iquery.bind(2, stage);
                iquery.exec();

                return "";
            }
        } catch (const std::exception& e) {
            logger::error("Failed to execute select query: {}", e.what());
            return "";
        }
    }

    // MARK: - GetObjectiveDescription
    RE::BSFixedString GetObjectiveDescription(RE::StaticFunctionTag*, std::string quest_eid, int objective) {
        try {
            SQLite::Database db(DATABASE_PATH, SQLite::OPEN_READWRITE);

            SQLite::Statement query(db, "SELECT description FROM objectives WHERE quest_eid = ? AND objective = ?");
            query.bind(1, quest_eid);
            query.bind(2, objective);

            if (query.executeStep()) {
                return query.getColumn(0).getString();
            } else {
                logger::warn("No value found for key: {}", quest_eid);

                SQLite::Statement iquery(db, "INSERT OR IGNORE INTO objectives (quest_eid, objective) VALUES (?, ?)");
                iquery.bind(1, quest_eid);
                iquery.bind(2, objective);
                iquery.exec();

                return "";
            }
        } catch (const std::exception& e) {
            logger::error("Failed to execute select query: {}", e.what());
            return "";
        }
    }

    // MARK: - GetSceneDescription
    RE::BSFixedString GetSceneDescription(RE::StaticFunctionTag*, std::string scene_eid, int phase, bool exactMatch) {
        try {
            SQLite::Database db(DATABASE_PATH, SQLite::OPEN_READWRITE);

            std::string sql = "SELECT description FROM scenes WHERE scene_eid = ? AND phase = ?";

            if (!exactMatch) {
                sql =
                    "SELECT description FROM scenes WHERE scene_eid = ? AND phase <= ? AND description IS NOT NULL ORDER BY phase DESC "
                    "LIMIT 1";
            }

            SQLite::Statement query(db, sql);
            query.bind(1, scene_eid);
            query.bind(2, phase);

            if (query.executeStep()) {
                return query.getColumn(0).getString();
            } else {
                logger::warn("No value found for key: {}", scene_eid);

                SQLite::Statement iquery(db, "INSERT OR IGNORE INTO scenes (scene_eid, phase) VALUES (?, ?)");
                iquery.bind(1, scene_eid);
                iquery.bind(2, phase);
                iquery.exec();

                return "";
            }
        } catch (const std::exception& e) {
            logger::error("Failed to execute select query: {}", e.what());
            return "";
        }
    }

    /*
      // Import a single file asynchronously
      auto futureFile = plugin::ImportDataFromFileAsync("path/to/file.json");
      futureFile.get(); // Optionally wait for the operation to complete
    */
    // MARK: - ImportDataFromFileAsync
    std::future<void> ImportDataFromFileAsync(const std::string& filename) {
        return std::async(std::launch::async, [filename]() {
            try {
                // Check if the file exists
                if (!std::filesystem::exists(filename)) {
                    logger::error("File does not exist: {}", filename);
                    return;
                }

                // Open and parse the JSON file
                std::ifstream file(filename);
                if (!file.is_open()) {
                    logger::error("Failed to open file: {}", filename);
                    return;
                }

                nlohmann::json jsonData;
                try {
                    file >> jsonData;
                } catch (const nlohmann::json::parse_error& e) {
                    logger::error("Invalid JSON format in file: {}. Error: {}", filename, e.what());
                    return;
                }

                // Open the database
                SQLite::Database db(DATABASE_PATH, SQLite::OPEN_READWRITE);

                // Process cells
                if (jsonData.contains("cells") && jsonData["cells"].is_object()) {
                    for (const auto& [modName, cells]: jsonData["cells"].items()) {
                        if (!cells.is_object()) {
                            logger::warn("Skipping invalid cells data for mod: {}", modName);
                            continue;
                        }

                        for (const auto& [formID, cellData]: cells.items()) {
                            if (!cellData.is_object()) {
                                logger::warn("Skipping invalid cell data for formID: {}", formID);
                                continue;
                            }

                            std::string name = cellData.value("name", "");
                            std::string description = cellData.value("description", "");

                            SQLite::Statement query(db, R"(
                                INSERT INTO description_cell (mod_name, form_id, name, description)
                                VALUES (?, ?, ?, ?)
                                ON CONFLICT(mod_name, form_id) DO UPDATE SET
                                    name = excluded.name,
                                    description = excluded.description
                            )");
                            query.bind(1, modName);
                            query.bind(2, formID);
                            query.bind(3, name);
                            query.bind(4, description);
                            query.exec();
                        }
                    }
                } else {
                    logger::warn("No valid cells object found in the JSON file.");
                }

                // Process worldspaces
                if (jsonData.contains("worldspaces") && jsonData["worldspaces"].is_object()) {
                    for (const auto& [modName, worldspaces]: jsonData["worldspaces"].items()) {
                        if (!worldspaces.is_object()) {
                            logger::warn("Skipping invalid worldspaces data for mod: {}", modName);
                            continue;
                        }

                        for (const auto& [editorID, worldspaceData]: worldspaces.items()) {
                            if (!worldspaceData.is_object()) {
                                logger::warn("Skipping invalid worldspace data for editorID: {}", editorID);
                                continue;
                            }

                            std::string name = worldspaceData.value("name", "");
                            std::string description = worldspaceData.value("description", "");

                            SQLite::Statement query(db, R"(
                                INSERT INTO description_worldspace (mod_name, editor_id, name, description)
                                VALUES (?, ?, ?, ?)
                                ON CONFLICT(mod_name, editor_id) DO UPDATE SET
                                    name = excluded.name,
                                    description = excluded.description
                            )");
                            query.bind(1, modName);
                            query.bind(2, editorID);
                            query.bind(3, name);
                            query.bind(4, description);
                            query.exec();
                        }
                    }
                } else {
                    logger::warn("No valid worldspaces object found in the JSON file.");
                }

                // Process locations
                if (jsonData.contains("locations") && jsonData["locations"].is_object()) {
                    for (const auto& [modName, locations]: jsonData["locations"].items()) {
                        if (!locations.is_object()) {
                            logger::warn("Skipping invalid locations data for mod: {}", modName);
                            continue;
                        }

                        for (const auto& [editorID, locationData]: locations.items()) {
                            if (!locationData.is_object()) {
                                logger::warn("Skipping invalid location data for editorID: {}", editorID);
                                continue;
                            }

                            std::string name = locationData.value("name", "");
                            std::string description = locationData.value("description", "");

                            SQLite::Statement query(db, R"(
                                INSERT INTO description_location (mod_name, editor_id, name, description)
                                VALUES (?, ?, ?, ?)
                                ON CONFLICT(mod_name, editor_id) DO UPDATE SET
                                    name = excluded.name,
                                    description = excluded.description
                            )");
                            query.bind(1, modName);
                            query.bind(2, editorID);
                            query.bind(3, name);
                            query.bind(4, description);
                            query.exec();
                        }
                    }
                } else {
                    logger::warn("No valid locations object found in the JSON file.");
                }

                // Process quests
                if (jsonData.contains("quests") && jsonData["quests"].is_object()) {
                    for (const auto& [questID, questData]: jsonData["quests"].items()) {
                        if (!questData.is_object()) {
                            logger::warn("Skipping invalid quest data for questID: {}", questID);
                            continue;
                        }

                        std::string name = questData.value("name", "");
                        std::string description = questData.value("description", "");

                        SQLite::Statement query(db, R"(
                            INSERT INTO quests (quest_eid, name, description)
                            VALUES (?, ?, ?)
                            ON CONFLICT(quest_eid) DO UPDATE SET
                                name = excluded.name,
                                description = excluded.description
                        )");
                        query.bind(1, questID);
                        query.bind(2, name);
                        query.bind(3, description);
                        query.exec();

                        // Process stages
                        if (questData.contains("stages") && questData["stages"].is_object()) {
                            for (const auto& [stageKey, stageDesc]: questData["stages"].items()) {
                                try {
                                    int stage = std::stoi(stageKey);
                                    std::string stageDescription = stageDesc.is_null() ? "" : stageDesc.get<std::string>();

                                    SQLite::Statement stageQuery(db, R"(
                                        INSERT INTO stages (quest_eid, stage, description)
                                        VALUES (?, ?, ?)
                                        ON CONFLICT(quest_eid, stage) DO UPDATE SET
                                            description = excluded.description
                                    )");
                                    stageQuery.bind(1, questID);
                                    stageQuery.bind(2, stage);
                                    stageQuery.bind(3, stageDescription);
                                    stageQuery.exec();
                                } catch (const std::exception& e) {
                                    logger::warn("Skipping invalid stage data for quest {}: {}", questID, e.what());
                                }
                            }
                        }

                        // Process objectives
                        if (questData.contains("objectives") && questData["objectives"].is_object()) {
                            for (const auto& [objectiveKey, objectiveDesc]: questData["objectives"].items()) {
                                try {
                                    int objective = std::stoi(objectiveKey);
                                    std::string objectiveDescription = objectiveDesc.is_null() ? "" : objectiveDesc.get<std::string>();

                                    SQLite::Statement objectiveQuery(db, R"(
                                        INSERT INTO objectives (quest_eid, objective, description)
                                        VALUES (?, ?, ?)
                                        ON CONFLICT(quest_eid, objective) DO UPDATE SET
                                            description = excluded.description
                                    )");
                                    objectiveQuery.bind(1, questID);
                                    objectiveQuery.bind(2, objective);
                                    objectiveQuery.bind(3, objectiveDescription);
                                    objectiveQuery.exec();
                                } catch (const std::exception& e) {
                                    logger::warn("Skipping invalid objective data for quest {}: {}", questID, e.what());
                                }
                            }
                        }
                    }
                } else {
                    logger::warn("No valid quests object found in the JSON file.");
                }

                logger::info("Data imported successfully from file: {}", filename);
            } catch (const std::exception& e) {
                logger::error("Failed to import data from file: {}. Error: {}", filename, e.what());
            }
        });
    }

    /*
      // Import all files in a folder asynchronously
      auto futureFolder = plugin::ImportDataFromFolderAsync("path/to/folder");
      futureFolder.get(); // Optionally qait for the operation to complete
    */
    // MARK: - ImportDataFromFolderAsync
    std::future<void> ImportDataFromFolderAsync(const std::string& folderPath) {
        return std::async(std::launch::async, [folderPath]() {
            try {
                // Check if the folder exists
                if (!std::filesystem::exists(folderPath) || !std::filesystem::is_directory(folderPath)) {
                    logger::error("Folder does not exist or is not a directory: {}", folderPath);
                    return;
                }

                // Iterate through all files in the folder
                for (const auto& entry: std::filesystem::directory_iterator(folderPath)) {
                    if (entry.is_regular_file() && entry.path().extension() == ".json") {
                        const std::string filePath = entry.path().string();
                        logger::info("Processing file: {}", filePath);

                        // Use the ImportDataFromFileAsync function for each JSON file
                        ImportDataFromFileAsync(filePath).get();  // Wait for each file to complete
                    }
                }

                logger::info("Finished processing all JSON files in folder: {}", folderPath);
            } catch (const std::exception& e) {
                logger::error("Failed to process folder: {}. Error: {}", folderPath, e.what());
            }
        });
    }

    /**
     * @brief Adds a placeholder entry for a cell in the database.
     *
     * @param mod_name The name of the mod.
     * @param form_id The form ID of the cell.
     */
    // MARK: - AddPlaceholderCell
    void AddPlaceholderCell(const std::string& mod_name, const std::string& form_id) {
        try {
            SQLite::Database db(DATABASE_PATH, SQLite::OPEN_READWRITE);

            SQLite::Statement query(db, R"(
                INSERT OR IGNORE INTO description_cell (mod_name, form_id, name, description)
                VALUES (?, ?, NULL, NULL)
            )");
            query.bind(1, mod_name);
            query.bind(2, form_id);
            query.exec();

            logger::info("Added placeholder cell entry: mod_name = {}, form_id = {}", mod_name, form_id);
        } catch (const std::exception& e) {
            logger::error("Failed to add placeholder cell entry: {}. Error: {}", form_id, e.what());
        }
    }

    /**
     * @brief Adds a placeholder entry for a location in the database.
     *
     * @param mod_name The name of the mod.
     * @param editor_id The editor ID of the location.
     */
    // MARK: - AddPlaceholderLocation
    void AddPlaceholderLocation(const std::string& mod_name, const std::string& editor_id) {
        try {
            SQLite::Database db(DATABASE_PATH, SQLite::OPEN_READWRITE);

            SQLite::Statement query(db, R"(
                INSERT OR IGNORE INTO description_location (mod_name, editor_id, name, description)
                VALUES (?, ?, NULL, NULL)
            )");
            query.bind(1, mod_name);
            query.bind(2, editor_id);
            query.exec();

            logger::info("Added placeholder location entry: mod_name = {}, editor_id = {}", mod_name, editor_id);
        } catch (const std::exception& e) {
            logger::error("Failed to add placeholder location entry: {}. Error: {}", editor_id, e.what());
        }
    }

    /**
     * @brief Adds a placeholder entry for a worldspace in the database.
     *
     * @param mod_name The name of the mod.
     * @param editor_id The editor ID of the worldspace.
     */
    // MARK: - AddPlaceholderWorldspace
    void AddPlaceholderWorldspace(const std::string& mod_name, const std::string& editor_id) {
        try {
            SQLite::Database db(DATABASE_PATH, SQLite::OPEN_READWRITE);

            SQLite::Statement query(db, R"(
                INSERT OR IGNORE INTO description_worldspace (mod_name, editor_id, name, description)
                VALUES (?, ?, NULL, NULL)
            )");
            query.bind(1, mod_name);
            query.bind(2, editor_id);
            query.exec();

            logger::info("Added placeholder worldspace entry: mod_name = {}, editor_id = {}", mod_name, editor_id);
        } catch (const std::exception& e) {
            logger::error("Failed to add placeholder worldspace entry: {}. Error: {}", editor_id, e.what());
        }
    }

}  // namespace plugin::DatabaseFunctions