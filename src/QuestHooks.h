#include "RE/Skyrim.h"

namespace plugin::QuestHooks {
    void SetCurrentStageID_Hook(RE::TESQuest* quest, std::uint16_t stage);
    void InstallQuestStageHook();

    // Add declarations for the new hook
    void SetObjectiveCompleted_Hook(RE::TESQuest* quest, std::uint32_t objectiveIndex, bool completed);
    void InstallObjectiveCompletedHook();
}