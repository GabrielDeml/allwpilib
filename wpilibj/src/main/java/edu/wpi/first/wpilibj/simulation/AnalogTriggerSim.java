/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.hal.sim.NotifyCallback;
import edu.wpi.first.hal.sim.mockdata.AnalogTriggerDataJNI;
import edu.wpi.first.wpilibj.AnalogTrigger;
import java.util.NoSuchElementException;

public class AnalogTriggerSim {
  private final int m_index;

  public AnalogTriggerSim(AnalogTrigger analogTrigger) {
    m_index = analogTrigger.getIndex();
  }

  private AnalogTriggerSim(int index) {
    m_index = index;
  }

  public static AnalogTriggerSim createForChannel(int channel) {
    int index = AnalogTriggerDataJNI.findAnalogTriggerDataForChannel(channel);
    if (index < 0) {
      throw new NoSuchElementException("no encoder found for channel " + channel);
    }
    return new AnalogTriggerSim(index);
  }

  public static AnalogTriggerSim createForIndex(int index) {
    return new AnalogTriggerSim(index);
  }

  public CallbackStore registerInitializedCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = AnalogTriggerDataJNI.registerInitializedCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, AnalogTriggerDataJNI::cancelInitializedCallback);
  }
  public boolean getInitialized() {
    return AnalogTriggerDataJNI.getInitialized(m_index);
  }
  public void setInitialized(boolean initialized) {
    AnalogTriggerDataJNI.setInitialized(m_index, initialized);
  }

  public CallbackStore registerTriggerLowerBoundCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = AnalogTriggerDataJNI.registerTriggerLowerBoundCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, AnalogTriggerDataJNI::cancelTriggerLowerBoundCallback);
  }
  public double getTriggerLowerBound() {
    return AnalogTriggerDataJNI.getTriggerLowerBound(m_index);
  }
  public void setTriggerLowerBound(double triggerLowerBound) {
    AnalogTriggerDataJNI.setTriggerLowerBound(m_index, triggerLowerBound);
  }

  public CallbackStore registerTriggerUpperBoundCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = AnalogTriggerDataJNI.registerTriggerUpperBoundCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, AnalogTriggerDataJNI::cancelTriggerUpperBoundCallback);
  }
  public double getTriggerUpperBound() {
    return AnalogTriggerDataJNI.getTriggerUpperBound(m_index);
  }
  public void setTriggerUpperBound(double triggerUpperBound) {
    AnalogTriggerDataJNI.setTriggerUpperBound(m_index, triggerUpperBound);
  }

  public void resetData() {
    AnalogTriggerDataJNI.resetData(m_index);
  }
}
