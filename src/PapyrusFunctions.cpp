#include "PapyrusFunctions.h"
#include "HTTPClient.h"
#include "DatabaseFunctions.h"
#include "VrelkUtil.h"

namespace plugin::PapyrusFunctions {

    bool RegisterFunctions(RE::BSScript::IVirtualMachine* vm) {
        if (!vm) {
            return false;
        }

        vm->RegisterFunction("httpGetAsync", "VrelkHttpClient", PapyrusHttpGet);
        vm->RegisterFunction("httpPostAsync", "VrelkHttpClient", PapyrusHttpPost);

        //vm->RegisterFunction("getCellDescription", "VrelkHttpClient", plugin::DatabaseFunctions::GetCellDescription);
        //vm->RegisterFunction("getWorldspaceDescription", "VrelkHttpClient", plugin::DatabaseFunctions::GetWorldspaceDescription);
        //vm->RegisterFunction("getCellByFormIDDescription", "VrelkHttpClient", plugin::DatabaseFunctions::GetCellByFormIDDescription);
        //vm->RegisterFunction("getWorldspaceByFormIDDescription", "VrelkHttpClient", plugin::DatabaseFunctions::GetWorldspaceByFormIDDescription);

        vm->RegisterFunction("getQuestDescription", "VrelkHttpClient", GetQuestDescription);
        vm->RegisterFunction("getStageDescription", "VrelkHttpClient", GetStageDescription);
        vm->RegisterFunction("getObjectiveDescription", "VrelkHttpClient", GetObjectiveDescription);
        vm->RegisterFunction("getSceneDescription", "VrelkHttpClient", GetSceneDescription);

        // testing functions
        vm->RegisterFunction("insertCellPlaceholder", "VrelkHttpClient", InsertCellPlaceholder);
        vm->RegisterFunction("insertLocationPlaceholder", "VrelkHttpClient", InsertLocationPlaceholder);
        vm->RegisterFunction("insertWorldspacePlaceholder", "VrelkHttpClient", InsertWorldspacePlaceholder);

        return true;
    }

    void InsertCellPlaceholder(RE::StaticFunctionTag*, const std::string mod_name, int form_id) {
        std::string form_id_str = VrelkUtil::IntToString(form_id);
        plugin::DatabaseFunctions::AddPlaceholderCell(mod_name, form_id_str);
        std::string message = std::format("Inserted placeholder cell: mod_name = {}, form_id = {}", mod_name, form_id_str);
        RE::ConsoleLog::GetSingleton()->Print(message.c_str());
    }

    void InsertLocationPlaceholder(RE::StaticFunctionTag*, const std::string mod_name, const std::string editor_id) {
        plugin::DatabaseFunctions::AddPlaceholderLocation(mod_name, editor_id);
        std::string message = std::format("Inserted placeholder location: mod_name = {}, editor_id = {}", mod_name, editor_id);
        RE::ConsoleLog::GetSingleton()->Print(message.c_str());
    }

    void InsertWorldspacePlaceholder(RE::StaticFunctionTag*, const std::string mod_name, const std::string editor_id) {
        plugin::DatabaseFunctions::AddPlaceholderWorldspace(mod_name, editor_id);
        std::string message = std::format("Inserted placeholder worldspace: mod_name = {}, editor_id = {}", mod_name, editor_id);
        RE::ConsoleLog::GetSingleton()->Print(message.c_str());
    }

    RE::BSFixedString GetQuestDescription(RE::StaticFunctionTag*, std::string quest_eid) {
        return plugin::DatabaseFunctions::GetQuestDescription(quest_eid);
    }

    RE::BSFixedString GetStageDescription(RE::StaticFunctionTag*, std::string quest_eid, int stage) {
        return plugin::DatabaseFunctions::GetStageDescription(quest_eid, stage);
    }

    RE::BSFixedString GetObjectiveDescription(RE::StaticFunctionTag*, std::string quest_eid, int objective) {
        return plugin::DatabaseFunctions::GetObjectiveDescription(quest_eid, objective);
    }

    RE::BSFixedString GetSceneDescription(RE::StaticFunctionTag*, std::string scene_eid, int phase, bool exactMatch) {
        return plugin::DatabaseFunctions::GetSceneDescription(scene_eid, phase, exactMatch);
    }
}  // namespace plugin::PapyrusFunctions