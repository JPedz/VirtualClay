#pragma once
#include "stdafx.h"
#include "Leap_Reader.h"
#include "Leap_Hand.h"
#include "cameraWrapper.h"
#include "MeshOps.h"

namespace mb = mudbox;
class Leap_Updater : public mb::Node {
  mudbox::Vector dir;
  ID_List *idList;
  void setDir(mudbox::Vector dir);
  Leap_Hand *hand_l;
  Leap_Hand *hand_r;
  cameraWrapper *viewCam;
  mb::Vector fitToCameraSpace(cameraWrapper *viewCam);
  mb::Vector rotateCamera(cameraWrapper *viewCam);
  mb::aevent frameEvent;
  Leap_Reader *leapReader;
  MeshOps *meshOp;
  void getFingerScreenSpace(mb::Vector &camPos,cameraWrapper *viewCam);
  
  void Leap_Updater::selectMesh(mb::Vector &camPos);
public:
  Leap_Updater(ID_List *idl,Leap_Hand *l,Leap_Hand *r);
  void OnEvent(const mb::EventGate &cEvent);
  mudbox::Vector getDir();
};

