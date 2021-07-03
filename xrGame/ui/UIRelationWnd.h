// skif : (разделы в ПДА) - раздел "Отношения"

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

class CUIRelationWnd : public CUIWindow, public CUIWndCallback
{
private:
	typedef CUIWindow	inherited;

	CUIFrameWindow*		m_background;
	CUIFrameWindow*		m_down_background;
	CUITextWnd*			m_center_caption;
	CUIListBox*			m_rel_list;

public:
						CUIRelationWnd			();
	virtual				~CUIRelationWnd			();

	virtual void		SendMessage				(CUIWindow *pWnd, s16 msg, void *pData);
	virtual void 		Show					(bool status);
			void		UpdateInfo				();
	virtual void		ResetAll				();

			void		Init					();
};