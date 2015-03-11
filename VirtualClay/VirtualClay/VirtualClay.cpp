// VirtualClay.cpp : Defines the entry point for the console application.
//
#include "VirtualClay.h"
#include "stdafx.h"
#include "cameraWrapper.h"
#include "MenuUI.h"


IMPLEMENT_CLASS (VirtualClay,mb::Node,"MenuUI");
namespace mb = mudbox;
MB_PLUGIN( "VirtualClay", "CameraCreator", "James Pedlingham", "URL", VirtualClay::Initializer);

void VirtualClay::Initializer(void) {
  //mb::Kernel()->Interface()->AddCallbackMenuItem( mb::Interface::menuPlugins, QString::null, QObject::tr("TurnOn"), VirtualClay::Execute);
  //mb::Kernel()->Interface()->AddCallbackMenuItem( mb::Interface::menuPlugins, QString::null, QObject::tr("TurnOff"),VirtualClay::Cleanup);
  mb::Kernel()->Interface()->AddClassMenuItem(mb::Interface::menuCreate,"Leap Motion",MenuUI::StaticClass(),"Instatiate");
}


//TOOLS:

//Sphere DuringScene 142
//Sphere DuringTools - Erase 141
//Sphere DuringTools - Mask 140
//Sphere DuringTools - Freeze 139
//Sphere DuringTools - Scale 138
//Sphere DuringTools - Rotate 137
//Sphere DuringTools - Translate 136
//Sphere DuringTools - Invert 135
//Sphere DuringTools - Hue Shift 134
//Sphere DuringTools - Hue 133
//Sphere DuringTools - Sponge 132
//Sphere DuringTools - Contrast 131
//Sphere DuringTools - Burn 130
//Sphere DuringTools - Dodge 129
//Sphere DuringTools - Blur 128
//Sphere DuringTools - Dry Brush 127
//Sphere DuringTools - Clone 126
//Sphere DuringTools - Paint Erase 125
//Sphere DuringTools - Pencil 124
//Sphere DuringTools - Airbrush 123
//Sphere DuringTools - Projection 122
//Sphere DuringTools - Paint Brush 121
//Sphere During 120
//Sphere During 119
//Sphere During 118
//Sphere During 117
//Sphere During 116
//Sphere During 115
//Sphere During 114
//Sphere DuringTileHistogram 113
//Sphere During 112
//Sphere DuringTools - UV Shells 111
//Sphere DuringTools - Holes 110
//Sphere DuringTools - Objects 109
//Sphere DuringTools - Faces 108
//Sphere During 107
//Sphere During 106
//Sphere During 105
//Sphere During 104
//Sphere DuringTools - Grab 103
//Sphere DuringTools - Imprint 102
//Sphere DuringTools - Foamy 101
//Sphere DuringTools - Knife 100
//Sphere DuringTools - Repeat 99
//Sphere DuringTools - Spray 98
//Sphere DuringTools - Bulge 97
//Sphere DuringTools - Wax 96
//Sphere DuringTools - Fill 95
//Sphere DuringTools - Scrape 94
//Sphere DuringTools - Flatten 93
//Sphere DuringTools - Amplify 92
//Sphere DuringTools - Smooth 91
//Sphere DuringTools - Pinch 90
//Sphere DuringTools - Smear 89
//Sphere DuringTools - Sculpt 88
//Sphere DuringTools - Eyedropper 87