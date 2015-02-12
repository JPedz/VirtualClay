// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"
#include "stdio.h"
#include "Mudbox\mudbox.h"
#include <vector>
#include "Leap.h"
#include "LeapListener.h"


class LeapPlugin : mudbox::Node{
  DECLARE_CLASS
public:
	static void Initializer(void);
	static void Execute(void);   // Execute 
	
};


// TODO: reference additional headers your program requires here
