#include "stdafx.h"
#include "MenuUI.h"
#include "cameraWrapper.h"
#include "Leap_Fingers.h"
#include "Leap_Hand.h"
#include "Leap_Updater.h"
#include "ID_List.h"

IMPLEMENT_CLASS( MenuUI, mb::Node, "MenuUI" );
MenuUI::MenuUI(void) {
  idList = new ID_List();
  MenuUI::Execute();
}



void MenuUI::Execute() {
  cameraWrapper *L_Cam = new cameraWrapper("L_HandCam");
  cameraWrapper *R_Cam = new cameraWrapper("R_HandCam");
  idList->storeHandCamID(L_Cam->getID(),l);
  idList->storeHandCamID(R_Cam->getID(),r);
  Leap_Hand *hand_l = new Leap_Hand(idList,l);
  Leap_Hand *hand_r = new Leap_Hand(idList,r);
  Leap_Updater *lU = new Leap_Updater(idList,hand_l,hand_r);
  L_Cam->setTNode();
  R_Cam->setTNode();
  L_Cam->setTranslation(mb::Vector(+200.0f,200.0f,500.0f));
  R_Cam->setTranslation(mb::Vector(-200.0f,200.0f,500.0f));
  L_Cam->setAim(mb::Vector(0,0,0));
  R_Cam->setAim(mb::Vector(0,0,0));

  mb::Kernel()->ViewPort()->Redraw();
}



