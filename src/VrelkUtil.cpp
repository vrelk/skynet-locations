#include <VrelkUtil.h>
#include <JSON.hpp>         // Ensure you have the nlohmann/json library included
#include <jcontainers.hpp>  // Ensure you have JContainers included

namespace plugin {

    using json = nlohmann::json;

    int FloatToInt(RE::StaticFunctionTag*, float a_value) {
        assert(sizeof(int) == sizeof(float));
        return *((int*) &a_value);
    }

    bool IsPlayer(RE::StaticFunctionTag*, RE::Actor* a_actor) {
        if (a_actor == nullptr)
            return false;
        return a_actor->IsPlayerRef();
    }

    std::string GetActorName(RE::StaticFunctionTag*, RE::Actor* a_actor) {
        if (a_actor == nullptr)
            return "ERROR";
        const auto loc_base = a_actor->GetActorBase();

        if (loc_base == nullptr) {
            logger::error("GetActorName - Actor {:08X} have no actor base!", a_actor->GetFormID());
            return a_actor->GetName();
        }

        std::string loc_res = loc_base->GetName();

        if (loc_res == "") {
            switch (loc_base->GetSex()) {
                case RE::SEX::kMale:
                    loc_res = "Unnamed male";
                    break;
                case RE::SEX::kFemale:
                    loc_res = "Unnamed female";
                    break;
                case RE::SEX::kNone:
                    loc_res = "Unnamed person";
                    break;
                default:
                    loc_res = "::ERROR::";
            }
        }

        return loc_res;
    }

    void ForEachReferenceInRange(RE::TESObjectREFR* origin, float radius,
                                 std::function<RE::BSContainer::ForEachResult(RE::TESObjectREFR& ref)> callback) {
        if (origin && radius > 0.0f) {
            const auto originPos = origin->GetPosition();
            auto* tesSingleton = RE::TES::GetSingleton();
            auto* interiorCell = tesSingleton->interiorCell;
            if (interiorCell) {
                interiorCell->ForEachReferenceInRange(originPos, radius, [&](RE::TESObjectREFR* a_ref) {
                    if (a_ref) {
                        return callback(*a_ref);  // Dereference the pointer to pass as a reference
                    }
                    return RE::BSContainer::ForEachResult::kContinue;
                });
            } else {
                if (const auto gridLength = tesSingleton->gridCells ? tesSingleton->gridCells->length : 0; gridLength > 0) {
                    const float yPlus = originPos.y + radius;
                    const float yMinus = originPos.y - radius;
                    const float xPlus = originPos.x + radius;
                    const float xMinus = originPos.x - radius;

                    std::uint32_t x = 0;
                    do {
                        std::uint32_t y = 0;
                        do {
                            if (const auto cell = tesSingleton->gridCells->GetCell(x, y); cell && cell->IsAttached()) {
                                if (const auto cellCoords = cell->GetCoordinates(); cellCoords) {
                                    const RE::NiPoint2 worldPos{cellCoords->worldX, cellCoords->worldY};
                                    if (worldPos.x < xPlus && (worldPos.x + 4096.0f) > xMinus && worldPos.y < yPlus &&
                                        (worldPos.y + 4096.0f) > yMinus) {
                                        cell->ForEachReferenceInRange(originPos, radius, [&](RE::TESObjectREFR* a_ref) {
                                            if (a_ref) {
                                                return callback(*a_ref);  // Dereference the pointer to pass as a reference
                                            }
                                            return RE::BSContainer::ForEachResult::kContinue;
                                        });
                                    }
                                }
                            }
                            ++y;
                        } while (y < gridLength);
                        ++x;
                    } while (x < gridLength);
                }
            }
        } else {
            // Adapt the lambda to match the expected signature
            RE::TES::GetSingleton()->ForEachReference([&](RE::TESObjectREFR* a_ref) {
                if (a_ref) {
                    return callback(*a_ref);  // Dereference the pointer to pass as a reference
                }
                return RE::BSContainer::ForEachResult::kContinue;
            });
        }
    }

    /*
    std::string PapyrusJsonEncStrArr(RE::StaticFunctionTag*, RE::BSScript::VMArray<RE::BSFixedString> stringArray) {
        try {
            json jsonArray = json::array();

            stringArray.ForEach([&](const RE::BSFixedString& str) {
                jsonArray.push_back(str.c_str());  // Convert BSFixedString to std::string
                return true;                       // Continue iteration
            });

            return jsonArray.dump();  // Convert the JSON array to a string
        } catch (const std::exception& e) {
            logger::error("PapyrusJsonEncStrArr - Exception occurred: {}", e.what());
            return "::ERROR::";
        }
    }
    */
    std::string PapyrusJsonEncJArray(RE::StaticFunctionTag*, int32_t jArrayHandle) {
        try {
            if (jcontainers::JArray::count == nullptr || jcontainers::JArray::getInt == nullptr || jcontainers::JArray::getStr == nullptr) {
                logger::error("PapyrusJsonEncJArray - JContainers functions not initialized.");
                return "::ERROR::";
            }

            if (!jcontainers::JValue::isArray(jArrayHandle)) {
                logger::error("PapyrusJsonEncJArray - Provided handle is not a valid JArray.");
                return "::ERROR::";
            }

            json jsonArray = json::array();
            int count = jcontainers::JArray::count(jArrayHandle);

            for (int i = 0; i < count; ++i) {
                // Determine the type of the value at the current index
                int valueType = jcontainers::JArray::valueType(jArrayHandle, i);

                switch (valueType) {
                    case 1:  // Integer
                        jsonArray.push_back(jcontainers::JArray::getInt(jArrayHandle, i, 0));
                        break;
                    case 2:  // Float
                        jsonArray.push_back(jcontainers::JArray::getFlt(jArrayHandle, i, 0.0f));
                        break;
                    case 3:  // String
                        jsonArray.push_back(jcontainers::JArray::getStr(jArrayHandle, i, ""));
                        break;
                    default:
                        logger::warn("PapyrusJsonEncJArray - Unsupported value type at index {}, type {}.", i, valueType);
                        jsonArray.push_back(nullptr);  // Push null for unsupported types
                        break;
                }
            }

            return jsonArray.dump();  // Convert the JSON array to a string
        } catch (const std::exception& e) {
            logger::error("PapyrusJsonEncJArray - Exception occurred: {}", e.what());
            return "::ERROR::";
        }
    }

    std::string PapyrusJsonEncJMap(RE::StaticFunctionTag*, int32_t jMapHandle) {
        try {
            if (jcontainers::JMap::count == nullptr || jcontainers::JMap::getInt == nullptr || jcontainers::JMap::getStr == nullptr) {
                logger::error("PapyrusJsonEncJMap - JContainers functions not initialized.");
                return "::ERROR::";
            }

            if (!jcontainers::JValue::isMap(jMapHandle)) {
                logger::error("PapyrusJsonEncJMap - Provided handle is not a valid JMap.");
                //return "::ERROR::";
            }

            json jsonObject = json::object();

            auto key = jcontainers::JMap::nextKey(jMapHandle, "", "");  // Start with the first key
            while (!key.empty()) {
                std::string stdKey = key.c_str();  // Convert BSFixedString to std::string

                int valueType = jcontainers::JMap::valueType(jMapHandle, key);

                switch (valueType) {
                    case 1:  // Integer
                        jsonObject[stdKey] = jcontainers::JMap::getInt(jMapHandle, key, 0);
                        break;
                    case 2:  // Float
                        jsonObject[stdKey] = jcontainers::JMap::getFlt(jMapHandle, key, 0.0f);
                        break;
                    case 3:  // String
                        jsonObject[stdKey] = jcontainers::JMap::getStr(jMapHandle, key, "").c_str();
                        break;
                    default:
                        logger::warn("PapyrusJsonEncJMap - Unsupported value type for key '{}', type {}.", stdKey, valueType);
                        jsonObject[stdKey] = nullptr;  // Push null for unsupported types
                        break;
                }

                key = jcontainers::JMap::nextKey(jMapHandle, key, "");  // Get the next key
            }

            return jsonObject.dump();  // Convert the JSON object to a string
        } catch (const std::exception& e) {
            logger::error("PapyrusJsonEncJMap - Exception occurred: {}", e.what());
            return "ERROR";
        }
    }

}  // namespace plugin