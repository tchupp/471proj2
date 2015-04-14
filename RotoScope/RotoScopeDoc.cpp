// RotoScopeDoc.cpp : implementation of the CRotoScopeDoc class
//

#include "stdafx.h"
#include "RotoScope.h"

#include "RotoScopeDoc.h"
#include "ImageProcessing.h"
#include "xmlhelp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include "ImageTransformations.h"

using namespace std;

// Simple inline function to range bound a double and cast to a short.
inline short ShortRange(double a)
{
	return a > 32767 ? 32767 : (a < -32768. ? -32768 : short(a));
}

// CRotoScopeDoc

IMPLEMENT_DYNCREATE(CRotoScopeDoc, CDocument)

BEGIN_MESSAGE_MAP(CRotoScopeDoc, CDocument)
	ON_COMMAND(ID_MOVIES_OPENSOURCEMOVIE, &CRotoScopeDoc::OnMoviesOpensourcemovie)
	ON_COMMAND(ID_MOVIES_OPENOUTPUTMOVIE, &CRotoScopeDoc::OnMoviesOpenoutputmovie)
	ON_COMMAND(ID_FRAMES_CREATEONEFRAME, &CRotoScopeDoc::OnFramesCreateoneframe)
	ON_COMMAND(ID_FRAMES_WRITEONEFRAME, &CRotoScopeDoc::OnFramesWriteoneframe)
	ON_UPDATE_COMMAND_UI(ID_FRAMES_WRITEONEFRAME, &CRotoScopeDoc::OnUpdateFramesWriteoneframe)
	ON_COMMAND(ID_MOVIES_CLOSESOURCEMOVIE, &CRotoScopeDoc::OnMoviesClosesourcemovie)
	ON_UPDATE_COMMAND_UI(ID_MOVIES_CLOSESOURCEMOVIE, &CRotoScopeDoc::OnUpdateMoviesClosesourcemovie)
	ON_COMMAND(ID_MOVIES_CLOSEOUTPUTMOVIE, &CRotoScopeDoc::OnMoviesCloseoutputmovie)
	ON_UPDATE_COMMAND_UI(ID_MOVIES_CLOSEOUTPUTMOVIE, &CRotoScopeDoc::OnUpdateMoviesCloseoutputmovie)
	ON_COMMAND(ID_FRAMES_WRITETHENCREATEONEFRAME, &CRotoScopeDoc::OnFramesWritethencreateoneframe)
	ON_UPDATE_COMMAND_UI(ID_FRAMES_WRITETHENCREATEONEFRAME, &CRotoScopeDoc::OnUpdateFramesWritethencreateoneframe)
	ON_COMMAND(ID_FRAMES_WRITETHENCREATEONESECOND, &CRotoScopeDoc::OnFramesWritethencreateonesecond)
	ON_COMMAND(ID_FRAMES_WRITETHENCREATEREMAINING, &CRotoScopeDoc::OnFramesWritethencreateremaining)
	ON_COMMAND(ID_MOVIES_OPENBACKGROUNDAUDIO, &CRotoScopeDoc::OnMoviesOpenbackgroundaudio)
	ON_COMMAND(ID_MOVIES_CLOSEBACKGROUNDAUDIO, &CRotoScopeDoc::OnMoviesClosebackgroundaudio)
	ON_UPDATE_COMMAND_UI(ID_MOVIES_CLOSEBACKGROUNDAUDIO, &CRotoScopeDoc::OnUpdateMoviesClosebackgroundaudio)
	ON_COMMAND(ID_EDIT_CLEARFRAME, &CRotoScopeDoc::OnEditClearframe)
	ON_COMMAND(ID_EFFECTS_OUTLINE, &CRotoScopeDoc::OnEffectsOutline)
	ON_UPDATE_COMMAND_UI(ID_EFFECTS_OUTLINE, &CRotoScopeDoc::OnUpdateEffectsOutline)
	ON_COMMAND(ID_EFFECTS_REPLACEHEA, &CRotoScopeDoc::OnEffectsReplacehea)
	ON_UPDATE_COMMAND_UI(ID_EFFECTS_REPLACEHEA, &CRotoScopeDoc::OnUpdateEffectsReplacehea)
	ON_COMMAND(ID_EFFECTS_REPLACEMENTHEAD, &CRotoScopeDoc::OnEffectsReplacementhead)
	ON_COMMAND(ID_EFFECTS_REPLACEBACKGROUND, &CRotoScopeDoc::OnEffectsReplacebackground)
	ON_UPDATE_COMMAND_UI(ID_EFFECTS_REPLACEBACKGROUND, &CRotoScopeDoc::OnUpdateEffectsReplacebackground)
	ON_COMMAND(ID_EFFECTS_OPENBACKGROUNDIMAGE, &CRotoScopeDoc::OnEffectsOpenbackgroundimage)
END_MESSAGE_MAP()

//! Constructor for the document class.
CRotoScopeDoc::CRotoScopeDoc()
{
	CoInitialize(nullptr);

	mMovieMake.SetProfileName(L"profile720p.prx");

	// Set the image size to an initial default value and black.
	mImage.SetSize(640, 480);
	mImage.Fill(0, 0, 0);
	mMovieFrame = 0;

	// Task 1
	mFindSaber = false;

	// Task 2
	mRotate = false;
	mRotationRad = 0;

	// Task 3
	mReplacementHead.LoadFile(L"bird.png");
	mReplaceHead = false;

	// Project 2
	mShowOutline = false;
	mReplaceBackground = false;
}

//! Destructor
CRotoScopeDoc::~CRotoScopeDoc()
{
	CoUninitialize();
}

//! Function that is called when a new document is created
//! \returns true if successful
BOOL CRotoScopeDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument()) return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}


// CRotoScopeDoc diagnostics

#ifdef _DEBUG
void CRotoScopeDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CRotoScopeDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG



///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Source movie management
//
///////////////////////////////////////////////////////////////////////////////////////////////////


//
// Name :         CRotoScopeDoc::OnMoviesOpensourcemovie()
// Description :  Open a video input source.
//

void CRotoScopeDoc::OnMoviesOpensourcemovie()
{
	static TCHAR BASED_CODE szFilter[] = TEXT("Video Files (*.avi;*.wmv;*.asf)|*.avi; *.wmv; *.asf|All Files (*.*)|*.*||");

	CFileDialog dlg(TRUE, TEXT(".avi"), nullptr, 0, szFilter, nullptr);
	if (dlg.DoModal() != IDOK) return;

	if (!mMovieSource.Open(dlg.GetPathName())) return;
}

void CRotoScopeDoc::OnMoviesClosesourcemovie()
{
	mMovieSource.Close();
}

void CRotoScopeDoc::OnUpdateMoviesClosesourcemovie(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(mMovieSource.IsOpen());
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Background music management
//
///////////////////////////////////////////////////////////////////////////////////////////////////

//
// Name :        CRotoScopeDoc::OnMoviesOpenbackgroundaudio()
// Description : Opens an audio file we can use as a background music source.
//

void CRotoScopeDoc::OnMoviesOpenbackgroundaudio()
{
	static TCHAR BASED_CODE szFilter[] = TEXT("Audio Files (*.wav;*.mp3)|*.wav; *.mp3|All Files (*.*)|*.*||");

	CFileDialog dlg(TRUE, TEXT(".wav"), nullptr, 0, szFilter, nullptr);
	if (dlg.DoModal() != IDOK) return;

	if (!mBackAudio.Open(dlg.GetPathName())) return;
}

void CRotoScopeDoc::OnMoviesClosebackgroundaudio()
{
	mBackAudio.Close();
}

void CRotoScopeDoc::OnUpdateMoviesClosebackgroundaudio(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(mBackAudio.IsOpen());
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Output movie management
//
///////////////////////////////////////////////////////////////////////////////////////////////////


//
// Name :        CRotoScopeDoc::OnMoviesOpenoutputmovie()
// Description : Open an output movie.
//

void CRotoScopeDoc::OnMoviesOpenoutputmovie()
{
	static TCHAR BASED_CODE szFilter[] = TEXT("ASF Files (*.asf)|*.asf|All Files (*.*)|*.*||");

	CFileDialog dlg(FALSE, TEXT(".asf"), nullptr, 0, szFilter, nullptr);
	if (dlg.DoModal() != IDOK) return;

	if (!mMovieMake.Open(dlg.GetPathName())) return;

	// Make CImage match the size of the output movie.
	mImage.SetSize(mMovieMake.GetWidth(), mMovieMake.GetHeight());

	// Make the audio buffer match the storage requirement for one video frame
	mAudio.clear();
	mAudio.resize(size_t(mMovieMake.GetSampleRate() / mMovieMake.GetFPS()) * mMovieMake.GetNumChannels());

	mMovieFrame = 0;

	UpdateAllViews(nullptr);
}

void CRotoScopeDoc::OnMoviesCloseoutputmovie()
{
	mMovieMake.Close();
}

void CRotoScopeDoc::OnUpdateMoviesCloseoutputmovie(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(mMovieMake.IsOpen());
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Creating and writing video frames.
//
///////////////////////////////////////////////////////////////////////////////////////////////////


//
// Name :         CRotoScopeDoc::OnFramesCreateoneframe()
// Description :  Menu handler for Frame/Create One Frame menu option.
//                This will call the function that creates one frame.
//

void CRotoScopeDoc::OnFramesCreateoneframe()
{
	CreateOneFrame();
	UpdateAllViews(nullptr);
}


//
// Name :        CRotoScopeDoc::CreateOneFrame()
// Description : This function creates a frame for display.
//               This is a demonstration of how we can read frames and audio 
//               and create an output frame.
//

void CRotoScopeDoc::CreateOneFrame()
{
	//
	// Clear our frame first
	//

	mImage.Fill(0, 0, 0);

	//
	// Read any image from source video?
	//

	if (mMovieSource.HasVideo())
	{
		// Important:  Don't read directly into mImage.  Our source may be a 
		// different size!  I'm reading into a temporary image, then copying
		// the data over.

		if (mMovieSource.ReadImage(mInitial))
		{
			DrawImage();
			if (mRotate) mRotationRad += 2 * PI / mMovieSource.GetFrameCount();
		}
	}

	//
	// Read any audio from the source video?  Note that we read and write the 
	// audio associated with one frame of video.
	//
	
	vector<short> audio;
	if (mMovieSource.HasAudio() && mMovieSource.ReadAudio(audio))
	{
		// The problem is that the input audio may not be in the same format
		// as the output audio.  For example, we may have a different number of 
		// audio frames for a given video frame.  Also, the channels may be
		// different.  I'll assume my output is stereo here, since I created a
		// stereo profile, but the input may be mono.

		if (mMovieSource.GetNumChannels() == 2)
		{
			// Easiest, both are the same.
			// What's the ratio of playback?
			double playrate = double(audio.size()) / double(mAudio.size());
			for (unsigned f = 0; f < mAudio.size() / 2; f++)
			{
				int srcframe = int(playrate * f);
				mAudio[f * 2] = audio[srcframe * 2];
				mAudio[f * 2 + 1] = audio[srcframe * 2 + 1];
			}
		}
		else
		{
			// Mono into stereo
			double playrate = double(2. * audio.size()) / double(mAudio.size());
			for (unsigned f = 0; f < mAudio.size() / 2; f++)
			{
				int srcframe = int(playrate * f);
				mAudio[f * 2] = audio[srcframe];
				mAudio[f * 2 + 1] = audio[srcframe];
			}
		}
	}
	else
	{
		// If there is no audio to read, set to silence.
		for (unsigned int i = 0; i < mAudio.size(); i++) mAudio[i] = 0;
	}

	//
	// Is there any background audio to mix in?
	//

	if (mBackAudio.IsOpen())
	{
		for (auto i = mAudio.begin(); i != mAudio.end();)
		{
			short audioC[2];
			mBackAudio.ReadStereoFrame(audioC);

			*i = ShortRange(*i + audioC[0] * 0.3);
			++i;
			*i = ShortRange(*i + audioC[1] * 0.3);
			++i;
		}
	}
}

void CRotoScopeDoc::SaveMovieData(IXMLDOMDocument* xmlDoc, IXMLDOMNode* inNode)
{
	for (auto frame = 0; frame < int(mDraw.size()); frame++)
	{
		// Create an XML node for the frame
		CComPtr<IXMLDOMNode> node;
		CComPtr<IXMLDOMElement> pe;
		xmlDoc->createElement(L"frame", &pe);

		// Add an attribute for the frame number
		pe->setAttribute(L"num", CComVariant(frame));

		// Append the node to the node we are nested inside.
		inNode->appendChild(pe, &node);
		pe.Release();

		//
		// Now save the point data for the frame
		//

		for (auto p = mDraw[frame].begin(); p != mDraw[frame].end(); ++p)
		{
			// Create an XML node for the point
			xmlDoc->createElement(L"point", &pe);

			// Add attributes for the point
			pe->setAttribute(L"x", CComVariant(p->x));
			pe->setAttribute(L"y", CComVariant(p->y));

			// Append the node to the node we are nested inside.
			CComPtr<IXMLDOMNode> nodePoint;
			node->appendChild(pe, &nodePoint);
			pe.Release();
			nodePoint.Release();
		}

		// When done, release <frame> the node
		node.Release();
	}
}

//
// Most of the following are various menu options for video processing designed to make
// the user interface easier.
//

void CRotoScopeDoc::OnFramesWriteoneframe()
{
	mMovieMake.WriteImage(mImage);
	mMovieMake.WriteAudio(mAudio);
	mMovieFrame++;
}

void CRotoScopeDoc::OnUpdateFramesWriteoneframe(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(mMovieMake.IsOpen());
}

void CRotoScopeDoc::OnFramesWritethencreateoneframe()
{
	OnFramesWriteoneframe();
	OnFramesCreateoneframe();
}

void CRotoScopeDoc::OnUpdateFramesWritethencreateoneframe(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(mMovieMake.IsOpen());
}

void CRotoScopeDoc::OnFramesWritethencreateonesecond()
{
	if (mMovieSource.IsOpen() && !mMovieSource.HasAudio()) return;

	// Do the creation operation for one entire second
	for (int i = 0; i < int(mMovieMake.GetFPS() + 0.5); i++)
	{
		OnFramesWriteoneframe();
		OnFramesCreateoneframe();
		if (mMovieSource.IsOpen() && !mMovieSource.HasVideo()) break;

		MessagePump();
	}
}

//
// Name :        CRotoScopeDoc::OnFramesWritethencreateremaining()
// Description : This is an example of a loop that will consume all remaining 
//               source video.
//

void CRotoScopeDoc::OnFramesWritethencreateremaining()
{
	// This only makes sense if there's an input video
	if (!mMovieSource.IsOpen() || !mMovieSource.HasAudio()) return;

	// Do the creation operation for one entire second
	for (int i = 0; ; i++)
	{
		OnFramesWriteoneframe();
		OnFramesCreateoneframe();
		if (!mMovieSource.HasVideo()) break;

		MessagePump();
	}

	AfxMessageBox(TEXT("All Done"));
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
// The mouse-based user interface.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

//! This function is called by the view class when the mouse is
//! pressed or moved on the screen.
/*! It sets the associated pixel in the image to red right now. You'll 
 * likely make it do something different
 * \param p_x X location in image
 * \param p_y Y location in image */

void CRotoScopeDoc::Mouse(int p_x, int p_y)
{
	// We need to convert screen locations to image locations
	auto x = p_x; // No problem there.
	auto y = mImage.GetHeight() - p_y - 1; // Just invert it.

	list<CPoint> empty;
	while (int(mDraw.size()) < mMovieFrame + 1)
	{
		mDraw.push_back(empty);
	}

	mDraw[mMovieFrame].push_back(CPoint(x, y));

	DrawImage();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Utility functions...
//
///////////////////////////////////////////////////////////////////////////////////////////////////

//
// Name :        CRotoScopeDoc::MessagePump(void)
// Description : This function is a Windows message pump. It allows message processing to 
//               proceed while your program is in some loop.  This is so the menus and such
//               will continue to work.  Be very careful.  Usually you pop up a modal 
//               dialog box while this is going on to prevent accessing anything other than
//               a cancel button.  I'm not doing that, so be careful about what you hit.
//
void CRotoScopeDoc::MessagePump()
{
	// Allow any messages to be processed
	MSG msg;
	while (::PeekMessage(&msg, nullptr, NULL, NULL, PM_NOREMOVE))
	{
		AfxGetThread()->PumpMessage();
	}
}

void CRotoScopeDoc::DrawImage()
{
	// Write image from m_initial into the current image
	for (auto r = 0; r < mImage.GetHeight() && r < mInitial.GetHeight(); r++)
	{
		for (auto c = 0; c < mImage.GetWidth() && c < mInitial.GetWidth(); c++) 
		{
			mImage[r][c * 3 + 0] = mInitial[r][c * 3 + 0];
			mImage[r][c * 3 + 1] = mInitial[r][c * 3 + 1];
			mImage[r][c * 3 + 2] = mInitial[r][c * 3 + 2];
		}
	}

	// Write any saved drawings into the frame
	if (mMovieFrame < int(mDraw.size()))
	{
		for (auto i = mDraw[mMovieFrame].begin(); i != mDraw[mMovieFrame].end(); ++i)
		{
			mImage.Set(i->x, i->y, 255, 0, 0);
		}
	}

	if (mFindSaber)
	{
		auto topPoint = FindLightsaberTop(mInitial, 240, 290, 175, 300);
		auto botPoint = FindLightsaberHandle(mInitial, 310, 390, 150, 225);
		mImage.DrawLine(botPoint.x, botPoint.y, topPoint.x, topPoint.y, 255);
	}

	if (mReplaceHead) mImage.AddForegroundImage(mReplacementHead, FindTopOfHead(mImage, 200, 600, 100, 400));
	
	if (mReplaceBackground) mImage.AddBackgroundImage(mReplacementBackground);

	if (mRotate) RotateImage(mImage, mRotationRad);

	if (mShowOutline) PrewittOperation(mImage, false);

	UpdateAllViews(nullptr);
}

BOOL CRotoScopeDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	//
	// Create an XML document
	//

	// CComPtr<> is a "smart" pointer. It takes care of reference counting on COM objects for
	// us so they know when to delete themselves.
	CComPtr<IXMLDOMDocument> xmlDoc;
	auto hr = CoCreateInstance(CLSID_DOMDocument, nullptr, CLSCTX_INPROC_SERVER, IID_IXMLDOMDocument, (void**)&xmlDoc);
	if (hr != S_OK)
	{
		AfxMessageBox(L"Unable to create XML document");
		return false;
	}

	// This is a pointer we'll use for nodes in our XML
	CComPtr<IXMLDOMNode> node;

	// Create a processing instruction targeted for xml.
	// This creates the line:  <?xml version='1.0'>
	CComPtr<IXMLDOMProcessingInstruction> pi;
	xmlDoc->createProcessingInstruction(L"xml", L"version='1.0'", &pi);
	xmlDoc->appendChild(pi, &node);
	pi.Release();
	node.Release();

	// Create the root element
	CComPtr<IXMLDOMElement> pe;
	xmlDoc->createElement(L"movie", &pe);
	xmlDoc->appendChild(pe, &node);
	pe.Release();

	SaveMovieData(xmlDoc, node);

	node.Release();

	xmlDoc->save(CComVariant(lpszPathName));
	xmlDoc.Release();

	return TRUE;
}

BOOL CRotoScopeDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	// Clear any previous frame information
	mDraw.clear();

	//
	// Create an XML document
	//

	CComPtr<IXMLDOMDocument> pXMLDoc;
	auto succeeded = SUCCEEDED(CoCreateInstance(CLSID_DOMDocument, nullptr, CLSCTX_INPROC_SERVER,
		IID_IXMLDOMDocument, (void**)&pXMLDoc));
	if (!succeeded)
	{
		AfxMessageBox(L"Failed to create an XML document to use");
		return FALSE;
	}

	// Open the XML document
	VARIANT_BOOL ok;
	succeeded = SUCCEEDED(pXMLDoc->load(CComVariant(lpszPathName), &ok));
	if (!succeeded || ok == VARIANT_FALSE)
	{
		AfxMessageBox(L"Failed to open XML movie file");
		return FALSE;
	}

	//
	// Traverse the XML document in memory!!!!
	// Top level tag is <movie>
	//

	CComPtr<IXMLDOMNode> node;
	pXMLDoc->get_firstChild(&node);
	for (; node != nullptr; NextNode(node))
	{
		// Get the name of the node
		CComBSTR nodeName;
		node->get_nodeName(&nodeName);

		if (nodeName == L"movie")
		{
			XmlLoadMovie(node);
		}
	}

	return TRUE;
}

void CRotoScopeDoc::XmlLoadMovie(IXMLDOMNode* xml)
{
	// Handle the children of a <movie> tag
	CComPtr<IXMLDOMNode> node;
	xml->get_firstChild(&node);
	for (; node != nullptr; NextNode(node))
	{
		// Get the name of the node
		CComBSTR nodeName;
		node->get_nodeName(&nodeName);

		if (nodeName == L"frame")
		{
			XmlLoadFrame(node);
		}
	}
}

void CRotoScopeDoc::XmlLoadFrame(IXMLDOMNode* xml)
{
	// When this function is called we have a new <frame> tag.
	// We assume we don't skip any tag numbers.
	// Push on an empty frame
	list<CPoint> empty;
	mDraw.push_back(empty);

	// Traverse the children of the <frame> tag
	CComPtr<IXMLDOMNode> node;
	xml->get_firstChild(&node);
	for (; node != nullptr; NextNode(node))
	{
		// Get the name of the node
		CComBSTR nodeName;
		node->get_nodeName(&nodeName);

		// Handle finding a nested <point> tag
		if (nodeName == L"point")
		{
			CPoint point;

			// Get a list of all attribute nodes and the
			// length of that list
			CComPtr<IXMLDOMNamedNodeMap> attributes;
			node->get_attributes(&attributes);
			long len;
			attributes->get_length(&len);

			// Loop over the list of attributes
			for (auto i = 0; i < len; i++)
			{
				// Get attribute i
				CComPtr<IXMLDOMNode> attrib;
				attributes->get_item(i, &attrib);

				// Get the name of the attribute
				CComBSTR name;
				attrib->get_nodeName(&name);

				// Get the value of the attribute.  A CComVariant is a variable
				// that can have any type. It loads the attribute value as a
				// string (UNICODE), but we can then change it to an integer 
				// (VT_I4) or double (VT_R8) using the ChangeType function 
				// and then read its integer or double value from a member variable.
				CComVariant value;
				attrib->get_nodeValue(&value);

				if (name == "x")
				{
					value.ChangeType(VT_I4);
					point.x = value.intVal;
				}
				else if (name == "y")
				{
					value.ChangeType(VT_I4);
					point.y = value.intVal;
				}
			}

			// When we've pulled the x,y values from the
			// tag, push it onto the end of our list of 
			// points.
			mDraw.back().push_back(point);
		}

		// Handle finding a nested <point> tag
		if (nodeName == L"line")
		{
			CPoint point1;
			CPoint point2;

			// Get a list of all attribute nodes and the
			// length of that list
			CComPtr<IXMLDOMNamedNodeMap> attributes;
			node->get_attributes(&attributes);
			long len;
			attributes->get_length(&len);

			// Loop over the list of attributes
			for (auto i = 0; i < len; i++)
			{
				// Get attribute i
				CComPtr<IXMLDOMNode> attrib;
				attributes->get_item(i, &attrib);

				// Get the name of the attribute
				CComBSTR name;
				attrib->get_nodeName(&name);

				// Get the value of the attribute.  A CComVariant is a variable
				// that can have any type. It loads the attribute value as a
				// string (UNICODE), but we can then change it to an integer 
				// (VT_I4) or double (VT_R8) using the ChangeType function 
				// and then read its integer or double value from a member variable.
				CComVariant value;
				attrib->get_nodeValue(&value);

				if (name == "x1")
				{
					value.ChangeType(VT_I4);
					point1.x = value.intVal;
				}
				else if (name == "y1")
				{
					value.ChangeType(VT_I4);
					point1.y = value.intVal;
				}
				else if (name == "x2")
				{
					value.ChangeType(VT_I4);
					point2.x = value.intVal;
				}
				else if (name == "y2")
				{
					value.ChangeType(VT_I4);
					point2.y = value.intVal;
				}
			}

			// Handle making a line for the frame
		}
	}
}

void CRotoScopeDoc::OnEditClearframe()
{
	if (mMovieFrame >= 0 && mMovieFrame < int(mDraw.size()))
	{
		mDraw[mMovieFrame].clear();
	}

	DrawImage();
}

void CRotoScopeDoc::OnEffectsOutline()
{
	mShowOutline = !mShowOutline;
}

void CRotoScopeDoc::OnUpdateEffectsOutline(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(mShowOutline);
}

void CRotoScopeDoc::OnEffectsReplacehea()
{
	mReplaceHead = !mReplaceHead;
}

void CRotoScopeDoc::OnUpdateEffectsReplacehea(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(mReplaceHead);
}

void CRotoScopeDoc::OnEffectsReplacementhead()
{
	static TCHAR BASED_CODE szFilter[] = TEXT("Image Files (*.png;*.bmp)|*.png; *.bmp|All Files (*.*)|*.*||");

	CFileDialog dlg(TRUE, TEXT(".png"), nullptr, 0, szFilter, nullptr);
	if (dlg.DoModal() != IDOK) return;

	if (!mReplacementHead.LoadFile(dlg.GetPathName())) return;
}

void CRotoScopeDoc::OnEffectsReplacebackground()
{
	mReplaceBackground = !mReplaceBackground;
}

void CRotoScopeDoc::OnUpdateEffectsReplacebackground(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(mReplaceBackground);
}

void CRotoScopeDoc::OnEffectsOpenbackgroundimage()
{
	static TCHAR BASED_CODE szFilter[] = TEXT("Image Files (*.png;*.bmp)|*.png; *.bmp|All Files (*.*)|*.*||");

	CFileDialog dlg(TRUE, TEXT(".png"), nullptr, 0, szFilter, nullptr);
	if (dlg.DoModal() != IDOK) return;

	if (!mReplacementBackground.LoadFile(dlg.GetPathName())) return;
}
