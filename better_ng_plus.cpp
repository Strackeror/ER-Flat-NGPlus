#include <Windows.h>
#include <coresystem/cs_param.hpp>
#include <format>
#include <iostream>
#include <param/param.hpp>

// Open the console and redirect output
// Useful for writing to the console from inside ELDEN RING.
// Optionally, do not flush console unless explicitly flushed
// with std::endl or std::cout::flush
inline void con_allocate(bool no_flush) noexcept {
  AllocConsole();
  FILE *out;
  freopen_s(&out, "CON", "w", stdout);
  if (no_flush) {
    std::ios_base::sync_with_stdio(false);
    std::setvbuf(stdout, nullptr, _IOFBF, BUFSIZ);
  }
}

void start() {
  con_allocate(true);

  std::cout << "toast" << std::endl;
  from::CS::SoloParamRepository::wait_for_params(-1);

  static const int TARGET_ID = 7160;
  auto base_scaling = from::param::SpEffectParam[TARGET_ID].first;
  auto base_ngplus_scaling = from::param::SpEffectParam[TARGET_ID].first;

  for (int i = 7000; i < TARGET_ID; i += 100) {
    auto current_base = from::param::SpEffectParam[i].first;
    auto &current_ng = from::param::SpEffectParam[i + 400].first;

    auto rescale = [&](auto field) {
      auto scaled_value = base_scaling.*field * base_ngplus_scaling.*field;
      current_ng.*field = scaled_value / current_base.*field;
      std::cout << std::format("id:{} {} * {} = {} ----> {} * {}", i,
                               base_scaling.*field, base_ngplus_scaling.*field,
                               scaled_value, current_base.*field,
                               current_ng.*field)
                << std::endl;
    };

    rescale(&from::paramdef::SP_EFFECT_PARAM_ST::maxHpRate);
    rescale(&from::paramdef::SP_EFFECT_PARAM_ST::maxStaminaRate);
    rescale(&from::paramdef::SP_EFFECT_PARAM_ST::staminaAttackRate);

    rescale(&from::paramdef::SP_EFFECT_PARAM_ST::physicsAttackPowerRate);
    rescale(&from::paramdef::SP_EFFECT_PARAM_ST::magicAttackPowerRate);
    rescale(&from::paramdef::SP_EFFECT_PARAM_ST::fireAttackPowerRate);
    rescale(&from::paramdef::SP_EFFECT_PARAM_ST::thunderAttackPowerRate);
    rescale(&from::paramdef::SP_EFFECT_PARAM_ST::darkAttackPowerRate);

    rescale(&from::paramdef::SP_EFFECT_PARAM_ST::physicsDiffenceRate);
    rescale(&from::paramdef::SP_EFFECT_PARAM_ST::magicDiffenceRate);
    rescale(&from::paramdef::SP_EFFECT_PARAM_ST::fireDiffenceRate);
    rescale(&from::paramdef::SP_EFFECT_PARAM_ST::thunderDiffenceRate);
    rescale(&from::paramdef::SP_EFFECT_PARAM_ST::darkDiffenceRate);

    rescale(&from::paramdef::SP_EFFECT_PARAM_ST::registPoizonChangeRate);
    rescale(&from::paramdef::SP_EFFECT_PARAM_ST::registBloodChangeRate);
    rescale(&from::paramdef::SP_EFFECT_PARAM_ST::registDiseaseChangeRate);
    rescale(&from::paramdef::SP_EFFECT_PARAM_ST::registFreezeChangeRate);
    rescale(&from::paramdef::SP_EFFECT_PARAM_ST::registSleepChangeRate);
    rescale(&from::paramdef::SP_EFFECT_PARAM_ST::registMadnessChangeRate);
  }
}

// DllMain - will be used as the test dll's entry point
BOOL DllMain(HINSTANCE hinstDll, DWORD fdwReason, LPVOID lpvReserved) {
  if (fdwReason == DLL_PROCESS_ATTACH)
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&start, NULL, 0, NULL);
  return TRUE;
}