#include "HTTPClient.h"
#include "DatabaseFunctions.h"
#include "VrelkUtil.h"
#include "SkyrimHelpers.h"
#include "LookupHelpers.h"
#include <nlohmann/JSON.hpp>  // nlohmann::json
#include "DataTypes.hpp"
#include "SlaveTats.hpp"
#include <inja/inja.hpp>

namespace plugin::PapyrusFunctions {

    inline RE::BSFixedString GetLocationDescriptionByActor(RE::StaticFunctionTag*, RE::Actor* actor) {
        if (!actor) {
            return RE::BSFixedString("{\"location\":\"\",\"description\":\"\"}");
        }

        auto cell = LookupHelpers::GetActorLocationData(actor);
        if (cell.isError) {
            return RE::BSFixedString("{\"location\":\"\",\"description\":\"\"}");
        }

        auto resultCell = plugin::DatabaseFunctions::GetLocationDescription(cell.formID);
        if (resultCell.found) {
            nlohmann::json jsonResponse = {{"location", resultCell.name}, {"description", resultCell.description}};
            return RE::BSFixedString(jsonResponse.dump().c_str());
        }

        auto location = LookupHelpers::GetActorLocationData(actor);
        if (location.isError) {
            return RE::BSFixedString("{\"location\":\"\",\"description\":\"\"}");
        }

        auto resultLoc = plugin::DatabaseFunctions::GetLocationDescription(location.editorID);
        if (resultLoc.found) {
            nlohmann::json jsonResponse = {{"location", location.name}, {"description", resultLoc.description}};
            return RE::BSFixedString(jsonResponse.dump().c_str());
        } else {
            nlohmann::json jsonResponse = {{"location", location.name}, {"description", ""}};
            return RE::BSFixedString(jsonResponse.dump().c_str());
        }
    }

    inline RE::BSFixedString GetLocationDescriptionByEID(RE::StaticFunctionTag*, const std::string locationEditorID) {
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

    inline void InsertCellPlaceholder(RE::StaticFunctionTag*, const std::string mod_name, int form_id, const std::string name = "",
                                      const std::string notes = "") {
        std::string form_id_str = VrelkUtil::IntToString(form_id);
        plugin::DatabaseFunctions::AddPlaceholderCell(mod_name, form_id_str, name, notes);
        std::string message = std::format("Inserted placeholder cell: mod_name = {}, form_id = {}", mod_name, form_id_str);
        RE::ConsoleLog::GetSingleton()->Print(message.c_str());
    }

    inline void InsertLocationPlaceholder(RE::StaticFunctionTag*, const std::string mod_name, const std::string editor_id,
                                          const std::string name = "") {
        plugin::DatabaseFunctions::AddPlaceholderLocation(mod_name, editor_id, name);
        std::string message = std::format("Inserted placeholder location: mod_name = {}, editor_id = {}", mod_name, editor_id);
        RE::ConsoleLog::GetSingleton()->Print(message.c_str());
    }

    inline void InsertWorldspacePlaceholder(RE::StaticFunctionTag*, const std::string mod_name, const std::string editor_id,
                                            const std::string name = "") {
        plugin::DatabaseFunctions::AddPlaceholderWorldspace(mod_name, editor_id, name);
        std::string message = std::format("Inserted placeholder worldspace: mod_name = {}, editor_id = {}", mod_name, editor_id);
        RE::ConsoleLog::GetSingleton()->Print(message.c_str());
    }

    inline RE::BSFixedString GetQuestDescription(RE::StaticFunctionTag*, const std::string quest_eid) {
        return plugin::DatabaseFunctions::GetQuestDescription(quest_eid);
    }

    inline RE::BSFixedString GetStageDescription(RE::StaticFunctionTag*, const std::string quest_eid, int stage) {
        return plugin::DatabaseFunctions::GetStageDescription(quest_eid, stage);
    }

    inline RE::BSFixedString GetObjectiveDescription(RE::StaticFunctionTag*, const std::string quest_eid, int objective) {
        return plugin::DatabaseFunctions::GetObjectiveDescription(quest_eid, objective);
    }

    inline RE::BSFixedString GetSceneDescription(RE::StaticFunctionTag*, const std::string scene_eid, int phase, bool exactMatch = false) {
        return plugin::DatabaseFunctions::GetSceneDescription(scene_eid, phase, exactMatch);
    }

    inline RE::BSFixedString GetActorBirthday(RE::StaticFunctionTag*, RE::Actor* actor, bool thirdPerson = false) {
        try {
            if (!actor) {
                return RE::BSFixedString("{\"dob\":\"\",\"age\":\"\",\"birthday_str\":\"\"}");
            }

            std::string actor_id = VrelkUtil::IntToString(actor->GetFormID(), true);
            if (actor_id.empty()) {
                return RE::BSFixedString("{\"dob\":\"\",\"age\":\"\",\"birthday_str\":\"\"}");
            }

            std::string sourceMod = LookupHelpers::GetFormModName(actor, false);

            std::string birthday = plugin::DatabaseFunctions::GetActorBirthdayString(sourceMod, actor_id, thirdPerson);
            if (birthday.empty()) {
                actor_id = VrelkUtil::IntToString(actor->GetActorBase()->GetFormID(), true);
                sourceMod = LookupHelpers::GetFormModName(actor->GetActorBase(), false);
                if (actor_id.empty()) {
                    return RE::BSFixedString("{\"dob\":\"\",\"age\":\"\",\"birthday_str\":\"\"}");
                }
                birthday = plugin::DatabaseFunctions::GetActorBirthdayString(sourceMod, actor_id, thirdPerson);
                if (birthday.empty()) {
                    return RE::BSFixedString("{\"dob\":\"\",\"age\":\"\",\"birthday_str\":\"\"}");
                }
            }
            return RE::BSFixedString(birthday.c_str());
        } catch (const std::exception& e) {
            logger::error("Exception in GetActorBirthday: {}", e.what());
            return RE::BSFixedString("{\"dob\":\"\",\"age\":\"\",\"birthday_str\":\"\"}");
        }
    }

    inline RE::BSFixedString GetSlaveInfo(RE::StaticFunctionTag*, RE::Actor* actor) {
        try {
            if (!actor) {
                return RE::BSFixedString("{}");
            }

            std::string actor_id = VrelkUtil::IntToString(actor->GetFormID(), true);
            if (actor_id.empty()) {
                return RE::BSFixedString("{}");
            }

            std::string sourceMod = LookupHelpers::GetFormModName(actor, false);

            nlohmann::json slaveInfo = plugin::DatabaseFunctions::GetCustomSlaveEntry(sourceMod, actor_id);
            if (slaveInfo.empty()) {
                return RE::BSFixedString("{}");
            }

            std::string ownerMod = slaveInfo.value("owner_mod", "");
            std::string ownerId = slaveInfo.value("owner_id", "");

            RE::Actor* ownerActor = nullptr;

            if (!ownerMod.empty() && !ownerId.empty()) {
                ownerActor = plugin::LookupHelpers::GetActorPtrByModAndFormIDString(ownerMod, ownerId);
                slaveInfo["owner_name"] = ownerActor ? SkyrimHelpers::GetActorDisplayName(ownerActor, "") : "";
            }

            slaveInfo["actor_name"] = SkyrimHelpers::GetActorDisplayName(actor, "Slave");

            /*slaveInfo["slave_status_str"] = switch(slaveInfo.value("slave_status", 0)) {
                    case 0: yield "free";
                    case 1: yield "owned";
                    case 2: yield "ex-slave";
                    default: yield "unknown";
                };*/

            if (slaveInfo.contains("collar_text")) {
                /*
                std::string collarText = slaveInfo["collar_text"];
                size_t pos = collarText.find("{{owner.name}}");
                if (pos != std::string::npos) {
                    if (ownerActor) {
                        std::string ownerName = SkyrimHelpers::GetActorDisplayName(ownerActor, "Mistress");
                        collarText.replace(pos, std::string("{{owner.name}}").length(), ownerName);
                    } else {
                        collarText.replace(pos, std::string("{{owner.name}}").length(), "Mistress");
                    }
                }
                slaveInfo["collar_text"] = collarText;
                */

                slaveInfo["collar_text"] =
                    inja::render(slaveInfo["collar_text"], {{"actor_name", slaveInfo["actor_name"] ? slaveInfo["actor_name"] : "Slave"},
                                                            {"owner_name", slaveInfo["owner_name"] ? slaveInfo["owner_name"] : "Mistress"},
                                                            {"id", slaveInfo.contains("slave_id") ? slaveInfo["slave_id"].get<int>() : 0}});
            }

            nlohmann::json outputObj;
            outputObj["slave_name"] = (slaveInfo.contains("actor_name") && !slaveInfo["actor_name"].is_null())
                                          ? slaveInfo["actor_name"].get<std::string>()
                                          : "Slave";
            outputObj["owner_name"] = (slaveInfo.contains("owner_name") && !slaveInfo["owner_name"].is_null())
                                          ? slaveInfo["owner_name"].get<std::string>()
                                          : "Master";
            outputObj["status"] = (slaveInfo.contains("slave_status_str") && !slaveInfo["slave_status_str"].is_null())
                                      ? slaveInfo["slave_status_str"].get<std::string>()
                                      : "unknown";

            outputObj["collar_text"] = (slaveInfo.contains("collar_text") && !slaveInfo["collar_text"].is_null())
                                           ? slaveInfo["collar_text"].get<std::string>()
                                           : "";
            outputObj["id"] = slaveInfo.contains("slave_id") ? slaveInfo["slave_id"].get<int>() : 0;

            return RE::BSFixedString(outputObj.dump().c_str());
        } catch (const std::exception& e) {
            logger::error("Exception in GetSlaveInfo: {}", e.what());
            return RE::BSFixedString("{}");
        }
    }

    inline RE::BSFixedString GetActorTattoosJson(RE::StaticFunctionTag*, RE::Actor* actor, const std::string area = "") {
        try {
            if (!actor) {
                return RE::BSFixedString("[]");
            }

            auto tattoos = SlaveTats::GetActorTattoos(actor, area);
            return RE::BSFixedString(nlohmann::json(tattoos).dump().c_str());
        } catch (const std::exception& e) {
            logger::error("Exception in GetActorTattoos: {}", e.what());
            return RE::BSFixedString("[]");
        }
    }

    inline bool RegisterFunctions(RE::BSScript::IVirtualMachine* vm) {
        if (!vm) {
            return false;
        }

        vm->RegisterFunction("httpGetAsync", "VrelkHttpClient", PapyrusHttpGet);
        vm->RegisterFunction("httpPostAsync", "VrelkHttpClient", PapyrusHttpPost);

        //vm->RegisterFunction("getCellDescription", "VrelkHttpClient", plugin::DatabaseFunctions::GetCellDescription);
        //vm->RegisterFunction("getWorldspaceDescription", "VrelkHttpClient", plugin::DatabaseFunctions::GetWorldspaceDescription);
        //vm->RegisterFunction("getCellByFormIDDescription", "VrelkHttpClient", plugin::DatabaseFunctions::GetCellByFormIDDescription);
        //vm->RegisterFunction("getWorldspaceByFormIDDescription", "VrelkHttpClient", plugin::DatabaseFunctions::GetWorldspaceByFormIDDescription);

        vm->RegisterFunction("getQuestDescription", "SkynetDetailsAPI", GetQuestDescription);
        vm->RegisterFunction("getStageDescription", "SkynetDetailsAPI", GetStageDescription);
        vm->RegisterFunction("getObjectiveDescription", "SkynetDetailsAPI", GetObjectiveDescription);
        vm->RegisterFunction("getSceneDescription", "SkynetDetailsAPI", GetSceneDescription);
        vm->RegisterFunction("getLocationDescriptionByActor", "SkynetDetailsAPI", GetLocationDescriptionByActor);
        vm->RegisterFunction("getLocationDescriptionByEID", "SkynetDetailsAPI", GetLocationDescriptionByEID);

        // testing functions
        vm->RegisterFunction("insertCellPlaceholder", "SkynetDetailsAPI", InsertCellPlaceholder);
        vm->RegisterFunction("insertLocationPlaceholder", "SkynetDetailsAPI", InsertLocationPlaceholder);
        vm->RegisterFunction("insertWorldspacePlaceholder", "SkynetDetailsAPI", InsertWorldspacePlaceholder);

        vm->RegisterFunction("getActorBirthday", "SkynetDetailsAPI", GetActorBirthday);
        vm->RegisterFunction("getSlaveInfo", "SkynetDetailsAPI", GetSlaveInfo);
        vm->RegisterFunction("getActorTattoosJson", "SkynetDetailsAPI", GetActorTattoosJson);

        return true;
    }
}  // namespace plugin::PapyrusFunctions