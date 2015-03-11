#pragma once
#include "stdafx.h"
#include "Leap_Reader.h"
#include "Leap_Hand.h"
#include "cameraWrapper.h"
#include "MeshOps.h"
#include "Leap_HUD.h"
#include <Mudbox/image.h>
#include "Leap_Tool.h"

namespace mb = mudbox;
class Leap_Updater : public mb::Node {
  mudbox::Vector dir;
  ID_List *idList;
  Leap_Hand *hand_l;
  Leap_Hand *hand_r;
  cameraWrapper *viewCam;
  mb::Vector fitToCameraSpace();
  void rotateCamera(mb::Vector r);
  void CameraRotate(LR lOrR);
  void CameraZoom(LR lOrR);
  mb::aevent frameEvent;
  Leap_Reader *leapReader;
  MeshOps *meshOp;
  mb::Vector lastFrameHandPos;
  mb::Vector lastFrameThumbPos;
  void setDir(mudbox::Vector dir);
  void SetHandAndFingerPositions();
  void ScreenTap();
  mb::Vector GetRelativeScreenSpaceFromWorldPos(mb::Vector wPos);
  void MoveMesh();
  bool selectMesh(mb::Vector &camPos);
  bool selectMeshPinch(mb::Vector &camPos);
  int countIntersectingFingers(LR lOrR);
  void Extrusion();
  void MenuSettings_R();
  void MenuSettings_L();
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
  bool toolStamp;
  bool brushSizeMenuToggle;
  bool SceneNavigationToggle;
  bool pivotHandsOnMesh;
  bool ThumbSmoothMove();
  float thumbMoveStrength;
  mb::Vector menuStartSpace;
  Leap_HUD *menuFilter;
  
  __inline void checkNavigationGestures();
  __inline void checkMenuGesture();
  __inline void checkScreenTapGesture();
  __inline void checkUndoGesture();
  __inline void checkGrabbingGesture();
  __inline void checkToolIntersection();


  mb::Vector cameraPivot;
  //Tools:
  Leap_Tool *tool;
  void ToolSmoothMove();
  void ToolStampMove();
public:
  Leap_Updater(ID_List *idl,Leap_Hand *l,Leap_Hand *r);
  void OnEvent(const mb::EventGate &cEvent);
  mudbox::Vector getDir();
};

