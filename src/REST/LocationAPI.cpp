#include "LocationAPI.hpp"
#include "../DatabaseFunctions.h"
#include <crow.h>
#include "../../include/cors.h"  // don't know why, but it refuses to find it in main includes folder from vcpkg
#include <string>
#include <SQLiteCpp/SQLiteCpp.h>

namespace plugin::REST::LocationAPI {

    // Helper for CRUD on description tables
    static crow::response HandleGetDescription(const std::string& table, const std::string& mod_name, const std::string& editor_id) {
        try {
            SQLite::Database db(plugin::DatabaseFunctions::DATABASE_PATH, SQLite::OPEN_READONLY);
            std::string sql = std::format("SELECT name, description FROM {} WHERE mod_name = ? AND editor_id = ? LIMIT 1;", table);
            SQLite::Statement query(db, sql);
            query.bind(1, mod_name);
            query.bind(2, editor_id);

            if (query.executeStep()) {
                crow::json::wvalue result;
                result["name"] = query.getColumn(0).isNull() ? "" : query.getColumn(0).getString();
                result["description"] = query.getColumn(1).isNull() ? "" : query.getColumn(1).getString();
                return crow::response(200, result);
            } else {
                return crow::response(404, "Not found");
            }
        } catch (const std::exception& e) {
            return crow::response(500, std::string("DB error: ") + e.what());
        }
    }

    static crow::response HandleUpsertDescription(const std::string& table, const crow::json::rvalue& body) {
        if (!body.has("mod_name") || !body.has("editor_id")) {
            return crow::response(400, "Missing mod_name or editor_id");
        }
        try {
            SQLite::Database db(plugin::DatabaseFunctions::DATABASE_PATH, SQLite::OPEN_READWRITE);
            std::string sql = std::format(
                "INSERT INTO {} (mod_name, editor_id, name, description, notes) VALUES (?, ?, ?, ?, ?) "
                "ON CONFLICT(mod_name, editor_id) DO UPDATE SET name = excluded.name, description = excluded.description, notes = "
                "excluded.notes;",
                table);
            SQLite::Statement query(db, sql);
            query.bind(1, body["mod_name"].s());
            query.bind(2, body["editor_id"].s());
            query.bind(3, body.has("name") ? std::string(body["name"]) : std::string(""));
            query.bind(4, body.has("description") ? std::string(body["description"]) : std::string(""));
            query.bind(5, body.has("notes") ? std::string(body["notes"]) : std::string(""));
            query.exec();
            return crow::response(200, "Saved");
        } catch (const std::exception& e) {
            return crow::response(500, std::string("DB error: ") + e.what());
        }
    }

    static crow::response HandleUpdateDescription(const std::string& table, const crow::json::rvalue& body) {
        if (!body.has("mod_name") || !body.has("editor_id")) {
            return crow::response(400, "Missing mod_name or editor_id");
        }
        try {
            SQLite::Database db(plugin::DatabaseFunctions::DATABASE_PATH, SQLite::OPEN_READWRITE);
            std::string sql =
                std::format("UPDATE {} SET name = ?, description = ?, notes = ? WHERE mod_name = ? AND editor_id = ?;", table);
            SQLite::Statement query(db, sql);
            query.bind(1, body.has("name") ? std::string(body["name"]) : std::string(""));
            query.bind(2, body.has("description") ? std::string(body["description"]) : std::string(""));
            query.bind(3, body.has("notes") ? std::string(body["notes"]) : std::string(""));
            query.bind(4, body["mod_name"].s());
            query.bind(5, body["editor_id"].s());
            int rows = query.exec();
            if (rows == 0) {
                return crow::response(404, "Not found");
            }
            return crow::response(200, "Updated");
        } catch (const std::exception& e) {
            return crow::response(500, std::string("DB error: ") + e.what());
        }
    }

    static crow::response HandleDeleteDescription(const std::string& table, const std::string& mod_name, const std::string& editor_id) {
        try {
            SQLite::Database db(plugin::DatabaseFunctions::DATABASE_PATH, SQLite::OPEN_READWRITE);
            std::string sql = std::format("DELETE FROM {} WHERE mod_name = ? AND editor_id = ?;", table);
            SQLite::Statement query(db, sql);
            query.bind(1, mod_name);
            query.bind(2, editor_id);
            int rows = query.exec();
            if (rows == 0) {
                return crow::response(404, "Not found");
            }
            return crow::response(200, "Deleted");
        } catch (const std::exception& e) {
            return crow::response(500, std::string("DB error: ") + e.what());
        }
    }

    static crow::response HandleListAllDescriptions(const std::string& table, const std::string& key) {
        try {
            SQLite::Database db(plugin::DatabaseFunctions::DATABASE_PATH, SQLite::OPEN_READONLY);
            std::string sql = std::format("SELECT mod_name, editor_id, name, description, notes FROM {};", table);
            SQLite::Statement query(db, sql);
            std::vector<crow::json::wvalue> items;
            while (query.executeStep()) {
                crow::json::wvalue item;
                item["mod_name"] = query.getColumn(0).getString();
                item["editor_id"] = query.getColumn(1).getString();
                item["name"] = query.getColumn(2).getString();
                item["description"] = query.getColumn(3).getString();
                item["notes"] = query.getColumn(4).getString();
                items.push_back(std::move(item));
            }
            crow::json::wvalue result;
            result[key] = std::move(items);
            return crow::response(200, result);
        } catch (const std::exception& e) {
            return crow::response(500, std::string("DB error: ") + e.what());
        }
    }

    // Helper for CRUD on cell description table
    static crow::response HandleGetCellDescription(const std::string& mod_name, const std::string& form_id) {
        try {
            SQLite::Database db(plugin::DatabaseFunctions::DATABASE_PATH, SQLite::OPEN_READONLY);
            std::string sql = "SELECT name, description FROM description_cell WHERE mod_name = ? AND form_id = ? LIMIT 1;";
            SQLite::Statement query(db, sql);
            query.bind(1, mod_name);
            query.bind(2, form_id);

            if (query.executeStep()) {
                crow::json::wvalue result;
                result["name"] = query.getColumn(0).isNull() ? "" : query.getColumn(0).getString();
                result["description"] = query.getColumn(1).isNull() ? "" : query.getColumn(1).getString();
                return crow::response(200, result);
            } else {
                return crow::response(404, "Not found");
            }
        } catch (const std::exception& e) {
            return crow::response(500, std::string("DB error: ") + e.what());
        }
    }

    static crow::response HandleUpsertCellDescription(const crow::json::rvalue& body) {
        if (!body.has("mod_name") || !body.has("form_id")) {
            return crow::response(400, "Missing mod_name or form_id");
        }
        try {
            SQLite::Database db(plugin::DatabaseFunctions::DATABASE_PATH, SQLite::OPEN_READWRITE);
            std::string sql =
                "INSERT INTO description_cell (mod_name, form_id, name, description, notes) VALUES (?, ?, ?, ?, ?) "
                "ON CONFLICT(mod_name, form_id) DO UPDATE SET name = excluded.name, description = excluded.description, notes = "
                "excluded.notes;";
            SQLite::Statement query(db, sql);
            query.bind(1, body["mod_name"].s());
            query.bind(2, body["form_id"].s());
            query.bind(3, body.has("name") ? std::string(body["name"]) : std::string(""));
            query.bind(4, body.has("description") ? std::string(body["description"]) : std::string(""));
            query.bind(5, body.has("notes") ? std::string(body["notes"]) : std::string(""));
            query.exec();
            return crow::response(200, "Saved");
        } catch (const std::exception& e) {
            return crow::response(500, std::string("DB error: ") + e.what());
        }
    }

    static crow::response HandleUpdateCellDescription(const crow::json::rvalue& body) {
        if (!body.has("mod_name") || !body.has("form_id")) {
            return crow::response(400, "Missing mod_name or form_id");
        }
        try {
            SQLite::Database db(plugin::DatabaseFunctions::DATABASE_PATH, SQLite::OPEN_READWRITE);
            std::string sql = "UPDATE description_cell SET name = ?, description = ?, notes = ? WHERE mod_name = ? AND form_id = ?;";
            SQLite::Statement query(db, sql);
            query.bind(1, body.has("name") ? std::string(body["name"]) : std::string(""));
            query.bind(2, body.has("description") ? std::string(body["description"]) : std::string(""));
            query.bind(3, body.has("notes") ? std::string(body["notes"]) : std::string(""));
            query.bind(4, body["mod_name"].s());
            query.bind(5, body["form_id"].s());
            int rows = query.exec();
            if (rows == 0) {
                return crow::response(404, "Not found");
            }
            return crow::response(200, "Updated");
        } catch (const std::exception& e) {
            return crow::response(500, std::string("DB error: ") + e.what());
        }
    }

    static crow::response HandleDeleteCellDescription(const std::string& mod_name, const std::string& form_id) {
        try {
            SQLite::Database db(plugin::DatabaseFunctions::DATABASE_PATH, SQLite::OPEN_READWRITE);
            std::string sql = "DELETE FROM description_cell WHERE mod_name = ? AND form_id = ?;";
            SQLite::Statement query(db, sql);
            query.bind(1, mod_name);
            query.bind(2, form_id);
            int rows = query.exec();
            if (rows == 0) {
                return crow::response(404, "Not found");
            }
            return crow::response(200, "Deleted");
        } catch (const std::exception& e) {
            return crow::response(500, std::string("DB error: ") + e.what());
        }
    }

    static crow::response HandleListAllCellDescriptions() {
        try {
            SQLite::Database db(plugin::DatabaseFunctions::DATABASE_PATH, SQLite::OPEN_READONLY);
            std::string sql = "SELECT mod_name, form_id, name, description, notes FROM description_cell;";
            SQLite::Statement query(db, sql);
            std::vector<crow::json::wvalue> items;
            while (query.executeStep()) {
                crow::json::wvalue item;
                item["mod_name"] = query.getColumn(0).getString();
                item["form_id"] = query.getColumn(1).getString();
                item["name"] = query.getColumn(2).getString();
                item["description"] = query.getColumn(3).getString();
                item["notes"] = query.getColumn(4).getString();
                items.push_back(std::move(item));
            }
            crow::json::wvalue result;
            result["cells"] = std::move(items);
            return crow::response(200, result);
        } catch (const std::exception& e) {
            return crow::response(500, std::string("DB error: ") + e.what());
        }
    }

    void RegisterRoutes(crow::App<crow::CORSHandler>& app) {
        // LOCATION ENDPOINTS
        CROW_ROUTE(app, "/api/locations").methods("GET"_method)([](const crow::request& req) {
            auto mod_name = req.url_params.get("mod_name");
            auto editor_id = req.url_params.get("editor_id");
            if (!mod_name || !editor_id) {
                return crow::response(400, "Missing mod_name or editor_id");
            }
            return HandleGetDescription("description_location", mod_name, editor_id);
        });

        CROW_ROUTE(app, "/api/locations").methods("POST"_method)([](const crow::request& req) {
            auto body = crow::json::load(req.body);
            if (!body)
                return crow::response(400, "Invalid JSON");
            return HandleUpsertDescription("description_location", body);
        });

        CROW_ROUTE(app, "/api/locations").methods("PUT"_method)([](const crow::request& req) {
            auto body = crow::json::load(req.body);
            if (!body)
                return crow::response(400, "Invalid JSON");
            return HandleUpdateDescription("description_location", body);
        });

        CROW_ROUTE(app, "/api/locations").methods("DELETE"_method)([](const crow::request& req) {
            auto mod_name = req.url_params.get("mod_name");
            auto editor_id = req.url_params.get("editor_id");
            if (!mod_name || !editor_id) {
                return crow::response(400, "Missing mod_name or editor_id");
            }
            return HandleDeleteDescription("description_location", mod_name, editor_id);
        });

        CROW_ROUTE(app, "/api/locations/all").methods("GET"_method)([]() {
            return HandleListAllDescriptions("description_location", "locations");
        });

        // WORLDSPACE ENDPOINTS
        CROW_ROUTE(app, "/api/worldspaces").methods("GET"_method)([](const crow::request& req) {
            auto mod_name = req.url_params.get("mod_name");
            auto editor_id = req.url_params.get("editor_id");
            if (!mod_name || !editor_id) {
                return crow::response(400, "Missing mod_name or editor_id");
            }
            return HandleGetDescription("description_worldspace", mod_name, editor_id);
        });

        CROW_ROUTE(app, "/api/worldspaces").methods("POST"_method)([](const crow::request& req) {
            auto body = crow::json::load(req.body);
            if (!body)
                return crow::response(400, "Invalid JSON");
            return HandleUpsertDescription("description_worldspace", body);
        });

        CROW_ROUTE(app, "/api/worldspaces").methods("PUT"_method)([](const crow::request& req) {
            auto body = crow::json::load(req.body);
            if (!body)
                return crow::response(400, "Invalid JSON");
            return HandleUpdateDescription("description_worldspace", body);
        });

        CROW_ROUTE(app, "/api/worldspaces").methods("DELETE"_method)([](const crow::request& req) {
            auto mod_name = req.url_params.get("mod_name");
            auto editor_id = req.url_params.get("editor_id");
            if (!mod_name || !editor_id) {
                return crow::response(400, "Missing mod_name or editor_id");
            }
            return HandleDeleteDescription("description_worldspace", mod_name, editor_id);
        });

        CROW_ROUTE(app, "/api/worldspaces/all").methods("GET"_method)([]() {
            return HandleListAllDescriptions("description_worldspace", "worldspaces");
        });

        // CELL ENDPOINTS
        CROW_ROUTE(app, "/api/cells").methods("GET"_method)([](const crow::request& req) {
            auto mod_name = req.url_params.get("mod_name");
            auto form_id = req.url_params.get("form_id");
            if (!mod_name || !form_id) {
                return crow::response(400, "Missing mod_name or form_id");
            }
            return HandleGetCellDescription(mod_name, form_id);
        });

        CROW_ROUTE(app, "/api/cells").methods("POST"_method)([](const crow::request& req) {
            auto body = crow::json::load(req.body);
            if (!body)
                return crow::response(400, "Invalid JSON");
            return HandleUpsertCellDescription(body);
        });

        CROW_ROUTE(app, "/api/cells").methods("PUT"_method)([](const crow::request& req) {
            auto body = crow::json::load(req.body);
            if (!body)
                return crow::response(400, "Invalid JSON");
            return HandleUpdateCellDescription(body);
        });

        CROW_ROUTE(app, "/api/cells").methods("DELETE"_method)([](const crow::request& req) {
            auto mod_name = req.url_params.get("mod_name");
            auto form_id = req.url_params.get("form_id");
            if (!mod_name || !form_id) {
                return crow::response(400, "Missing mod_name or form_id");
            }
            return HandleDeleteCellDescription(mod_name, form_id);
        });

        CROW_ROUTE(app, "/api/cells/all").methods("GET"_method)([]() { return HandleListAllCellDescriptions(); });
    }

}  // namespace plugin::REST::LocationAPI