#include "stdafx.h"
#include "menuUI.h"
#include "cameraWrapper.h"
#include "fingers.h"

menuUI::menuUI(void) {
  ID_List.reserve(2);
  menuUI::Execute();
}

void menuUI::Execute() {
  cameraWrapper *L_Cam = new cameraWrapper("L_HandCam");
  cameraWrapper *R_Cam = new cameraWrapper("R_HandCam");
  L_Cam->addCameraToScene();
  R_Cam->addCameraToScene();
  QString s = QString::number(L_Cam->getID());
  ID_List[0] = L_Cam->getID();
  ID_List[1] = R_Cam->getID();
  mb::Kernel()->Interface()->HUDMessageShow(s);
  Fingers *finger = new Fingers(this);
  mb::Node *n = mb::Node::ByID(L_Cam->getID());
  mb::Kernel()->Interface()->HUDMessageShow(n->DisplayName());

  mb::Transformation *nd = mb::CreateInstance<mb::Transformation>();
  nd->SetPosition(mb::Vector(0.0,0.0,0.0));
  nd->SetDisplayName("NEWNODE");
  
  L_Cam->setAim(mb::Vector(0,0,0));
  R_Cam->setAim(mb::Vector(0,0,0));
  L_Cam->setTNode();
  R_Cam->setTNode();
  L_Cam->setTranslation(mb::Vector(+200.0f,200.0f,500.0f));
  R_Cam->setTranslation(mb::Vector(-200.0f,200.0f,500.0f));
  mb::Kernel()->ViewPort()->Redraw();
}

void menuUI::addToIDList(int ID){
  ID_List.at(0) = ID;
}

std::vector<int> menuUI::getIDList(void) {
  return ID_List;
}


