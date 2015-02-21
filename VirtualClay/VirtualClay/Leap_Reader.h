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
  int lastFrameID;
  std::vector<bool> handvisi;
  mb::Vector Leap_Reader::LeapDirectionToMudbox(Leap::Vector dir);
  mb::Vector Leap_Reader::LeapPositionToMudbox(Leap::Vector dir);
  mb::Vector scale;
public:
  bool isConnected;
  bool ishands;
  bool isScreenTap;
  bool isGrabbing_L;
  bool grabswitch;
  bool isGrabbing_R;
  bool isUndo;
  bool isCircleCW;
  Leap_Reader(void);
  ~Leap_Reader(void);
  Leap::Vector dir;
  bool updateAll(void); //returns true if updated false if frame is the same.
  void SetScale(mb::Vector s);
  mb::Vector getDirection_L(void);
  mb::Vector getDirection_R(void);
  mb::Vector getPosition_L(void);
  mb::Vector getPosition_R(void);
  mb::Vector getFingerDirection_L(fingerEnum fn);
  mb::Vector getFingerDirection_R(fingerEnum fn);
  std::vector<mb::Vector> getFingerPosition(fingerEnum fn,LR LOrR);
  mb::Vector getFingerPosition_R(fingerEnum fn);
  bool Leap_Reader::isFist(LR lr);
  bool Leap_Reader::isVisible(LR lr);
  mb::Vector Leap_Reader::rotateScene();
  mb::Vector Leap_Reader::TestFunct();
  std::vector<bool> GetExtendedFingers(LR lOrR);
};

