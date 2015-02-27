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
  mb::Vector fitToCameraSpace();
  mb::Vector rotateCamera();
  void Leap_Updater::CameraMovement();
  mb::aevent frameEvent;
  Leap_Reader *leapReader;
  MeshOps *meshOp;
  mb::Vector lastFrameHandPos;
  mb::Vector lastFrameThumbPos;
  void setDir(mudbox::Vector dir);
  void SetHandAndFingerPositions(mb::Vector &camRot, mb::Vector &camPivot);
  void getFingerScreenSpace(mb::Vector &camPos);
  mb::Vector GetRelativeScreenSpaceFromWorldPos(mb::Vector &wPos);
  void MoveMesh();
  bool selectMesh(mb::Vector &camPos);
  bool selectMeshPinch(mb::Vector &camPos);
  int countIntersectingFingers(LR lOrR);
  void Leap_Updater::Extrusion(mb::Vector &cameraPivot);
  void MenuSettings();
  void Leap_Updater::CollisionDectectionMovement();
  bool facesAreSelected;
  bool firstmoveswitch;
  bool inMenu;
  bool menuUp;
  bool menuRight;
  bool menuDown;
  bool menuLeft;
  bool collisionToggle;
  bool thumbGrabModeToggle;
  bool ThumbSelect();
  bool stickyMovement;
  bool Leap_Updater::ThumbSmoothMove();
  float thumbMoveStrength;
  mb::Vector menuStartSpace;
  Leap_HUD *menuFilter;
public:
  Leap_Updater(ID_List *idl,Leap_Hand *l,Leap_Hand *r);
  void OnEvent(const mb::EventGate &cEvent);
  mudbox::Vector getDir();
};

