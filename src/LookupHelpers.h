#pragma once

#include <string>
#include <tuple>

namespace plugin::LookupHelpers {
    struct FormResult {
            bool isError = false;
            std::string errorMessage;
            std::string formID;
            std::string editorID;
            std::string name;
            std::string sourceMod;
            /** @brief Generates a mod key in the format "modName|formID".
             *
             * This function constructs a mod key by concatenating the source mod name
             * and the form ID, separated by a pipe character ("|"). If the form ID is empty,
             * an empty string is returned.
             *
             * @return A string representing the mod key, or an empty string if formID is empty.
            */
            std::string ToModKey() const {
                if (formID.empty()) {
                    return "";
                }
                return sourceMod + "|" + formID;
            }
    };
    //FormResult GetActorWorldspaceData(RE::Actor* actor);
    std::string GetFormModName(const RE::TESForm* form, bool lastModified = false);
    std::string GetModNameFromFormID(uint32_t formID, bool lastModified = false);
    FormResult GetActorLocationData(RE::Actor* actor);
    FormResult GetActorCellData(RE::Actor* actor);
    float GetGlobalValueByName(const std::string& globalName);
    int GetGlobalIntValueByName(const std::string& globalName);
    RE::Actor* GetActorPtrByModAndFormIDString(const std::string& modName, const std::string& formIDStr);
    int GetSex(RE::Actor* actor);
    std::string GetActorAdj(RE::Actor* actor);
}  // namespace plugin::LookupHelpers