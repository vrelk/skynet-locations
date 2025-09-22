#pragma once

#include <string>
#include <vector>
#include "RE/Skyrim.h"
#include "../include/jcontainers.hpp"
#include "../include/SlaveTatsNG_Interface.h"
#include "DataTypes.hpp"
#include "DatabaseFunctions.h"
#include <inja/inja.hpp>
#include "LookupHelpers.h"

namespace plugin::SlaveTats {
    inline const SlaveTatsNG::Addresses* iface = nullptr;

    inline void messaging_hook(SKSE::MessagingInterface::Message* a_msg) {
        if (a_msg && a_msg->type == SlaveTatsNG::MessageType::Interface) {
            iface = SlaveTatsNG::Addresses::from_void(a_msg->data);
            logger::info("SlaveTatsNG Interface found: address = {}", (void*) iface);
        }
    }

    inline bool query_available_tattoos(int a_template, int matches, int applied = 0, RE::BSFixedString domain = "default") {
        if (!iface) {
            logger::warn("SlaveTatsNG Interface not found");
            return true;
        }
        return iface->query_available_tattoos(a_template, matches, applied, domain);
    }
    inline bool query_applied_tattoos(RE::Actor* target, int a_template, int matches, RE::BSFixedString except_area = "",
                                      int except_slot = -1) {
        if (!iface)
            return true;
        return iface->query_applied_tattoos(target, a_template, matches, except_area, except_slot);
    }
    inline bool simple_remove_tattoo(RE::Actor* target, RE::BSFixedString section, RE::BSFixedString name, bool last = true,
                                     bool silent = false) {
        if (!iface)
            return true;
        return iface->simple_remove_tattoo(target, section, name, last, silent);
    }
    inline bool remove_tattoos(RE::Actor* target, int a_template, bool ignore_lock = false, bool silent = false) {
        if (!iface)
            return true;
        return iface->remove_tattoos(target, a_template, ignore_lock, silent);
    }
    inline bool add_tattoo(RE::Actor* target, int tattoo, int slot = -1, bool ignore_lock = false, bool silent = false) {
        if (!iface)
            return true;
        return iface->add_tattoo(target, tattoo, slot, ignore_lock, silent);
    }
    inline bool synchronize_tattoos(RE::Actor* target, bool silent = false) {
        if (!iface)
            return true;
        return iface->synchronize_tattoos(target, silent);
    }

    inline std::vector<DataTypes::SlaveTatsAvailableTat> GetAllTattoos() {
        using namespace jcontainers;

        if (!iface)
            return {};
        if (!jcontainers::JCWrapper::GetSingleton()->IsInitialized())
            return {};

        try {
            std::vector<DataTypes::SlaveTatsAvailableTat> tattoos;

            int a_template = JValue::addToPool(JMap::object(), "SkynetDetails");
            int matches = JValue::addToPool(JArray::object(), "SkynetDetails");

            if (query_available_tattoos(a_template, matches)) {
                logger::warn("JContainers error query_available_tattoos");
                JValue::cleanPool("SkynetDetails");
                return tattoos;
            }

            int count = JArray::count(matches);
            for (int i = 0; i < count; ++i) {
                int tatMap = JArray::getObj(matches, i, 0);
                if (tatMap == 0)
                    continue;

                DataTypes::SlaveTatsAvailableTat tat;
                tat.area = JMap::getStr(tatMap, "area");
                tat.name = JMap::getStr(tatMap, "name");
                tat.section = JMap::getStr(tatMap, "section");
                tat.texture = JMap::getStr(tatMap, "texture");
                tattoos.push_back(tat);
            }

            JValue::cleanPool("SkynetDetails");
            return tattoos;
        } catch (const std::exception& e) {
            try {
                JValue::cleanPool("SkynetDetails");
            } catch (...) {}

            logger::error("Failed to get all tattoos: {}", e.what());
            return {};
        }
    }

    inline std::vector<DataTypes::SlaveTatsAppliedTat> GetActorTattoos(RE::Actor* actor, std::string area = "") {
        using namespace jcontainers;

        if (!actor)
            return {};
        if (!iface)
            return {};
        if (!jcontainers::JCWrapper::GetSingleton()->IsInitialized())
            return {};

        try {
            std::vector<DataTypes::SlaveTatsAppliedTat> tattoos;

            int a_template = JValue::addToPool(JMap::object(), "SkynetDetails");
            int matches = JValue::addToPool(JArray::object(), "SkynetDetails");

            if (!area.empty())
                JMap::setStr(a_template, "area", area);

            if (query_applied_tattoos(actor, a_template, matches)) {
                logger::warn("JContainers error query_applied_tattoos");
                JValue::cleanPool("SkynetDetails");
                return tattoos;
            }

            int count = JArray::count(matches);
            for (int i = 0; i < count; ++i) {
                int tatMap = JArray::getObj(matches, i, 0);
                if (tatMap == 0)
                    continue;

                DataTypes::SlaveTatsAppliedTat tat;
                tat.area = JMap::getStr(tatMap, "area");
                std::transform(tat.area.begin(), tat.area.end(), tat.area.begin(), ::tolower);
                tat.name = JMap::getStr(tatMap, "name");
                tat.section = JMap::getStr(tatMap, "section");
                tat.slot = JMap::getInt(tatMap, "slot");
                tat.texture = JMap::getStr(tatMap, "texture");
                tat.description = DatabaseFunctions::GetTattooDesc(tat.section, tat.name, tat.area, tat.texture);

                tat.description = inja::render(tat.description, {{"actor_adj", LookupHelpers::GetActorAdj(actor)}});

                tattoos.push_back(tat);
            }

            JValue::cleanPool("SkynetDetails");
            return tattoos;
        } catch (const std::exception& e) {
            try {
                JValue::cleanPool("SkynetDetails");
            } catch (...) {}

            logger::error("Failed to get actor tattoos: {}", e.what());
            return {};
        }
    }
}  // namespace plugin::SlaveTats