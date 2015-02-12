#pragma once
#include "stdafx.h"
#include "Leap_Reader.h"
#include "Leap_Hand.h"

namespace mb = mudbox;
class Leap_Updater : public mb::Node {
  mudbox::Vector dir;
  QList<int> ID_List;
  void setDir(mudbox::Vector dir);
  Leap_Hand *hand_l;
  Leap_Hand *hand_r;
public:
  mb::aevent frameEvent;
  Leap_Reader *leapReader;
  int frameCounter;
  Leap_Updater(Leap_Hand *l,Leap_Hand *r);
  void OnEvent(const mb::EventGate &cEvent);
  mudbox::Vector getDir();
};

