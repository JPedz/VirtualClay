#pragma once
#include "stdafx.h"
#include "Leap_Reader.h"
#include "Leap_Hand.h"
#include "cameraWrapper.h"
#include "MeshOps.h"
#include "Leap_HUD.h"

namespace mb = mudbox;
class Leap_Updater : public mb::Node {
  std::vector<Leap_Fingers *> tools;
  mudbox::Vector dir;
  ID_List *idList;
  Leap_Hand *hand_l;
  Leap_Hand *hand_r;
  cameraWrapper *viewCam;
  mb::Vector fitToCameraSpace();
  void Leap_Updater::rotateCamera(mb::Vector r);
  void Leap_Updater::CameraMovement();
  mb::aevent frameEvent;
  Leap_Reader *leapReader;
  MeshOps *meshOp;
  mb::Vector lastFrameHandPos;
  mb::Vector lastFrameThumbPos;
  void setDir(mudbox::Vector dir);
  void SetHandAndFingerPositions(mb::Vector &camPivot);
  void ScreenTap(mb::Vector &camPos);
  mb::Vector GetRelativeScreenSpaceFromWorldPos(mb::Vector &wPos);
  void MoveMesh();
  bool selectMesh(mb::Vector &camPos);
  bool selectMeshPinch(mb::Vector &camPos);
  int countIntersectingFingers(LR lOrR);
  void Leap_Updater::Extrusion(mb::Vector &cameraPivot);
  void MenuSettings_R();
  void MenuSettings_L();
  void Leap_Updater::CollisionDectectionMovement();
  bool facesAreSelected;
  bool firstmoveswitch;
  bool inMenu_L;
  bool inMenu_R;
  bool menuUp;
  bool menuRight;
  bool menuDown;
  bool menuLeft;
  bool collisionToggle;
  bool thumbGrabModeToggle;
  bool ThumbSelect();
  bool stickyMovement;
  bool pinchGrab;
  bool Leap_Updater::ThumbSmoothMove();
  float thumbMoveStrength;
  mb::Vector menuStartSpace;
  Leap_HUD *menuFilter;
  
  __inline void Leap_Updater::checkRotateGesture();
  __inline void Leap_Updater::checkMenuGesture();
  __inline void Leap_Updater::checkScreenTapGesture(mb::Vector &cameraPivot);
  __inline void Leap_Updater::checkUndoGesture();
  __inline void Leap_Updater::checkGrabbingGesture(mb::Vector &cameraPivot);
public:
  Leap_Updater(ID_List *idl,Leap_Hand *l,Leap_Hand *r);
  void OnEvent(const mb::EventGate &cEvent);
  mudbox::Vector getDir();
};

