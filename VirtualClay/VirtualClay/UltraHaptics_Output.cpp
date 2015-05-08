#include "stdafx.h"
#include "UltraHaptics_Output.h"
#include "Ultrahaptics.hpp"
//
//
//UltraHaptics_Output::UltraHaptics_Output(void)
//{
//  //emitter = new Ultrahaptics::Emitter();
//  frequency = 200.f * Ultrahaptics::Units::hertz;
//  intensity = 1.0f;
//}
//
//Ultrahaptics::Vector3 UltraHaptics_Output::MbVectorToUltraHapticsVector(mb::Vector v1) {
//  Ultrahaptics::Vector3 output;
//  Ultrahaptics::Vector3 tmp;
//  Ultrahaptics::Alignment align;
//  tmp.x = v1.x;
//  tmp.y = v1.y;
//  tmp.z = v1.z;
//  output = align.fromTrackingPositionToDevicePosition(tmp);
//  return output;
//}
//
//
//void UltraHaptics_Output::sendFocalPoints(std::vector<mb::Vector> focalPoints) {
//  size_t focalCount = focalPoints.size();
//  if(focalCount > 0) {
//  //std::vector<Ultrahaptics::ControlPoint> controlPoints;
//  ////controlPoints.resize(focalCount);
//  //for(int i = 0 ; i < focalCount ; i++) {
//  //  const Ultrahaptics::Vector3 pos = MbVectorToUltraHapticsVector(focalPoints.at(i));
//  //  const Ultrahaptics::ControlPoint point1(pos, intensity, frequency);
//  //  controlPoints.push_back(point1);
//  //}
//  //  
//  //for(int i = 0 ; i < controlPoints.size(); i++) {
//  //  emitter.update(&controlPoints.at(i),focalCount);
//  //}
//  } else {
//    //emitter->stop();
//  }
//}
//
//UltraHaptics_Output::~UltraHaptics_Output(void)
//{
//}
