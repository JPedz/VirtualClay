//
// Copyright (C) airClay
// 
// File: airClayCmd.cpp
//
// MEL Command: airClay
//
// Author: Maya Plug-in Wizard 2.0
//

#include "airClayCmd.h"

#include <maya/MGlobal.h>

MStatus airClay::doIt( const MArgList& )
//
//	Description:
//		implements the MEL airClay command.
//
//	Arguments:
//		args - the argument list that was passes to the command from MEL
//
//	Return Value:
//		MS::kSuccess - command succeeded
//		MS::kFailure - command failed (returning this value will cause the 
//                     MEL script that is being run to terminate unless the
//                     error is caught using a "catch" statement.
//
{
	MStatus stat = MS::kSuccess;


	// Typically, the doIt() method only collects the infomation required
	// to do/undo the action and then stores it in class members.  The 
	// redo method is then called to do the actuall work.  This prevents
	// code duplication.
	//
	return redoIt();
}

MStatus airClay::redoIt()
//
//	Description:
//		implements redo for the MEL airClay command. 
//
//		This method is called when the user has undone a command of this type
//		and then redoes it.  No arguments are passed in as all of the necessary
//		information is cached by the doIt method.
//
//	Return Value:
//		MS::kSuccess - command succeeded
//		MS::kFailure - redoIt failed.  this is a serious problem that will
//                     likely cause the undo queue to be purged
//
{
	// Since this class is derived off of MPxCommand, you can use the 
	// inherited methods to return values and set error messages
	//
	setResult( "airClay command executed!\n" );

	return MS::kSuccess;
}

MStatus airClay::undoIt()
//
//	Description:
//		implements undo for the MEL airClay command.  
//
//		This method is called to undo a previous command of this type.  The 
//		system should be returned to the exact state that it was it previous 
//		to this command being executed.  That includes the selection state.
//
//	Return Value:
//		MS::kSuccess - command succeeded
//		MS::kFailure - redoIt failed.  this is a serious problem that will
//                     likely cause the undo queue to be purged
//
{

	// You can also display information to the command window via MGlobal
	//
    MGlobal::displayInfo( "airClay command undone!\n" );

	return MS::kSuccess;
}

void* airClay::creator()
//
//	Description:
//		this method exists to give Maya a way to create new objects
//      of this type. 
//
//	Return Value:
//		a new object of this type
//
{
	return new airClay();
}

airClay::airClay()
//
//	Description:
//		airClay constructor
//
{}

airClay::~airClay()
//
//	Description:
//		airClay destructor
//
{
}

bool airClay::isUndoable() const
//
//	Description:
//		this method tells Maya this command is undoable.  It is added to the 
//		undo queue if it is.
//
//	Return Value:
//		true if this command is undoable.
//
{
	return true;
}
