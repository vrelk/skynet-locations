Scriptname SkynetDetailsAPI

string Function getLocationDescriptionByActor(Actor akActor) global native
string Function getLocationDescriptionByEID(string editorID) global native

string Function getQuestDescription(string questEID) global native
string Function getStageDescription(string questEID, int stage) global native
string Function getObjectiveDescription(string questEID, int stage, int objective) global native
string Function getSceneDescription(string scene, int phase, bool exactMatch = false) global native

String Function getActorBirthday(Actor akActor, bool thirdPerson = false) global native
String Function getSlaveInfo(Actor akActor) global native

Function insertCellPlaceholder(string modName, int formID, string name = "", string notes = "") global native
Function insertLocationPlaceholder(string modName, string editorID, string name = "") global native
Function insertWorldspacePlaceholder(string modName, string editorID, string name = "") global native

String Function getActorTattoosJson(Actor akActor, string area = "") global native