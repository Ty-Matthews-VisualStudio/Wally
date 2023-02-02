//------------------------------------------------------------------------------
// Name:    singleinstance.cpp
// Author:  Justin Hallet
// Date:    30/11/2002
//------------------------------------------------------------------------------

// MFC pre-compiled headers
#include "stdafx.h"

// MFC multi-threaded extensions
#include <afxmt.h>

// Single Instance header
#include "singleinstance.h"
#include "MiscFunctions.h"

//------------------------------------------------------------------------------

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // _DEBUG

//------------------------------------------------------------------------------
//---------------------------- SHARED DATA CLASS -------------------------------
//------------------------------------------------------------------------------

class CSingleInstanceData
// Name:        CSingleInstanceData
// Parent:		 N/A
// Description: Manages shared data between applications, could be anything,
//              but for this case it is the "command line arguments"
{
	public :

		// Constructor/Destructor
		CSingleInstanceData ( LPCTSTR aName ) ;
		virtual ~CSingleInstanceData () ;

		// Getter/Setter
		void SetValue ( LPCTSTR aData ) ;
		CString GetValue () const ;

	private :

		enum { MAX_DATA = 32768 } ;

		// Data pointer
		LPTSTR  mData ;

		// File handle
		HANDLE  mMap ;

		// Acces mutex
		CMutex* mMutex ;
} ;

//------------------------------------------------------------------------------

CSingleInstanceData::CSingleInstanceData ( LPCTSTR aName ) 
// Name:        CSingleInstanceData
// Type:        Constructor
// Description: Create shared memory mapped file or create view of it
{	

	// Build names
	CString lFileName = aName ;
	lFileName += _T("-Data-Mapping-File") ;

	CString lMutexName = aName ;
	lMutexName += _T("-Data-Mapping-Mutex") ;

	// Create mutex, global scope
	mMutex = new CMutex ( FALSE, lMutexName ) ;	

	// Create file mapping
	mMap = CreateFileMapping((HANDLE)0xFFFFFFFF,
		NULL,
		PAGE_READWRITE,
		0,
		sizeof(TCHAR) * MAX_DATA,
		lFileName);

	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		// Close handle
		CloseHandle(mMap);

		// Open existing file mapping
		mMap = OpenFileMapping(FILE_MAP_WRITE, FALSE, lFileName);
	}

	// Set up data mapping
	mData = (LPTSTR)MapViewOfFile(mMap, FILE_MAP_WRITE, 0, 0, sizeof(TCHAR) * MAX_DATA);

	// Lock file
	CSingleLock lLock(mMutex, TRUE);
	if (lLock.IsLocked())
	{
		// Clear data
		ZeroMemory(mData, sizeof(TCHAR) * MAX_DATA);
	}
}

//------------------------------------------------------------------------------

CSingleInstanceData::~CSingleInstanceData () 
// Name:        ~CSingleInstanceData
// Type:        Destructor
// Description: Close memory mapped file
{
	if ( mMap )
	{
		// Unmap data from file
		UnmapViewOfFile ( mData ) ;

		// Close file
		CloseHandle ( mMap ) ;
	}
		
	// Clean up mutex
	if ( mMutex )
	{
		delete mMutex ;
	}
}

//------------------------------------------------------------------------------

void CSingleInstanceData::SetValue ( LPCTSTR aData )		
// Name:        SetValue 
// Type:        Function
// Description: Set value in memory mapped file
{
	// Lock file
	CSingleLock lLock ( mMutex, TRUE ) ;
	if ( lLock . IsLocked () )
	{
		// Copy data
		_tcscpy_s ( mData, sizeof(TCHAR) * MAX_DATA, aData ) ;
	}
}


//------------------------------------------------------------------------------

CString CSingleInstanceData::GetValue () const
// Name:        GetValue 
// Type:        Function
// Description: Get value from memory mapped file
{
	// Lock file
	CSingleLock lLock ( mMutex, TRUE ) ;
	if ( lLock . IsLocked () )
	{
		// Return the data
		return mData ;
	}
	// Not locked to return empty data
	return _T("") ;
}

//------------------------------------------------------------------------------
//---------------------------- IMPLEMENTOR CLASS -------------------------------
//------------------------------------------------------------------------------

class CSingleInstanceImpl
// Name:        CSingleInstanceImpl
// Parent:		 N/A
// Description: Implementation to manage single instance activation
{
	public :
		CString m_sNamedMutex;
		
		// Constructor/Destructor
		CSingleInstanceImpl ( const CSingleInstance* aOwner ) ;
		virtual ~CSingleInstanceImpl () ;

		// Creates the instance handler
		BOOL Create ( LPCTSTR aName ) ;

		// Sleeping thread waiting for activation
		static UINT Sleeper ( LPVOID aObject ) ;

	public :

		// Events to singal new instance, and kill thread
		CEvent* mEvent;
		CEvent* mSignal;
		CEvent* mReturn;
		CSingleInstanceData* mData;
		
		// Owner, so we can get at virtual callbacks
		const CSingleInstance* mOwner ;
} ;

//------------------------------------------------------------------------------

CSingleInstanceImpl::CSingleInstanceImpl ( const CSingleInstance* aOwner)
: mOwner  ( aOwner ),
	mEvent  ( NULL ),
	mSignal ( NULL ),
	mReturn(NULL),
	mData (NULL)	
// Name:        CSingleInstanceImpl 
// Type:        Constructor
// Description: Initialise member attributes
{
	
}

//------------------------------------------------------------------------------

CSingleInstanceImpl::~CSingleInstanceImpl ()
// Name:        ~CSingleInstanceImpl 
// Type:        Destructor
// Description: Signal event to end thread, wait for thread to signal back, cleanup.
{
	// If event and signal exist
	if ( mEvent && mSignal )
	{
		// Set signal event to allow thread to exit
		if ( mSignal -> PulseEvent () )
		{
			// Wait for thread to start exiting
			CSingleLock lWaitForEvent ( mEvent, TRUE ) ;
		}

		// Close all open handles
		delete mEvent ;
		delete mSignal ;
	}
	if (mReturn)
	{
		delete mReturn;
	}	
	if ( mData )
	{
		delete mData ;
	}
}


//------------------------------------------------------------------------------

UINT CSingleInstanceImpl::Sleeper ( LPVOID aObject )
// Name:        Sleeper
// Type:        Thread function
// Description: Sleep on events, wake and activate application or wake and quit.
{
	// Get single instance pointer
	CSingleInstanceImpl* lSingleInstanceImpl = (CSingleInstanceImpl*) aObject ;

	// Build event handle array
	CSyncObject* lEvents [] = 
	{ 
		lSingleInstanceImpl -> mEvent, 
		lSingleInstanceImpl -> mSignal 
	} ;

	try
	{
		class MutexRemove
		{
		public:
			CString m_sName;
			MutexRemove(CString sName)
			{
				m_sName = sName;
				boost::interprocess::permissions allow_all;
				allow_all.set_unrestricted();
				boost::interprocess::named_mutex::remove(m_sName.GetBuffer());
				boost::interprocess::named_mutex mutex(::boost::interprocess::create_only, m_sName.GetBuffer(), allow_all);
			}
			~MutexRemove()
			{
				boost::interprocess::named_mutex::remove(m_sName.GetBuffer());
			}
		};

		MutexRemove mRemove(lSingleInstanceImpl->m_sNamedMutex);

		// Forever
		BOOL lForever = TRUE;
		while (lForever)
		{
			CMultiLock lWaitForEvents(lEvents, sizeof(lEvents) / sizeof(CSyncObject*));

			// Goto sleep until one of the events signals, zero CPU overhead
			DWORD lResult = lWaitForEvents.Lock(INFINITE, FALSE);

			// What signaled, 0 = event, another instance started
			if (lResult == WAIT_OBJECT_0 + 0)
			{
				if (lSingleInstanceImpl->mOwner)
				{
					// Wake up the owner with the data (last command line)
					lSingleInstanceImpl->mOwner->WakeUp(lSingleInstanceImpl->mData->GetValue());

					// The above call to WakeUp() is serial, so we will block waiting for its return.
					// This is how we prevent the other threads from stomping on CWallyApp::m_sSingleInstanceCommandLine
					// Once WakeUp() is done, we know the file has been opened.  Set this event to tell the
					// calling thread to continue and release the global mutex lock
					lSingleInstanceImpl->mReturn->SetEvent();
				}
			}
			// 1 = signal, time to exit the thread
			else if (lResult == WAIT_OBJECT_0 + 1)
			{
				// Break the forever loop
				lForever = FALSE;
			}

			lWaitForEvents.Unlock();
		}

		// Set event to say thread is exiting
		lSingleInstanceImpl->mEvent->SetEvent();
	}
	catch (boost::interprocess::interprocess_exception& ex)
	{		
	}
	return 0 ;
}

//------------------------------------------------------------------------------


BOOL CSingleInstanceImpl::Create ( LPCTSTR aName ) 
// Name:        Create
// Type:        Function
// Description: Create event, if new then create thread, if existing signal it and quit.
{
	// Default error condition
	BOOL lResult = FALSE ;

	// Create shared data
	mData = new CSingleInstanceData ( aName ) ;

	// Create event name
	CString lEventName = aName ;
	lEventName += _T("-Event") ;

	// Create named event, global scope
	mEvent = new CEvent ( FALSE, FALSE, lEventName  ) ;
	DWORD lLastError = GetLastError ();

	lEventName = aName;
	lEventName += _T("-Return-Event");
	mReturn = new CEvent(FALSE, FALSE, lEventName);	
	
	m_sNamedMutex = aName;
	m_sNamedMutex += _T("-Boost-Named-Mutex");

	// Check we have a handle to a valid event
	if ( mEvent ) 
	{
		// Check last error status
		if ( lLastError == ERROR_ALREADY_EXISTS )
		{
			try
			{	
				// Open the named mutex
				boost::interprocess::permissions allow_all;
				allow_all.set_unrestricted();
				boost::interprocess::named_mutex mutex(::boost::interprocess::open_or_create, m_sNamedMutex.GetBuffer(), allow_all);
				
				while (1)
				{
					boost::interprocess::scoped_lock<::boost::interprocess::named_mutex> lock(mutex, boost::interprocess::try_to_lock);
					if (lock)
					{
						// Set command line data
						mData->SetValue(GetCommandLine());

						// Not our event, so an instance is already running, signal thread in other instance to wake up
						if (mEvent->PulseEvent())
						{
							// Close open handles
							delete mEvent;

							// Reset event handle
							mEvent = NULL;
						}

						CSingleLock lWaitForReturn(mReturn);
						// Go to sleep until the return event is signalled then reset it
						DWORD lResult = lWaitForReturn.Lock(INFINITE);
						mReturn->ResetEvent();						
						break;
					}
					else
					{
						Sleep(100);
					}
				}				
			}
			catch (boost::interprocess::interprocess_exception& ex)
			{
			}
		}
		else
		{			
			// Create event of thread syncronization, nameless local scope
			mSignal = new CEvent;
			if (mSignal)
			{
				// Create thread
				AfxBeginThread(Sleeper, this);

				// Set pass condition
				lResult = TRUE;
			}
			
		}
	}
	return lResult ;
}

//------------------------------------------------------------------------------
//---------------------------- INTERFACE CLASS ---------------------------------
//------------------------------------------------------------------------------

CSingleInstance::CSingleInstance ()
// Name:        CSingleInstance
// Type:        Constructor
// Description: Create implementor
{
	// Create the implementor class
	mImplementor = new CSingleInstanceImpl ( this ) ;
}

//------------------------------------------------------------------------------

CSingleInstance::~CSingleInstance () 
// Name:        ~CSingleInstance
// Type:        Destructor
// Description: Delete implementor
{
	// If implementor exists delete it
	if ( mImplementor )
	{
		delete mImplementor ;
	}
}

//------------------------------------------------------------------------------

BOOL CSingleInstance::Create ( LPCTSTR aName ) 
// Name:        Create
// Type:        Function
// Description: Pass message to implementor
{
	// Pass message on to the implementor
	if ( mImplementor )
	{
		BOOL bR = mImplementor -> Create ( aName ) ;
		return bR;
	}
	return FALSE ;
} 

//------------------------------------------------------------------------------

void CSingleInstance::WakeUp ( LPCTSTR aCommandLine ) const
// Name:        WakeUp
// Type:        Callback function
// Description: Default action, find main application window and make foreground.
{
	// Find application and main window
	CWinApp* lApplication = AfxGetApp () ;
	if ( lApplication && lApplication -> m_pMainWnd )
	{
		// Get window handle
		HWND lWnd = lApplication -> m_pMainWnd -> GetSafeHwnd () ;

		if ( lWnd )
		{
			// Make main window foreground, flashy, flashy time
			SetForegroundWindow ( lWnd ) ;	
		}
	}
}


//------------------------------------------------------------------------------

