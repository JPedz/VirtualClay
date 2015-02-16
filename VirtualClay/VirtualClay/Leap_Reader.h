#pragma once
#include "Leap.h"
#include <QTime>
#include "Leap_Listener.h"
using namespace Leap;
namespace mb = mudbox;
class Leap_Reader {
  Leap_Listener listener;
  Leap::Controller controller;
  void getAndSetHandDirection(Hand &h,LR lr);
  void HandSetup(Frame &f);
  void Leap_Reader::updateDirection(Frame &f);
  float handConfidenceLevel;
  Hand hand_l;
  Hand hand_r;
  std::vector<bool> handvisi;
  mb::Vector Leap_Reader::LeapDirectionToMudbox(Leap::Vector dir);
  mb::Vector Leap_Reader::LeapPositionToMudbox(Leap::Vector dir);
public:
  bool isConnected;
  bool ishands;
  Leap_Reader(void);
  ~Leap_Reader(void);
  Leap::Vector dir;
  void updateAll(void);
  mb::Vector getDirection_L(void);
  mb::Vector getDirection_R(void);
  mb::Vector getPosition_L(void);
  mb::Vector getPosition_R(void);
  mb::Vector getFingerDirection_L(fingerEnum fn);
  mb::Vector getFingerDirection_R(fingerEnum fn);
  mb::Vector getFingerPosition_L(fingerEnum fn);
  mb::Vector getFingerPosition_R(fingerEnum fn);
  bool Leap_Reader::isFist(LR lr);
  bool Leap_Reader::isVisible(LR lr);
  mb::Vector Leap_Reader::rotateScene();
};

