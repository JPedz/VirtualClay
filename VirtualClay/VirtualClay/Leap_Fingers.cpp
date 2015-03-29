#include "stdafx.h"
#include "Leap_Fingers.h"

namespace mb = mudbox;

Leap_Fingers::Leap_Fingers() {
  name = "";
}

Leap_Fingers::Leap_Fingers(QString n) {
  name = n;
}

void Leap_Fingers::UpdateCollisionPos(mb::Vector pos, mb::Vector pivot, mb::Vector a) {
  fakePosVect = (RotateVectorAroundPivot(pos,pivot,a));
  mblog("testpos Centre  "+VectorToQString(fakePosVect)+"\n");
  if(fakePos)
    delete fakePos;
  fakePos = NULL;
  mb::Vector size = mb::Vector(2,2,2);
  fakePos = new mb::AxisAlignedBoundingBox(fakePosVect-size,fakePosVect+size);
  mb::AxisAlignedBoundingBox ab = mb::AxisAlignedBoundingBox(fakePosVect,1.0f);
  mblog("FakePos Centre  "+QString::number(fakePos->m_vStart)+" "+QString::number(ab.m_vEnd)+"\n");
}

mb::AxisAlignedBoundingBox Leap_Fingers::GetCollisionBox() {
  return *fakePos;
}
  

//http://around-the-corner.typepad.com/adn/2013/06/accessing-unsubdivide-functionality-from-a-mudbox-plug-in.html
//Import Geo returns ID of sphere made.
int Leap_Fingers::ImportGeo(void) {
  QMenu *m_create = mb::Kernel()->Interface()->DropDownMenu(mb::Interface::ddmCreate);
  QAction *mesh_A = NULL;
  QList<QAction *> pMenus =m_create->findChildren<QAction *>() ;
  //Find Create Sphere Menu to add a sphere to the scene;
  for ( int i =0 ; i < pMenus.size () && mesh_A == NULL ; i++ ) {
    QString menu_s = pMenus.at(i)->text();
    mblog(QString ("menu %1\n").arg (menu_s)) ;
    if ( menu_s == "Sphere" ) {
        mblog(QString ("THIS %1\n").arg (menu_s)) ;
        mesh_A =pMenus.at(i);
        break;
    }
  }
  if(mesh_A != NULL) {
    if(mesh_A->text().contains(QString("Sphere"))) {
      mesh_A->activate(QAction::Trigger);
    }
  }
  for(mb::Node *nodes = mb::Node::First() ; nodes ; nodes = nodes->Next()) {
    //REMEMBER THIS SEARCHES THROUGH ALL NODES,
    // CURVES, TOOLS (brushes etc..) and scene nodes.
    if(nodes->IsKindOf(mb::Transformation::StaticClass())) {
      mb::Transformation *sphere = dynamic_cast<mb::Transformation *>(nodes);
      MB_SAFELY(sphere) {
        QString s_Name = sphere->Name();
        if(sphere->Name() != NULL) {
          if(s_Name.indexOf( "sphere" ) != -1) {
            sphere->SetScale(mb::Vector(0.3f,0.3f,0.6f));
            TNode = sphere;
            ID = sphere->ID();
            mb::Kernel()->Log(QString("Sphere During")+" "+QString::number(sphere->ID())+"\n");
            if(name == "")
              sphere->SetName("Finger"+QString::number(ID));
            else
              sphere->SetName(name+"_"+QString::number(ID));
            break;
          }
        }
      }
    } 
  }
  mblog("Looking for geo node\n");
  for(mb::Node *nodes = mb::Node::First() ; nodes ; nodes = nodes->Next()) {
    if(nodes->IsKindOf(mb::Geometry::StaticClass())) {
      mb::Geometry *geoPotential = dynamic_cast<mb::Geometry *>(nodes);
      MB_SAFELY(geoPotential) {
        QString s_GName = geoPotential->Name();
        if((s_GName.indexOf( "Finger"+QString::number(ID) ) != -1) || 
          (s_GName.indexOf(name+"_"+QString::number(ID) ) != -1)) {
          mblog("found the geo node\n");
          GeoNode = geoPotential;
          GeoNode->SetActiveLevel(GeoNode->HighestLevel());
          GeoNode->ChangeActiveLevel(GeoNode->LowestLevel());
        }
      }
	  }
  }
  mblog("\nreturning after creation\n")
  return ID;
}

int Leap_Fingers::CopyGeo(Leap_Fingers *copyNode) {
  GeoNode = mb::CreateInstance<mb::Geometry>();
  if(copyNode->GetGeoNode() ==NULL)
    mblog("NO GEO NODE TO COPY!\n");
  GeoNode = dynamic_cast<mb::Geometry *>(copyNode->GetGeoNode()->Duplicate());
  mb::Kernel()->Scene()->AddGeometry(GeoNode);
  TNode = GeoNode->Transformation();
  ID = GeoNode->ID();
  TNode->SetName(name+"_"+QString::number(ID));
  GeoNode->SetName(name+"_"+QString::number(ID));
  GeoNode->SetActiveLevel(GeoNode->HighestLevel());
  return ID;
}

mb::Geometry* Leap_Fingers::GetGeoNode() {
  if(GeoNode) {
    return GeoNode;
  }
  return NULL;
}


void Leap_Fingers::changeMaterial() {

}

mb::Transformation * Leap_Fingers::getTNode() {
  return TNode;
}

void Leap_Fingers::SetPos(mb::Vector &v) {
  if(TNode != NULL)
    TNode->SetPosition(v);
}

void Leap_Fingers::SetRot(mb::Vector v) {
  if(TNode != NULL)
    TNode->SetRotation(v);
}

void Leap_Fingers::SetRot(mb::Matrix m) {
  if(TNode != NULL)
    TNode->SetRotation(m);
}

void Leap_Fingers::SetRotMatrix(mb::Vector &rotation) {
  if(TNode != NULL) {
    mb::Matrix rX = createRotateXMatrix(rotation.x);
    mb::Matrix rY = createRotateYMatrix(rotation.y);
    mb::Matrix rZ = createRotateZMatrix(rotation.z);
    mb::Matrix rotationMatrix = rX*rZ*rY;
    TNode->SetRotation(rotationMatrix);
  }
}

void Leap_Fingers::AddRot(mb::Vector v) {
  if(TNode != NULL)
    TNode->AddRotation(v,true);
}

mb::Vector Leap_Fingers::GetRot() {
  return TNode->Rotation();
}

mb::Vector Leap_Fingers::GetPos() {
  return TNode->Position();
}


void Leap_Fingers::RotateAroundPivot(mb::Vector a, mb::Vector pivot) {
    mb::Vector tmp;
  if(TNode != NULL) {
    tmp = TNode->Position();
    TNode->SetPosition(RotateVectorAroundPivot(tmp,pivot,a));
    //TNode->SetRotation(TNode->Rotation()-a);
  }
}

void Leap_Fingers::SetScale(mb::Vector v) {
  if(TNode != NULL)
    TNode->SetScale(v);
}


void Leap_Fingers::SetVisi(bool visi) {
  if(TNode != NULL) {
    if(TNode->Visible() != visi) {
      TNode->SetVisible(visi);
    }
  }
}

void Leap_Fingers::BuildGeo() {
  ImportGeo();
}

mb::AxisAlignedBoundingBox Leap_Fingers::GetBoundingBox() {
  return mb::AxisAlignedBoundingBox(TNode->Position(),3.0f);
}

//Old rotation matrix stuff;
//TODO: remove
    //float s_x, s_y, s_z;
    //s_x = TNode->Scale().x;
    //s_y = TNode->Scale().y;
    //s_z = TNode->Scale().z;
    //float XD = pivot.x - x ;
    //float YD = pivot.y-y;
    //float ZD = pivot.z-z;

    //float ZX = XD*cos(a.z) - YD*sin(a.z) - XD;
    //float ZY = XD*sin(a.z) + YD*cos(a.z) - YD;

    // float YX = (XD+ZX)*cos(a.y) - ZD*sin(a.y)- (XD+ZX);
    // float YZ = (XD+ZX)*sin(a.y) + ZD*cos(a.y) - ZD;

    // float XY = (YD+ZY)*cos(a.x)- (ZD+YZ)*sin(a.x) - (YD+ZY);
    // float XZ = (YD+ZY)*sin(a.x) + (ZD+YZ)*cos(a.x)- (ZD+YZ);

    // float XROTOFFSET = YX+ZX;
    // float YROTOFFSET = ZY+XY;
    // float ZROTOFFSET = XZ+YZ;
    // 
    // float newX = (x + XROTOFFSET);
    // float newY = (y + YROTOFFSET);
    // float newZ = (z + ZROTOFFSET);