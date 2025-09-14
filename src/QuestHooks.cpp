#include "RE/Skyrim.h"
#include "SKSE/API.h"
#include "REL/Relocation.h"
#include <format>

#include "QuestHooks.h"

namespace plugin::QuestHooks {
    // Hook function for quest stage updates
    void SetCurrentStageID_Hook(RE::TESQuest* quest, std::uint16_t stage) {
        logger::info("SetCurrentStageID_Hook called");
        try {
            static auto original = REL::Relocation<decltype(SetCurrentStageID_Hook)*>{REL::ID(24478)};  // Replace with the correct ID
            original(quest, stage);

            if (quest) {
                auto editorID = quest->GetFormEditorID();
                if (editorID) {
                    std::string message = std::format("Quest '{}' advanced to stage {}", editorID, stage);
                    RE::ConsoleLog::GetSingleton()->Print(message.c_str());
                    logger::info("Quest '{}' advanced to stage {}", editorID, stage);

                    SKSE::ModCallbackEvent modEvent{"VrelkQuestStageUpdate", editorID, static_cast<float>(stage), nullptr};
                    SKSE::GetModCallbackEventSource()->SendEvent(&modEvent);
                }
            }
        } catch (const std::exception& e) {
            logger::error("Exception in SetCurrentStageID_Hook: {}", e.what());
        } catch (...) {
            logger::error("Unknown exception in SetCurrentStageID_Hook");
        }
    }

    void InstallQuestStageHook() {
        try {
            REL::Relocation<std::uintptr_t> target{REL::ID(24478)};  // Replace with the correct ID
            auto& trampoline = SKSE::GetTrampoline();
            trampoline.write_call<5>(target.address(), SetCurrentStageID_Hook);
            logger::info("QuestStageHook installed successfully");
        } catch (const std::exception& e) {
            logger::error("Exception in InstallQuestStageHook: {}", e.what());
        } catch (...) {
            logger::error("Unknown exception in InstallQuestStageHook");
        }
    }

    // Hook function for quest objective completion
    void SetObjectiveCompleted_Hook(RE::TESQuest* quest, std::uint32_t objectiveIndex, bool completed) {
        logger::info("SetObjectiveCompleted_Hook called");
        try {
            static auto original = REL::Relocation<decltype(SetObjectiveCompleted_Hook)*>{REL::ID(24479)};  // Replace with the correct ID
            original(quest, objectiveIndex, completed);

            if (quest && completed) {
                auto editorID = quest->GetFormEditorID();
                if (editorID) {
                    std::string message = std::format("Objective {} of quest '{}' completed", objectiveIndex, editorID);
                    RE::ConsoleLog::GetSingleton()->Print(message.c_str());
                    logger::info("Objective {} of quest '{}' completed", objectiveIndex, editorID);

                    SKSE::ModCallbackEvent modEvent{"VrelkObjectiveCompleted", editorID, static_cast<float>(objectiveIndex), nullptr};
                    SKSE::GetModCallbackEventSource()->SendEvent(&modEvent);
                }
            }
        } catch (const std::exception& e) {
            logger::error("Exception in SetObjectiveCompleted_Hook: {}", e.what());
        } catch (...) {
            logger::error("Unknown exception in SetObjectiveCompleted_Hook");
        }
    }

    void InstallObjectiveCompletedHook() {
        try {
            REL::Relocation<std::uintptr_t> target{REL::ID(24479)};  // Replace with the correct ID
            auto& trampoline = SKSE::GetTrampoline();
            trampoline.write_call<5>(target.address(), SetObjectiveCompleted_Hook);
            logger::info("ObjectiveCompletedHook installed successfully");
        } catch (const std::exception& e) {
            logger::error("Exception in InstallObjectiveCompletedHook: {}", e.what());
        } catch (...) {
            logger::error("Unknown exception in InstallObjectiveCompletedHook");
        }
    }
}  // namespace plugin::QuestHooks