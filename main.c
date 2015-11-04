/*

File: main.c

Version: <1.0>

Disclaimer: IMPORTANT:  This Apple software is supplied to you by Apple
Computer, Inc. ("Apple") in consideration of your agreement to the
following terms, and your use, installation, modification or
redistribution of this Apple software constitutes acceptance of these
terms.  If you do not agree with these terms, please do not use,
install, modify or redistribute this Apple software.

In consideration of your agreement to abide by the following terms, and
subject to these terms, Apple grants you a personal, non-exclusive
license, under Apple's copyrights in this original Apple software (the
"Apple Software"), to use, reproduce, modify and redistribute the Apple
Software, with or without modifications, in source and/or binary forms;
provided that if you redistribute the Apple Software in its entirety and
without modifications, you must retain this notice and the following
text and disclaimers in all such redistributions of the Apple Software. 
Neither the name, trademarks, service marks or logos of Apple Computer,
Inc. may be used to endorse or promote products derived from the Apple
Software without specific prior written permission from Apple.  Except
as expressly stated in this notice, no other rights or licenses, express
or implied, are granted by Apple herein, including but not limited to
any patent rights that may be infringed by your derivative works or by
other works in which the Apple Software may be incorporated.

The Apple Software is provided by Apple on an "AS IS" basis.  APPLE
MAKES NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION
THE IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS
FOR A PARTICULAR PURPOSE, REGARDING THE APPLE SOFTWARE OR ITS USE AND
OPERATION ALONE OR IN COMBINATION WITH YOUR PRODUCTS.

IN NO EVENT SHALL APPLE BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL
OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) ARISING IN ANY WAY OUT OF THE USE, REPRODUCTION,
MODIFICATION AND/OR DISTRIBUTION OF THE APPLE SOFTWARE, HOWEVER CAUSED
AND WHETHER UNDER THEORY OF CONTRACT, TORT (INCLUDING NEGLIGENCE),
STRICT LIABILITY OR OTHERWISE, EVEN IF APPLE HAS BEEN ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.

Copyright � 2004 Apple Computer, Inc., All Rights Reserved

*/ 
// This code will run on Mac OS X 10.2 (or later) ONLY!!!
#include <Carbon/Carbon.h>

#include "Window.h"
#include "MenuHandler.h"

// ------------------------------------------------------------------------------
// Function Prototypes
// ------------------------------------------------------------------------------

static OSStatus HandleCommandEvents( EventHandlerCallRef inHandlerCallRef,
	EventRef inEvent, void *inUserData );

// ------------------------------------------------------------------------------
// main
// ------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
	const EventTypeSpec	commandSpecs[] =
		{ { kEventClassCommand, kEventCommandProcess } };

    OSStatus		err;

    // Create a Nib reference passing the name of the nib file (without the 
    // .nib extension) CreateNibReference only searches into the application
    // bundle.
    err = CreateNibReference(CFSTR("main"), &gMainNibRef);
    require_noerr( err, main_err );
	
	// create the menu bar
	err = InitializeDemoMenus();
	require_noerr( err, main_nib_err );
 
	// install the application event handler for HI command events
	err = InstallApplicationEventHandler(
		NewEventHandlerUPP( HandleCommandEvents ), 
		GetEventTypeCount( commandSpecs ), commandSpecs, NULL, NULL );
	require_noerr( err, main_nib_err );
    
    // Then create a window. 
	err = NewDemoWindow();
	require_noerr( err, main_nib_err );
	    
    // Call the event loop
    RunApplicationEventLoop();
	
main_nib_err:
	
    // We don't need the nib reference anymore.
    DisposeNibReference(gMainNibRef);

main_err:

	return err;
}

// ------------------------------------------------------------------------------
// HandleCommandEvents												[INTERNAL]
// ------------------------------------------------------------------------------

static
OSStatus HandleCommandEvents( 
	EventHandlerCallRef	inHandlerCallRef, 
	EventRef 			inEvent,
	void 				*inUserData 		)
{
	OSStatus		err;
	HICommand		newCommand;

	// Get the event from the menu
	err = GetEventParameter( inEvent, kEventParamDirectObject, typeHICommand, 
		NULL, sizeof( HICommand ), NULL, (void *) &newCommand );
	require_noerr( err, HandleCommandEventsBadEvent_err );
	
	// reset the error to event not handled
	err = eventNotHandledErr;

	// get the event kind and handle that
	switch ( GetEventKind( inEvent ) )
	{
		// check to see if we got a process event
		case kEventCommandProcess:
			switch ( newCommand.commandID )
			{
				// handle the "new window" command
				case kHICommandNew:
					err = NewDemoWindow();
					break;
				
				// make sure that all of the windows are closed for the
				// quit event. Don't handle this to allow the app to
				// exit.
				case kHICommandQuit:
					CloseAllWindows();
					break;
					
				case kHICommandAbout:
				case kHICommandUndo:
				case kHICommandRedo:
				case kHICommandCut:
				case kHICommandCopy:
				case kHICommandPaste:
				case kHICommandClear:
				case kHICommandSelectAll:
				default:
					break;
					
			}
			break; // kEventCommandProcess
			
		default:
			break;
			
	}
					
					
HandleCommandEventsBadEvent_err:

	return err;

}
