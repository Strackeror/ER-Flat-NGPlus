#include "deps/from/modutils.hpp"
#include <Windows.h>
#include <chrono>
#include <exception>
#include <filesystem>
#include <format>
#include <iostream>
#include <ostream>
#include <paramdef/SP_EFFECT_PARAM_ST.hpp>
#include <params.hpp>
#include <thread>

#define TOML_EXCEPTIONS 0
#include <toml++/toml.hpp>


std::filesystem::path get_folder(HINSTANCE dll_instance) {
  wchar_t dll_filename[MAX_PATH] = {0};
  GetModuleFileNameW(dll_instance, dll_filename, MAX_PATH);
  return std::filesystem::path(dll_filename).parent_path();
}

void start(HINSTANCE dll_instance) {
  std::this_thread::sleep_for(std::chrono::seconds(2));
  auto folder = get_folder(dll_instance);

  auto result = toml::parse_file((folder / "better_ng_plus.toml").c_str());
  toml::table table{};
  if (result) {
    table = std::move(result.table());
  }

  const auto log_file = table["log"]["file"].value<std::string_view>();
  const auto scaling_base_game = table["scaling"]["target"].value_or(16);

  std::optional<std::ofstream> maybe_ofstream;
  auto *log = &std::cout;
  if (log_file) {
    maybe_ofstream = std::ofstream((folder / *log_file));
    log = &*maybe_ofstream;
  }

  modutils::initialize();
  from::params::initialize();

  const auto base_id = 7000;
  const auto target_id = scaling_base_game * 10 + base_id;
  auto params = from::params::get_param<from::paramdef::SP_EFFECT_PARAM_ST>(
      L"SpEffectParam");
  auto base_scaling = params[target_id];
  auto base_ngplus_scaling = params[target_id + 400];

  for (int i = base_id; i < target_id; i += 10) {
    auto current_base = params[i];
    auto &current_ng = params[i + 400];

    auto rescale = [&](auto field) {
      auto scaled_value = base_scaling.*field * base_ngplus_scaling.*field;
      current_ng.*field = scaled_value / current_base.*field;
      *log << std::format("id:{} --- {} * {} = {}", i, //
                          current_base.*field, current_ng.*field, scaled_value)
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
  if (fdwReason == DLL_PROCESS_ATTACH) {
    std::thread{[=]() {
      try {
        start(hinstDll);
      } catch (std::exception e) {
        MessageBox(0, e.what(), "error", 0);
      }
    }}.detach();
  }
  return TRUE;
}