#pragma once
#include <string>
#include <nlohmann/json.hpp>

struct Profile {
  std::string id, name, exe;  // exe vazio = perfil padrão (Windows)
  bool enabled = true;
  int saturation = 100;  // 0..300 (%), 100 = neutro
  int contrast = 50;     // 0..100, 50 = neutro
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Profile, id, name, exe, enabled, saturation, contrast)
