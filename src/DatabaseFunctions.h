#ifndef DATABASEFUNCTIONS_H
#define DATABASEFUNCTIONS_H

#include <string>

namespace plugin {
    extern const std::string DATABASE_PATH;

    void InitializeDatabase();
    std::string SelectValue(const std::string& key);
    void InsertValueIfNotExists(const std::string& key, const std::string& value);
}  // namespace plugin

#endif  // DATABASEFUNCTIONS_H