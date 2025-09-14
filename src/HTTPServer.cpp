#include "HTTPServer.h"
#include "crow.h"
#include "VrelkUtil.h"
#include "SkyrimHelpers.h"

namespace plugin::HTTPServer {
    using namespace plugin::SkyrimHelpers;

    void startServer() {
        crow::SimpleApp app;

        CROW_ROUTE(app, "/")([]() { return "Hello, World!"; });

        // Serve static HTML file at /player
        CROW_ROUTE(app, "/player").methods("GET"_method)([]() {
            std::ifstream file("Data/SKSE/Plugins/vrelk/status.html");
            if (!file.is_open()) {
                return crow::response(404, "File not found");
            }
            std::stringstream buffer;
            buffer << file.rdbuf();
            return crow::response(200, buffer.str());
        });

        // Example JSON endpoint - returns player info
        CROW_ROUTE(app, "/player/info").methods("GET"_method)([]() {
            crow::json::wvalue response;

            if (!IsPlayerLoaded()) {
                response["status"] = "error";
                response["message"] = "Player not loaded";
                return crow::response(400, response);
            }

            RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();

            auto* cell = player->GetParentCell();

            response["status"] = "success";
            response["player"]["name"] = player->GetName();
            response["player"]["level"] = player->GetLevel();
            response["player"]["worldspace"]["name"] = player->GetWorldspace() ? player->GetWorldspace()->GetName() : "Unknown";
            response["player"]["worldspace"]["id"] =
                player->GetWorldspace() ? VrelkUtil::IntToString(player->GetWorldspace()->GetFormID()) : "Unknown";
            response["player"]["worldspace"]["editor_id"] = SkyrimHelpers::GetFormEditorID(player->GetWorldspace());
            response["player"]["health"] = GetPercentageAV(player, RE::ActorValue::kHealth);
            response["player"]["magicka"] = GetPercentageAV(player, RE::ActorValue::kStamina);
            response["player"]["stamina"] = GetPercentageAV(player, RE::ActorValue::kMagicka);
            response["player"]["cell"]["id"] = cell ? VrelkUtil::IntToString(cell->GetFormID()) : "Unknown";
            response["player"]["cell"]["editor_id"] = SkyrimHelpers::GetFormEditorID(cell);
            response["player"]["cell"]["name"] = cell ? cell->GetName() : "Unknown";
            //response["player"]["is_interior"] = cell ? cell->IsInterior() : false;
            response["player"]["location"]["id"] =
                player->GetCurrentLocation() ? VrelkUtil::IntToString(player->GetCurrentLocation()->GetFormID()) : "Unknown";
            response["player"]["location"]["editor_id"] = SkyrimHelpers::GetFormEditorID(player->GetCurrentLocation());
            response["player"]["location"]["name"] = player->GetCurrentLocation() ? player->GetCurrentLocation()->GetName() : "Unknown";
            response["player"]["coordinates"] = {{"x", player->GetPositionX()},
                                                 {"y", player->GetPositionY()},
                                                 {"z", player->GetPositionZ()}};

            return crow::response(200, response);
        });

        app.bindaddr("127.0.0.1").port(8880).multithreaded().run();
    }

}  // namespace plugin::HTTPServer