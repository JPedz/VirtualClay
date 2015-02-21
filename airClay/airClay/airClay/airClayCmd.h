#ifndef _airClayCmd
#define _airClayCmd
//
// Copyright (C) airClay
// 
// File: airClayCmd.h
//
// MEL Command: airClay
//
// Author: Maya Plug-in Wizard 2.0
//

#include <maya/MPxCommand.h>

class MArgList;

class airClay : public MPxCommand
{

public:
				airClay();
	virtual		~airClay();

	MStatus		doIt( const MArgList& );
	MStatus		redoIt();
	MStatus		undoIt();
	bool		isUndoable() const;

	static		void* creator();

private:
	// Store the data you will need to undo the command here
	//
};

#endif
