#include "SkyrimHelpers.h"

namespace plugin::SkyrimHelpers {

    //=============================================================================
    // ACTOR VALUE FUNCTIONS
    //=============================================================================

    /**
     * @brief Gets the maximum value of an actor's attribute (base + permanent + temporary modifiers).
     * 
     * @param actor The actor to query
     * @param value The actor value to retrieve
     * @return The maximum actor value (excluding damage)
     */
    float GetMaxActorValue(RE::Actor* actor, RE::ActorValue value) {
        if (!actor)
            return 0.0f;

        auto baseValue = actor->AsActorValueOwner()->GetBaseActorValue(value);
        auto permMod = actor->GetActorValueModifier(RE::ACTOR_VALUE_MODIFIERS::kPermanent, value);
        auto tempMod = actor->GetActorValueModifier(RE::ACTOR_VALUE_MODIFIERS::kTemporary, value);

        return baseValue + permMod + tempMod;
    }

    /**
     * @brief Gets the current value of an actor's attribute (including damage).
     * 
     * @param actor The actor to query
     * @param value The actor value to retrieve
     * @return The current actor value
     */
    float GetActorValue(RE::Actor* actor, RE::ActorValue value) {
        if (!actor)
            return 0.0f;

        float baseValue = GetMaxActorValue(actor, value);
        auto damageMod = actor->GetActorValueModifier(RE::ACTOR_VALUE_MODIFIERS::kDamage, value);

        return baseValue + damageMod;
    }

    /**
     * @brief Gets the percentage of an actor value (current / max), clamped between 0.0 and 1.0.
     * 
     * @param actor The actor to query
     * @param av The actor value to calculate percentage for
     * @return Percentage as a float between 0.0 and 1.0
     */
    float GetPercentageAV(RE::Actor* actor, RE::ActorValue av) {
        if (!actor)
            return 0.0f;

        float current = GetActorValue(actor, av);
        float maximum = GetMaxActorValue(actor, av);

        if (maximum <= 0.0f)
            return 0.0f;

        return std::max(std::min(current / maximum, 1.0f), 0.0f);
    }

    //=============================================================================
    // PLAYER AND ACTOR STATE FUNCTIONS
    //=============================================================================

    /**
     * @brief Gets the player character singleton.
     * 
     * @return Pointer to the player character, or nullptr if not available
     */
    RE::PlayerCharacter* GetPlayer() {
        return RE::PlayerCharacter::GetSingleton();
    }

    /**
     * @brief Checks if the player is loaded and ready for interaction.
     * 
     * @return True if player exists, is 3D loaded, and not disabled
     */
    bool IsPlayerLoaded() {
        auto player = GetPlayer();
        return player && player->Is3DLoaded() && !player->IsDisabled();
    }

    /**
     * @brief Checks if an actor is loaded and ready for interaction.
     * 
     * @param actor The actor to check
     * @return True if actor exists, is 3D loaded, and not disabled
     */
    bool IsActorLoaded(RE::Actor* actor) {
        return actor && actor->Is3DLoaded() && !actor->IsDisabled();
    }

    /**
     * @brief Gets the display name of an actor.
     * 
     * This function retrieves the display name that appears in-game for an actor.
     * For the player, it returns the character's name. For NPCs, it returns their
     * full name. Falls back to the base form name if no display name is available.
     * 
     * @param actor The actor to get the display name from
     * @param fallback The string to return if no name is found (default: "")
     * @return The display name as a string, or "" if actor is null or has no name
     * 
     * @code
     * auto player = RE::PlayerCharacter::GetSingleton();
     * std::string playerName = plugin::SkyrimHelpers::GetActorDisplayName(player);
     * logger::info("Player name: {}", playerName);
     * @endcode
     */
    std::string GetActorDisplayName(RE::Actor* actor, const std::string& fallback) {
        if (!actor) {
            return fallback;
        }

        // Try to get the display name first (works for both player and NPCs)
        auto displayName = actor->GetDisplayFullName();
        if (displayName && strlen(displayName) > 0) {
            return std::string(displayName);
        }

        // Fall back to actor base name if display name is not available
        auto actorBase = actor->GetActorBase();
        if (actorBase) {
            auto baseName = actorBase->GetFullName();
            if (baseName && strlen(baseName) > 0) {
                return std::string(baseName);
            }
        }

        // Last resort: try editor ID
        auto editorID = GetFormEditorID(actor);
        if (!editorID.empty()) {
            return editorID;
        }

        return fallback;
    }

    /**
     * @brief Finds all nearby actors within a specified radius.
     * 
     * @param centerPoint The position to search from
     * @param radius The search radius in game units (default: 4096.0f)
     * @param includePlayer Whether to include the player in results (default: false)
     * @return Vector of pairs containing actor pointers and their distances
     */
    std::vector<std::pair<RE::Actor*, float>> FindNearbyActors(const RE::NiPoint3& centerPoint, float radius, bool includePlayer) {
        std::vector<std::pair<RE::Actor*, float>> nearbyActors;

        auto processLists = RE::ProcessLists::GetSingleton();
        if (!processLists) {
            return nearbyActors;
        }

        // Search through high process actors (nearby, active actors)
        auto& highActors = processLists->highActorHandles;
        for (auto& handle: highActors) {
            auto actor = handle.get();
            if (!actor || !actor->Is3DLoaded()) {
                continue;
            }

            // Skip player if not requested
            if (!includePlayer && actor->IsPlayerRef()) {
                continue;
            }

            // Calculate distance
            float distance = centerPoint.GetDistance(actor->GetPosition());
            if (distance <= radius) {
                nearbyActors.emplace_back(actor.get(), distance);
            }
        }

        // Also search medium process actors (less active but still loaded)
        auto& midActors = processLists->middleHighActorHandles;
        for (auto& handle: midActors) {
            auto actor = handle.get();
            if (!actor || !actor->Is3DLoaded()) {
                continue;
            }

            if (!includePlayer && actor->IsPlayerRef()) {
                continue;
            }

            float distance = centerPoint.GetDistance(actor->GetPosition());
            if (distance <= radius) {
                nearbyActors.emplace_back(actor.get(), distance);
            }
        }

        return nearbyActors;
    }

    /**
     * @brief Finds an actor by name from nearby actors.
     * 
     * This function searches for actors within a specified radius that match the given name.
     * If multiple actors have the same name, it returns the closest one. The search is
     * case-insensitive and supports partial matching.
     * 
     * @param actorName The name to search for (case-insensitive)
     * @param centerPoint The position to search from
     * @param radius The search radius in game units (default: 4096.0f)
     * @param exactMatch Whether to require exact name match (default: false for partial matching)
     * @param includePlayer Whether to include the player in search results (default: true)
     * @return Pointer to the closest matching actor, or nullptr if not found
     * 
     * @code
     * auto player = RE::PlayerCharacter::GetSingleton();
     * if (player) {
     *     auto lydia = plugin::SkyrimHelpers::FindActorByName("Lydia", player->GetPosition());
     *     if (lydia) {
     *         logger::info("Found Lydia nearby!");
     *     }
     * }
     * @endcode
     */
    RE::Actor* FindActorByName(const std::string& actorName, const RE::NiPoint3& centerPoint, float radius, bool exactMatch,
                               bool includePlayer) {
        if (actorName.empty()) {
            return nullptr;
        }

        // Convert search name to lowercase for case-insensitive comparison
        std::string searchName = actorName;
        std::transform(searchName.begin(), searchName.end(), searchName.begin(), ::tolower);

        auto nearbyActors = FindNearbyActors(centerPoint, radius, includePlayer);

        RE::Actor* closestMatch = nullptr;
        float closestDistance = std::numeric_limits<float>::max();

        for (const auto& [actor, distance]: nearbyActors) {
            if (!actor)
                continue;

            // Get actor's display name
            std::string displayName = GetActorDisplayName(actor);
            if (displayName.empty())
                continue;

            // Convert to lowercase for comparison
            std::string lowerDisplayName = displayName;
            std::transform(lowerDisplayName.begin(), lowerDisplayName.end(), lowerDisplayName.begin(), ::tolower);

            // Check for match
            bool isMatch = false;
            if (exactMatch) {
                isMatch = (lowerDisplayName == searchName);
            } else {
                isMatch = (lowerDisplayName.find(searchName) != std::string::npos);
            }

            // If it's a match and closer than previous matches, update closest
            if (isMatch && distance < closestDistance) {
                closestMatch = actor;
                closestDistance = distance;
            }
        }

        return closestMatch;
    }

    /**
     * @brief Finds an actor by name from actors near the player.
     * 
     * Convenience function that searches for actors near the player's current position.
     * 
     * @param actorName The name to search for (case-insensitive)
     * @param radius The search radius in game units (default: 4096.0f)
     * @param exactMatch Whether to require exact name match (default: false)
     * @param includePlayer Whether to include the player in search results (default: false)
     * @return Pointer to the closest matching actor, or nullptr if not found
     */
    RE::Actor* FindActorByNameNearPlayer(const std::string& actorName, float radius, bool exactMatch, bool includePlayer) {
        auto player = GetPlayer();
        if (!player) {
            return nullptr;
        }

        return FindActorByName(actorName, player->GetPosition(), radius, exactMatch, includePlayer);
    }

    /**
     * @brief Gets all actors with a specific name within radius, sorted by distance.
     * 
     * @param actorName The name to search for (case-insensitive)
     * @param centerPoint The position to search from
     * @param radius The search radius in game units (default: 4096.0f)
     * @param exactMatch Whether to require exact name match (default: false)
     * @param includePlayer Whether to include the player in search results (default: true)
     * @return Vector of actors sorted by distance (closest first)
     */
    std::vector<RE::Actor*> FindAllActorsByName(const std::string& actorName, const RE::NiPoint3& centerPoint, float radius,
                                                bool exactMatch, bool includePlayer) {
        std::vector<RE::Actor*> matchingActors;

        if (actorName.empty()) {
            return matchingActors;
        }

        std::string searchName = actorName;
        std::transform(searchName.begin(), searchName.end(), searchName.begin(), ::tolower);

        auto nearbyActors = FindNearbyActors(centerPoint, radius, includePlayer);

        // Collect all matches with distances
        std::vector<std::pair<RE::Actor*, float>> matches;
        for (const auto& [actor, distance]: nearbyActors) {
            if (!actor)
                continue;

            std::string displayName = GetActorDisplayName(actor);
            if (displayName.empty())
                continue;

            std::string lowerDisplayName = displayName;
            std::transform(lowerDisplayName.begin(), lowerDisplayName.end(), lowerDisplayName.begin(), ::tolower);

            bool isMatch = exactMatch ? (lowerDisplayName == searchName) : (lowerDisplayName.find(searchName) != std::string::npos);

            if (isMatch) {
                matches.emplace_back(actor, distance);
            }
        }

        // Sort by distance (closest first)
        std::sort(matches.begin(), matches.end(), [](const auto& a, const auto& b) { return a.second < b.second; });

        // Extract just the actors
        for (const auto& [actor, distance]: matches) {
            matchingActors.push_back(actor);
        }

        return matchingActors;
    }

    //=============================================================================
    // FORM EDITOR ID FUNCTIONS
    //=============================================================================

    /**
     * @brief Gets the editor ID of a form.
     * 
     * @param form The form to get the editor ID from
     * @return The editor ID as a string, or empty string if form is null or has no editor ID
     */
    std::string GetFormEditorID(RE::TESForm* form) {
        if (!form) {
            return "";
        }

        auto editorID = form->GetFormEditorID();
        return editorID ? std::string(editorID) : "";
    }

    /**
     * @brief Gets the editor ID of a form by its FormID.
     * 
     * This function looks up a form using its FormID and retrieves its editor ID.
     * 
     * @param formID The FormID of the form to look up
     * @return The editor ID as a string, or empty string if form not found
     * 
     * @code
     * RE::FormID formID = 0x123456;
     * std::string editorID = plugin::SkyrimHelpers::GetFormEditorIDByFormID(formID);
     * if (!editorID.empty()) {
     *     logger::info("Editor ID: {}", editorID);
     * }
     * @endcode
     */
    std::string GetFormEditorIDByFormID(RE::FormID formID) {
        auto dataHandler = RE::TESDataHandler::GetSingleton();
        if (!dataHandler) {
            return "";
        }

        auto form = dataHandler->LookupForm(formID, "");
        return GetFormEditorID(form);
    }

    /**
     * @brief Safely gets the editor ID of a form, with additional null checks.
     * 
     * This function performs extra safety checks including string length validation.
     * 
     * @param form The form to get the editor ID from
     * @return The editor ID as a string, or empty string if form is null or has empty/null editor ID
     */
    std::string GetFormEditorIDSafe(RE::TESForm* form) {
        if (!form) {
            return "";
        }

        auto editorID = form->GetFormEditorID();
        if (!editorID || strlen(editorID) == 0) {
            return "";
        }

        return std::string(editorID);
    }

    //=============================================================================
    // FACTION FUNCTIONS
    //=============================================================================

    /**
     * @brief Gets an actor's rank in a specific faction.
     * 
     * @param actor The actor to check
     * @param faction The faction to check membership in
     * @return The faction rank, or -1 if not in faction or parameters are null
     */
    std::int32_t GetFactionRank(RE::Actor* actor, RE::TESFaction* faction) {
        if (!actor || !faction) {
            return -1;
        }

        return actor->GetFactionRank(faction, false);
    }

    /**
     * @brief Gets an actor's rank in a faction by faction FormID.
     * 
     * @param actor The actor to check
     * @param factionFormID The FormID of the faction
     * @return The faction rank, or -1 if not in faction or faction not found
     */
    std::int32_t GetFactionRankByFormID(RE::Actor* actor, RE::FormID factionFormID) {
        if (!actor) {
            return -1;
        }

        auto dataHandler = RE::TESDataHandler::GetSingleton();
        if (!dataHandler) {
            return -1;
        }

        auto faction = dataHandler->LookupForm<RE::TESFaction>(factionFormID, "");
        if (!faction) {
            return -1;
        }

        return GetFactionRank(actor, faction);
    }

    /**
     * @brief Gets an actor's rank in a faction by faction editor ID.
     * 
     * @param actor The actor to check
     * @param factionEditorID The editor ID of the faction
     * @return The faction rank, or -1 if not in faction or faction not found
     */
    std::int32_t GetFactionRankByEditorID(RE::Actor* actor, const std::string& factionEditorID) {
        if (!actor || factionEditorID.empty()) {
            return -1;
        }

        auto dataHandler = RE::TESDataHandler::GetSingleton();
        if (!dataHandler) {
            return -1;
        }

        // Find faction by editor ID
        auto& factions = dataHandler->GetFormArray<RE::TESFaction>();
        for (auto faction: factions) {
            if (faction && faction->GetFormEditorID() == factionEditorID) {
                return GetFactionRank(actor, faction);
            }
        }

        return -1;  // Faction not found
    }

    /**
     * @brief Checks if an actor is a member of a specific faction (rank >= 0).
     * 
     * @param actor The actor to check
     * @param faction The faction to check membership in
     * @return True if actor is in the faction
     */
    bool IsInFaction(RE::Actor* actor, RE::TESFaction* faction) {
        return GetFactionRank(actor, faction) >= 0;
    }

    /**
     * @brief Checks if an actor is a member of a faction by editor ID.
     * 
     * @param actor The actor to check
     * @param factionEditorID The editor ID of the faction
     * @return True if actor is in the faction
     */
    bool IsInFactionByEditorID(RE::Actor* actor, const std::string& factionEditorID) {
        return GetFactionRankByEditorID(actor, factionEditorID) >= 0;
    }

    /**
     * @brief Gets all factions an actor belongs to along with their ranks.
     * 
     * @param actor The actor to query
     * @return Vector of pairs containing faction pointers and ranks
     */
    std::vector<std::pair<RE::TESFaction*, std::int32_t>> GetAllFactionRanks(RE::Actor* actor) {
        std::vector<std::pair<RE::TESFaction*, std::int32_t>> factionRanks;

        if (!actor) {
            return factionRanks;
        }

        auto actorBase = actor->GetActorBase();
        if (!actorBase) {
            return factionRanks;
        }

        // Get factions from actor base
        auto& factions = actorBase->factions;
        for (auto& factionInfo: factions) {
            if (factionInfo.faction) {
                auto rank = actor->GetFactionRank(factionInfo.faction, false);
                factionRanks.emplace_back(factionInfo.faction, rank);
            }
        }

        return factionRanks;
    }

}  // namespace plugin::SkyrimHelpers