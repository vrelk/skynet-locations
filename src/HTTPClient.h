// filepath: r:\SKSE-DEV\skse-clibng-template\src\HTTPClient.h
#pragma once

#include <string>
#include "RE/Skyrim.h"

namespace plugin {
    void PapyrusHttpPost(RE::StaticFunctionTag *, const std::string url, const std::string payload, const bool logResponse = false);
    void HttpPostAsync(const std::string url, const std::string payload, const bool logResponse = false);
}  // namespace plugin