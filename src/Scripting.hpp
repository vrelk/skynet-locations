#pragma once

#undef GetObject

using namespace SKSE;

namespace VSHOCK {

    RE::BSScript::IVirtualMachine* GetVM() {
        return RE::BSScript::Internal::VirtualMachine::GetSingleton();
    }

    RE::BSScript::IObjectHandlePolicy* GetHP() {
        auto vm = GetVM();
        return vm->GetObjectHandlePolicy();
    }

    bool CheckESPLoaded() {
        RE::TESForm* form = RE::TESDataHandler::GetSingleton()->LookupForm(DCURSES_MCM, "Devious Curses.esp");
        if (form == nullptr) {
            return false;
        }
        return true;
    }

    class ScriptIntent {
        public:
            std::string script_name;
            std::string function;
            RE::TESForm* form = nullptr;
            RE::FormType type;

            RE::BSScript::IFunctionArguments* args;

            ScriptIntent(std::string script_name, std::string function, RE::BSScript::IFunctionArguments* args) {
                this->script_name = script_name;
                this->function = function;
                this->args = args;
            }
            ScriptIntent(std::string script_name, std::string function) {
                this->script_name = script_name;
                this->function = function;
                this->args = new RE::BSScript::ZeroFunctionArguments;
            }
            ScriptIntent(RE::TESForm* form, RE::FormType type, std::string script_name, std::string function,
                         RE::BSScript::IFunctionArguments* args) {
                this->script_name = script_name;
                this->function = function;
                this->form = form;
                this->type = type;
                this->args = args;
            }
            ScriptIntent(RE::TESForm* form, RE::FormType type, std::string script_name, std::string function) {
                this->script_name = script_name;
                this->function = function;
                this->form = form;
                this->type = type;
                this->args = new RE::BSScript::ZeroFunctionArguments;
            }
    };

    class VirtualScriptCallbackFunctor : public RE::BSScript::IStackCallbackFunctor {
        public:
            RE::BSTSmartPointer<VirtualScriptCallbackFunctor> nextCallback = nullptr;
            ScriptIntent intent = ScriptIntent("", "", nullptr);

            void dispatch() {
                RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor> callback =
                    RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor>(this);
                if (intent.form == nullptr) {
                    GetVM()->DispatchStaticCall(intent.script_name, intent.function, intent.args, callback);
                    delete intent.args;
                } else {
                    RE::VMHandle hand = GetHP()->GetHandleForObject(intent.type, intent.form);
                    RE::BSTSmartPointer<RE::BSScript::Object> obj;
                    GetVM()->FindBoundObject(hand, intent.script_name.c_str(), obj);
                    GetVM()->DispatchMethodCall(obj, intent.function, intent.args, callback);
                    delete intent.args;
                }
            }

            virtual void SetObject(const RE::BSTSmartPointer<RE::BSScript::Object>&) override {}

            VirtualScriptCallbackFunctor(ScriptIntent intent) {
                this->intent = intent;
            }

            void SetNextCallback(RE::BSTSmartPointer<VirtualScriptCallbackFunctor> next) {
                nextCallback = next;
            }

            RE::BSTSmartPointer<VirtualScriptCallbackFunctor> PushNewCallback(RE::BSTSmartPointer<VirtualScriptCallbackFunctor> next) {
                auto tempCallback = nextCallback;
                nextCallback = next;
                next->nextCallback = tempCallback;
                return next;
            }
    };

    class ScriptingManager {
        private:
            typedef std::vector<std::pair<ScriptIntent, ScriptCallback>> IntentList;
            static IntentList* intents() {
                static IntentList intents;
                return &intents;
            }

            static uint32_t refCount;

            static void DoSequential(IntentList intents) {
                RE::BSTSmartPointer<VirtualScriptCallbackFunctor> dispatch = intents[0].second;

                for (int i = static_cast<int>(intents.size()) - 2; i >= 0; i--) {
                    intents[i].second->SetNextCallback(intents[i + 1].second);
                }

                dispatch->dispatch();
            }

            static void RunOrderedIntent() {
                if (!intents()->empty()) {
                    DoSequential(*intents());
                    intents()->clear();
                }
            }

        public:
            ScriptingManager(ScriptingManager const&) = delete;
            void operator=(ScriptingManager const&) = delete;

            ScriptingManager() {
                ScriptingManager::refCount += 1;
            }

            ~ScriptingManager() {
                ScriptingManager::refCount -= 1;
                if (ScriptingManager::refCount == 0) {
                    ScriptingManager::RunOrderedIntent();
                }
            }

            void RunIntent(ScriptIntent intent) {
                intents()->push_back({intent, ScriptCallback(new ScriptCallbackFunctor(intent))});
            }

            template <class T>
            void RunIntentWith(ScriptIntent intent, std::function<void(std::optional<T>, ScriptCallback)> onCallback) {
                intents()->push_back({intent, ScriptCallback(new ScriptCallbackFunctor_R<T>(intent, onCallback))});
            }

            template <class T>
            void RunIntentWithResult(ScriptIntent intent, std::function<ScriptCallback(std::optional<T>)> onCallback) {
                std::function<void(std::optional<T>, ScriptCallback)> newCallbackFunction = [onCallback](std::optional<T> data,
                                                                                                         ScriptCallback firstCallback) {
                    auto newCallback = onCallback(data);
                    auto tempCallback = firstCallback->nextCallback;
                    firstCallback->nextCallback = newCallback;
                    newCallback->nextCallback = tempCallback;
                };
                intents()->push_back({intent, ScriptCallback(new ScriptCallbackFunctor_R<T>(intent, newCallbackFunction))});
            }

            template <class T>
            void RunIntentWithResultSimple(ScriptIntent intent, std::function<void(T)> onCallback) {
                std::function<void(T, ScriptCallback, ScriptCallback)> newCallbackFunction =
                    [onCallback](T data, ScriptCallback firstCallback, ScriptCallback nextCallback) { onCallback(data); };
                RunIntentWithResult(intent, newCallbackFunction);
            }

            void PushActorAway(RE::TESObjectREFR* object, RE::Actor* akActor, float force) {
                RE::BSScript::IFunctionArguments* args = RE::MakeFunctionArguments<RE::Actor*, float>(std::move(akActor), std::move(force));
                auto intent = ScriptIntent(object, RE::FormType::Reference, "ObjectReference", "PushActorAway", args);
                RunIntent(intent);
            }

            void DBGMessageBox(Translator trans) {
                auto message = trans.GetTranslation();
                RE::BSScript::IFunctionArguments* args = RE::MakeFunctionArguments<std::string>(std::move(message));
                auto intent3 = ScriptIntent("Debug", "MessageBox", args);
                RunIntent(intent3);
            }

            void DBGNotification(Translator trans) {
                auto message = trans.GetTranslation();
                RE::BSScript::IFunctionArguments* args = RE::MakeFunctionArguments<std::string>(std::move(message));
                auto intent3 = ScriptIntent("Debug", "Notification", args);
                RunIntent(intent3);
            }
    };

    uint32_t ScriptingManager::refCount = 0;
}  // namespace VSHOCK