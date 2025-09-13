#include "GameEventHandler.h"
#include "Hooks.h"
#include "DatabaseFunctions.h"

using namespace plugin::DatabaseFunctions;

namespace plugin {

    void GameEventHandler::onLoad() {
        logger::info("onLoad()");
        Hooks::install();
    }

    void GameEventHandler::onPostLoad() {
        logger::info("onPostLoad()");
    }

    void GameEventHandler::onPostPostLoad() {
        logger::info("onPostPostLoad()");
    }

    void GameEventHandler::onInputLoaded() {
        logger::info("onInputLoaded()");
    }

    void GameEventHandler::onDataLoaded() {
        logger::info("onDataLoaded()");
        InitializeDatabase();
        auto jsonImport = ImportDataFromFolderAsync(IMPORT_FOLDER_PATH);
        jsonImport.get();  // Optionally wait for the operation to complete

        //plugin::ConsoleCommands::RegisterMyConsoleCommand();
    }

    void GameEventHandler::onNewGame() {
        logger::info("onNewGame()");
    }

    void GameEventHandler::onPreLoadGame() {
        logger::info("onPreLoadGame()");
    }

    void GameEventHandler::onPostLoadGame() {
        logger::info("onPostLoadGame()");
    }

    void GameEventHandler::onSaveGame() {
        logger::info("onSaveGame()");
    }

    void GameEventHandler::onDeleteGame() {
        logger::info("onDeleteGame()");
    }
}  // namespace plugin