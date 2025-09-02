#pragma once

#include "Scripting.hpp"
#include "Json.hpp"
#include "Utils.hpp"

#include <stdlib.h>

using namespace SKSE;

typedef int color;
typedef std::string text;

constexpr auto SETTINGS_FILE = "Data/SKSE/Plugins/VrelkHttpClient.json";

namespace plugin {

    struct Settings {
            float baseChance = 6.5;       //Base Event Chance//How likely are you to trigger a trap before modifiers.//{1}%//(0,100,0.1)
            float containerModifier = 1;  //Container Modifier//Modifier for containers.//{1}x//(0,10,0.1)
            float bossContainerModifier =
                2;  //Boss Chest Modifier//Modifier for boss chests.\nIs applied with the container modifier.//{1}x//(0,10,0.1)
    } settings;

    void PlayerMessage(Translator trans) {
        auto scriptManager = ScriptingManager();
        if (settings.noMessageBoxes) {
            scriptManager.DBGNotification(trans);
        } else {
            scriptManager.DBGMessageBox(trans);
        }

        void ResetSettings() {
            settings.minGoldRequired = 20;
            settings.eventScalingMod = 15;
        }

        void SaveSettings() {
            std::ofstream o(SETTINGS_FILE);
            nlohmann::json j = nlohmann::json{
                {"minGoldRequired", settings.minGoldRequired},
                {"LMNudityAditionalForms", settings.LMNudityAditionalForms},
            };
            o << j << std::endl;
        }

        void P_UpdateSKSE(RE::StaticFunctionTag *);

        void Loadettings() {
            std::ifstream i(SETTINGS_FILE);
            nlohmann::json j = nlohmann::json::parse("{}");
            if (!std::filesystem::exists(SETTINGS_FILE)) {
                log::info("Settings store file does not exist, will attempt to recover settings from save game.");
                P_UpdateSKSE(nullptr);
                return;
            }

            try {
                i >> j;
            } catch (...) {
                log::error("Settings store file has garbled data, will attempt to recover settings from save game.");
                P_UpdateSKSE(nullptr);
                return;
            }

            settings.minGoldRequired = static_cast<int>(j.value("minGoldRequired", 20));
            settings.eventScalingMod = static_cast<int>(j.value("eventScalingMod", 15));
        }
    }