#pragma once

#include "RE/Skyrim.h"

namespace plugin::PapyrusFunctions {
    bool RegisterFunctions(RE::BSScript::IVirtualMachine* vm);
    RE::BSFixedString GetQuestDescription(RE::StaticFunctionTag*, std::string quest_eid);
    RE::BSFixedString GetStageDescription(RE::StaticFunctionTag*, std::string quest_eid, int stage);
    RE::BSFixedString GetObjectiveDescription(RE::StaticFunctionTag*, std::string quest_eid, int objective);
    RE::BSFixedString GetSceneDescription(RE::StaticFunctionTag*, std::string scene_eid, int phase, bool exactMatch);

    // testing functions
    void InsertCellPlaceholder(RE::StaticFunctionTag*, const std::string mod_name, int form_id);
    void InsertLocationPlaceholder(RE::StaticFunctionTag*, const std::string mod_name, const std::string editor_id);
    void InsertWorldspacePlaceholder(RE::StaticFunctionTag*, const std::string mod_name, const std::string editor_id);
}