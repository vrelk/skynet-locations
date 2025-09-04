// filepath: r:\SKSE-DEV\skse-clibng-template\src\HTTPClient.h
#pragma once

#include <string>
#include "RE/Skyrim.h"

namespace plugin {
    int PapyrusHttpGet(RE::StaticFunctionTag*, const std::string url);
    int HttpGet(const std::string& url, std::string& responseBody);

    void PapyrusHttpPost(RE::StaticFunctionTag*, const std::string url, const std::string payload, const bool logResponse = false);
    void HttpPostAsync(const std::string& url, const std::string& payload, const bool logResponse = false);
}  // namespace plugin