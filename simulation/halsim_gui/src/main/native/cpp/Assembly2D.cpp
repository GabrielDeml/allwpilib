/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Assembly2D.h"

#include <cmath>

#include <GL/gl3w.h>
#include <hal/SimDevice.h>
#include <imgui.h>

#define IMGUI_DEFINE_MATH_OPERATORS

#include <imgui_internal.h>
#include <mockdata/SimDeviceData.h>
#include <units/units.h>
#include <wpi/Path.h>
#include <wpi/SmallString.h>
#include <wpi/json.h>
#include <wpi/raw_istream.h>
#include <wpi/raw_ostream.h>


#include "GuiUtil.h"
#include "HALSimGui.h"
#include "SimDeviceGui.h"
#include "portable-file-dialogs.h"
#include <mockdata/EncoderData.h>

#include <vector>
#include <map>
#include <list>
#include <list>
#include <string>
#include <cmath>

using namespace halsimgui;

int windowWidth = 100;
int windowHeight = 100;

static std::vector<std::string> listOfColors = {"white", "silver", "gray", "black", "red", "maroon", "yellow", "olive",
                                                "lime",
                                                "green", "aqua", "teal", "blue", "navy", "fuchsia", "purple"};

static std::vector<ImColor> listOfRGB = {IM_COL32(255, 255, 255, 255), IM_COL32(192, 192, 192, 255),
                                         IM_COL32(128, 128, 128, 255), IM_COL32(0, 0, 0, 255), IM_COL32(255, 0, 0, 255),
                                         IM_COL32(128, 0, 0, 255), IM_COL32(255, 255, 0, 255),
                                         IM_COL32(128, 128, 0, 255), IM_COL32(0, 255, 0, 255), IM_COL32(0, 128, 0, 255),
                                         IM_COL32(0, 255, 255, 255),
                                         IM_COL32(0, 128, 128, 255), IM_COL32(0, 0, 255, 255), IM_COL32(0, 0, 128, 255),
                                         IM_COL32(255, 0, 255, 255), IM_COL32(128, 0, 128, 255)};

std::map<std::string, ImColor> colorLookUpTable;


static void buildColorTable() {
    for (int i = 0; i < (int) listOfColors.size(); i++) {
        colorLookUpTable[listOfColors[i]] = listOfRGB[i];
    }
}

struct BodyConfig {
    std::string name;
    std::string type = "line";
    int startLocation = 0;
    int length = 100;
    std::string color = "green";
    int angle = 0;
    // These were for the hard stop, but I don't think we should have them anymore
    int maxAngle = 999;
    int minAngle = -999;
    std::list<BodyConfig> children;
    int lineWidth = 1;
};

std::list<BodyConfig> bodyConfigList;

//HAL_SimDeviceHandle assembly2DHandle = HALSIM_GetSimDeviceHandle("Assembly2D");

std::tuple<float, float, float>
DrawLine(int startXLocation, int startYLocation, int length, double angle, ImDrawList *drawList, ImVec2 windowPos,
         ImColor color, const BodyConfig &bodyConfig, const std::string &previousPath) {
    // Find the current path do the ligament
    std::string currentPath = previousPath + bodyConfig.name;
    // Find the angle in radians
    double radAngle = (angle - 90) * 3.14159 / 180;
    // Get the start X and Y location
    double xEnd = startXLocation + length * std::cos(radAngle);
    double yEnd = startYLocation + length * std::sin(radAngle);
    // Add the line to the drawList
    drawList->AddLine(windowPos + ImVec2(startXLocation, startYLocation),
                      windowPos + ImVec2(xEnd, yEnd),
                      color, bodyConfig.lineWidth);
    // Return the end X, Y, and angle
    return {xEnd, yEnd, angle};
}

HAL_SimDeviceHandle m_devHandle = 0;


static void buildDrawList(int startXLocation, int startYLocation, ImDrawList *drawList, int previousAngle,
                          const std::list<BodyConfig> &subBodyConfigs, ImVec2 windowPos) {
    for (BodyConfig const &bodyConfig : subBodyConfigs) {
        hal::SimDouble m_aHandle;
        double m_a;
        // Get the smallest of width or height
        int minSize;

        if (ImGui::GetWindowHeight() > ImGui::GetWindowWidth()) {
            minSize = ImGui::GetWindowWidth();
        } else {
            minSize = ImGui::GetWindowHeight();
        }


        if (m_devHandle == 0) m_devHandle = HALSIM_GetSimDeviceHandle("Assembly2D");
        if (m_devHandle == 0) return;

        if (!m_aHandle) m_aHandle = HALSIM_GetSimValueHandle(m_devHandle, bodyConfig.name.c_str());
        if (m_aHandle) m_a = m_aHandle.Get();
        else m_a = 0;
        wpi::outs() << bodyConfig.name << " " << std::to_string(m_a) << "\n";

        // Calculate the next angle to go to
        int angleToGoTo = m_a + bodyConfig.angle + previousAngle;

        // Hard stop -- This probably should be taken out
//        if (bodyConfig.maxAngle < HALSIM_GetEncoderCount(0) + bodyConfig.angle) {
//            angleToGoTo = bodyConfig.maxAngle + previousAngle;
//        } else if (HALSIM_GetEncoderCount(0) + bodyConfig.angle < bodyConfig.minAngle) {
//            angleToGoTo = bodyConfig.minAngle + previousAngle;
//        }

        // Draw the first line and get the ending coordinates
        auto[XEnd, YEnd, angle] = DrawLine(startXLocation, startYLocation,
                                           (minSize / 100) * bodyConfig.length, angleToGoTo, drawList,
                                           windowPos, colorLookUpTable[bodyConfig.color], bodyConfig, "");

        // If the line has children then draw them with the stating points being the end of the parent
        if (bodyConfig.children.size() != 0) {
            buildDrawList(XEnd, YEnd, drawList, angle,
                          bodyConfig.children, windowPos);
        }
    }
}

static void DisplayAssembly2D() {
    ImVec2 windowPos = ImGui::GetWindowPos();
    ImDrawList *drawList = ImGui::GetWindowDrawList();
    buildDrawList(ImGui::GetWindowWidth() / 2, ImGui::GetWindowHeight(), drawList, 0,
                  bodyConfigList, windowPos);
}

BodyConfig readSubJson(const std::string &name, wpi::json const &body) {
    BodyConfig c;
    try {
        c.name = name + body.at("name").get<std::string>() + "/";
    } catch (const wpi::json::exception &e) {
        wpi::errs() << "could not read body name: " << e.what() << '\n';
    }
    try {
        c.type = body.at("type").get<std::string>();
    } catch (const wpi::json::exception &e) {
        wpi::errs() << "camera '" << c.name << "': could not type path: " << e.what() << '\n';
    }
    try {
        c.startLocation = body.at("startLocation").get<int>();
    } catch (const wpi::json::exception &e) {
        wpi::errs() << "startLocation '" << c.name << "': could not find startLocation path: " << e.what()
                    << '\n';
    }
    try {
        c.length = body.at("length").get<int>();
    } catch (const wpi::json::exception &e) {
        wpi::errs() << "length '" << c.name << "': could not find length path: " << e.what() << '\n';
    }
    try {
        c.color = body.at("color").get<std::string>();
    } catch (const wpi::json::exception &e) {
        wpi::errs() << "color '" << c.name << "': could not find color path: " << e.what() << '\n';
    }
    try {
        c.angle = body.at("angle").get<int>();
    } catch (const wpi::json::exception &e) {
        wpi::errs() << "angle '" << c.name << "': could not find angle path: " << e.what() << '\n';
    }
    try {
        c.maxAngle = body.at("maxAngle").get<int>();
    } catch (const wpi::json::exception &e) {
        wpi::errs() << "maxAngle '" << c.name << "': could not find maxAngle path: " << e.what() << '\n';
    }
    try {
        c.minAngle = body.at("minAngle").get<int>();
    } catch (const wpi::json::exception &e) {
        wpi::errs() << "minAngle '" << c.name << "': could not find minAngle path: " << e.what() << '\n';
    }
    try {
        c.lineWidth = body.at("lineWidth").get<int>();
    } catch (const wpi::json::exception &e) {
        wpi::errs() << "lineWidth '" << c.name << "': could not find lineWidth path: " << e.what() << '\n';
    }
    try {
        for (wpi::json const &child : body.at("children")) {
            c.children.push_back(readSubJson(c.name, child));
            wpi::outs() << "Reading Child with name " << c.name << '\n';
        }
    } catch (const wpi::json::exception &e) {
        wpi::errs() << "could not read body: " << e.what() << '\n';
    }
    return c;
}

static std::list<BodyConfig> readJson(std::string jFile) {
    std::list<BodyConfig> cList;
    std::error_code ec;
    wpi::raw_fd_istream is(jFile, ec);
    if (ec) {
        wpi::errs() << "could not open '" << jFile << "': " << ec.message() << '\n';
    }

    // parse file
    wpi::json j;
    try {
        j = wpi::json::parse(is);
    } catch (const wpi::json::parse_error &e) {
        wpi::errs() << "byte " << e.byte << ": " << e.what() << '\n';
    }

    // top level must be an object
    if (!j.is_object()) {
        wpi::errs() << "must be JSON object\n";
    }
    try {
        for (wpi::json const &body : j.at("body")) {
            cList.push_back(readSubJson("/", body));
        }

    } catch (const wpi::json::exception &e) {
        wpi::errs() << "could not read body: " << e.what() << '\n';
    }
    try {
        windowHeight = j.at("windowHeight").get<int>();
    } catch (const wpi::json::exception &e) {
        wpi::errs() << "Could not find windowHeight path: " << e.what() << '\n';
    }
    try {
        windowWidth = j.at("windowWidth").get<int>();
    } catch (const wpi::json::exception &e) {
        wpi::errs() << "Could not find windowWidth path: " << e.what() << '\n';
    }
    return cList;
}

void Assembly2D::Initialize() {
    buildColorTable();
    bodyConfigList = readJson("/home/gabe/github/allwpilib/Assembly2D.json");
    HALSimGui::AddWindow("2D Assembly", DisplayAssembly2D);
    HALSimGui::SetDefaultWindowPos("2D Assembly", 200, 200);
    HALSimGui::SetDefaultWindowSize("2D Assembly", 600, 600);
    HALSimGui::SetWindowPadding("2D Assembly", 0, 0);

    for (BodyConfig const &bodyConfig : bodyConfigList) {
        wpi::outs() << bodyConfig.name << " " << bodyConfig.type << " " << bodyConfig.startLocation << " "
                    << bodyConfig.length << "\n";
    }
}