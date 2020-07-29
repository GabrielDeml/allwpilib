/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once
#include <cmath>

#include <hal/SimDevice.h>
#include <imgui.h>

#define IMGUI_DEFINE_MATH_OPERATORS

#include <imgui_internal.h>
#include <mockdata/SimDeviceData.h>
#include <units/units.h>
#include <wpi/json.h>
#include <wpi/raw_istream.h>
#include <wpi/raw_ostream.h>
#include "HALSimGui.h"
#include <string>
#include <wpi/math>
#include "portable-file-dialogs.h"

namespace halsimgui {

class Mechanism2D {
 public:
  void Initialize();

 private:
  struct BodyConfig {
    std::string name;
    std::string type = "line";
    // TODO: use this
    int startLocation = 0;
    int length = 100;
    std::string color = "green";
    int angle = 0;
    std::vector<BodyConfig> children;
    int lineWidth = 1;
  };

  struct DrawLineStruct {
    float xEnd;
    float yEnd;
    float angle;
  };

  std::vector<BodyConfig> bodyConfigVector;
  void buildColorTable();
  bool ReadIni(wpi::StringRef name, wpi::StringRef value);
  void WriteIni(ImGuiTextBuffer* out);
  void* Mechanism2DReadOpen(ImGuiContext* ctx, ImGuiSettingsHandler* handler,
                            const char* name);
  void Mechanism2DReadLine(ImGuiContext* ctx, ImGuiSettingsHandler* handler,
                           void* entry, const char* lineStr);
  void Mechanism2DWriteAll(ImGuiContext* ctx, ImGuiSettingsHandler* handler,
                           ImGuiTextBuffer* out_buf);
  void GetJsonFileLocation();
  DrawLineStruct DrawLine(float startXLocation, float startYLocation,
                          int length, float angle, ImDrawList* drawList,
                          ImVec2 windowPos, ImColor color,
                          const BodyConfig& bodyConfig,
                          const std::string& previousPath);
  void buildDrawList(float startXLocation, float startYLocation,
                     ImDrawList* drawList, float previousAngle,
                     const std::vector<BodyConfig>& subBodyConfigs,
                     ImVec2 windowPos);
  BodyConfig readSubJson(const std::string& name, wpi::json const& body);
  void readJson(std::string jFile);
  void OptionMenuLocateJson();
  void DisplayAssembly2D();
};

}  // namespace halsimgui
