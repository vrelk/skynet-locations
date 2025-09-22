#pragma once

#include <string>
#include <nlohmann/JSON.hpp>  // nlohmann::json

namespace plugin::DataTypes {

    // MARK: LocationLookupResult
    struct LocationLookupResult {
            bool found = false;
            std::string name;
            std::string description;

            std::string ToJSON() const {
                if (!found) {
                    return nlohmann::json({{"found", found}, {"name", ""}, {"description", ""}}).dump();
                }
                return nlohmann::json{{"found", found}, {"name", name}, {"description", description}}.dump();
            }
    };
    // nlohmann::json support
    inline void to_json(nlohmann::json& j, const LocationLookupResult& v) {
        j = nlohmann::json{{"found", v.found}, {"name", v.name}, {"description", v.description}};
    }

    //struct CommandInfo {
    //        const char* longName;
    //        const char* shortName;
    //        uint16_t opcode;
    //        uint16_t pad;
    //        const char* helpText;
    //        bool needsParent;
    //        bool isRefRequired;
    //        void* paramInfo;
    //        uint32_t numParams;
    //        void (*execute)(void* paramInfo, void* scriptData, RE::TESObjectREFR* thisObj, RE::TESObjectREFR* containingObj,
    //                        RE::Script* scriptObj, RE::ScriptLocals* locals, double& result, uint32_t& opcodeOffsetPtr);
    //        void* parse;
    //        void* eval;
    //        uint32_t flags;
    //};

    // MARK: ActorLocation
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

            std::string ToJSON() const {
                nlohmann::json j = {{"cellFormID", cellFormID},
                                    {"cellEditorID", cellEditorID},
                                    {"cellName", cellName},
                                    {"worldSpaceFormID", worldSpaceFormID},
                                    {"worldSpaceEditorID", worldSpaceEditorID},
                                    {"worldSpaceName", worldSpaceName},
                                    {"locationFormID", locationFormID},
                                    {"locationEditorID", locationEditorID},
                                    {"locationName", locationName},
                                    {"x", x},
                                    {"y", y},
                                    {"z", z}};
                return j.dump();
            }
    };
    // nlohmann::json support
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

    // MARK: FormResult
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
    inline void to_json(nlohmann::json& j, const FormResult& v) {
        j = nlohmann::json{{"isError", v.isError}, {"errorMessage", v.errorMessage}, {"formID", v.formID}, {"editorID", v.editorID},
                           {"name", v.name},       {"sourceMod", v.sourceMod}};
    }

    // MARK: SlaveTatsAvailableTat
    struct SlaveTatsAvailableTat {
            std::string area;  // Body, Face, Hands, Feet
            std::string name;
            std::string section;  // Usually the pack name
            std::string texture;
            std::string tags;  // Comma-separated tags

            std::vector<std::string> GetTagsArray() const {
                std::vector<std::string> result;
                size_t start = 0, end = 0;
                while ((end = tags.find(',', start)) != std::string::npos) {
                    auto tag = tags.substr(start, end - start);
                    if (!tag.empty())
                        result.push_back(tag);
                    start = end + 1;
                }
                auto lastTag = tags.substr(start);
                if (!lastTag.empty())
                    result.push_back(lastTag);
                return result;
            }

            std::string ToJSON() const {
                nlohmann::json j = {{"area", area}, {"name", name}, {"section", section}, {"texture", texture}, {"tags", GetTagsArray()}};
                return j.dump();
            }
    };
    // nlohmann::json support
    inline void to_json(nlohmann::json& j, const SlaveTatsAvailableTat& v) {
        j = nlohmann::json{{"area", v.area}, {"name", v.name}, {"section", v.section}, {"texture", v.texture}, {"tags", v.GetTagsArray()}};
    }

    // MARK: SlaveTatsAppliedTat
    struct SlaveTatsAppliedTat {
            std::string area;  // Body, Face, Hands, Feet
            std::string name;
            std::string section;  // Usually the pack name
            int slot;
            std::string texture;
            std::string description;

            std::string ToJSON() const {
                nlohmann::json j = {{"area", area}, {"name", name},       {"section", section},
                                    {"slot", slot}, {"texture", texture}, {"description", description}};
                return j.dump();
            }
    };
    // nlohmann::json support
    inline void to_json(nlohmann::json& j, const SlaveTatsAppliedTat& v) {
        j = nlohmann::json{{"area", v.area}, {"name", v.name},       {"section", v.section},
                           {"slot", v.slot}, {"texture", v.texture}, {"description", v.description}};
    }

}  // namespace plugin::DataTypes