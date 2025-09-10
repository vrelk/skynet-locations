#include "HTTPClient.h"
#include <cpr/cpr.h>
#include <thread>
#include <chrono>           // For measuring time
#include "jcontainers.hpp"  // Include JContainers

namespace plugin {
    int PapyrusHttpGet(RE::StaticFunctionTag*, const std::string url) {
        std::string responseBody;
        int statusCode = plugin::HttpGet(url, responseBody);

        // Create a JMap object
        int jmapObj = jcontainers::JMap::object();
        if (jmapObj == 0) {
            logger::error("Failed to create JMap object for HTTP GET response.");
            return 0;  // Return 0 to indicate failure
        }
        // Store the response code and body in the JMap
        jcontainers::JMap::setInt(jmapObj, "status", statusCode);
        jcontainers::JMap::setStr(jmapObj, "response", responseBody);

        return jmapObj;
    }

    void PapyrusHttpPost(RE::StaticFunctionTag*, const std::string url, const std::string payload, const bool logResponse) {
        HttpPostAsync(url, payload, logResponse);
    }

    /*
     * Performs an HTTP GET request to the specified URL and returns the status code.
     * The response body is returned via the responseBody reference parameter.
     * Returns -1 in case of an error.
    */
    int HttpGet(const std::string& url, std::string& responseBody) {
        try {
            std::string userAgent = std::string(USER_AGENT_NAME) + "/" + std::string(USER_AGENT_VERSION);
            auto start = std::chrono::high_resolution_clock::now();

            auto response = cpr::Get(cpr::Url{url}, cpr::Header{{"User-Agent", userAgent}}, cpr::Timeout{5000},
                                     cpr::VerifySsl{false});  // Increased timeout

            if (response.status_code == 0) {
                logger::error("HTTP GET to '{}' failed with error: {} (code: {})", url, response.error.message,
                              static_cast<int>(response.error.code));
                return -1;  // Return -1 to indicate an error
            }

            responseBody = response.text;
            responseBody.erase(responseBody.begin(),
                               std::find_if(responseBody.begin(), responseBody.end(), [](unsigned char ch) { return !std::isspace(ch); }));
            responseBody.erase(
                std::find_if(responseBody.rbegin(), responseBody.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(),
                responseBody.end());

            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

            logger::info("HTTP GET to '{}' completed with status code: {} in {} ms", url, response.status_code, duration);
            return response.status_code;
        } catch (const std::exception& e) {
            logger::error("HTTP GET to '{}' failed: {}", url, e.what());
            return -1;
        }
    }

    void HttpPostAsync(const std::string& url, const std::string& payload, const bool logResponse) {
        if (logResponse) {
            logger::info("HttpPostAsync called with URL: {}, Payload: {}, logResponse: {}", url, payload, logResponse);
        }

        // Run the HTTP POST request in a detached thread
        std::thread([url, payload]() {
            try {
                // Construct the User-Agent string
                std::string userAgent = std::string(USER_AGENT_NAME) + "/" + std::string(USER_AGENT_VERSION);

                auto start = std::chrono::high_resolution_clock::now();  // Start timing

                auto response =
                    cpr::Post(cpr::Url{url}, cpr::Body{payload},
                              cpr::Header{{"Content-Type", "application/json"}, {"User-Agent", userAgent}}, cpr::VerifySsl{false});

                auto end = std::chrono::high_resolution_clock::now();  // End timing
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

                logger::info("HTTP POST to '{}' completed with status code: {} in {} ms", url, response.status_code, duration);
            } catch (const std::exception& e) {
                logger::error("HTTP POST to '{}' failed: {}", url, e.what());
            }
        }).detach();
    }
}  // namespace plugin