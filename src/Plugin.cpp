using namespace SKSE;
using namespace SKSE::log;
using namespace SKSE::stl;

#include "Plugin.h"
#include "GameEventHandler.h"

//#include "src/Settings.hpp"
//#include "src/Scripting.hpp"
#include "../include/jc_interface.h"
#include "jcontainers.hpp"
#include "DatabaseFunctions.h"
#include "HTTPClient.h"
#include "VrelkUtil.h"

namespace plugin {
    std::optional<std::filesystem::path> getLogDirectory() {
        using namespace std::filesystem;
        PWSTR buf;
        SHGetKnownFolderPath(FOLDERID_Documents, KF_FLAG_DEFAULT, nullptr, &buf);
        std::unique_ptr<wchar_t, decltype(&CoTaskMemFree)> documentsPath{buf, CoTaskMemFree};
        path directory{documentsPath.get()};
        directory.append("My Games"sv);

        if (exists("steam_api64.dll"sv)) {
            if (exists("openvr_api.dll") || exists("Data/SkyrimVR.esm")) {
                directory.append("Skyrim VR"sv);
            } else {
                directory.append("Skyrim Special Edition"sv);
            }
        } else if (exists("Galaxy64.dll"sv)) {
            directory.append("Skyrim Special Edition GOG"sv);
        } else if (exists("eossdk-win64-shipping.dll"sv)) {
            directory.append("Skyrim Special Edition EPIC"sv);
        } else {
            return current_path().append("skselogs");
        }
        return directory.append("SKSE"sv).make_preferred();
    }

    void initializeLogging() {
        auto path = getLogDirectory();
        if (!path) {
            report_and_fail("Can't find SKSE log directory");
        }
        *path /= std::format("{}.log"sv, Plugin::Name);

        std::shared_ptr<spdlog::logger> log;
        if (IsDebuggerPresent()) {
            log = std::make_shared<spdlog::logger>("Global", std::make_shared<spdlog::sinks::msvc_sink_mt>());
        } else {
            log = std::make_shared<spdlog::logger>("Global", std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true));
        }
        log->set_level(spdlog::level::info);
        log->flush_on(spdlog::level::info);

        spdlog::set_default_logger(std::move(log));
        spdlog::set_pattern(PLUGIN_LOGPATTERN_DEFAULT);
    }

    // Function to bind Papyrus functions
    bool BindPapyrusFunctions(RE::BSScript::IVirtualMachine* vm) {
        vm->RegisterFunction("httpGetAsync", "VrelkHttpClient", PapyrusHttpGet);
        vm->RegisterFunction("httpPostAsync", "VrelkHttpClient", PapyrusHttpPost);
        //vm->RegisterFunction("floatToInt", "VrelkHttpClient", FloatToInt);
        //vm->RegisterFunction("isPlayer", "VrelkHttpClient", IsPlayer);
        //vm->RegisterFunction("getActorName", "VrelkHttpClient", GetActorName);
        //vm->RegisterFunction("jsonEncStrArr", "VrelkHttpClient", PapyrusJsonEncStrArr);
        vm->RegisterFunction("jsonEncJArray", "VrelkHttpClient", PapyrusJsonEncJArray);
        vm->RegisterFunction("jsonEncJMap", "VrelkHttpClient", PapyrusJsonEncJMap);

        vm->RegisterFunction("getLocationDescription", "VrelkHttpClient", GetLocationDescription);
        vm->RegisterFunction("getQuestDescription", "VrelkHttpClient", GetQuestDescription);
        vm->RegisterFunction("getStageDescription", "VrelkHttpClient", GetStageDescription);
        vm->RegisterFunction("getObjectiveDescription", "VrelkHttpClient", GetObjectiveDescription);
        vm->RegisterFunction("getSceneDescription", "VrelkHttpClient", GetSceneDescription);
        return true;
    }
}  // namespace plugin

std::string getJContainersPluginName() {
    auto patchVersion = REL::Module::get().version().patch();

    std::string pluginName{"JContainers64"};
    if (REL::Module::IsVR()) {
        pluginName = "JContainersVR";
    } else if (patchVersion == 659 || patchVersion == 1179) {
        pluginName = "JContainersGOG";
    }

    return pluginName;
}

void loadJContainers() {
    std::string pluginName = getJContainersPluginName();

    SKSE::GetMessagingInterface()->RegisterListener(pluginName.c_str(), [](SKSE::MessagingInterface::Message* a_msg) {
        if (a_msg && a_msg->type == jc::message_root_interface) {
            const jc::root_interface* root = jc::root_interface::from_void(a_msg->data);
            if (root)
                jcontainers::JCWrapper::GetSingleton()->PreInit(root);
        }
    });
}

using namespace plugin;

extern "C" DLLEXPORT bool SKSEPlugin_Load(const LoadInterface* skse) {
    initializeLogging();

    logger::info("'{} {}' is loading, game version '{}'...", Plugin::Name, Plugin::VersionString, REL::Module::get().version().string());
    Init(skse);

    // Get the Papyrus interface from SKSE.
    auto papyrus = SKSE::GetPapyrusInterface();
    if (!papyrus) {
        return false;
    }

    loadJContainers();

    // Register our Papyrus functions.
    if (!papyrus->Register(BindPapyrusFunctions)) {
        return false;
    }

    GameEventHandler::getInstance().onLoad();
    logger::info("{} has finished loading.", Plugin::Name);
    return true;
}