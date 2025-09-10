#include <SQLiteCpp/SQLiteCpp.h>
#include <SQLiteCpp/VariadicBind.h>
#include <filesystem>
#include "DatabaseFunctions.h"

namespace plugin {
    const std::string DATABASE_PATH = "Data/SKSE/Plugins/vrelk/VrelkDb.db";

    void InitializeDatabase() {
        try {
            // Ensure the database directory exists
            std::filesystem::create_directories(std::filesystem::path(DATABASE_PATH).parent_path());

            // Open or create the database
            SQLite::Database db(DATABASE_PATH, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);

            // Create a table if it does not exist
            db.exec(R"(
                CREATE TABLE IF NOT EXISTS MyTable (
                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                    key TEXT UNIQUE NOT NULL,
                    value TEXT NOT NULL
                );
            )");

            logger::info("Database initialized successfully.");
        } catch (const std::exception& e) {
            logger::error("Failed to initialize database: {}", e.what());
        }
    }

    std::string SelectValue(const std::string& key) {
        try {
            SQLite::Database db(DATABASE_PATH, SQLite::OPEN_READONLY);

            SQLite::Statement query(db, "SELECT value FROM MyTable WHERE key = ?");
            query.bind(1, key);

            if (query.executeStep()) {
                return query.getColumn(0).getString();
            } else {
                logger::warn("No value found for key: {}", key);
                return "";
            }
        } catch (const std::exception& e) {
            logger::error("Failed to execute select query: {}", e.what());
            return "";
        }
    }

    void InsertValueIfNotExists(const std::string& key, const std::string& value) {
        try {
            SQLite::Database db(DATABASE_PATH, SQLite::OPEN_READWRITE);

            SQLite::Statement query(db, "INSERT OR IGNORE INTO MyTable (key, value) VALUES (?, ?)");
            query.bind(1, key);
            query.bind(2, value);

            query.exec();
            logger::info("Value inserted or ignored for key: {}", key);
        } catch (const std::exception& e) {
            logger::error("Failed to insert value: {}", e.what());
        }
    }
}  // namespace plugin