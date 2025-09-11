#include <SQLiteCpp/SQLiteCpp.h>
#include <SQLiteCpp/VariadicBind.h>
#include <filesystem>
#include "DatabaseFunctions.h"
#include <future>  // Required for async operations
#include "JSON.hpp"

/*
{
    "locations":{
        "WhiterunLocation": {
            "name": "Whiterun",
            "description": "Something here"
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

namespace plugin {
    const std::string DATABASE_PATH = "Data/SKSE/Plugins/vrelk/VrelkDb.db";
    const std::string IMPORT_FOLDER_PATH = "Data/SKSE/Plugins/vrelk/import";

    void InitializeDatabase() {
        try {
            // Ensure the database directory exists
            std::filesystem::create_directories(std::filesystem::path(DATABASE_PATH).parent_path());

            // Open or create the database
            SQLite::Database db(DATABASE_PATH, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);

            // Enable foreign key constraints
            db.exec("PRAGMA foreign_keys = ON;");

            // Create the locations table
            db.exec(R"(
                CREATE TABLE IF NOT EXISTS "locations" (
                    "location_eid" VARCHAR(128) NOT NULL COLLATE NOCASE,
                    "name" VARCHAR(128) NULL,
                    "description" TEXT NULL,
                    "notes" TEXT NULL,
                    PRIMARY KEY ("location_eid")
                );
            )");

            // Create the quests table
            db.exec(R"(
                CREATE TABLE IF NOT EXISTS "quests" (
                    "quest_eid" VARCHAR(128) NOT NULL COLLATE NOCASE,
                    "name" VARCHAR(256) NULL,
                    "description" TEXT NULL,
                    "notes" TEXT NULL,
                    PRIMARY KEY ("quest_eid")
                );
            )");

            // Create the stages table with a foreign key constraint
            db.exec(R"(
                CREATE TABLE IF NOT EXISTS "stages" (
                    "quest_eid" VARCHAR(128) NOT NULL COLLATE NOCASE,
                    "stage" INT NOT NULL,
                    "description" TEXT NULL,
                    "notes" TEXT NULL,
                    PRIMARY KEY ("quest_eid", "stage"),
                    FOREIGN KEY ("quest_eid") REFERENCES "quests" ("quest_eid")
                    ON DELETE RESTRICT
                    ON UPDATE CASCADE
                );
            )");

            // Create the objectives table with a foreign key constraint
            db.exec(R"(
                CREATE TABLE IF NOT EXISTS "objectives" (
                    "quest_eid" VARCHAR(128) NOT NULL COLLATE NOCASE,
                    "objective" INT NOT NULL,
                    "description" TEXT NULL,
                    "notes" TEXT NULL,
                    PRIMARY KEY ("quest_eid", "objective"),
                    FOREIGN KEY ("quest_eid") REFERENCES "quests" ("quest_eid")
                    ON DELETE RESTRICT
                    ON UPDATE CASCADE
                );
            )");

            // Create the scenes table
            db.exec(R"(
                CREATE TABLE IF NOT EXISTS "scenes" (
                    "scene_eid" VARCHAR(128) NOT NULL COLLATE NOCASE,
                    "phase" INT NOT NULL,
                    "description" TEXT NULL,
                    "notes" TEXT NULL,
                    PRIMARY KEY ("scene_eid", "phase")
                );
            )");

            logger::info("Database initialized successfully.");
        } catch (const std::exception& e) {
            logger::error("Failed to initialize database: {}", e.what());
        }
    }

    // proposed usage:
    // 1: get load order.
    // 2: get all sql entries, then pick the one belonging to the highest load order mod.
    //
    // lookup order:
    // 1: cell
    // 2: location
    // 3: worldspace
    RE::BSFixedString GetLocationDescription(RE::StaticFunctionTag*, std::string location_eid) {
        try {
            SQLite::Database db(DATABASE_PATH, SQLite::OPEN_READWRITE);

            SQLite::Statement query(db, "SELECT description FROM locations WHERE location_eid = ?");
            query.bind(1, location_eid);

            if (query.executeStep()) {
                return query.getColumn(0).getString();
            } else {
                logger::warn("No value found for key: {}", location_eid);

                SQLite::Statement iquery(db, "INSERT OR IGNORE INTO locations (location_eid) VALUES (?)");
                iquery.bind(1, location_eid);
                iquery.exec();

                return "";
            }
        } catch (const std::exception& e) {
            logger::error("Failed to execute select query: {}", e.what());
            return "";
        }
    }

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
      futureFile.get(); // Wait for the operation to complete
    */
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

                // Process locations
                if (jsonData.contains("locations") && jsonData["locations"].is_object()) {
                    for (const auto& [locationID, locationData]: jsonData["locations"].items()) {
                        if (!locationData.is_object()) {
                            logger::warn("Skipping invalid location data for key: {}", locationID);
                            continue;
                        }

                        std::string name = locationData.value("name", "");                // Default to empty string if null
                        std::string description = locationData.value("description", "");  // Default to empty string if null

                        // Insert or update location description if it is null
                        SQLite::Statement query(db, R"(
                            INSERT INTO locations (location_eid, name, description)
                            VALUES (?, ?, ?)
                            ON CONFLICT(location_eid) DO UPDATE SET
                                name = excluded.name,
                                description = CASE WHEN description IS NULL THEN excluded.description ELSE description END
                        )");
                        query.bind(1, locationID);
                        query.bind(2, name);
                        query.bind(3, description);
                        query.exec();
                    }
                } else {
                    logger::warn("No valid locations object found in the JSON file.");
                }

                // Process quests
                if (jsonData.contains("quests") && jsonData["quests"].is_object()) {
                    for (const auto& [questID, questData]: jsonData["quests"].items()) {
                        if (!questData.is_object()) {
                            logger::warn("Skipping invalid quest data for key: {}", questID);
                            continue;
                        }

                        std::string name = questData.value("name", "");                // Default to empty string if null
                        std::string description = questData.value("description", "");  // Default to empty string if null

                        // Insert or update quest description if it is null
                        SQLite::Statement query(db, R"(
                            INSERT INTO quests (quest_eid, name, description)
                            VALUES (?, ?, ?)
                            ON CONFLICT(quest_eid) DO UPDATE SET
                                name = excluded.name,
                                description = CASE WHEN description IS NULL THEN excluded.description ELSE description END
                        )");
                        query.bind(1, questID);
                        query.bind(2, name);
                        query.bind(3, description);
                        query.exec();

                        // Process stages for the quest
                        if (questData.contains("stages") && questData["stages"].is_object()) {
                            for (const auto& [stageKey, stageDesc]: questData["stages"].items()) {
                                try {
                                    int stage = std::stoi(stageKey);
                                    std::string stageDescription = stageDesc.is_null() ? "" : stageDesc.get<std::string>();

                                    // Insert or update stage description if it is null
                                    SQLite::Statement stageQuery(db, R"(
                                        INSERT INTO stages (quest_eid, stage, description)
                                        VALUES (?, ?, ?)
                                        ON CONFLICT(quest_eid, stage) DO UPDATE SET
                                            description = CASE WHEN description IS NULL THEN excluded.description ELSE description END
                                    )");
                                    stageQuery.bind(1, questID);
                                    stageQuery.bind(2, stage);
                                    stageQuery.bind(3, stageDescription);
                                    stageQuery.exec();
                                } catch (const std::exception& e) {
                                    logger::warn("Skipping invalid stage key or description for quest {}: {}", questID, e.what());
                                }
                            }
                        }

                        // Process objectives for the quest
                        if (questData.contains("objectives") && questData["objectives"].is_object()) {
                            for (const auto& [objectiveKey, objectiveDesc]: questData["objectives"].items()) {
                                try {
                                    int objective = std::stoi(objectiveKey);
                                    std::string objectiveDescription = objectiveDesc.is_null() ? "" : objectiveDesc.get<std::string>();

                                    // Insert or update objective description if it is null
                                    SQLite::Statement objectiveQuery(db, R"(
                                        INSERT INTO objectives (quest_eid, objective, description)
                                        VALUES (?, ?, ?)
                                        ON CONFLICT(quest_eid, objective) DO UPDATE SET
                                            description = CASE WHEN description IS NULL THEN excluded.description ELSE description END
                                    )");
                                    objectiveQuery.bind(1, questID);
                                    objectiveQuery.bind(2, objective);
                                    objectiveQuery.bind(3, objectiveDescription);
                                    objectiveQuery.exec();
                                } catch (const std::exception& e) {
                                    logger::warn("Skipping invalid objective key or description for quest {}: {}", questID, e.what());
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
      futureFolder.get(); // Wait for the operation to complete
    */
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
}  // namespace plugin