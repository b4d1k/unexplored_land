// skif : (транспорт)

#pragma once
#include "UIWindow.h"
#include "UIDialogWnd.h"
#include "UIWndCallback.h"
#include "UICellItem.h"

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
class CUIDialogWnd;
class CUIDragDropListEx;
class CCar;
class CUICellItem;

class CUICarWnd : public CUIDialogWnd
{
private:
	CUIStatic*		m_background;
	CUI3tButton*	m_btn_close;

	CUI3tButton*	m_btn_add_fuel;
	CUITextWnd*		m_txt_cur_fuel_level;
	CUIProgressBar* m_fuel_progress_back;
	CUIProgressBar* m_fuel_progress_current;
	CUIProgressBar* m_fuel_progress_current_add;
	CUIListBox*		m_fuel_canister_list;


	CUI3tButton*	m_btn_repair;
	CUITextWnd*		m_txt_cur_health;
	CUIProgressBar* m_health_progress_back;
	CUIProgressBar* m_health_progress_current;
	CUIProgressBar* m_health_progress_current_add;
	CUIListBox*		m_toolkits_list;

	CUIDragDropListEx* d_list_actor_items;
	CUIDragDropListEx* d_list_trunk;

	CCar*			owner_car;
	xr_vector<u16>  canister_list;
	xr_vector<u16>  toolkits_list;

	void			BindDragDropListEvents(CUIDragDropListEx* lst);
	//void			move_item_from_actor(u16 what_id);
	//void			move_item_to_actor(u16 what_id);
	void			move_item_from_to(u16 from_id, u16 to_id, u16 what_id);

	bool		xr_stdcall		OnItemDrop(CUICellItem* itm);
	bool		xr_stdcall		OnItemStartDrag(CUICellItem* itm);
	bool		xr_stdcall		OnItemDbClick(CUICellItem* itm);
	bool		xr_stdcall		OnItemSelected(CUICellItem* itm);
	bool		xr_stdcall		OnItemRButtonClick(CUICellItem* itm);
	bool		xr_stdcall		OnItemFocusReceive(CUICellItem* itm);
	bool		xr_stdcall		OnItemFocusLost(CUICellItem* itm);
	bool		xr_stdcall		OnItemFocusedUpdate(CUICellItem* itm);
	
public:
						CUICarWnd();
	virtual				~CUICarWnd();

	virtual void		SendMessage				(CUIWindow* pWnd, s16 msg, void* pData = 0);
	virtual bool		OnKeyboardAction		(int dik, EUIMessages keyboard_action);
			void		Init					(CCar* car);
			void		UpdateInfo();
};