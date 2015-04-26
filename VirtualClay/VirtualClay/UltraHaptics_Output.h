#pragma once
#include "stdafx.h"
#include "ultrahaptics\Ultrahaptics.hpp"
class UltraHaptics_Output
{
  Ultrahaptics::Emitter *emitter;
  float frequency;
  float intensity;
  Ultrahaptics::Vector3 MbVectorToUltraHapticsVector(mb::Vector v1);
public:
  UltraHaptics_Output(void);
  void sendFocalPoints(std::vector<mb::Vector> focalPoints);
  ~UltraHaptics_Output(void);
};

