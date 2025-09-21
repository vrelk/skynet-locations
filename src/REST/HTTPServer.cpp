#include "HTTPServer.hpp"
#include <crow.h>
#include "../../include/cors.h"  // don't know why, but it refuses to find it in main includes folder from vcpkg
#include "../VrelkUtil.h"
#include "../SkyrimHelpers.h"
#include "../DatabaseFunctions.h"
#include "../DataTypes.hpp"

#include "BirthdayAPI.hpp"
#include "LocationAPI.hpp"

namespace plugin::REST::HTTPServer {
    using namespace plugin::SkyrimHelpers;

    void startServer() {
        crow::App<crow::CORSHandler> app;

        CROW_ROUTE(app, "/")([]() { return "Hello, World!"; });
    https:  //crowcpp.org/1.0/guides/CORS/

        // Register birthday API endpoints
        plugin::REST::BirthdayAPI::RegisterRoutes(app);

        // Register location API endpoints
        plugin::REST::LocationAPI::RegisterRoutes(app);

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
            //response["player"]["cell"]["x"] = cell ? cell->GetCoordinates()->cellX : 0;  //CRASH!
            //response["player"]["cell"]["y"] = cell ? cell->GetCoordinates()->cellY : 0;  //CRASH!

            if (cell) {
                std::string formIDStr = VrelkUtil::IntToString(cell->GetFormID());
                plugin::DataTypes::LocationLookupResult cellDescription = plugin::DatabaseFunctions::GetCellDescription(formIDStr);

                if (cellDescription.found) {
                    response["player"]["cell"]["custom_name"] = cellDescription.name;
                    response["player"]["cell"]["description"] = cellDescription.description;
                } else {
                    response["player"]["cell"]["custom_name"] = "";
                    response["player"]["cell"]["description"] = "";
                }
            }

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

}  // namespace plugin::REST::HTTPServer