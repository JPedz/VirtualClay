#ifndef FINGERS_H_
#define FINGERS_H_
#include "stdafx.h"
#include "MenuUI.h"
#include "qmenu.h"
#include "qmainwindow.h"
#include "qmenubar.h"
namespace mb = mudbox;

class Leap_Fingers {
  private:
    mb::Node *Node;
    mb::Transformation *TNode;
    mb::GroupNode *GNode;
    mb::Geometry *GeoNode;
    mb::Mesh *MNode;
    void Leap_Fingers::BuildGeo(void);
    int ID;
  public:
    Leap_Fingers(void);
    mb::Vector Leap_Fingers::GetPos(void);
    mb::Transformation * Leap_Fingers::getTNode();
    void Leap_Fingers::SetPos(mb::Vector v);
    void Leap_Fingers::SetRot(mb::Vector r);
    void Leap_Fingers::SetScale(mb::Vector s);
    int Leap_Fingers::ImportGeo(void);
};


#endif