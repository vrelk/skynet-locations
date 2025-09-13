#include "Hooks.h"
#include "QuestHooks.h"

namespace plugin {
    void Hooks::install() {
        QuitGameHook::install();

        // Install the quest stage hook
        plugin::QuestHooks::InstallQuestStageHook();
    }

    void Hooks::quitGame() {
        logger::info("Game quitting");
    }
}  // namespace plugin



// https://github.com/ryobg/sse-hooks