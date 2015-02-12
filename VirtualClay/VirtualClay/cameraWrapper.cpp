#include "stdafx.h"
#include "cameraWrapper.h"


cameraWrapper::cameraWrapper(QString name) {
  cam = dynamic_cast<mb::Camera *>(mb::Kernel()->Scene()->ByName("Camera",name));
  if(cam == NULL) {
    cam = mb::CreateInstance<mb::Camera>();
    cam->SetName(name);
    mb::Kernel()->Scene()->AddCamera(cam);  
	  mb::Kernel()->Interface()->RefreshUI();
  }
  cam_GNode = dynamic_cast<mb::GroupNode *>(cam);
  cam_TNode = cam_GNode->Transformation();
}

cameraWrapper::cameraWrapper(int id) {
  cam = dynamic_cast<mb::Camera *>(mb::Kernel()->Scene()->ByID(id));
  cam_GNode = dynamic_cast<mb::GroupNode *>(cam);
  cam_TNode = cam_GNode->Transformation();
}

mb::Camera *cameraWrapper::getCamera(void) {
  return cam;
}

int cameraWrapper::getID(void) {
  return cam->ID();
}

QString cameraWrapper::getName(void) {
  return cam->Name();
}


mb::GroupNode *cameraWrapper::getGNode(void) {
  return cam_GNode;
}

mb::Transformation*cameraWrapper::getTNode(void) {
  return cam_TNode;
}

void cameraWrapper::setTNode(void) {
  cam_TNode = cam_GNode->Transformation();
}


void cameraWrapper::setAim(mb::Vector v) {
  cam->SetAim(v);
  cam->SetTarget(v);
}

void cameraWrapper::setTranslation(mb::Vector v) {
  //cam_TNode = cam_GNode->Transformation();
  cam_TNode->SetPosition(v);
}

void cameraWrapper::selectPoints(mb::Vector v) {

}

void cameraWrapper::deleteCam(void) {
  delete cam_TNode;
}