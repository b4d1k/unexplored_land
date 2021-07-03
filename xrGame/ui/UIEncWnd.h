// skif : (разделы в ПДА) - раздел "Энциклопедия"

#pragma once
#include "UIWindow.h"
#include "UIWndCallback.h"

class CUIStatic;
class CUIXml;
class CUIProgressBar;
class CUIFrameLineWnd;
class CUIFrameWindow;
class CUITextWnd;
class CUIListBox;
class CUICharacterInfo;
class CUIScrollView;

class CUIEncWnd : public CUIWindow, public CUIWndCallback
{
private:
	typedef CUIWindow	inherited;

	CUIFrameWindow*		m_background;
	CUIFrameWindow*		m_down_background;

	CUITextWnd*			m_center_caption;
	CUIListBox*			m_main_list;
	CUIListBox*			m_items_list;
	CUIStatic*			m_icon;
	CUITextWnd*			m_descr;
	
	/*xr_vector<shared_str> artefact_articles;
	xr_vector<shared_str> level_articles;
	xr_vector<shared_str> anomaly_articles;
	xr_vector<shared_str> monster_articles;
	xr_vector<shared_str> faction_articles;
	xr_vector<shared_str> note_articles;*/

public:
						CUIEncWnd			();
	virtual				~CUIEncWnd			();

	virtual void		SendMessage				(CUIWindow *pWnd, s16 msg, void *pData);
	virtual void 		Show					(bool status);
	virtual void		ResetAll				();

			void		Init					();
			
			xr_vector<shared_str> 		FillArray				(LPCSTR str);
			void 						FillItemsList			(u8 chapter);
			void 						FillArticleInfo			(u8 chapter);

};