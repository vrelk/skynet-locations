#pragma once

#include <string>
#include "RE/Skyrim.h"

namespace plugin {
    void ForEachReferenceInRange(RE::TESObjectREFR* origin, float radius,
                                 std::function<RE::BSContainer::ForEachResult(RE::TESObjectREFR& ref)> callback);

    int FloatToInt(RE::StaticFunctionTag*, float a_value);
    bool IsPlayer(RE::StaticFunctionTag*, RE::Actor* a_actor);
    std::string GetActorName(RE::StaticFunctionTag*, RE::Actor* a_actor);
    //std::string PapyrusJsonEncStrArr(RE::StaticFunctionTag*, RE::BSScript::VMArray<RE::BSFixedString> stringArray);
    std::string PapyrusJsonEncJArray(RE::StaticFunctionTag*, int32_t jArrayHandle);
    std::string PapyrusJsonEncJMap(RE::StaticFunctionTag*, int32_t jMapHandle);
}  // namespace plugin