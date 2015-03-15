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
  cameraWrapper *ToolCam = new cameraWrapper("ToolCam");
  mblog("Original LCAM ID = "+QString::number(L_Cam->getID())+"\n");
  //L_Cam->getCamera()->SetOrthographic(true);
  //R_Cam->getCamera()->SetOrthographic(true);
  idList->setViewCam();
  cameraWrapper *viewCam_tmp = new cameraWrapper(idList->getViewCam());
  viewCam_tmp->getTNode()->SetRotation(mb::Vector(0,0,0));
  viewCam_tmp->getTNode()->SetPosition(mb::Vector(0,0,600));
  delete viewCam_tmp;
  idList->storeHandCamID(L_Cam->getID(),l);
  idList->storeHandCamID(R_Cam->getID(),r);
  idList->setToolCam(ToolCam->getID());
  Leap_Hand *hand_l = new Leap_Hand(idList,l);
  Leap_Hand *hand_r = new Leap_Hand(idList,r,hand_l);
  Leap_Updater *lU = new Leap_Updater(idList,hand_l,hand_r);

}



