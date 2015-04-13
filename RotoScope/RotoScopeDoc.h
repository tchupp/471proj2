/*! \file RotoScopeDoc.h
 *
 * Header file for the RotoScope document class
 * \author Charles B. Owen
 */

#pragma once

#include <vector>

#include "video/MovieSource.h"
#include "video/MovieMake.h"
#include "audio/DirSoundSource.h"
#include "graphics/GrImage.h"

/*! \mainpage RotoScope Starter Application
 *
 * The Rotoscope application is a simple program that allows you to 
 * load frames of video, modify them in interesting ways, and save 
 * them to a new video file. Rotoscoping is an animation technique where 
 * frames of a movie are advanced one by one and edited by a user. 
 */

//! Rotoscope document class 
/*! RotoScope document class */
class CRotoScopeDoc : public CDocument
{
public:
	CRotoScopeDoc();
	virtual ~CRotoScopeDoc();
	DECLARE_DYNCREATE(CRotoScopeDoc)

	void Mouse(int p_x, int p_y);

	/*! Get the image we are editing 
     * \returns Constant pointer to image we are editing */
	const CGrImage& GetImage() const { return mImage; }

	// Implementation
public:
#ifdef _DEBUG
	virtual void AssertValid() const override;
	virtual void Dump(CDumpContext& dc) const override;
#endif

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

private:
	void MessagePump();
	void CreateOneFrame();
	void DrawImage();

	void SaveMovieData(IXMLDOMDocument* xmlDoc, IXMLDOMNode* inNode);
	void XmlLoadMovie(IXMLDOMNode* xml);
	void XmlLoadFrame(IXMLDOMNode* xml);

	int mMovieFrame;
	CMovieSource mMovieSource; // Any source movie
	CMovieMake mMovieMake; // Any movie we are outputing to
	CDirSoundSource mBackAudio; // Any background audio?

	CGrImage mInitial;
	CGrImage mImage; // Current image being displayed
	std::vector<short> mAudio; // Current audio to be written to the output
	std::vector<std::list<CPoint>> mDraw;

	// Task 1
	bool mFindSaber;

	// Task 2
	bool mRotate;
	double mRotationRad;

	// Task 3
	bool mReplaceHead;
	CGrImage mReplacementHead;
	CPoint mBirdPos;

	// Project 2
	bool mShowOutline;

	afx_msg void OnFramesCreateoneframe();
	afx_msg void OnFramesWriteoneframe();
	afx_msg void OnFramesWritethencreateoneframe();
	afx_msg void OnFramesWritethencreateonesecond();
	afx_msg void OnFramesWritethencreateremaining();

	afx_msg void OnMoviesOpensourcemovie();
	afx_msg void OnMoviesOpenoutputmovie();
	afx_msg void OnMoviesOpenbackgroundaudio();
	afx_msg void OnMoviesClosesourcemovie();
	afx_msg void OnMoviesCloseoutputmovie();
	afx_msg void OnMoviesClosebackgroundaudio();

	afx_msg void OnEditClearframe();

	afx_msg void OnUpdateMoviesClosesourcemovie(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMoviesCloseoutputmovie(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMoviesClosebackgroundaudio(CCmdUI* pCmdUI);

	afx_msg void OnUpdateFramesWritethencreateoneframe(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFramesWriteoneframe(CCmdUI* pCmdUI);

	virtual BOOL OnNewDocument() override;
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName) override;
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName) override;
};
