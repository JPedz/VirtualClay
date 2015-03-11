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
    void BuildGeo(void);
    int ID;
  public:
    Leap_Fingers(void);
    Leap_Fingers(QString n);
    int CopyGeo(Leap_Fingers *copyNode);
    mb::Geometry *GetGeoNode();
    mb::Transformation * getTNode();
    void SetPos(mb::Vector &v);
    mb::Vector GetPos(void);
    void SetRot(mb::Vector r);
    void SetRotMatrix(mb::Vector &r);
    void AddRot(mb::Vector r);
    mb::Vector GetRot();
    void SetScale(mb::Vector s);
    int ImportGeo(void);
    void SetVisi(bool visi);
    void RotateAroundPivot(mb::Vector angle, mb::Vector pivot);
    void changeMaterial();
    mb::AxisAlignedBoundingBox GetBoundingBox();
    
    //Collision Detection
    void UpdateCollisionPos(mb::Vector pos, mb::Vector pivot, mb::Vector a);
    mb::AxisAlignedBoundingBox Leap_Fingers::GetCollisionBox();
};


#endif