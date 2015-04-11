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
//
//complicated schematics for all of the functions i need to implement.
//
//	layers, selecting brushes
//
//fire off an email to aardman.
//


mb::Vector cameraWrapper::getForward(void) {
  return cam->Forward();
}

mb::Vector cameraWrapper::getPosision() {
  return cam_TNode->Position();
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

void cameraWrapper::setRotation(mb::Vector &ypr) {
//  cam->SetAim(mb::Vector(0,0,0));
//  cam->SetTarget(mb::Vector(0,0,0));
  cam_TNode->SetRotation(ypr);
}

void cameraWrapper::setTranslation(mb::Vector v) {
  //cam_TNode = cam_GNode->Transformation();
  cam_TNode->SetPosition(v);
}

void cameraWrapper::setPosition(mb::Vector &pos) {
    cam->SetPosition(pos);
}

void cameraWrapper::setPosition(mb::Vector pos) {
    cam->SetPosition(pos);
}

void cameraWrapper::MoveForward(float dist) {
  if(dist > 0) 
    cam->MoveForward(dist);
  else 
    cam->MoveBackward(dist);
}

void cameraWrapper::deleteCam(void) {
  delete cam_TNode;
}