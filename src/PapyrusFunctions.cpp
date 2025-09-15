#include "PapyrusFunctions.h"
#include "HTTPClient.h"
#include "DatabaseFunctions.h"
#include "VrelkUtil.h"
#include "SkyrimHelpers.h"
#include "LookupHelpers.h"
#include "JSON.hpp"

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
        vm->RegisterFunction("getLocationDescriptionByActor", "VrelkHttpClient", GetLocationDescriptionByActor);
        vm->RegisterFunction("getLocationDescriptionByEID", "VrelkHttpClient", GetLocationDescriptionByEID);

        // testing functions
        vm->RegisterFunction("insertCellPlaceholder", "VrelkHttpClient", InsertCellPlaceholder);
        vm->RegisterFunction("insertLocationPlaceholder", "VrelkHttpClient", InsertLocationPlaceholder);
        vm->RegisterFunction("insertWorldspacePlaceholder", "VrelkHttpClient", InsertWorldspacePlaceholder);

        vm->RegisterFunction("getActorBirthday", "VrelkHttpClient", GetActorBirthday);

        return true;
    }

    RE::BSFixedString GetLocationDescriptionByActor(RE::StaticFunctionTag*, RE::Actor* actor) {
        if (!actor) {
            return RE::BSFixedString("{\"location\":\"\",\"description\":\"\"}");
        }

        auto location = LookupHelpers::GetActorLocationData(actor);
        if (location.isError) {
            return RE::BSFixedString("{\"location\":\"\",\"description\":\"\"}");
        }

        auto result = plugin::DatabaseFunctions::GetLocationDescription(location.editorID);
        if (result.found) {
            nlohmann::json jsonResponse = {{"location", location.name}, {"description", result.description}};
            return RE::BSFixedString(jsonResponse.dump().c_str());
            //return RE::BSFixedString(result.description.c_str());
        } else {
            nlohmann::json jsonResponse = {{"location", location.name}, {"description", ""}};
            return RE::BSFixedString(jsonResponse.dump().c_str());
        }
    }

    RE::BSFixedString GetLocationDescriptionByEID(RE::StaticFunctionTag*, const std::string locationEditorID) {
        if (locationEditorID.empty()) {
            return RE::BSFixedString("{\"location\":\"\",\"description\":\"\"}");
        }

        auto result = plugin::DatabaseFunctions::GetLocationDescription(locationEditorID);
        if (result.found) {
            nlohmann::json jsonResponse = {{"location", locationEditorID}, {"description", result.description}};
            return RE::BSFixedString(jsonResponse.dump().c_str());
        } else {
            return RE::BSFixedString("{\"location\":\"\",\"description\":\"\"}");
        }
    }

    void InsertCellPlaceholder(RE::StaticFunctionTag*, const std::string mod_name, int form_id, std::string name) {
        std::string form_id_str = VrelkUtil::IntToString(form_id);
        plugin::DatabaseFunctions::AddPlaceholderCell(mod_name, form_id_str, name);
        std::string message = std::format("Inserted placeholder cell: mod_name = {}, form_id = {}", mod_name, form_id_str);
        RE::ConsoleLog::GetSingleton()->Print(message.c_str());
    }

    void InsertLocationPlaceholder(RE::StaticFunctionTag*, const std::string mod_name, const std::string editor_id, std::string name) {
        plugin::DatabaseFunctions::AddPlaceholderLocation(mod_name, editor_id, name);
        std::string message = std::format("Inserted placeholder location: mod_name = {}, editor_id = {}", mod_name, editor_id);
        RE::ConsoleLog::GetSingleton()->Print(message.c_str());
    }

    void InsertWorldspacePlaceholder(RE::StaticFunctionTag*, const std::string mod_name, const std::string editor_id, std::string name) {
        plugin::DatabaseFunctions::AddPlaceholderWorldspace(mod_name, editor_id, name);
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

    RE::BSFixedString GetActorBirthday(RE::StaticFunctionTag*, RE::Actor* actor, bool thirdPerson) {
        if (!actor) {
            return RE::BSFixedString("");
        }

        std::string actor_id = VrelkUtil::IntToString(actor->GetFormID(), true);
        if (actor_id.empty()) {
            return RE::BSFixedString("");
        }

        std::string birthday = plugin::DatabaseFunctions::GetActorBirthdayString(actor_id, thirdPerson);
        if (birthday.empty()) {
            std::string actor_base_id = VrelkUtil::IntToString(actor->GetActorBase()->GetFormID(), true);
            if (actor_base_id.empty()) {
                return RE::BSFixedString("");
            }
            birthday = plugin::DatabaseFunctions::GetActorBirthdayString(actor_base_id, thirdPerson);
            if (birthday.empty()) {
                return RE::BSFixedString("");
            }
        }
        return RE::BSFixedString(birthday.c_str());
    }

}  // namespace plugin::PapyrusFunctions