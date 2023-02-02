//------------------------------------------------------------------------------
// Name:    singleinstance.h
// Author:  Justin Hallet
// Date:    30/11/2002
//------------------------------------------------------------------------------

#ifndef __SINGLEINSTANCE_H__
#define __SINGLEINSTANCE_H__

//------------------------------------------------------------------------------

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//------------------------------------------------------------------------------

class CSingleInstance
// Name:        CSingleInstance
// Parent:		 N/A
// Description: Manages single instance activation, just call Create with unique 
//              application name.
{
	public :

		// Constructor/Destructor
		CSingleInstance () ;
		virtual ~CSingleInstance () ;

		// Creates the instance handler
		BOOL Create ( LPCTSTR aName ) ;

		// Callback when the instance is woken up by another
		virtual void WakeUp ( LPCTSTR aCommandLine ) const ;
			
	private :

		// The implementation handler, "pimple"
		class CSingleInstanceImpl* mImplementor ;
} ;

//------------------------------------------------------------------------------

#endif // __SINGLEINSTANCE_H__

//------------------------------------------------------------------------------

