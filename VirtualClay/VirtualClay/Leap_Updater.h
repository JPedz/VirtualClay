#pragma once
#include "stdafx.h"
#include "Leap_Reader.h"
#include "Leap_Hand.h"
#include "cameraWrapper.h"
#include "MeshOps.h"
#include "Leap_HUD.h"

namespace mb = mudbox;
class Leap_Updater : public mb::Node {
  mudbox::Vector dir;
  ID_List *idList;
  Leap_Hand *hand_l;
  Leap_Hand *hand_r;
  cameraWrapper *viewCam;
  mb::Vector fitToCameraSpace(cameraWrapper *viewCam);
  mb::Vector rotateCamera(cameraWrapper *viewCam);
  mb::aevent frameEvent;
  Leap_Reader *leapReader;
  MeshOps *meshOp;
  mb::Vector lastFrameHandPos;
  void setDir(mudbox::Vector dir);
  void SetHandAndFingerPositions(mb::Vector &camRot, mb::Vector &camPivot);
  void getFingerScreenSpace(mb::Vector &camPos,cameraWrapper *viewCam);
  void MoveMesh();
  bool selectMesh(mb::Vector &camPos);
  bool selectMeshPinch(mb::Vector &camPos);
  int countIntersectingFingers(LR lOrR);
  bool facesAreSelected;
  bool firstmoveswitch;
public:
  Leap_Updater(ID_List *idl,Leap_Hand *l,Leap_Hand *r);
  void OnEvent(const mb::EventGate &cEvent);
  mudbox::Vector getDir();
};

