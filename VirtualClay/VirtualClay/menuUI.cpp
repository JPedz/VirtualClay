#include "stdafx.h"
#include "menuUI.h"

menuUI::menuUI(void) {
  ID_List.reserve(2);
}

void menuUI::addToIDList(int ID){
  ID_List.at(0) = ID;
}

std::vector<int> menuUI::getIDList(void) {
  return ID_List;
}


