#include "Hooks.h"

namespace plugin {
    void Hooks::install() {
        QuitGameHook::install();

        // Install the quest stage hook
        //plugin::QuestHooks::InstallQuestStageHook();
        //plugin::QuestHooks::InstallObjectiveCompletedHook();

        //auto& eventProcessor = EventProcessor::GetSingleton();
        //not useful. gets called for every event??? Not sure, but gets called multiple times for the same stage
        //RE::ScriptEventSourceHolder::GetSingleton()->AddEventSink<RE::TESQuestStageEvent>(&eventProcessor);

        // NOPE. Can't use this
        //RE::ScriptEventSourceHolder::GetSingleton()->AddEventSink<RE::ObjectiveState::Event>(&eventProcessor);
    }

    void Hooks::quitGame() {
        logger::info("Game quitting");
    }
}  // namespace plugin
