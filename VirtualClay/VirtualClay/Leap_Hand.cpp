#include "stdafx.h"
#include "Leap_Hand.h"


Leap_Hand::Leap_Hand(ID_List *idl,LR lOrR)
{
  lr = lOrR;
  fingers.resize(5);
  idList = idl;
  AddHand();
  AddLeap_Fingers();
}

void Leap_Hand::AddHand(void) {
  palm = new Leap_Fingers();
  int id = palm->ImportGeo();
  idList->storeHandID(id,lr);
  TNode = palm->getTNode();
  TNode->SetName("Palm"+QString::number(id));
  palm->SetScale(mb::Vector(0.2f,0.2f,0.2f));
}

void Leap_Hand::AddLeap_Fingers(void) {
  mblog("Leap_Fingers size:"+QString::number(fingers.size()));
  QString lOrR = "L_";
  if(lr == r) {
    lOrR = "R_";
  }
  fingers.at(INDEX) = new Leap_Fingers(lOrR+"Index");
  fingers.at(MIDDLE) = new Leap_Fingers(lOrR+"Middle");
  fingers.at(RING) = new Leap_Fingers(lOrR+"Ring");
  fingers.at(PINKY) = new Leap_Fingers(lOrR+"Pinky");
  fingers.at(THUMB) = new Leap_Fingers(lOrR+"Thumb");
  int id = -1;
  for(int i = 0; i < 5; i++) {
    id = fingers.at(i)->ImportGeo();
    idList->storeFingerID(id,fingerEnum(i),lr);
    fingers.at(i)->SetPos(mb::Vector(i*20.0f,0.0f,0.0f));
    fingers.at(i)->SetScale(mb::Vector(0.1f,0.1f,0.1f));
  }
}




void Leap_Hand::SetFingerPos(fingerEnum f, mb::Vector v) {
  fingers.at(f)->SetPos(v);
}

void Leap_Hand::SetFingerRot(fingerEnum f, mb::Vector v) {
  fingers.at(f)->SetRot(v);
}

void Leap_Hand::SetRot(mb::Vector v) {
  TNode->SetRotation(v);
}

void Leap_Hand::SetPos(mb::Vector v) {
  TNode->SetPosition(v);
}

void Leap_Hand::SetVisi(bool vis) {
  TNode->SetVisible(vis);
  for(int i = 0 ; i < fingers.size() ; i++) {
    fingers.at(i)->SetVisi(vis);
  }
}


void Leap_Hand::RotateAroundPivot(fingerEnum f,mb::Vector a,mb::Vector pivot) {
  //mblog("Finger: "+QString::number(f)+"\n");
  fingers.at(f)->RotateAroundPivot(a,pivot);
}

void Leap_Hand::RotateAroundPivot(mb::Vector a,mb::Vector pivot) {
  palm->RotateAroundPivot(a,pivot);
}

//
//mb::Vector Leap_Hand::GetFingerPos(fingerEnum fn) {
//  return fingers.at(fn)->GetPos();
//}

Leap_Hand::~Leap_Hand(void)
{
}
