#pragma once
#include "stdafx.h"
#include "Leap_Reader.h"
#include "Leap_Hand.h"
#include "cameraWrapper.h"
#include "MeshOps.h"
#include "Leap_HUD.h"
#include <Mudbox/image.h>
#include "Leap_Tool.h"
#include <QTCore/QTime>
namespace mb = mudbox;
static const float menuDeadZone = 0.15f;
static const float menuActivateZone = 0.35f;

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
  MeshOps *meshOp_R;
  mb::Vector lastFrameHandPos_L;
  mb::Vector lastFrameHandPos_R;
  mb::Vector lastFrameThumbPos;
  void setDir(mudbox::Vector dir);
  void SetHandAndFingerPositions();
  void ScreenTap(LR lr);
  mb::Vector GetRelativeScreenSpaceFromWorldPos(mb::Vector wPos);
  void MoveMesh(LR lOrR);
  bool selectMesh(LR lOrR);
  bool selectMeshPinch();
  int countIntersectingFingers(LR lOrR);
  int countTouchingFingers(LR lOrR);
  void Extrusion(LR LorR);
  void MenuSettings_R();
  void MenuSettings_L();
  bool facesAreSelected_L;
  bool facesAreSelected_Tool;
  bool facesAreSelected_R;
  bool firstmoveswitch;
  bool firstmoveswitch_R;
  bool inMenu_L;
  bool inMenu_R;
  bool menuUp;
  bool menuRight;
  bool menuDown;
  bool menuLeft;

  bool selectWithBrushSize;
  bool reqIntersectionForSelection;
  bool thumbGrabModeToggle;
  bool stickyMovement;
  bool pinchGrab;
  bool toolStamp;
  bool brushSizeMenuToggle;
  bool SceneNavigationToggle;
  bool pivotHandsOnMesh;
  bool moveObjectMode;
  bool saveCurrentHandPivot;
  bool resetHandPivotToZero;
  bool ThumbSmoothMove();
  float thumbMoveStrength;
  mb::Vector menuStartSpace;
  Leap_HUD *menuFilter;
  
  bool ThumbSelect();
  float brushSize;
  mb::Vector brushSizeStartFingerStartPos;
  void BrushSize();

  __inline void checkNavigationGestures();
  __inline void checkMenuGesture();
  __inline void checkScreenTapGesture();
  __inline void checkUndoGesture();
  __inline void checkGrabbingGesture();
  __inline void checkToolIntersection();
  __inline void checkMoveObjGesture();
  __inline void checkUndoMoveGesture();
  //Moveing Object:
  void MoveSelectedObject(LR lr);
  bool isFirstGrab;

  mb::Vector cameraPivot;
  mb::Vector savedHandPivotPoint;
  //Tools:
  Leap_Tool *tool;
  void ToolSmoothMove();
  void ToolStampMove();
public:
  Leap_Updater(ID_List *idl,Leap_Hand *l,Leap_Hand *r);
  void OnEvent(const mb::EventGate &cEvent);
  mudbox::Vector getDir();
};

