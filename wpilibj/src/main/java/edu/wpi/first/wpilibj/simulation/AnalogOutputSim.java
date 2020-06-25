/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.hal.sim.NotifyCallback;
import edu.wpi.first.hal.sim.mockdata.AnalogOutDataJNI;
import edu.wpi.first.wpilibj.AnalogOutput;
import java.util.NoSuchElementException;

public class AnalogOutputSim {
  private final int m_index;

  public AnalogOutputSim(AnalogOutput analogOutput) {
    m_index = analogOutput.getChannel();
  }

  private AnalogOutputSim(int index) {
    m_index = index;
  }

  public static AnalogOutputSim createForChannel(int channel) {
    int index = AnalogOutDataJNI.findAnalogOutForChannel(channel);
    if (index < 0) {
      throw new NoSuchElementException("no analog output found for channel " + channel);
    }
    return new AnalogOutputSim(index);
  }

  public static AnalogOutputSim createForIndex(int index) {
    return new AnalogOutputSim(index);
  }

  public CallbackStore registerVoltageCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = AnalogOutDataJNI.registerVoltageCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, AnalogOutDataJNI::cancelVoltageCallback);
  }
  public double getVoltage() {
    return AnalogOutDataJNI.getVoltage(m_index);
  }
  public void setVoltage(double voltage) {
    AnalogOutDataJNI.setVoltage(m_index, voltage);
  }

  public CallbackStore registerInitializedCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = AnalogOutDataJNI.registerInitializedCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, AnalogOutDataJNI::cancelInitializedCallback);
  }
  public boolean getInitialized() {
    return AnalogOutDataJNI.getInitialized(m_index);
  }
  public void setInitialized(boolean initialized) {
    AnalogOutDataJNI.setInitialized(m_index, initialized);
  }

  public void resetData() {
    AnalogOutDataJNI.resetData(m_index);
  }
}
