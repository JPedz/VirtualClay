#include "stdafx.h"
#include "Leap_Fingers.h"

namespace mb = mudbox;

Leap_Fingers::Leap_Fingers() {
  name = "";
}

Leap_Fingers::Leap_Fingers(QString n) {
  name = n;
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
  mblog("\nreturning after creation\n")
  return ID;
}

mb::Transformation * Leap_Fingers::getTNode() {
  return TNode;
}

void Leap_Fingers::SetPos(mb::Vector v) {
  if(TNode != NULL)
    TNode->SetPosition(v);
}

void Leap_Fingers::SetRot(mb::Vector v) {
  if(TNode != NULL)
    TNode->SetRotation(v);
}

void Leap_Fingers::RotateAroundPivot(mb::Vector angle, mb::Vector pivot) {
  if(TNode != NULL) {
    mb::Vector storedPivot = TNode->Pivot();
    TNode->SetPivot(pivot);
    mblog("Pivots: "+QString::number(storedPivot.y)+" "+QString::number(pivot.y)+"\n");
    TNode->SetRotation(angle);
    TNode->SetPivot(storedPivot);
  }
}

void Leap_Fingers::SetScale(mb::Vector v) {
  if(TNode != NULL)
    TNode->SetScale(v);
}


void Leap_Fingers::SetVisi(bool visi) {
  if(TNode != NULL)
    TNode->SetVisible(visi);
}

void Leap_Fingers::BuildGeo() {
  ImportGeo();

}