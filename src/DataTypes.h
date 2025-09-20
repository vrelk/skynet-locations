#pragma once

#include <string>
#include <json.hpp>  // nlohmann::json

namespace plugin::DataTypes {
    struct LocationLookupResult {
            bool found = false;
            std::string name;
            std::string description;

            std::string ToJSON() const;
    };

    struct CommandInfo {
            const char* longName;
            const char* shortName;
            uint16_t opcode;
            uint16_t pad;
            const char* helpText;
            bool needsParent;
            bool isRefRequired;
            void* paramInfo;
            uint32_t numParams;
            void (*execute)(void* paramInfo, void* scriptData, RE::TESObjectREFR* thisObj, RE::TESObjectREFR* containingObj,
                            RE::Script* scriptObj, RE::ScriptLocals* locals, double& result, uint32_t& opcodeOffsetPtr);
            void* parse;
            void* eval;
            uint32_t flags;
    };

    struct ActorLocation {
            std::string cellFormID;
            std::string cellEditorID;
            std::string cellName;

            std::string worldSpaceFormID;
            std::string worldSpaceEditorID;
            std::string worldSpaceName;

            std::string locationFormID;
            std::string locationEditorID;
            std::string locationName;

            float x;
            float y;
            float z;

            std::string ToJSON() const;
    };

    struct FormResult {
            bool isError = false;
            std::string errorMessage;

            std::string formID;
            std::string editorID;
            std::string name;
            std::string sourceMod;

            std::string ToJSON() const {
                nlohmann::json j = {{"isError", isError}, {"errorMessage", errorMessage}, {"formID", formID}, {"editorID", editorID},
                                    {"name", name},       {"sourceMod", sourceMod}};
                return j.dump();
            }
    };

    // nlohmann::json support
    inline void to_json(nlohmann::json& j, const LocationLookupResult& v) {
        j = nlohmann::json{{"found", v.found}, {"name", v.name}, {"description", v.description}};
    }

    inline void to_json(nlohmann::json& j, const ActorLocation& v) {
        j = nlohmann::json{{"cellFormID", v.cellFormID},
                           {"cellEditorID", v.cellEditorID},
                           {"cellName", v.cellName},
                           {"worldSpaceFormID", v.worldSpaceFormID},
                           {"worldSpaceEditorID", v.worldSpaceEditorID},
                           {"worldSpaceName", v.worldSpaceName},
                           {"locationFormID", v.locationFormID},
                           {"locationEditorID", v.locationEditorID},
                           {"locationName", v.locationName},
                           {"x", v.x},
                           {"y", v.y},
                           {"z", v.z}};
    }

    inline void to_json(nlohmann::json& j, const FormResult& v) {
        j = nlohmann::json{{"isError", v.isError}, {"errorMessage", v.errorMessage}, {"formID", v.formID}, {"editorID", v.editorID},
                           {"name", v.name},       {"sourceMod", v.sourceMod}};
    }

    // CommandInfo is not trivially serializable due to pointers and function pointers, so skip it unless you need specific fields.

}  // namespace plugin::DataTypes