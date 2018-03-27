// HSFoxx.h : main header file for the HSFOXX application
//

#if !defined(AFX_HSFOXX_H__0ED47800_5B67_4A46_81DC_43F411B09D51__INCLUDED_)
#define AFX_HSFOXX_H__0ED47800_5B67_4A46_81DC_43F411B09D51__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CHSFoxxApp:
// See HSFoxx.cpp for the implementation of this class
//

class CHSFoxxApp : public CWinApp
{
public:
	CHSFoxxApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHSFoxxApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CHSFoxxApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HSFOXX_H__0ED47800_5B67_4A46_81DC_43F411B09D51__INCLUDED_)
