#include "DataTypes.h"

namespace plugin::DataTypes {
    std::string LocationLookupResult::ToJSON() const {
        if (!found) {
            return nlohmann::json({{"found", found}, {"name", ""}, {"description", ""}}).dump();
        }
        return nlohmann::json{{"found", found}, {"name", name}, {"description", description}}.dump();
    }
}  // namespace plugin::DataTypes
