#pragma once
#include "RE/Skyrim.h"
#include <vector>
#include <string>

namespace plugin::SkyrimHelpers {
    float GetMaxActorValue(RE::Actor* actor, RE::ActorValue value);
    float GetActorValue(RE::Actor* actor, RE::ActorValue value);
    float GetPercentageAV(RE::Actor* actor, RE::ActorValue av);
    RE::PlayerCharacter* GetPlayer();
    bool IsPlayerLoaded();
    bool IsActorLoaded(RE::Actor* actor);
    std::string GetActorDisplayName(RE::Actor* actor, const std::string& fallback = "");

    // Faction functions
    std::int32_t GetFactionRank(RE::Actor* actor, RE::TESFaction* faction);
    std::int32_t GetFactionRankByFormID(RE::Actor* actor, RE::FormID factionFormID);
    std::int32_t GetFactionRankByEditorID(RE::Actor* actor, const std::string& factionEditorID);
    bool IsInFaction(RE::Actor* actor, RE::TESFaction* faction);
    bool IsInFactionByEditorID(RE::Actor* actor, const std::string& factionEditorID);
    std::vector<std::pair<RE::TESFaction*, std::int32_t>> GetAllFactionRanks(RE::Actor* actor);

    // Form functions
    std::string GetFormEditorID(RE::TESForm* form);
    std::string GetFormEditorIDByFormID(RE::FormID formID);
    std::string GetFormEditorIDSafe(RE::TESForm* form);

    // Actor Search Functions
    std::vector<std::pair<RE::Actor*, float>> FindNearbyActors(const RE::NiPoint3& centerPoint, float radius = 4096.0f,
                                                               bool includePlayer = false);
    RE::Actor* FindActorByName(const std::string& actorName, const RE::NiPoint3& centerPoint, float radius = 4096.0f,
                               bool exactMatch = false, bool includePlayer = true);
    RE::Actor* FindActorByNameNearPlayer(const std::string& actorName, float radius = 4096.0f, bool exactMatch = false,
                                         bool includePlayer = false);
    std::vector<RE::Actor*> FindAllActorsByName(const std::string& actorName, const RE::NiPoint3& centerPoint, float radius = 4096.0f,
                                                bool exactMatch = false, bool includePlayer = true);
}  // namespace plugin::SkyrimHelpers