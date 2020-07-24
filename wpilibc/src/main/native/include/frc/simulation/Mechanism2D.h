/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <hal/SimDevice.h>
#include <units/units.h>

#include "frc/geometry/Pose2d.h"
#include "frc/geometry/Rotation2d.h"
#include <string>
#include <map>
#include <wpi/StringMap.h>

namespace frc {
    class Mechanism2D {
    public:
        Mechanism2D();

        /**
         * Set/Create the angle of a ligament
         *
         * @param ligamentPath json path to the ligament
         * @param angle to set the ligament
         */
        void SetLigamentAngle(std::string ligamentPath, float angle);

        /**
         * Set/Create the length of a ligament
         *
         * @param ligamentPath json path to the ligament
         * @param length to set the ligament
         */
        void SetLigamentALength(std::string ligamentPath, float length);

    private:
        wpi::StringMap<hal::SimDouble> createdItems;
        hal::SimDevice m_device;
    };

}  // namespace frc
