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
  int sphereID = 0;
  QMenu *m_create = mb::Kernel()->Interface()->DropDownMenu(mb::Interface::ddmCreate);
  QList<QAction *> menuActionList = m_create->actions();
  QAction *mesh_A = NULL;

  QList<int> sphere_t_list_before;
  QList<int> sphere_t_list_after;
  mb::Node *nodes = mb::Node::First();
  for(nodes ; nodes ; nodes = nodes->Next()) {
    // go through to end;  
    if(nodes->IsKindOf(mb::Transformation::StaticClass())) {
      mb::Transformation *sphere  = dynamic_cast<mb::Transformation *>(nodes);
      MB_SAFELY(sphere) {
        QString s_Name = sphere->Name();
        if(sphere->Name() != NULL) {
          //if(sphere->Name().compare(QString("sphere")))
          if(s_Name.indexOf( "sphere" ) != -1) {
            sphere_t_list_before.push_back(sphere->ID());
          }
        }
      }
    }
  }
  mblog("\n");
  for(int i = 0; i < sphere_t_list_before.size() ; i++ ) {
    mblog(QString("Sphere")+" "+QString::number(sphere_t_list_before.at(i))+"\n");
  }

  QList<QAction *> pMenus =m_create->findChildren<QAction *>() ;//Find Create Menu ;
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
      mblog( QString ("Got the action!!\n"));
      mesh_A->activate (QAction::Trigger) ;
    }
  }
  bool isUnique;
  for(nodes = mb::Node::First() ; nodes ; nodes = nodes->Next()) {
    isUnique = true;
    //REMEMBER THIS SEARCHES THROUGH ALL NODES,
    // CURVES, TOOLS (brushes etc..) and scene nodes.
    // go through to end;  
    //mb::Kernel()->Log(QString("Sphere During")+nodes->Name()+" "+QString::number(nodes->ID())+"\n");
    if(nodes->IsKindOf(mb::Transformation::StaticClass())) {
      mb::Transformation *sphere  = dynamic_cast<mb::Transformation *>(nodes);
      MB_SAFELY(sphere) {
        for(int j = 0; j < sphere_t_list_before.size() ; j++) {
        if(sphere_t_list_before.at(j) == sphere->ID()) 
          isUnique = false;
        }
        if(isUnique) {
          QString s_Name = sphere->Name();
          if(sphere->Name() != NULL) {
            if(s_Name.indexOf( "sphere" ) != -1) {
              sphere_t_list_after.push_back(sphere->ID());
              sphere->SetScale(mb::Vector(0.3f,0.3f,0.6f));
              TNode = sphere;
              sphereID = sphere->ID();
              ID = sphereID;
              mb::Kernel()->Log(QString("Sphere During")+" "+QString::number(sphere->ID())+"\n");
              if(name == "")
                sphere->SetName("Finger"+QString::number(sphereID));
              else
                sphere->SetName(name+"_"+QString::number(sphereID));
              break;
            }
          }
        }
      }
    } 
  }

  for(int i = 0; i < sphere_t_list_after.size() ; i++ ) {
    mblog(QString("SphereAfter ")+sphere_t_list_after.at(i)+"\n");
  }
  mblog("\nreturning after creation\n")
  return sphereID;
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