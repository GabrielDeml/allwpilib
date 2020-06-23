/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>
#include <utility>

#include <mockdata/RelayData.h>

#include "CallbackStore.h"
#include "frc/Relay.h"

namespace frc {
namespace sim {
class RelaySim {
 public:
  explicit RelaySim(const Relay& relay)
      : m_index(relay.GetChannel()) {}

  static RelaySim CreateForChannel(int channel) {
    return RelaySim{HALSIM_FindRelayForChannel(channel)};
  }

  static RelaySim CreateForIndex(int index) {
    return RelaySim{index};
  }

  std::unique_ptr<CallbackStore> RegisterInitializedForwardCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback, &HALSIM_CancelRelayInitializedForwardCallback);
    store->SetUid(HALSIM_RegisterRelayInitializedForwardCallback(
        m_index, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  bool GetInitializedForward() const {
    return HALSIM_GetRelayInitializedForward(m_index);
  }

  void SetInitializedForward(bool initializedForward) {
    HALSIM_SetRelayInitializedForward(m_index, initializedForward);
  }

  std::unique_ptr<CallbackStore> RegisterInitializedReverseCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback, &HALSIM_CancelRelayInitializedReverseCallback);
    store->SetUid(HALSIM_RegisterRelayInitializedReverseCallback(
        m_index, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  bool GetInitializedReverse() const {
    return HALSIM_GetRelayInitializedReverse(m_index);
  }

  void SetInitializedReverse(bool initializedReverse) {
    HALSIM_SetRelayInitializedReverse(m_index, initializedReverse);
  }

  std::unique_ptr<CallbackStore> RegisterForwardCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback, &HALSIM_CancelRelayForwardCallback);
    store->SetUid(HALSIM_RegisterRelayForwardCallback(
        m_index, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  bool GetForward() const { return HALSIM_GetRelayForward(m_index); }

  void SetForward(bool forward) { HALSIM_SetRelayForward(m_index, forward); }

  std::unique_ptr<CallbackStore> RegisterReverseCallback(
      NotifyCallback callback, bool initialNotify) {
    auto store = std::make_unique<CallbackStore>(
        m_index, -1, callback, &HALSIM_CancelRelayReverseCallback);
    store->SetUid(HALSIM_RegisterRelayReverseCallback(
        m_index, &CallbackStoreThunk, store.get(), initialNotify));
    return store;
  }

  bool GetReverse() const { return HALSIM_GetRelayReverse(m_index); }

  void SetReverse(bool reverse) { HALSIM_SetRelayReverse(m_index, reverse); }

  void ResetData() { HALSIM_ResetRelayData(m_index); }

 private:
  explicit RelaySim(int index) : m_index(index) {}
  int m_index;
};
}  // namespace sim
}  // namespace frc