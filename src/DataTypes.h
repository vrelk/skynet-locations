#pragma once

#include <string>
#include <json.hpp>  // Include this if you use nlohmann::json in the struct

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

}  // namespace plugin::DataTypes