#pragma once

#include "RE/Skyrim.h"

namespace plugin::PapyrusFunctions {
    bool RegisterFunctions(RE::BSScript::IVirtualMachine* vm);
    RE::BSFixedString GetQuestDescription(RE::StaticFunctionTag*, const std::string quest_eid);
    RE::BSFixedString GetStageDescription(RE::StaticFunctionTag*, const std::string quest_eid, int stage);
    RE::BSFixedString GetObjectiveDescription(RE::StaticFunctionTag*, const std::string quest_eid, int objective);
    RE::BSFixedString GetSceneDescription(RE::StaticFunctionTag*, const std::string scene_eid, int phase, bool exactMatch);

    RE::BSFixedString GetLocationDescriptionByActor(RE::StaticFunctionTag*, RE::Actor* actor);
    RE::BSFixedString GetLocationDescriptionByEID(RE::StaticFunctionTag*, const std::string locationEditorID);

    // testing functions
    void InsertCellPlaceholder(RE::StaticFunctionTag*, const std::string mod_name, int form_id, std::string name = "");
    void InsertLocationPlaceholder(RE::StaticFunctionTag*, const std::string mod_name, const std::string editor_id, std::string name = "");
    void InsertWorldspacePlaceholder(RE::StaticFunctionTag*, const std::string mod_name, const std::string editor_id,
                                     std::string name = "");

    RE::BSFixedString GetActorBirthday(RE::StaticFunctionTag*, RE::Actor* actor, bool thirdPerson = false);
}  // namespace plugin::PapyrusFunctions