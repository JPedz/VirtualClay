#pragma once
#include "stdafx.h"
#include "Leap_Reader.h"
#include "Leap_Hand.h"
#include "cameraWrapper.h"

namespace mb = mudbox;
class Leap_Updater : public mb::Node {
  mudbox::Vector dir;
  ID_List *idList;
  void setDir(mudbox::Vector dir);
  Leap_Hand *hand_l;
  Leap_Hand *hand_r;
  mb::Vector fitToCameraSpace();
  mb::aevent frameEvent;
  Leap_Reader *leapReader;
public:
  Leap_Updater(ID_List *idl,Leap_Hand *l,Leap_Hand *r);
  void OnEvent(const mb::EventGate &cEvent);
  mudbox::Vector getDir();
};

