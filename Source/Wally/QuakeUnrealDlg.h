// QuakeUnrealDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CConvertQuakeUnrealDlg dialog

class CConvertQuakeUnrealDlg : public CDialog
{
// Construction
public:
	CConvertQuakeUnrealDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CConvertQuakeUnrealDlg)
	enum { IDD = IDD_UNREAL_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConvertQuakeUnrealDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CConvertQuakeUnrealDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
