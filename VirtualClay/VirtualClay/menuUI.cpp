#include "stdafx.h"
#include "menuUI.h"
#include "cameraWrapper.h"
#include "fingers.h"

IMPLEMENT_CLASS( menuUI, mb::Node, "menuUI" );
menuUI::menuUI(void) {
  mb::Kernel()->Interface()->MessageBox(mb::Interface::MessageBoxType::msgInformation,"Test Title", "Some string",2,0);
  menuUI::Execute();
}



void menuUI::Execute() {
  cameraWrapper *L_Cam = new cameraWrapper("L_HandCam");
  cameraWrapper *R_Cam = new cameraWrapper("R_HandCam");
  L_Cam->addCameraToScene();
  R_Cam->addCameraToScene();
  QString s = QString::number(L_Cam->getID());
  addToIDList(L_Cam->getID());
  addToIDList(R_Cam->getID());
  mb::Kernel()->Interface()->HUDMessageShow(s);
  Fingers *finger = new Fingers(this);
  finger->ImportGeo();
  size_t k = ID_List.size();
  mb::Kernel()->Interface()->SetStatus(mudbox::Interface::stNormal,QString::number(ID_List.at(k-1)));
  //mb::Kernel()->Interface()->HUDMessageShow(QString::number(ID_List.at(0)));
  //mb::Node *n = mb::Node::ByID(L_Cam->getID());
  //mb::Kernel()->Interface()->HUDMessageShow(n->DisplayName());

  //mb::Transformation *nd = mb::CreateInstance<mb::Transformation>();
  //nd->SetPosition(mb::Vector(0.0,0.0,0.0));
  //nd->SetDisplayName("NEWNODE");
  
  L_Cam->setAim(mb::Vector(0,0,0));
  R_Cam->setAim(mb::Vector(0,0,0));
  L_Cam->setTNode();
  R_Cam->setTNode();
  L_Cam->setTranslation(mb::Vector(+200.0f,200.0f,500.0f));
  R_Cam->setTranslation(mb::Vector(-200.0f,200.0f,500.0f));


  mb::Kernel()->ViewPort()->Redraw();
}

void menuUI::addToIDList(int ID){
  ID_List.push_back(ID);
  mb::Kernel()->Interface()->HUDMessageShow(QString::number(ID_List.at(0)));
}

std::vector<int> menuUI::getIDList(void) {
  return ID_List;
}


