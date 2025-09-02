#include "HTTPClient.h"
#include <cpr/cpr.h>
#include <thread>
#include <spdlog/spdlog.h>
#include <chrono>  // For measuring time

namespace plugin {
    void PapyrusHttpPost(RE::StaticFunctionTag*, const std::string url, const std::string payload, const bool logResponse) {
        HttpPostAsync(url, payload, logResponse);
    }

    void HttpPostAsync(const std::string url, const std::string payload, const bool logResponse) {
        if (logResponse) {
            logger::info("HttpPostAsync called with URL: {}, Payload: {}, logResponse: {}", url, payload, logResponse);
        }

        // Run the HTTP POST request in a detached thread
        std::thread([url, payload]() {
            try {
                auto start = std::chrono::high_resolution_clock::now();  // Start timing

                auto response = cpr::Post(cpr::Url{url}, cpr::Body{payload},
                                          cpr::Header{{"Content-Type", "application/json"}, {"User-Agent", "VrelkHttpClient/1.0"}});

                auto end = std::chrono::high_resolution_clock::now();  // End timing
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

                logger::info("HTTP POST to '{}' completed with status code: {} in {} ms", url, response.status_code, duration);
            } catch (const std::exception& e) {
                logger::error("HTTP POST to '{}' failed: {}", url, e.what());
            }
        }).detach();
    }
}  // namespace plugin