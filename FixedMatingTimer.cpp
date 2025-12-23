#include <API/ARK/Ark.h>

namespace FixedMatingTimer {
// Configuration
float g_FixedMatingInterval =
    18.0f; // Fixed interval in hours (default 18 hours)

// Hook to override mating cooldown
void Hook_APrimalDinoCharacter_SetMatingIntervalEnd(
    APrimalDinoCharacter *_this) {
  if (_this) {
    // Get current time
    UWorld *world = ArkApi::GetApiUtils().GetWorld();
    if (!world)
      return;

    double currentTime = world->GetTimeSeconds();

    // Set fixed mating interval (convert hours to seconds)
    double fixedInterval = g_FixedMatingInterval * 3600.0;

    // Set the new mating end time
    _this->NextAllowedMatingTimeField() = currentTime + fixedInterval;

    Log::GetLog()->info("Fixed mating timer set: {} hours for dino {}",
                        g_FixedMatingInterval,
                        _this->GetDinoNameTag().ToString());
  }
}

// Hook the function that sets mating cooldown
DECLARE_HOOK(APrimalDinoCharacter_SetMatingIntervalEnd, void,
             APrimalDinoCharacter *);
void Hook_APrimalDinoCharacter_SetMatingIntervalEnd_Implementation(
    APrimalDinoCharacter *_this) {
  Hook_APrimalDinoCharacter_SetMatingIntervalEnd(_this);
}

// Admin command to change the fixed interval
void SetMatingIntervalCommand(AShooterPlayerController *player,
                              FString *message, int mode) {
  TArray<FString> parsed;
  message->ParseIntoArray(parsed, L" ", true);

  if (parsed.Num() < 2) {
    ArkApi::GetApiUtils().SendChatMessage(player, "FixedMatingTimer",
                                          "Usage: /setmatinginterval <hours>");
    return;
  }

  try {
    float newInterval = std::stof(parsed[1].ToString());
    if (newInterval <= 0) {
      ArkApi::GetApiUtils().SendChatMessage(player, "FixedMatingTimer",
                                            "Interval must be greater than 0");
      return;
    }

    g_FixedMatingInterval = newInterval;

    FString response =
        FString::Format("Fixed mating interval set to {} hours", newInterval);
    ArkApi::GetApiUtils().SendChatMessage(player, "FixedMatingTimer", response);

    Log::GetLog()->info(
        "Admin {} changed mating interval to {} hours",
        ArkApi::GetApiUtils().GetCharacterName(player).ToString(), newInterval);
  } catch (...) {
    ArkApi::GetApiUtils().SendChatMessage(player, "FixedMatingTimer",
                                          "Invalid number format");
  }
}

// Load configuration from JSON
void LoadConfig() {
  nlohmann::json config;
  const std::string configPath = ArkApi::Tools::GetCurrentDir() +
                                 "/ArkApi/Plugins/FixedMatingTimer/config.json";

  std::ifstream file(configPath);
  if (file.is_open()) {
    try {
      file >> config;
      g_FixedMatingInterval = config.value("FixedMatingIntervalHours", 18.0f);

      Log::GetLog()->info(
          "FixedMatingTimer: Loaded config - Interval: {} hours",
          g_FixedMatingInterval);
    } catch (const std::exception &e) {
      Log::GetLog()->error("FixedMatingTimer: Failed to load config: {}",
                           e.what());
    }
    file.close();
  } else {
    // Create default config
    config["FixedMatingIntervalHours"] = g_FixedMatingInterval;

    std::ofstream outFile(configPath);
    if (outFile.is_open()) {
      outFile << config.dump(4);
      outFile.close();
      Log::GetLog()->info("FixedMatingTimer: Created default config");
    }
  }
}

void Load() {
  Log::GetLog()->info("FixedMatingTimer plugin loaded");

  LoadConfig();

  // Hook the mating interval setter
  ArkApi::GetHooks().SetHook(
      "APrimalDinoCharacter.SetMatingIntervalEnd",
      &Hook_APrimalDinoCharacter_SetMatingIntervalEnd_Implementation,
      &APrimalDinoCharacter_SetMatingIntervalEnd_original);

  // Register admin command
  ArkApi::GetCommands().AddChatCommand("/setmatinginterval",
                                       &SetMatingIntervalCommand);

  Log::GetLog()->info("FixedMatingTimer: Fixed mating interval set to {} hours",
                      g_FixedMatingInterval);
}

void Unload() {
  ArkApi::GetHooks().DisableHook(
      "APrimalDinoCharacter.SetMatingIntervalEnd",
      &Hook_APrimalDinoCharacter_SetMatingIntervalEnd_Implementation);

  ArkApi::GetCommands().RemoveChatCommand("/setmatinginterval");

  Log::GetLog()->info("FixedMatingTimer plugin unloaded");
}
} // namespace FixedMatingTimer

extern "C" __declspec(dllexport) void Plugin_Init() {
  FixedMatingTimer::Load();
}

extern "C" __declspec(dllexport) void Plugin_Unload() {
  FixedMatingTimer::Unload();
}
