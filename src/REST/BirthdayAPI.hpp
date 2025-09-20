#pragma once

#include <crow.h>
#include "../../include/cors.h"  // don't know why, but it refuses to find it in main includes folder from vcpkg
#include "HTTPServer.hpp"

namespace plugin::REST::BirthdayAPI {
    void RegisterRoutes(crow::App<crow::CORSHandler>& app);
}