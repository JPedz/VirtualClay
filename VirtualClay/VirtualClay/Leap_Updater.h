#pragma once
#include "stdafx.h"
#include "Leap_Reader.h"
#include "Leap_Hand.h"
#include "cameraWrapper.h"
#include "MeshOps.h"
#include "Leap_HUD.h"
#include "GestureHUD.h"
#include <Mudbox/image.h>
#include "Leap_Tool.h"
#include <QTCore/QTime>
#include "UltraHaptics_Output.h"
namespace mb = mudbox;
static const float menuDeadZone = 0.15f;
static const float menuActivateZone = 0.3f;
static const float menuActivateZone2 = 0.6f;


class Leap_Updater : public mb::Node {
    
  QTime *bigTimer;
  mudbox::Vector dir;
  ID_List *idList;
  Leap_Hand *hand_l;
  Leap_Hand *hand_r;
  cameraWrapper *viewCam;
  mb::Vector fitToCameraSpace();
  void rotateCamera(mb::Vector r);
  void CameraRotate(LR lOrR);
  void CameraZoom(LR lOrR);
  void CameraPan(LR lOrR);
  mb::aevent frameEvent;
  Leap_Reader *leapReader;
  MeshOps *meshOp;
  MeshOps *meshOp_R;
  mb::Vector lastFrameHandPos_L;
  mb::Vector lastFrameHandPos_R;
  mb::Vector lastFrameThumbPos;
  void setDir(mudbox::Vector dir);
  __inline void SetHandAndFingerPositions();
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
  bool menuUp_0;
  bool menuUp_1;
  bool menuRight_1;
  bool menuRight_0;
  bool menuDown;
  bool menuDown_0;
  bool menuDown_1;
  bool menuLeft;

  bool selectWithBrushSize;
  bool reqIntersectionForSelection;
  bool thumbGrabModeToggle;
  bool thumbDirectionBasedMovement;
  bool pinchGrab;
  bool toolStamp;
  bool brushSizeMenuToggle;
  bool brushStrengthMenuToggle;
  bool SceneNavigationToggle;
  bool pivotHandsOnMesh;
  bool moveObjectMode;
  mb::Vector menuStartSpace;
  Leap_HUD *menuFilter;
  GestureHUD *gestureHUD;
  //Leap_HUD *brushIcon;
  
  void ThumbSmoothMove(LR lr);
  void ThumbDirMove(LR lr);
  void ThumbPush(LR lr);

  bool ThumbSelect();
  float brushSize;
  mb::Vector brushSizeStartFingerStartPos;
  mb::Vector brushStrengthFingerStartPos;
  void BrushSize();
  void BrushStrength();
  float brushStrength;

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
  
  bool GimbalLockZXYMode_L;
  bool GimbalLockZXYMode_R;

  mb::Vector cameraPivot;
  mb::Vector savedPanHandPosition_L;
  mb::Vector savedPanHandPosition_R;
  bool firstPan_L;
  bool firstPan_R;
  mb::Vector savedHandPivotPoint;
  //Tools:
  Leap_Tool *tool;
  void ToolSmoothMove();
  void ToolStampMove();


  long missCounter;
  unsigned int uniqueMiss;
  bool uniqueMissBool;
public:
  Leap_Updater(ID_List *idl,Leap_Hand *l,Leap_Hand *r);
  void OnEvent(const mb::EventGate &cEvent);
  mudbox::Vector getDir();
};

