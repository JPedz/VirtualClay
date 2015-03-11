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
<<<<<<< HEAD
    mb::Material *mat;
    void BuildGeo(void);
    void SetMaterial();
=======
    mb::Vector fakePosVect;
<<<<<<< HEAD
    void Leap_Fingers::BuildGeo(void);
>>>>>>> 9e8b7a505bdeab7a07a9a3501a9f33b85881a056
=======
    void BuildGeo(void);
>>>>>>> da004796e4e964c1fbfae449684aa30bdec54feb
    int ID;
  public:
    Leap_Fingers(void);
    Leap_Fingers(QString n);
<<<<<<< HEAD
    mb::Transformation * getTNode();
    void SetPos(mb::Vector v);
    mb::Vector GetPos(void);
    void SetRot(mb::Vector r);
=======
    int CopyGeo(Leap_Fingers *copyNode);
    mb::Geometry *GetGeoNode();
    mb::Transformation * getTNode();
    void SetPos(mb::Vector &v);
    mb::Vector GetPos(void);
    void SetRot(mb::Vector r);
    void SetRotMatrix(mb::Vector &r);
    void AddRot(mb::Vector r);
>>>>>>> da004796e4e964c1fbfae449684aa30bdec54feb
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