/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.hal.sim.ConstBufferCallback;
import edu.wpi.first.hal.sim.NotifyCallback;
import edu.wpi.first.hal.sim.mockdata.AddressableLEDDataJNI;
import edu.wpi.first.wpilibj.AddressableLED;

public class AddressableLEDSim {
  private final int m_index;

  public AddressableLEDSim() {
    m_index = 0;
  }

  public CallbackStore registerInitializedCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = AddressableLEDDataJNI.registerInitializedCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, AddressableLEDDataJNI::cancelInitializedCallback);
  }
  public boolean getInitialized() {
    return AddressableLEDDataJNI.getInitialized(m_index);
  }
  public void setInitialized(boolean initialized) {
    AddressableLEDDataJNI.setInitialized(m_index, initialized);
  }

  public CallbackStore registerOutputPortCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = AddressableLEDDataJNI.registerOutputPortCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, AddressableLEDDataJNI::cancelOutputPortCallback);
  }
  public int getOutputPort() {
    return AddressableLEDDataJNI.getOutputPort(m_index);
  }
  public void setOutputPort(int outputPort) {
    AddressableLEDDataJNI.setOutputPort(m_index, outputPort);
  }

  public CallbackStore registerLengthCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = AddressableLEDDataJNI.registerLengthCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, AddressableLEDDataJNI::cancelLengthCallback);
  }
  public int getLength() {
    return AddressableLEDDataJNI.getLength(m_index);
  }
  public void setLength(int length) {
    AddressableLEDDataJNI.setLength(m_index, length);
  }

  public CallbackStore registerRunningCallback(NotifyCallback callback, boolean initialNotify) {
    int uid = AddressableLEDDataJNI.registerRunningCallback(m_index, callback, initialNotify);
    return new CallbackStore(m_index, uid, AddressableLEDDataJNI::cancelRunningCallback);
  }
  public boolean getRunning() {
    return AddressableLEDDataJNI.getRunning(m_index);
  }
  public void setRunning(boolean running) {
    AddressableLEDDataJNI.setRunning(m_index, running);
  }

  public CallbackStore registerDataCallback(ConstBufferCallback callback) {
    int uid = AddressableLEDDataJNI.registerDataCallback(m_index, callback);
    return new CallbackStore(m_index, uid, AddressableLEDDataJNI::cancelDataCallback);
  }
  public byte[] getData() {
    return AddressableLEDDataJNI.getData(m_index);
  }
  public void setData(byte[] data) {
    AddressableLEDDataJNI.setData(m_index, data);
  }

  public void resetData() {
    AddressableLEDDataJNI.resetData(m_index);
  }
}
