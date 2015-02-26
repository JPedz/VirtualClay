#ifndef FINGERS_H_
#define FINGERS_H_
#include "stdafx.h"
#include "MenuUI.h"
#include "qmenu.h"
#include "qmainwindow.h"
#include "qmenubar.h"
#include "util.h"
namespace mb = mudbox;

class Leap_Fingers {
  private:
    QString name;
    mb::Node *Node;
    mb::Transformation *TNode;
    mb::AxisAlignedBoundingBox *fakePos;//For Collision detection
    mb::GroupNode *GNode;
    mb::Geometry *GeoNode;
    mb::Mesh *MNode;
    mb::Vector fakePosVect;
    void Leap_Fingers::BuildGeo(void);
    int ID;
  public:
    Leap_Fingers(void);
    Leap_Fingers(QString n);
    mb::Transformation * Leap_Fingers::getTNode();
    void Leap_Fingers::SetPos(mb::Vector v);
    mb::Vector Leap_Fingers::GetPos(void);
    void Leap_Fingers::SetRot(mb::Vector r);
    mb::Vector Leap_Fingers::GetRot();
    void Leap_Fingers::SetScale(mb::Vector s);
    int Leap_Fingers::ImportGeo(void);
    void Leap_Fingers::SetVisi(bool visi);
    void Leap_Fingers::RotateAroundPivot(mb::Vector angle, mb::Vector pivot);
    void Leap_Fingers::changeMaterial();
    mb::AxisAlignedBoundingBox GetBoundingBox();
    
    //Collision Detection
    void UpdateCollisionPos(mb::Vector pos, mb::Vector pivot, mb::Vector a);
    mb::AxisAlignedBoundingBox Leap_Fingers::GetCollisionBox();
};


#endif