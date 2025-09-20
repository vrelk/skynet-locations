#include "LookupHelpers.h"
#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"
#include "VrelkUtil.h"

namespace plugin::LookupHelpers {
    /**
     * @struct FormResult
     * @brief Represents the result of a form lookup operation.
     *
     * This structure is used to encapsulate the result of a form lookup,
     * including whether an error occurred, an error message if applicable,
     * and details about the form such as its ID, editor ID, and name.
     *
     * @var isError
     * Indicates whether an error occurred during the lookup.
     *
     * @var errorMessage
     * Contains the error message if an error occurred. Empty if no error.
     *
     * @var formID
     * The unique identifier of the form.
     *
     * @var editorID
     * The editor ID of the form, if available.
     *
     * @var name
     * The name of the form, if available.
     * 
     * @var sourceMod
     * The name of the mod file that defines the form (original, not overwrites).
     */
    // MARK: - FormResult
    // struct FormResult --> In header file

    /**
     * @brief Retrieves the editor ID and form ID of the worldspace associated with the given actor.
     *
     * This function determines the worldspace in which the specified actor is located
     * and retrieves its editor ID, form ID, and name. If the actor, its parent cell, or
     * the associated worldspace is null, appropriate error messages are logged and returned.
     *
     * @param actor A pointer to the RE::Actor object whose worldspace information is to be retrieved.
     * @return FormResult A structure containing the following:
     *         - isError: A boolean indicating whether an error occurred.
     *         - errorMessage: A string describing the error, if any.
     *         - formID: The hexadecimal form ID of the worldspace (as a string).
     *         - editorID: The editor ID of the worldspace.
     *         - name: The name of the worldspace, or null if not available.
     *         - sourceMod: The name of the mod file that owns the worldspace form.
     *
     * @note Logs warnings if the actor, parent cell, or worldspace is null, or if the worldspace lacks an editor ID.
     */
    // MARK: - GetActorWorldspaceData
    //    FormResult GetActorWorldspaceData(RE::Actor* actor) {
    //        if (!actor) {
    //            SKSE::log::warn("[LookupHelpers::GetActorWorldspaceData] Actor is null.");
    //            return {.isError = true, .errorMessage = "Actor is null."};
    //        }
    //
    //        // Get the parent cell of the actor
    //        auto* cell = actor->GetParentCell();
    //        if (!cell) {
    //            SKSE::log::warn("[LookupHelpers::GetActorWorldspaceData] Actor has no parent cell.");
    //            return {.isError = true, .errorMessage = "Actor has no parent cell."};
    //        }
    //
    //        // Explicitly cast actor to RE::TESObjectREFR to access GetWorldSpace
    //        auto* worldspace = static_cast<RE::TESObjectREFR*>(actor)->GetWorldSpace();
    //        //if (!worldspace) {
    //        //    SKSE::log::warn("[LookupHelpers::GetActorWorldspaceData] Cell has no associated worldspace.");
    //        //    return {.isError = true, .errorMessage = "Cell has no associated worldspace."};
    //        //}
    //
    //        // Get the editor ID of the worldspace
    //        auto editorID = worldspace->GetFormEditorID();
    //        if (!editorID || editorID.empty()) {
    //            SKSE::log::warn("[LookupHelpers::GetActorWorldspaceData] Worldspace has no editor ID.");
    //        }
    //
    //        return {.isError = false,
    //                .errorMessage = "",
    //                .formID = VrelkUtil::IntToString(worldspace->GetFormID()),
    //                .editorID = editorID,
    //                .name = worldspace->GetName() ? worldspace->GetName() : "",
    //                .sourceMod = GetFormModName(worldspace, false)};
    //    }

    /**
     * @brief Retrieves the name of the mod file associated with the given form.
     *
     * This function returns the name of the mod file that owns the specified form.
     * If the form is null, a warning is logged. The function can optionally return
     * the name of the last modified file associated with the form.
     *
     * @param form A pointer to the TESForm object whose mod file name is to be retrieved.
     *               If this is null, a warning is logged and the function returns an empty string.
     * @param lastModified A boolean indicating whether to retrieve the name of the last modified file.
     *                       If true, the function retrieves the name of the file that last modified the form.
     *                       If false, it retrieves the name of the primary file associated with the form.
     * @return A string containing the name of the mod file, or an empty string if no file is found.
     */
    // MARK: - GetFormModName
    std::string GetFormModName(const RE::TESForm* form, bool lastModified) {
        if (!form) {
            logger::warn("GetFormModName called with null form");
            return "";  // Return an empty string if the form is null
        }

        if (const auto file = lastModified ? form->GetDescriptionOwnerFile() : form->GetFile(0)) {
            return std::string(file->GetFilename());  // Convert std::string_view to std::string
        }

        return "";  // Return an empty string if no file is found
    }

    /**
     * @brief Retrieves the editor ID of the actor's current location.
     *
     * This function determines the location in which the specified actor is currently located
     * and retrieves its editor ID. If the actor, its parent cell, or the associated location
     * is null, appropriate error messages are logged and returned.
     *
     * @param actor A pointer to the RE::Actor object whose location information is to be retrieved.
     * @return FormResult A structure containing the following:
     *         - isError: A boolean indicating whether an error occurred.
     *         - errorMessage: A string describing the error, if any.
     *         - formID: The form ID of the location (as a string).
     *         - editorID: The editor ID of the location.
     *         - name: The name of the location, or null if not available.
     *         - sourceMod: The name of the mod file that owns the location form.
     */
    // MARK: - GetActorLocationData
    FormResult GetActorLocationData(RE::Actor* actor) {
        if (!actor) {
            SKSE::log::warn("[LookupHelpers::GetActorLocationData] Actor is null.");
            return {.isError = true, .errorMessage = "Actor is null."};
        }

        // Get the parent cell of the actor
        auto* cell = actor->GetParentCell();
        if (!cell) {
            SKSE::log::warn("[LookupHelpers::GetActorLocationData] Actor has no parent cell.");
            return {.isError = true, .errorMessage = "Actor has no parent cell."};
        }

        // Get the location from the cell
        auto* location = cell->GetLocation();
        if (!location) {
            SKSE::log::warn("[LookupHelpers::GetActorLocationData] Cell has no associated location.");
            return {.isError = true, .errorMessage = "Cell has no associated location."};
        }

        const char* editorIDRaw = location->GetFormEditorID();
        std::string editorID = editorIDRaw ? std::string(editorIDRaw) : "";  // Convert to std::string and handle nullptr
        if (editorID.empty()) {
            SKSE::log::warn("[LookupHelpers::GetActorLocationData] Location has no editor ID.");
        }

        return {.isError = false,
                .errorMessage = "",
                .formID = VrelkUtil::IntToString(location->GetFormID()),
                .editorID = editorID,
                .name = location->GetName() ? location->GetName() : "",
                .sourceMod = GetFormModName(location, false)};
    }

    /**
     * @brief Retrieves the editor ID of the actor's current location.
     *
     * This function determines the location in which the specified actor is currently located
     * and retrieves its editor ID. If the actor, its parent cell, or the associated location
     * is null, appropriate error messages are logged and returned.
     *
     * @param actor A pointer to the RE::Actor object whose location information is to be retrieved.
     * @return FormResult A structure containing the following:
     *         - isError: A boolean indicating whether an error occurred.
     *         - errorMessage: A string describing the error, if any.
     *         - formID: The form ID of the location (as a string).
     *         - editorID: The editor ID of the location.
     *         - name: The name of the location, or null if not available.
     *         - sourceMod: The name of the mod file that owns the location form.
     */
    // MARK: - GetActorCellData
    FormResult GetActorCellData(RE::Actor* actor) {
        if (!actor) {
            SKSE::log::warn("[LookupHelpers::GetActorLocationData] Actor is null.");
            return {.isError = true, .errorMessage = "Actor is null."};
        }

        // Get the parent cell of the actor
        auto* cell = actor->GetParentCell();
        if (!cell) {
            SKSE::log::warn("[LookupHelpers::GetActorLocationData] Actor has no parent cell.");
            return {.isError = true, .errorMessage = "Actor has no parent cell."};
        }

        // Get the editor ID of the Cell
        const char* editorIDRaw = cell->GetFormEditorID();
        std::string editorID = editorIDRaw ? std::string(editorIDRaw) : "";  // Convert to std::string and handle nullptr
        if (editorID.empty()) {
            SKSE::log::warn("[LookupHelpers::GetActorLocationData] Cell has no editor ID.");
        }

        return {.isError = false,
                .errorMessage = "",
                .formID = VrelkUtil::IntToString(cell->GetFormID()),
                .editorID = editorID,
                .name = cell->GetName() ? std::string(cell->GetName()) : "",  // Convert std::string_view to std::string
                .sourceMod = GetFormModName(cell, false)};
    }

    float GetGlobalValueByName(const std::string& globalName) {
        auto dataHandler = RE::TESDataHandler::GetSingleton();
        if (!dataHandler) {
            return 0.0f;
        }

        RE::TESForm* form = RE::TESForm::LookupByEditorID(globalName);
        if (!form) {
            logger::error("Global variable '{}' not found!", globalName);
            return 0.0f;
        }

        auto global = form->As<RE::TESGlobal>();
        if (!global) {
            logger::error("Form '{}' is not a TESGlobal!", globalName);
            return 0.0f;
        }

        return global->value;
    }

    int GetGlobalIntValueByName(const std::string& globalName) {
        return static_cast<int>(GetGlobalValueByName(globalName));
    }

    /**
     * @brief Retrieves the mod name from an integer form ID.
     *
     * @param formID The integer form ID.
     * @param lastModified If true, returns the last modified file name; otherwise, the original mod.
     * @return The mod name as a string, or empty if not found.
     */
    std::string GetModNameFromFormID(uint32_t formID, bool lastModified) {
        const RE::TESForm* form = RE::TESForm::LookupByID(formID);
        if (!form) {
            logger::warn("GetModNameFromFormID: No form found for ID {:08X}", formID);
            return "";
        }
        return GetFormModName(form, lastModified);
    }

    /**
     * @brief Retrieves an RE::Actor* from a mod name and form ID string.
     *
     * @param modName The name of the mod (e.g., "Skyrim.esm").
     * @param formIDStr The form ID as a string (e.g., "0x123ABC" or "123ABC").
     * @return RE::Actor* pointer if found, nullptr if not found or error.
     */
    RE::Actor* GetActorPtrByModAndFormIDString(const std::string& modName, const std::string& formIDStr) {
        uint32_t formID = 0;
        try {
            std::string cleaned = formIDStr;
            if (cleaned.rfind("0x", 0) == 0 || cleaned.rfind("0X", 0) == 0) {
                cleaned = cleaned.substr(2);
            }
            formID = static_cast<uint32_t>(std::stoul(cleaned, nullptr, 16));
        } catch (...) {
            return nullptr;
        }

        auto dataHandler = RE::TESDataHandler::GetSingleton();
        if (!dataHandler) {
            return nullptr;
        }

        auto* modFile = dataHandler->LookupModByName(modName);
        if (!modFile) {
            return nullptr;
        }

        uint8_t modIndex = modFile->GetPartialIndex();
        if (modIndex == 0xFF) {
            return nullptr;
        }
        uint32_t actualFormID = (modIndex << 24) | (formID & 0x00FFFFFF);

        auto* form = RE::TESForm::LookupByID(actualFormID);
        if (!form) {
            return nullptr;
        }

        return form->As<RE::Actor>();
    }

}  // namespace plugin::LookupHelpers