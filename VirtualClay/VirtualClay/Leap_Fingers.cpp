#include "stdafx.h"
#include "Leap_Fingers.h"

namespace mb = mudbox;

Leap_Fingers::Leap_Fingers() {
}

//http://around-the-corner.typepad.com/adn/2013/06/accessing-unsubdivide-functionality-from-a-mudbox-plug-in.html
//Import Geo returns ID of sphere made.
int Leap_Fingers::ImportGeo(void) {
  int sphereID = 0;
  mb::Node *nodes = mb::Node::First();
  QMenu *m_create = mb::Kernel()->Interface()->DropDownMenu(mb::Interface::ddmCreate);
  QList<QAction *> menuActionList = m_create->actions();
  mb::Kernel ()->Log (QString ("MAINmenu %1\n\n").arg (m_create->accessibleName()));
  QAction *mesh_A = NULL;
  //QAction *pA = NULL;
  //QList<QAction *> openMenu =m_create->findChildren<QAction *>() ;//Find Create Menu ;
  ////for ( int i =0 ; i < openMenu.size () && pA == NULL ; i++ ) {
  //  QString menu_s = openMenu.at(i)->text();
  //  mb::Kernel ()->Log (QString ("menu %1\n").arg (menu_s)) ;
  //  if ( menu_s == "Create" ) {
  //    pA = openMenu.at(i);
  //      pA->setObjectName("hi");
  //      pA->activate(QAction::ActionEvent::Hover);
  //      mb::Kernel ()->Log (QString ("THIS %1\n").arg (menu_s));
  //      break;
  //  }
  //}
  QList<mb::Transformation *> sphere_t_list_before;
  QList<mb::Transformation *> sphere_t_list_after;
  for(nodes ; nodes ; nodes = nodes->Next()) {
    // go through to end;  
    if(nodes->IsKindOf(mb::Transformation::StaticClass())) {
      mb::Transformation *sphere  = dynamic_cast<mb::Transformation *>(nodes);
      MB_SAFELY(sphere) {
        QString s_Name = sphere->Name();
        if(sphere->Name() != NULL) {
          //if(sphere->Name().compare(QString("sphere")))
          if(s_Name.indexOf( "sphere" ) != -1) {
            sphere_t_list_before.push_back(sphere);
          }
        }
      }
    }
  }

  for(int i = 0; i < sphere_t_list_before.size() ; i++ ) {
    mb::Kernel()->Log(QString("Sphere")+sphere_t_list_before.at(i)->Name()+" "+QString::number(sphere_t_list_before.at(i)->ID())+"\n");
  }

  QList<QAction *> pMenus =m_create->findChildren<QAction *>() ;//Find Create Menu ;
  for ( int i =0 ; i < pMenus.size () && mesh_A == NULL ; i++ ) {
    QString menu_s = pMenus.at(i)->text();
    mb::Kernel ()->Log (QString ("menu %1\n").arg (menu_s)) ;
    if ( menu_s == "Sphere" ) {
        mb::Kernel ()->Log (QString ("THIS %1\n").arg (menu_s)) ;
        mesh_A =pMenus.at(i);
        break;
    }
  }
  if(mesh_A != NULL) {
    if(mesh_A->text().contains(QString("Sphere"))) {
      mb::Kernel()->Log( QString ("Got the action!!\n"));
      mesh_A->activate (QAction::Trigger) ;
    }
  }

  ////Maybe try:
  //TODO:
  //mb::TreeNode *pnodes = mb::Kernel()->Scene()->FirstChild();
  //for(pnodes ; pnodes ; pnodes->NextSibling()) {}
  for(nodes = mb::Node::First() ; nodes ; nodes = nodes->Next()) {
    //REMEMBER THIS SEARCHES THROUGH ALL NODES,
    // CURVES, TOOLS (brushes etc..) and scene nodes.

    // go through to end;  
    //mb::Kernel()->Log(QString("Sphere During")+nodes->Name()+" "+QString::number(nodes->ID())+"\n");
    if(nodes->IsKindOf(mb::Transformation::StaticClass())) {
      mb::Transformation *sphere  = dynamic_cast<mb::Transformation *>(nodes);
      MB_SAFELY(sphere) {
        //TODO: If unique sphere and not one from before.
        QString s_Name = sphere->Name();
        if(sphere->Name() != NULL) {
          //if(sphere->Name().compare(QString("sphere")))
          if(s_Name.indexOf( "sphere" ) != -1) {
            sphere_t_list_after.push_back(sphere);
            sphere->SetScale(mb::Vector(0.3f,0.3f,0.6f));
            TNode = sphere;
            sphereID = sphere->ID();
            ID = sphereID;
            mb::Kernel()->Log(QString("Sphere During")+sphere->Name()+" "+QString::number(nodes->ID())+"\n");
 
            sphere->SetName("Finger"+QString::number(sphereID));
          } 
        }
      }
    } 
  }

  for(int i = 0; i < sphere_t_list_after.size() ; i++ ) {
    mb::Kernel()->Log(QString("SphereAfter")+sphere_t_list_after.at(i)->Name()+"\n");
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

void Leap_Fingers::SetScale(mb::Vector v) {
  if(TNode != NULL)
    TNode->SetScale(v);
}

void Leap_Fingers::BuildGeo() {
  ImportGeo();

}