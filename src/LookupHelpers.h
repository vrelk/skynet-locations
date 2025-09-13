#pragma once

#include <string>
#include <tuple>

namespace plugin::LookupHelpers {
    struct FormResult;
    //FormResult GetActorWorldspaceData(RE::Actor* actor);
    std::string GetFormModName(const RE::TESForm* form, bool lastModified = false);
    FormResult GetActorLocationData(RE::Actor* actor);
    FormResult GetActorCellData(RE::Actor* actor);
}  // namespace plugin::LookupHelpers