// ChildFrm.h : interface of the CChildFrame class
//


#pragma once


class CChildFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CChildFrame)
public:
	CChildFrame();

	// Attributes
public:

	// Operations
public:

	// Overrides
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs) override;

	// Implementation
public:
	virtual ~CChildFrame();
#ifdef _DEBUG
	virtual void AssertValid() const override;
	virtual void Dump(CDumpContext& dc) const override;
#endif

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void ActivateFrame(int nCmdShow = -1) override;
};
