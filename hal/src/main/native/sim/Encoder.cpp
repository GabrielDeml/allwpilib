/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "hal/Encoder.h"

#include "CounterInternal.h"
#include "HALInitializer.h"
#include "PortsInternal.h"
#include "hal/Counter.h"
#include "hal/Errors.h"
#include "hal/handles/HandlesInternal.h"
#include "hal/handles/LimitedHandleResource.h"
#include "mockdata/EncoderDataInternal.h"

using namespace hal;

namespace {
struct Encoder {
  HAL_Handle nativeHandle;
  uint8_t index;
};
struct Empty {};
}  // namespace

static LimitedHandleResource<HAL_EncoderHandle, Encoder,
                             kNumEncoders + kNumCounters,
                             HAL_HandleEnum::Encoder>* encoderHandles;

static LimitedHandleResource<HAL_FPGAEncoderHandle, Empty, kNumEncoders,
                             HAL_HandleEnum::FPGAEncoder>* fpgaEncoderHandles;

namespace hal {
namespace init {
void InitializeEncoder() {
  static LimitedHandleResource<HAL_FPGAEncoderHandle, Empty, kNumEncoders,
                               HAL_HandleEnum::FPGAEncoder>
      feH;
  fpgaEncoderHandles = &feH;
  static LimitedHandleResource<HAL_EncoderHandle, Encoder,
                               kNumEncoders + kNumCounters,
                               HAL_HandleEnum::Encoder>
      eH;
  encoderHandles = &eH;
}
}  // namespace init
}  // namespace hal

extern "C" {
HAL_EncoderHandle HAL_InitializeEncoder(
    HAL_Handle digitalSourceHandleA, HAL_AnalogTriggerType analogTriggerTypeA,
    HAL_Handle digitalSourceHandleB, HAL_AnalogTriggerType analogTriggerTypeB,
    HAL_Bool reverseDirection, HAL_EncoderEncodingType encodingType,
    int32_t* status) {
  hal::init::CheckInit();
  HAL_Handle nativeHandle = HAL_kInvalidHandle;
  if (encodingType == HAL_EncoderEncodingType::HAL_Encoder_k4X) {
    // k4x, allocate encoder
    nativeHandle = fpgaEncoderHandles->Allocate();
  } else {
    // k2x or k1x, allocate counter
    nativeHandle = counterHandles->Allocate();
  }
  if (nativeHandle == HAL_kInvalidHandle) {
    *status = NO_AVAILABLE_RESOURCES;
    return HAL_kInvalidHandle;
  }
  auto handle = encoderHandles->Allocate();
  if (handle == HAL_kInvalidHandle) {
    *status = NO_AVAILABLE_RESOURCES;
    return HAL_kInvalidHandle;
  }
  auto encoder = encoderHandles->Get(handle);
  if (encoder == nullptr) {  // would only occur on thread issue
    *status = HAL_HANDLE_ERROR;
    return HAL_kInvalidHandle;
  }
  int16_t index = getHandleIndex(handle);
  SimEncoderData[index].digitalChannelA = getHandleIndex(digitalSourceHandleA);
  SimEncoderData[index].digitalChannelB = getHandleIndex(digitalSourceHandleB);
  SimEncoderData[index].initialized = true;
  SimEncoderData[index].reverseDirection = reverseDirection;
  SimEncoderData[index].simDevice = 0;
  SimEncoderData[index].encodingType = encodingType;
  encoder->index = index;
  encoder->nativeHandle = nativeHandle;
  return handle;
}

void HAL_FreeEncoder(HAL_EncoderHandle encoderHandle, int32_t* status) {
  auto encoder = encoderHandles->Get(encoderHandle);
  encoderHandles->Free(encoderHandle);
  if (encoder == nullptr) return;
  if (isHandleType(encoder->nativeHandle, HAL_HandleEnum::FPGAEncoder)) {
    fpgaEncoderHandles->Free(encoder->nativeHandle);
  } else if (isHandleType(encoder->nativeHandle, HAL_HandleEnum::Counter)) {
    counterHandles->Free(encoder->nativeHandle);
  }
  SimEncoderData[encoder->index].initialized = false;
}

void HAL_SetEncoderSimDevice(HAL_EncoderHandle handle,
                             HAL_SimDeviceHandle device) {
  int16_t index = encoderHandles->GetIndex(handle);
  if (index < 0 || index >= kNumEncoders || !SimEncoderData[index].initialized)
    return;
  SimEncoderData[index].simDevice = device;
}

int32_t HAL_GetEncoder(HAL_EncoderHandle encoderHandle, int32_t* status) {
  int16_t index = encoderHandles->GetIndex(encoderHandle);
  if (index < 0 || index >= kNumEncoders ||
      !SimEncoderData[index].initialized) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  return SimEncoderData[index].count;
}
int32_t HAL_GetEncoderRaw(HAL_EncoderHandle encoderHandle, int32_t* status) {
  int16_t index = encoderHandles->GetIndex(encoderHandle);
  if (index < 0 || index >= kNumEncoders ||
      !SimEncoderData[index].initialized) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  return SimEncoderData[index].count /
         SimEncoderData[index].GetDecodingScaleFactor();
}
int32_t HAL_GetEncoderEncodingScale(HAL_EncoderHandle encoderHandle,
                                    int32_t* status) {
  int16_t index = encoderHandles->GetIndex(encoderHandle);
  if (index < 0 || index >= kNumEncoders ||
      !SimEncoderData[index].initialized) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  return SimEncoderData[index].GetEncodingScaleFactor();
}
void HAL_ResetEncoder(HAL_EncoderHandle encoderHandle, int32_t* status) {
  int16_t index = encoderHandles->GetIndex(encoderHandle);
  if (index < 0 || index >= kNumEncoders ||
      !SimEncoderData[index].initialized) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  SimEncoderData[index].count = 0;
  SimEncoderData[index].period = std::numeric_limits<double>::max();
  SimEncoderData[index].reset = true;
}
double HAL_GetEncoderPeriod(HAL_EncoderHandle encoderHandle, int32_t* status) {
  int16_t index = encoderHandles->GetIndex(encoderHandle);
  if (index < 0 || index >= kNumEncoders ||
      !SimEncoderData[index].initialized) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  return SimEncoderData[index].period;
}
void HAL_SetEncoderMaxPeriod(HAL_EncoderHandle encoderHandle, double maxPeriod,
                             int32_t* status) {
  int16_t index = encoderHandles->GetIndex(encoderHandle);
  if (index < 0 || index >= kNumEncoders ||
      !SimEncoderData[index].initialized) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  SimEncoderData[index].maxPeriod = maxPeriod;
}
HAL_Bool HAL_GetEncoderStopped(HAL_EncoderHandle encoderHandle,
                               int32_t* status) {
  int16_t index = encoderHandles->GetIndex(encoderHandle);
  if (index < 0 || index >= kNumEncoders ||
      !SimEncoderData[index].initialized) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  return SimEncoderData[index].period > SimEncoderData[index].maxPeriod;
}
HAL_Bool HAL_GetEncoderDirection(HAL_EncoderHandle encoderHandle,
                                 int32_t* status) {
  int16_t index = encoderHandles->GetIndex(encoderHandle);
  if (index < 0 || index >= kNumEncoders ||
      !SimEncoderData[index].initialized) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  return SimEncoderData[index].direction;
}
double HAL_GetEncoderDistance(HAL_EncoderHandle encoderHandle,
                              int32_t* status) {
  int16_t index = encoderHandles->GetIndex(encoderHandle);
  if (index < 0 || index >= kNumEncoders ||
      !SimEncoderData[index].initialized) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  return SimEncoderData[index].count * SimEncoderData[index].distancePerPulse;
}
double HAL_GetEncoderRate(HAL_EncoderHandle encoderHandle, int32_t* status) {
  int16_t index = encoderHandles->GetIndex(encoderHandle);
  if (index < 0 || index >= kNumEncoders ||
      !SimEncoderData[index].initialized) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  return SimEncoderData[index].distancePerPulse / SimEncoderData[index].period;
}
void HAL_SetEncoderMinRate(HAL_EncoderHandle encoderHandle, double minRate,
                           int32_t* status) {
  int16_t index = encoderHandles->GetIndex(encoderHandle);
  if (index < 0 || index >= kNumEncoders ||
      !SimEncoderData[index].initialized) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  if (minRate == 0.0) {
    *status = PARAMETER_OUT_OF_RANGE;
    return;
  }

  SimEncoderData[index].maxPeriod =
      SimEncoderData[index].distancePerPulse / minRate;
}
void HAL_SetEncoderDistancePerPulse(HAL_EncoderHandle encoderHandle,
                                    double distancePerPulse, int32_t* status) {
  int16_t index = encoderHandles->GetIndex(encoderHandle);
  if (index < 0 || index >= kNumEncoders ||
      !SimEncoderData[index].initialized) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  if (distancePerPulse == 0.0) {
    *status = PARAMETER_OUT_OF_RANGE;
    return;
  }
  SimEncoderData[index].distancePerPulse = distancePerPulse;
}
void HAL_SetEncoderReverseDirection(HAL_EncoderHandle encoderHandle,
                                    HAL_Bool reverseDirection,
                                    int32_t* status) {
  int16_t index = encoderHandles->GetIndex(encoderHandle);
  if (index < 0 || index >= kNumEncoders ||
      !SimEncoderData[index].initialized) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  SimEncoderData[index].reverseDirection = reverseDirection;
}
void HAL_SetEncoderSamplesToAverage(HAL_EncoderHandle encoderHandle,
                                    int32_t samplesToAverage, int32_t* status) {
  int16_t index = encoderHandles->GetIndex(encoderHandle);
  if (index < 0 || index >= kNumEncoders ||
      !SimEncoderData[index].initialized) {
    *status = HAL_HANDLE_ERROR;
    return;
  }

  SimEncoderData[index].samplesToAverage = samplesToAverage;
}
int32_t HAL_GetEncoderSamplesToAverage(HAL_EncoderHandle encoderHandle,
                                       int32_t* status) {
  int16_t index = encoderHandles->GetIndex(encoderHandle);
  if (index < 0 || index >= kNumEncoders ||
      !SimEncoderData[index].initialized) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  return SimEncoderData[index].samplesToAverage;
}

void HAL_SetEncoderIndexSource(HAL_EncoderHandle encoderHandle,
                               HAL_Handle digitalSourceHandle,
                               HAL_AnalogTriggerType analogTriggerType,
                               HAL_EncoderIndexingType type, int32_t* status) {
  // Not implemented yet
}

int32_t HAL_GetEncoderFPGAIndex(HAL_EncoderHandle encoderHandle,
                                int32_t* status) {
  int16_t index = encoderHandles->GetIndex(encoderHandle);
  if (index < 0 || index >= kNumEncoders ||
      !SimEncoderData[index].initialized) {
    *status = HAL_HANDLE_ERROR;
    return 0;
  }

  return index;
}

double HAL_GetEncoderDecodingScaleFactor(HAL_EncoderHandle encoderHandle,
                                         int32_t* status) {
  int16_t index = encoderHandles->GetIndex(encoderHandle);
  if (index < 0 || index >= kNumEncoders ||
      !SimEncoderData[index].initialized) {
    *status = HAL_HANDLE_ERROR;
    return 0.0;
  }

  return SimEncoderData[index].GetDecodingScaleFactor();
}

double HAL_GetEncoderDistancePerPulse(HAL_EncoderHandle encoderHandle,
                                      int32_t* status) {
  int16_t index = encoderHandles->GetIndex(encoderHandle);
  if (index < 0 || index >= kNumEncoders ||
      !SimEncoderData[index].initialized) {
    *status = HAL_HANDLE_ERROR;
    return 0.0;
  }

  return SimEncoderData[index].distancePerPulse;
}

HAL_EncoderEncodingType HAL_GetEncoderEncodingType(
    HAL_EncoderHandle encoderHandle, int32_t* status) {
  int16_t index = encoderHandles->GetIndex(encoderHandle);
  if (index < 0 || index >= kNumEncoders ||
      !SimEncoderData[index].initialized) {
    *status = HAL_HANDLE_ERROR;
    return HAL_Encoder_k4X;  // default to k4x
  }

  return SimEncoderData[index].encodingType;
}
}  // extern "C"
