#include "RE/Skyrim.h"
#include <unordered_map>
#include <string>

namespace plugin {
    struct NudityHelper {
        private:
            static inline bool isInitialized = false;                 // Tracks whether initialization has occurred
            static inline RE::TESDataHandler* dataHandler = nullptr;  // Cached data handler

            static inline const const RE::BSFixedString modfile_Skyrim = "Skyrim.esm";
            static inline std::unordered_map<std::string, RE::BGSKeyword*> allKeywords;  // Map of all keywords by name

#pragma region AND Variables
            // Advanced Nudity Detection (AND)
            static inline const const RE::BSFixedString modfile_AND = "Advanced Nudity Detection.esp";
            static inline bool hasAND = false;
            //static inline std::unordered_map<std::string, RE::BGSKeyword*> keywordsAND;
#pragma endregion

#pragma region OCF Variables
            // Object Categorization Framework (OCF)
            static inline const RE::BSFixedString modfile_OCF = "OCF.esp";
            static inline bool hasOCF = false;
            //static inline std::unordered_map<std::string, RE::BGSKeyword*> keywordsOCF;
#pragma endregion

#pragma region SLA Variables
            // SexLab Aroused (SLA)
            static inline const RE::BSFixedString modfile_SLA = "SexLabAroused.esm";
            static inline bool hasSLA = false;
            //static inline std::unordered_map<std::string, RE::BGSKeyword*> keywordsSLA;
#pragma endregion

#pragma region Keyword Adder Funcs
            // Add a keyword to the general keyword map
            // Example usage: AddKeyword("Nudity_Covered", 0x123456);
            static void AddSkyrimKeyword(const std::string& key, RE::FormID formID) {
                if (dataHandler) {
                    allKeywords[key] = dataHandler->LookupForm<RE::BGSKeyword>(formID, modfile_Skyrim);
                }
            }

            // Add a keyword to the AND keyword map
            // Example usage: AddANDKeyword("Nudity_Covered", 0x123456);
            static void AddANDKeyword(const std::string& key, RE::FormID formID) {
                if (hasAND && dataHandler) {
                    allKeywords[key] = dataHandler->LookupForm<RE::BGSKeyword>(formID, modfile_AND);
                }
            }

            // Add a keyword to the OCF keyword map
            // Example usage: AddOCFKeyword("Nudity_Covered", 0x123456);
            static void AddOCFKeyword(const std::string& key, RE::FormID formID) {
                if (hasOCF && dataHandler) {
                    allKeywords[key] = dataHandler->LookupForm<RE::BGSKeyword>(formID, modfile_OCF);
                }
            }

            // Add a keyword to the SLA keyword map
            // Example usage: AddSLAKeyword("Nudity_Covered", 0x123456);
            static void AddSLAKeyword(const std::string& key, RE::FormID formID) {
                if (hasSLA && dataHandler) {
                    allKeywords[key] = dataHandler->LookupForm<RE::BGSKeyword>(formID, modfile_SLA);
                }
            }
#pragma endregion

#pragma region Initialization Funcs
            // Initialize Skyrim related variables
            // MARK: - InintializeSkyrim
            static void InintializeSkyrim() {
                try {
                    if (isInitialized) {
                        return;
                    }

                    AddSkyrimKeyword("ArmorBoots", 0x6C0ED);
                    AddSkyrimKeyword("ArmorClothing", 0x6BBE8);
                    AddSkyrimKeyword("ArmorCuirass", 0x6C0EC);
                    AddSkyrimKeyword("ArmorGauntlets", 0x6C0EF);
                    AddSkyrimKeyword("ArmorHelmet", 0x6C0EE);
                    AddSkyrimKeyword("ArmorJewelry", 0x6BBE9);
                    AddSkyrimKeyword("ClothingBody", 0xA8657);
                    AddSkyrimKeyword("ClothingCirclet", 0x10CD08);
                    AddSkyrimKeyword("ClothingFeet", 0x10CD12);
                    AddSkyrimKeyword("ClothingHands", 0x10CD13);
                    AddSkyrimKeyword("ClothingHead", 0x10CD11);
                    AddSkyrimKeyword("ClothingNecklace", 0x10CD0A);
                    AddSkyrimKeyword("ClothingRing", 0x10CD09);

                } catch (const std::exception& e) {
                    logger::error("NudityHelper::InintializeSkyrim exception: {}", e.what());
                    return;
                }
            }

            // Initialize Advanced Nudity Detection (AND) related variables
            // MARK: - InitialzeAND
            static void InitialzeAND() {
                try {
                    if (isInitialized || dataHandler->LookupModByName(modfile_AND) == nullptr) {
                        // Mod not found, or already initialized
                        return;
                    }

                    // Mark AND as present
                    hasAND = true;

                    AddANDKeyword("AND_ArmorBottom", 0x82C);
                    AddANDKeyword("AND_ArmorBottomT", 0x82D);
                    AddANDKeyword("AND_ArmorBottomT_High", 0x840);
                    AddANDKeyword("AND_ArmorBottomT_High_Male", 0x842);
                    AddANDKeyword("AND_ArmorBottomT_Low", 0x83F);
                    AddANDKeyword("AND_ArmorBottomT_Low_Male", 0x841);
                    AddANDKeyword("AND_ArmorBottomT_Male", 0x896);
                    AddANDKeyword("AND_ArmorBottom_Male", 0x894);
                    AddANDKeyword("AND_ArmorBottom_NoCover", 0x83C);
                    AddANDKeyword("AND_ArmorBottom_NoCover_Male", 0x895);
                    AddANDKeyword("AND_ArmorTop", 0x82A);
                    AddANDKeyword("AND_ArmorTopT", 0x82B);
                    AddANDKeyword("AND_ArmorTopT_High", 0x844);
                    AddANDKeyword("AND_ArmorTopT_High_Male", 0x846);
                    AddANDKeyword("AND_ArmorTopT_Low", 0x843);
                    AddANDKeyword("AND_ArmorTopT_Low_Male", 0x845);
                    AddANDKeyword("AND_ArmorTopT_Male", 0x877);
                    AddANDKeyword("AND_ArmorTop_Male", 0x897);
                    AddANDKeyword("AND_ArmorTop_NoCover", 0x83B);
                    AddANDKeyword("AND_ArmorTop_NoCover_Male", 0x898);
                    AddANDKeyword("AND_AssCurtain", 0x80C);
                    AddANDKeyword("AND_AssCurtainT", 0x815);
                    AddANDKeyword("AND_AssCurtainT_Male", 0x879);
                    AddANDKeyword("AND_AssCurtain_Male", 0x878);
                    AddANDKeyword("AND_AssFlashRisk", 0x811);
                    AddANDKeyword("AND_AssFlashRiskExtreme", 0x813);
                    AddANDKeyword("AND_AssFlashRiskExtreme_Male", 0x858);
                    AddANDKeyword("AND_AssFlashRiskHigh", 0x812);
                    AddANDKeyword("AND_AssFlashRiskHigh_Male", 0x857);
                    AddANDKeyword("AND_AssFlashRiskLow", 0x814);
                    AddANDKeyword("AND_AssFlashRiskLow_Male", 0x855);
                    AddANDKeyword("AND_AssFlashRiskUltra", 0x81C);
                    AddANDKeyword("AND_AssFlashRiskUltra_Male", 0x862);
                    AddANDKeyword("AND_AssFlashRisk_Male", 0x856);
                    AddANDKeyword("AND_BananaHammock", 0x893);
                    AddANDKeyword("AND_BananaHammockT", 0x880);
                    AddANDKeyword("AND_BananaHammockT_High", 0x848);
                    AddANDKeyword("AND_BananaHammockT_Low", 0x847);
                    AddANDKeyword("AND_Bra", 0x804);
                    AddANDKeyword("AND_BraT", 0x81D);
                    AddANDKeyword("AND_BraT_High", 0x84A);
                    AddANDKeyword("AND_BraT_High_Male", 0x84C);
                    AddANDKeyword("AND_BraT_Low", 0x849);
                    AddANDKeyword("AND_BraT_Low_Male", 0x84B);
                    AddANDKeyword("AND_BraT_Male", 0x87C);
                    AddANDKeyword("AND_Bra_Male", 0x87A);
                    AddANDKeyword("AND_Bra_NoCover", 0x839);
                    AddANDKeyword("AND_Bra_NoCover_Male", 0x87B);
                    AddANDKeyword("AND_ChestCurtain", 0x806);
                    AddANDKeyword("AND_ChestCurtainT", 0x80B);
                    AddANDKeyword("AND_ChestCurtainT_Male", 0x87E);
                    AddANDKeyword("AND_ChestCurtain_Male", 0x87D);
                    AddANDKeyword("AND_ChestFlashRisk", 0x800);
                    AddANDKeyword("AND_ChestFlashRiskExtreme", 0x803);
                    AddANDKeyword("AND_ChestFlashRiskExtreme_Male", 0x866);
                    AddANDKeyword("AND_ChestFlashRiskHigh", 0x802);
                    AddANDKeyword("AND_ChestFlashRiskHigh_Male", 0x865);
                    AddANDKeyword("AND_ChestFlashRiskLow", 0x801);
                    AddANDKeyword("AND_ChestFlashRiskLow_Male", 0x863);
                    AddANDKeyword("AND_ChestFlashRiskUltra", 0x81B);
                    AddANDKeyword("AND_ChestFlashRiskUltra_Male", 0x867);
                    AddANDKeyword("AND_ChestFlashRisk_Male", 0x864);
                    AddANDKeyword("AND_CoversAll", 0x807);
                    AddANDKeyword("AND_CoversAll_Male", 0x87F);
                    AddANDKeyword("AND_CString", 0x80A);
                    AddANDKeyword("AND_CStringT", 0x808);
                    AddANDKeyword("AND_CStringT_High", 0x84E);
                    AddANDKeyword("AND_CStringT_Low", 0x84D);
                    AddANDKeyword("AND_EffectivelyNaked", 0x876);
                    AddANDKeyword("AND_EffectivelyNaked_Male", 0x881);
                    AddANDKeyword("AND_HimboSkirt", 0x88B);
                    AddANDKeyword("AND_HimboSkirtT", 0x88C);
                    AddANDKeyword("AND_HimboSkirtT_High", 0x850);
                    AddANDKeyword("AND_HimboSkirtT_Low", 0x84F);
                    AddANDKeyword("AND_Hotpants", 0x828);
                    AddANDKeyword("AND_HotpantsT", 0x829);
                    AddANDKeyword("AND_HotpantsT_High", 0x852);
                    AddANDKeyword("AND_HotpantsT_High_Male", 0x854);
                    AddANDKeyword("AND_HotpantsT_Low", 0x851);
                    AddANDKeyword("AND_HotpantsT_Low_Male", 0x853);
                    AddANDKeyword("AND_HotpantsT_Male", 0x883);
                    AddANDKeyword("AND_Hotpants_Male", 0x882);
                    AddANDKeyword("AND_Ignore", 0x838);
                    AddANDKeyword("AND_Microskirt", 0x824);
                    AddANDKeyword("AND_Microskirt_Male", 0x884);
                    AddANDKeyword("AND_Miniskirt", 0x821);
                    AddANDKeyword("AND_MiniskirtT", 0x818);
                    AddANDKeyword("AND_MiniskirtT_Male", 0x886);
                    AddANDKeyword("AND_Miniskirt_Male", 0x885);
                    AddANDKeyword("AND_NearlyNaked", 0x825);
                    AddANDKeyword("AND_NearlyNaked_Male", 0x887);
                    AddANDKeyword("AND_NipplePasties", 0x826);
                    AddANDKeyword("AND_NipplePasties_Male", 0x888);
                    AddANDKeyword("AND_PelvicCurtain", 0x822);
                    AddANDKeyword("AND_PelvicCurtainT", 0x816);
                    AddANDKeyword("AND_PelvicCurtainT_Male", 0x88A);
                    AddANDKeyword("AND_PelvicCurtain_Male", 0x889);
                    AddANDKeyword("AND_PelvicFlashRisk", 0x80D);
                    AddANDKeyword("AND_PelvicFlashRiskExtreme", 0x80E);
                    AddANDKeyword("AND_PelvicFlashRiskExtreme_Male", 0x86B);
                    AddANDKeyword("AND_PelvicFlashRiskHigh", 0x80F);
                    AddANDKeyword("AND_PelvicFlashRiskHigh_Male", 0x86A);
                    AddANDKeyword("AND_PelvicFlashRiskLow", 0x810);
                    AddANDKeyword("AND_PelvicFlashRiskLow_Male", 0x868);
                    AddANDKeyword("AND_PelvicFlashRiskUltra", 0x81A);
                    AddANDKeyword("AND_PelvicFlashRiskUltra_Male", 0x86C);
                    AddANDKeyword("AND_PelvicFlashRisk_Male", 0x869);
                    AddANDKeyword("AND_ShowgirlSkirt", 0x817);
                    AddANDKeyword("AND_ShowgirlSkirtT", 0x819);
                    AddANDKeyword("AND_ShowgirlSkirtT_High", 0x85A);
                    AddANDKeyword("AND_ShowgirlSkirtT_Low", 0x859);
                    AddANDKeyword("AND_Thong", 0x81F);
                    AddANDKeyword("AND_ThongT", 0x820);
                    AddANDKeyword("AND_ThongT_High", 0x85C);
                    AddANDKeyword("AND_ThongT_High_Male", 0x85E);
                    AddANDKeyword("AND_ThongT_Low", 0x85B);
                    AddANDKeyword("AND_ThongT_Low_Male", 0x85D);
                    AddANDKeyword("AND_ThongT_Male", 0x88F);
                    AddANDKeyword("AND_Thong_Male", 0x88D);
                    AddANDKeyword("AND_Thong_NoCover", 0x83D);
                    AddANDKeyword("AND_Thong_NoCover_Male", 0x88E);
                    AddANDKeyword("AND_Underwear", 0x805);
                    AddANDKeyword("AND_UnderwearT", 0x81E);
                    AddANDKeyword("AND_UnderwearT_High", 0x860);
                    AddANDKeyword("AND_UnderwearT_High_Male", 0x83E);
                    AddANDKeyword("AND_UnderwearT_Low", 0x85F);
                    AddANDKeyword("AND_UnderwearT_Low_Male", 0x861);
                    AddANDKeyword("AND_UnderwearT_Male", 0x892);
                    AddANDKeyword("AND_Underwear_Male", 0x890);
                    AddANDKeyword("AND_Underwear_NoCover", 0x83A);
                    AddANDKeyword("AND_Underwear_NoCover_Male", 0x891);
                    AddANDKeyword("AND_VaginaPasties", 0x827);

                } catch (const std::exception& e) {
                    logger::error("NudityHelper::InitialzeAND exception: {}", e.what());
                    return;
                } catch (...) {
                    logger::error("NudityHelper::InitialzeAND unknown exception");
                    return;
                }
            }

            // Initialize Object Categorization Framework (OCF) related variables
            // MARK: - InitialzeOCF
            static void InitialzeOCF() {
                try {
                    if (isInitialized || dataHandler->LookupModByName(modfile_OCF) == nullptr) {
                        // Mod not found, or already initialized
                        return;
                    }

                    // Mark OCF as present
                    hasOCF = true;

                    // NOTE: I added the `OCF_` prefix to these keywords
                    AddOCFKeyword("OCF_ClothingBra", 0xAF0500);
                    AddOCFKeyword("OCF_ClothingBracelet", 0xE814F1);
                    AddOCFKeyword("OCF_ClothingChoker", 0xDD103);
                    AddOCFKeyword("OCF_ClothingCrown", 0x10AB46);
                    AddOCFKeyword("OCF_ClothingEarCuffs", 0xEC4C8);
                    AddOCFKeyword("OCF_ClothingEarrings", 0xDE3FD2);
                    AddOCFKeyword("OCF_ClothingHighNecklace", 0xFB81E);
                    AddOCFKeyword("OCF_ClothingPanties", 0xAF0501);
                    AddOCFKeyword("OCF_ClothingStrapOn", 0xAF0502);

                } catch (const std::exception& e) {
                    logger::error("NudityHelper::InitialzeOCF exception: {}", e.what());
                    return;
                } catch (...) {
                    logger::error("NudityHelper::InitialzeOCF unknown exception");
                    return;
                }
            }

            // Initialize SexLab Aroused (SLA) related variables
            // MARK: - InitialzeSLA
            static void InitialzeSLA() {
                try {
                    if (isInitialized || dataHandler->LookupModByName(modfile_SLA) == nullptr) {
                        // Mod not found, or already initialized
                        return;
                    }

                    // Mark SLA as present
                    hasSLA = true;

                    AddSLAKeyword("SLA_AnalPlug", 0x8F404);
                    AddSLAKeyword("SLA_AnalPlugBeads", 0x8F406);
                    AddSLAKeyword("SLA_AnalPlugTail", 0x8F405);
                    AddSLAKeyword("SLA_ArmorBondage", 0x8FEA5);
                    AddSLAKeyword("SLA_ArmorCapeFull", 0x8F3FE);
                    AddSLAKeyword("SLA_ArmorCapeMini", 0x8F3FF);
                    AddSLAKeyword("SLA_ArmorCurtain", 0x8F40C);
                    AddSLAKeyword("SLA_ArmorFemaleOnly", 0x8F90C);
                    AddSLAKeyword("SLA_ArmorHalfNaked", 0x8E855);
                    AddSLAKeyword("SLA_ArmorHalfNakedBikini", 0x8E854);
                    AddSLAKeyword("SLA_ArmorHarness", 0x8F3FC);
                    AddSLAKeyword("SLA_ArmorIllegal", 0x8F327);
                    AddSLAKeyword("SLA_ArmorLewdLeotard", 0x8F401);
                    AddSLAKeyword("SLA_ArmorPartBottom", 0x8FEA1);
                    AddSLAKeyword("SLA_ArmorPartTop", 0x8FEA0);
                    AddSLAKeyword("SLA_ArmorPretty", 0x8E853);
                    AddSLAKeyword("SLA_ArmorRubber", 0x8FEA4);
                    AddSLAKeyword("SLA_ArmorSpendex", 0x8E858);
                    AddSLAKeyword("SLA_ArmorTransparent", 0x8F3FD);
                    AddSLAKeyword("SLA_BootsHeels", 0x8F410);
                    AddSLAKeyword("SLA_Brabikini", 0x8E856);
                    AddSLAKeyword("SLA_Earrings", 0x8F3FB);
                    AddSLAKeyword("SLA_EroticArmor", 0x8C7F6);  // NOTE! I added the `SLA_` prefix to this keyword
                    AddSLAKeyword("SLA_FullSkirt", 0x8F40D);
                    AddSLAKeyword("SLA_HasLeggings", 0x8FE9F);
                    AddSLAKeyword("SLA_HasSleeves", 0x8FEA2);
                    AddSLAKeyword("SLA_HasStockings", 0x8FEA3);
                    AddSLAKeyword("SLA_ImpossibleClothes", 0x8F400);
                    AddSLAKeyword("SLA_KillerHeels", 0x8F326);
                    AddSLAKeyword("SLA_MicroHotpants", 0x8F3F4);
                    AddSLAKeyword("SLA_MicroSkirt", 0x8F40F);
                    AddSLAKeyword("SLA_MiniSkirt", 0x8F40E);
                    AddSLAKeyword("SLA_PantiesNormal", 0x8EDC1);
                    AddSLAKeyword("SLA_PantsNormal", 0x8F3F3);
                    AddSLAKeyword("SLA_PastiesCrotch", 0x8F409);
                    AddSLAKeyword("SLA_PastiesNipple", 0x8F40A);
                    AddSLAKeyword("SLA_PelvicCurtain", 0x8F402);
                    AddSLAKeyword("SLA_PiercingBelly", 0x8F3F7);
                    AddSLAKeyword("SLA_PiercingClit", 0x8F40B);
                    AddSLAKeyword("SLA_PiercingLips", 0x8F3F9);
                    AddSLAKeyword("SLA_PiercingNipple", 0x8F3F8);
                    AddSLAKeyword("SLA_PiercingNose", 0x8F3FA);
                    AddSLAKeyword("SLA_PiercingVulva", 0x8F3F6);
                    AddSLAKeyword("SLA_ShowgirlSkirt", 0x8F403);
                    AddSLAKeyword("SLA_ThongCString", 0x8EDC3);
                    AddSLAKeyword("SLA_ThongGstring", 0x8F3F5);
                    AddSLAKeyword("SLA_ThongLowleg", 0x8EDC2);
                    AddSLAKeyword("SLA_ThongT", 0x8E857);
                    AddSLAKeyword("SLA_VaginalBeads", 0x8F407);
                    AddSLAKeyword("SLA_VaginalDildo", 0x8F408);

                } catch (const std::exception& e) {
                    logger::error("NudityHelper::InitialzeSLA exception: {}", e.what());
                    return;
                } catch (...) {
                    logger::error("NudityHelper::InitialzeSLA unknown exception");
                    return;
                }
            }
#pragma endregion

        public:
            // MARK: - Initialize
            static void Initialize() {
                if (IsInitialized) {
                    // Prevent reinitialization
                    return;
                }

                dataHandler = RE::TESDataHandler::GetSingleton();
                if (!dataHandler) {
                    logger::error("NudityHelper::Initialize: TESDataHandler not available");
                    return;
                }

                InitialzeAND();
                InitialzeOCF();
                InitialzeSLA();
            }

            static bool IsInitialized() {
                return IsInitialized;
            }

            static bool HasAND() {
                return hasAND;
            }

            static bool HasOCF() {
                return hasOCF;
            }

            static bool HasSLA() {
                return hasSLA;
            }

            // Public getter to access a keyword by its string identifier (read-only)
            static RE::BGSKeyword* GetKeyword(const std::string& key) {
                auto it = allKeywords.find(key);
                return it != allKeywords.end() ? it->second : nullptr;
            }

            static bool WornHasKeyword(const RE::Actor* actor, const std::string& keyword) {
                if (!actor) {
                    return false;
                }

                RE::BGSKeyword* keywordPtr = GetKeyword(keyword);
                if (!keywordPtr) {
                    return false;
                }

                for (const auto& entry: actor->GetWornArmor()) {
                    RE::TESObjectARMO* armor = entry.first;
                    if (armor && armor->HasKeyword(keywordPtr)) {
                        return true;
                    }
                }

                return false;
            }
    }
}  // namespace plugin