// skif : (конвертер для денег)

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
class CUI3tButton;
class CUICharacterInfo;
class CUIScrollView;

class CUIInvMoneyWnd : public CUIWindow, public CUIWndCallback
{
private:
	typedef CUIWindow	inherited;

	CUIStatic*		m_background;
	
public:
						CUIInvMoneyWnd			();
	virtual				~CUIInvMoneyWnd			();
	
		CUITextWnd*		m_actor_money_caption;
		CUI3tButton*	m_btn_100_rub;
		CUI3tButton*	m_btn_1000_rub;
		CUI3tButton*	m_btn_5000_rub;
		CUI3tButton*	m_btn_10000_rub;
		CUI3tButton*	m_btn_50000_rub;
		CUI3tButton*	m_btn_100000_rub;

	virtual void		SendMessage				(CUIWindow *pWnd, s16 msg, void *pData);
	virtual void 		Show					(bool status);
			void		Init					();
			void		UpdateInfo				();
};