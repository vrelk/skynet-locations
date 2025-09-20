#include "BirthdayAPI.hpp"
#include "../DatabaseFunctions.h"
#include <crow.h>
#include "../../include/cors.h"  // don't know why, but it refuses to find it in main includes folder from vcpkg
#include <string>
#include <SQLiteCpp/SQLiteCpp.h>

namespace plugin::REST::BirthdayAPI {

    void RegisterRoutes(crow::App<crow::CORSHandler>& app) {
        // GET /birthday?mod_name=Skyrim.esm&actor_id=0x14
        CROW_ROUTE(app, "/api/birthdays").methods("GET"_method)([](const crow::request& req) {
            auto mod_name = req.url_params.get("mod_name");
            auto actor_id = req.url_params.get("actor_id");
            if (!mod_name || !actor_id) {
                return crow::response(400, "Missing mod_name or actor_id");
            }
            std::string result = plugin::DatabaseFunctions::GetActorBirthdayString(mod_name, actor_id, false);
            if (result.empty()) {
                return crow::response(404, "Birthday not found");
            }
            return crow::response(200, result);
        });

        // POST /birthday (JSON body: actor_id, actor_name, month, day, year)
        CROW_ROUTE(app, "/api/birthdays").methods("POST"_method)([](const crow::request& req) {
            auto body = crow::json::load(req.body);
            if (!body) {
                return crow::response(400, "Invalid JSON");
            }
            try {
                SQLite::Database db(plugin::DatabaseFunctions::DATABASE_PATH, SQLite::OPEN_READWRITE);
                SQLite::Statement query(db, R"(
                INSERT INTO birthdays (actor_id, actor_name, month, day, year)
                VALUES (?, ?, ?, ?, ?)
                ON CONFLICT(actor_id) DO UPDATE SET
                    actor_name = excluded.actor_name,
                    month = excluded.month,
                    day = excluded.day,
                    year = excluded.year
            )");
                query.bind(1, body["actor_id"].s());
                query.bind(2, body["actor_name"].s());
                query.bind(3, body["month"].i());
                query.bind(4, body["day"].i());
                query.bind(5, body["year"].i());
                query.exec();
                return crow::response(200, "Birthday saved");
            } catch (const std::exception& e) {
                return crow::response(500, std::string("DB error: ") + e.what());
            }
        });

        // PUT /birthday (JSON body: actor_id, actor_name, month, day, year)
        CROW_ROUTE(app, "/api/birthdays").methods("PUT"_method)([](const crow::request& req) {
            auto body = crow::json::load(req.body);
            if (!body) {
                return crow::response(400, "Invalid JSON");
            }
            try {
                SQLite::Database db(plugin::DatabaseFunctions::DATABASE_PATH, SQLite::OPEN_READWRITE);
                SQLite::Statement query(db, R"(
                UPDATE birthdays
                SET actor_name = ?, month = ?, day = ?, year = ?
                WHERE actor_id = ?
            )");
                query.bind(1, body["actor_name"].s());
                query.bind(2, body["month"].i());
                query.bind(3, body["day"].i());
                query.bind(4, body["year"].i());
                query.bind(5, body["actor_id"].s());
                int rows = query.exec();
                if (rows == 0) {
                    return crow::response(404, "Birthday not found");
                }
                return crow::response(200, "Birthday updated");
            } catch (const std::exception& e) {
                return crow::response(500, std::string("DB error: ") + e.what());
            }
        });

        // DELETE /birthday?actor_id=0x14
        CROW_ROUTE(app, "/api/birthdays").methods("DELETE"_method)([](const crow::request& req) {
            auto actor_id = req.url_params.get("actor_id");
            if (!actor_id) {
                return crow::response(400, "Missing actor_id");
            }
            try {
                SQLite::Database db(plugin::DatabaseFunctions::DATABASE_PATH, SQLite::OPEN_READWRITE);
                SQLite::Statement query(db, "DELETE FROM birthdays WHERE actor_id = ?");
                query.bind(1, actor_id);
                int rows = query.exec();
                if (rows == 0) {
                    return crow::response(404, "Birthday not found");
                }
                return crow::response(200, "Birthday deleted");
            } catch (const std::exception& e) {
                return crow::response(500, std::string("DB error: ") + e.what());
            }
        });

        // GET /api/birthdays/all - List all birthday records
        CROW_ROUTE(app, "/api/birthdays/all").methods("GET"_method)([]() {
            try {
                SQLite::Database db(plugin::DatabaseFunctions::DATABASE_PATH, SQLite::OPEN_READONLY);
                SQLite::Statement query(db, "SELECT actor_id, actor_name, month, day, year FROM birthdays");
                std::vector<crow::json::wvalue> birthdays;
                while (query.executeStep()) {
                    crow::json::wvalue record;
                    record["actor_id"] = query.getColumn(0).getString();
                    record["actor_name"] = query.getColumn(1).getString();
                    record["month"] = query.getColumn(2).getInt();
                    record["day"] = query.getColumn(3).getInt();
                    record["year"] = query.getColumn(4).getInt();
                    birthdays.push_back(std::move(record));
                }
                crow::json::wvalue result_json;
                result_json["birthdays"] = std::move(birthdays);
                return crow::response(200, result_json);
            } catch (const std::exception& e) {
                return crow::response(500, std::string("DB error: ") + e.what());
            }
        });
    }

}  // namespace plugin::REST::BirthdayAPI