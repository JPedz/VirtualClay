#pragma once
#include "Leap.h"
#include <QTCore/QTime>
#include <iostream>
#include <fstream>
#include "Leap_Listener.h"
using namespace Leap;
namespace mb = mudbox;
class Leap_Reader {
  Leap_Listener listener;
  Leap::Controller controller;
  void getAndSetHandDirection(Hand &h,LR lr);
  void HandSetup(Frame &f);
  float handConfidenceLevel;
  bool DEBUGMODEON;
  bool RECORD;
  
  std::ofstream outFile;
  std::ifstream inFile;

  Hand hand_l;
  Hand hand_r;
  Tool tool;
  int lastFrameID;
  std::vector<bool> handvisi;
  mb::Vector LeapDirectionToMudbox(Leap::Vector dir);
  mb::Vector LeapPositionToMudbox(Leap::Vector dir);
  mb::Vector scale;
  QTime *UndoTimeOut;
  //Frame readInFile();
  //void writeOutFile(Frame &f);

public:
  bool isConnected;
  bool ishands;
  bool isScreenTap_L;
  bool isScreenTap_R;
  bool isGrabbing_L;
  bool grabswitch;
  bool isGrabbing_R;
  bool isUndo;
  bool isCircleCW_R;
  bool isCircleCCW_R;
  bool isCircleCW_L;
  bool isCircleCCW_L;
  bool isTool;
  LR gestureHand;
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
  std::vector<mb::Vector> getBoneOrients(fingerEnum fn,LR lOrR);
  bool isFist(LR lr);
  bool isVisible(LR lr);
  mb::Vector rotateScene();
  mb::Vector TestFunct();
  std::vector<bool> GetExtendedFingers(LR lOrR);
  mb::Vector getMotionDirection(fingerEnum fn, LR lOrR);
  bool CheckRotateHandGesture(LR lOrR);
  bool CheckScaleHandGesture(LR lOrR);
  bool CheckFingerExtensions(LR lOrR,bool thumb,bool index,bool middle,bool ring,bool pinky);

  //Tools
  std::vector<mb::Vector> GetToolPositions();
  mb::Vector getToolMotionDirection();
  mb::Vector GetToolDirection();
};

