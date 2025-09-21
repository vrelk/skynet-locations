#include "GameEventHandler.h"
#include "Hooks.h"
#include "DatabaseFunctions.h"
#include "../include/jcontainers.hpp"
#include "SlaveTats.hpp"

using namespace plugin::DatabaseFunctions;

namespace plugin {

    // ----------------------------------
    // Helper function to determine JContainers plugin name based on SKSE version
    // ----------------------------------
    std::string getJContainersPluginName() {
        auto patchVersion = REL::Module::get().version().patch();

        std::string pluginName{"JContainers64"};
        if (REL::Module::IsVR()) {
            pluginName = "JContainersVR";
        } else if (patchVersion == 659 || patchVersion == 1179) {
            pluginName = "JContainersGOG";
        }

        return pluginName;
    }

    // ----------------------------------
    // GameEventHandler method implementations
    // ----------------------------------
    void GameEventHandler::onLoad() {
        logger::info("onLoad()");
        Hooks::install();
    }

    void GameEventHandler::onPostLoad() {
        logger::info("onPostLoad()");

        // Initialize JContainers
        std::string pluginName = getJContainersPluginName();
        logger::info("JContainers Plugin Name seems to be: {}", pluginName);

        SKSE::GetMessagingInterface()->RegisterListener(pluginName.c_str(), [](SKSE::MessagingInterface::Message* a_msg) {
            logger::info("a_msg={}, msgtype={}, message_root_interface={}", (void*) a_msg, a_msg ? a_msg->type : -1,
                         (int) jc::message_root_interface);
            if (a_msg && a_msg->type == jc::message_root_interface) {
                const jc::root_interface* root = jc::root_interface::from_void(a_msg->data);
                logger::info("root_interface={}", (void*) root);
                if (root)
                    // Seems to be a deadlock or so here, so we defer the actual JContainers initialization
                    jcontainers::JCWrapper::GetSingleton()->PreInit(root);
            }
        });

        SKSE::GetMessagingInterface()->RegisterListener("SlaveTatsNG", SlaveTats::messaging_hook);
    }

    void GameEventHandler::onPostPostLoad() {
        logger::info("onPostPostLoad()");
    }

    void GameEventHandler::onInputLoaded() {
        logger::info("onInputLoaded()");
    }

    void GameEventHandler::onDataLoaded() {
        logger::info("onDataLoaded()");

        jcontainers::JCWrapper::GetSingleton()->Init();

        // Initialize database and import data from JSON files
        InitializeDatabase();
        auto jsonImport = ImportDataFromFolderAsync(IMPORT_FOLDER_PATH);
        jsonImport.get();  // Optionally wait for the operation to complete
    }

    void GameEventHandler::onNewGame() {
        logger::info("onNewGame()");
    }

    void GameEventHandler::onPreLoadGame() {
        logger::info("onPreLoadGame()");
    }

    void GameEventHandler::onPostLoadGame() {
        logger::info("onPostLoadGame()");

        logger::info("Syncing SlaveTats tattoos to DB");
        auto syncTats = DatabaseFunctions::StoreAllTattoos();
        //syncTats.get();  // Optionally wait for the operation to complete
    }

    void GameEventHandler::onSaveGame() {
        logger::info("onSaveGame()");
    }

    void GameEventHandler::onDeleteGame() {
        logger::info("onDeleteGame()");
    }
}  // namespace plugin