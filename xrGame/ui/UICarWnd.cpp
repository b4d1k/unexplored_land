// skif : (транспорт)
#include "stdafx.h"
#include "pch_script.h"
#include "UICarWnd.h"

#include "UIFixedScrollBar.h"
#include "UIXmlInit.h"
#include "UIProgressBar.h"
#include "UIDragDropListEx.h"
#include "UI3tButton.h"
#include "UIFrameLineWnd.h"
#include "UIScrollView.h"
#include "UIHelper.h"
#include "UIListBox.h"
#include "UIDialogHolder.h"
#include "UIInventoryUtilities.h"

#include "ai_object_location.h"
#include "../actor.h"
#include "../ai_space.h"
#include "../alife_simulator.h"

#include "../../xrServerEntities/script_engine.h"
#include "../string_table.h"
#include "ui_base.h"
#include <dinput.h>
#include "../Car.h"
#include "../Inventory.h"
#include "../InventoryOwner.h"
#include "clsid_game.h"
#include "UICellItemFactory.h"
#include "UICellItem.h"

#define  CAR_WND_XML		"actor_menu.xml"

CUICarWnd::CUICarWnd()
{
	m_background = xr_new<CUIStatic>(); m_background->SetAutoDelete(true);
	AttachChild(m_background);
	m_btn_close = xr_new<CUI3tButton>(); m_btn_close->SetAutoDelete(true);
	AttachChild(m_btn_close);

	d_list_actor_items = xr_new<CUIDragDropListEx>(); d_list_actor_items->SetAutoDelete(true);
	AttachChild(d_list_actor_items);
	d_list_trunk = xr_new<CUIDragDropListEx>(); d_list_trunk->SetAutoDelete(true);
	AttachChild(d_list_trunk);

	BindDragDropListEvents(d_list_actor_items);
	BindDragDropListEvents(d_list_trunk);

	m_btn_repair = xr_new<CUI3tButton>(); m_btn_repair->SetAutoDelete(true);
	AttachChild(m_btn_repair);
	m_txt_cur_health = xr_new<CUITextWnd>(); m_txt_cur_health->SetAutoDelete(true);
	AttachChild(m_txt_cur_health);
	m_health_progress_back = xr_new<CUIProgressBar>(); m_health_progress_back->SetAutoDelete(true);
	AttachChild(m_health_progress_back);
	m_health_progress_current_add = xr_new<CUIProgressBar>(); m_health_progress_current_add->SetAutoDelete(true);
	AttachChild(m_health_progress_current_add);
	m_health_progress_current = xr_new<CUIProgressBar>(); m_health_progress_current->SetAutoDelete(true);
	AttachChild(m_health_progress_current);
	m_toolkits_list = xr_new<CUIListBox>(); m_toolkits_list->SetAutoDelete(true);
	AttachChild(m_toolkits_list);

	m_btn_add_fuel = xr_new<CUI3tButton>(); m_btn_add_fuel->SetAutoDelete(true);
	AttachChild(m_btn_add_fuel);
	m_txt_cur_fuel_level = xr_new<CUITextWnd>(); m_txt_cur_fuel_level->SetAutoDelete(true);
	AttachChild(m_txt_cur_fuel_level);
	m_fuel_progress_back = xr_new<CUIProgressBar>(); m_fuel_progress_back->SetAutoDelete(true);
	AttachChild(m_fuel_progress_back);
	m_fuel_progress_current_add = xr_new<CUIProgressBar>(); m_fuel_progress_current_add->SetAutoDelete(true);
	AttachChild(m_fuel_progress_current_add);
	m_fuel_progress_current = xr_new<CUIProgressBar>(); m_fuel_progress_current->SetAutoDelete(true);
	AttachChild(m_fuel_progress_current);
	m_fuel_canister_list = xr_new<CUIListBox>(); m_fuel_canister_list->SetAutoDelete(true);
	AttachChild(m_fuel_canister_list);

	owner_car = NULL;
}

CUICarWnd::~CUICarWnd()
{
	xr_delete(owner_car);
}

void CUICarWnd::BindDragDropListEvents(CUIDragDropListEx* lst)
{
	lst->m_f_item_drop = CUIDragDropListEx::DRAG_CELL_EVENT(this, &CUICarWnd::OnItemDrop);
	lst->m_f_item_start_drag = CUIDragDropListEx::DRAG_CELL_EVENT(this, &CUICarWnd::OnItemStartDrag);
	lst->m_f_item_db_click = CUIDragDropListEx::DRAG_CELL_EVENT(this, &CUICarWnd::OnItemDbClick);
	lst->m_f_item_selected = CUIDragDropListEx::DRAG_CELL_EVENT(this, &CUICarWnd::OnItemSelected);
	lst->m_f_item_rbutton_click = CUIDragDropListEx::DRAG_CELL_EVENT(this, &CUICarWnd::OnItemRButtonClick);
	lst->m_f_item_focus_received = CUIDragDropListEx::DRAG_CELL_EVENT(this, &CUICarWnd::OnItemFocusReceive);
	lst->m_f_item_focus_lost = CUIDragDropListEx::DRAG_CELL_EVENT(this, &CUICarWnd::OnItemFocusLost);
	lst->m_f_item_focused_update = CUIDragDropListEx::DRAG_CELL_EVENT(this, &CUICarWnd::OnItemFocusedUpdate);
}

void CUICarWnd::Init(CCar *car)
{
	CUIXml xml;
	xml.Load( CONFIG_PATH, UI_PATH, CAR_WND_XML);

	CUIXmlInit::InitWindow( xml, "car_wnd", 0, this );
	CUIXmlInit::InitStatic(xml, "car_wnd:background", 0, m_background);
	CUIXmlInit::Init3tButton(xml, "car_wnd:btn_close", 0, m_btn_close);
	CUIXmlInit::InitDragDropListEx(xml, "car_wnd:list_actor_items", 0, d_list_actor_items);
	CUIXmlInit::InitDragDropListEx(xml, "car_wnd:list_trunk", 0, d_list_trunk);

	CUIXmlInit::Init3tButton(xml, "car_wnd:btn_add_fuel", 0, m_btn_add_fuel);
	CUIXmlInit::InitTextWnd(xml, "car_wnd:txt_cur_fuel_level", 0, m_txt_cur_fuel_level);
	CUIXmlInit::InitProgressBar(xml, "car_wnd:fuel_progress_back", 0, m_fuel_progress_back);
	CUIXmlInit::InitProgressBar(xml, "car_wnd:fuel_progress_current_add", 0, m_fuel_progress_current_add);
	CUIXmlInit::InitProgressBar(xml, "car_wnd:fuel_progress_current", 0, m_fuel_progress_current);
	CUIXmlInit::InitListBox(xml, "car_wnd:fuel_canister_list", 0, m_fuel_canister_list);

	CUIXmlInit::Init3tButton(xml, "car_wnd:btn_repair", 0, m_btn_repair);
	CUIXmlInit::InitTextWnd(xml, "car_wnd:txt_cur_health", 0, m_txt_cur_health);
	CUIXmlInit::InitProgressBar(xml, "car_wnd:health_progress_back", 0, m_health_progress_back);
	CUIXmlInit::InitProgressBar(xml, "car_wnd:health_progress_current_add", 0, m_health_progress_current_add);
	CUIXmlInit::InitProgressBar(xml, "car_wnd:health_progress_current", 0, m_health_progress_current);
	CUIXmlInit::InitListBox(xml, "car_wnd:toolkits_list", 0, m_toolkits_list);

	owner_car = car;

	for (TIItemContainer::const_iterator it = Actor()->inventory().m_ruck.begin(); Actor()->inventory().m_ruck.end() != it; ++it)
	{
		CUICellItem* itm = create_cell_item(*it);
		d_list_actor_items->SetItem(itm);
	}
	for (xr_vector<u16>::const_iterator it = owner_car->m_trunk_items.begin(); owner_car->m_trunk_items.end() != it; ++it)
	{
		CObject* obj = Level().Objects.net_Find(*it);
		if (obj)
		{
			PIItem pitm = smart_cast<PIItem>(obj);
			VERIFY(pitm);
			CUICellItem* itm = create_cell_item(pitm);
			d_list_trunk->SetItem(itm);
		}
	}

	UpdateInfo();
}

void CUICarWnd::UpdateInfo()
{
	string64 fuel;
	xr_sprintf(fuel, "%.2f", owner_car->GetFuel());
	LPCSTR fuel_level_text;
	STRCONCAT(fuel_level_text, CStringTable().translate("ui_fuel_level").c_str(), fuel, CStringTable().translate("ui_litr").c_str());
	m_txt_cur_fuel_level->SetText(fuel_level_text);

	m_fuel_progress_current_add->Show(false);
	m_fuel_progress_back->SetProgressPos(100);
	m_fuel_progress_current->SetProgressPos((owner_car->GetFuel() / owner_car->GetFuelTank()) * 100);

	m_btn_add_fuel->Enable(false);
	m_fuel_canister_list->Clear();
	canister_list.clear();

	string64 health;
	xr_sprintf(health, "%.1f%c", owner_car->GetfHealth() * 100, char(37));
	LPCSTR health_text;
	STRCONCAT(health_text, CStringTable().translate("ui_car_health_level").c_str(), " ", health);
	m_txt_cur_health->SetText(health_text);

	m_health_progress_current_add->Show(false);
	m_health_progress_back->SetProgressPos(100);
	m_health_progress_current->SetProgressPos(owner_car->GetfHealth() * 100);

	m_btn_repair->Enable(false);
	m_toolkits_list->Clear();
	toolkits_list.clear();

	for (TIItemContainer::const_iterator it = Actor()->inventory().m_ruck.begin(); Actor()->inventory().m_ruck.end() != it; ++it)
	{
		PIItem pIItem = *it;
		if (pIItem->object().CLS_ID == CLSID_IITEM_CANISTER &&
			pIItem->Useful())
		{
			CCanister* canister = smart_cast<CCanister*>(pIItem);
			string64 cur_fuel;
			xr_sprintf(cur_fuel, "%.2f", canister->GetFuel());
			STRCONCAT(fuel_level_text, CStringTable().translate(canister->NameItem()).c_str(), ", ", cur_fuel, CStringTable().translate("ui_litr").c_str());

			if (canister->GetFuel() > 0)
			{
				canister_list.push_back(canister->ID());
				m_fuel_canister_list->AddTextItem(fuel_level_text);
			}
		}
		else if (pIItem->object().CLS_ID == CLSID_IITEM_CAR_TOOLKIT &&
				pIItem->Useful())
			{
				LPCSTR cur_toolkit_label;
				CCarToolkit* toolkit = smart_cast<CCarToolkit*>(pIItem);
				string64 applying;
				xr_sprintf(applying, "%d", toolkit->applying_remain);
				STRCONCAT(cur_toolkit_label, CStringTable().translate(toolkit->NameItem()).c_str(), ", ", applying, " ", CStringTable().translate("ui_remain_car_toolkit_applying").c_str());

				if (toolkit->applying_remain > 0)
				{
					toolkits_list.push_back(toolkit->ID());
					m_toolkits_list->AddTextItem(cur_toolkit_label);
				}
			}
	}
}

bool CUICarWnd::OnKeyboardAction(int dik, EUIMessages keyboard_action)
{
	if (dik == DIK_ESCAPE)
	{
		HideDialog();
		return true;
	}
	return CUIDialogWnd::OnKeyboardAction(dik, keyboard_action);
}

void CUICarWnd::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
	if (msg == BUTTON_CLICKED)
	{
		if (pWnd == m_btn_repair)
		{
			CCarToolkit* toolkit = smart_cast<CCarToolkit*>(Actor()->inventory().Get(toolkits_list[m_toolkits_list->GetSelectedIDX()], true));
			toolkit->applying_remain--;
			if (toolkit->applying_remain < 0)
				toolkit->applying_remain = 0;
			float h = owner_car->GetfHealth() + ((float)toolkit->capacity / 100);
			if (h > 1)
				h = 1;
			owner_car->SetfHealth(h);
			UpdateInfo();
		}
		else if (pWnd == m_btn_add_fuel)
		{
			CCanister* canister = smart_cast<CCanister*>(Actor()->inventory().Get(canister_list[m_fuel_canister_list->GetSelectedIDX()], true));
			float delta = owner_car->GetFuelTank() - owner_car->GetFuel();
			float to_add = delta > canister->GetFuel() ? canister->GetFuel() : delta;
			canister->ChangeFuel(to_add*-1);
			owner_car->ChangeFuel(to_add);
			UpdateInfo();
		}
		else if (pWnd == m_btn_close)
		{
			HideDialog();
		}
	}
	else if (msg == LIST_ITEM_SELECT)
	{
		if (pWnd == m_fuel_canister_list)
		{
			if (owner_car->GetFuel() < owner_car->GetFuelTank())
			{
				m_btn_add_fuel->Enable(true);
				CCanister* canister = smart_cast<CCanister*>(Actor()->inventory().Get(canister_list[m_fuel_canister_list->GetSelectedIDX()], true));
				float delta = owner_car->GetFuelTank() - owner_car->GetFuel();
				float to_add = delta > canister->GetFuel() ? canister->GetFuel() : delta;
				m_fuel_progress_current_add->SetProgressPos(((owner_car->GetFuel() / owner_car->GetFuelTank()) * 100) + to_add);
				m_fuel_progress_current_add->Show(true);
			}
		}
		else if (pWnd == m_toolkits_list)
		{
			if (owner_car->GetfHealth() < 1)
			{
				m_btn_repair->Enable(true);
				CCarToolkit* toolkit = smart_cast<CCarToolkit*>(Actor()->inventory().Get(toolkits_list[m_toolkits_list->GetSelectedIDX()], true));
				m_health_progress_current_add->SetProgressPos(owner_car->GetfHealth() * 100 + toolkit->capacity);
				m_health_progress_current_add->Show(true);
			}
		}
	}
}

/*void CUICarWnd::move_item_from_actor(u16 what_id)
{
	CObject* itm = Level().Objects.net_Find(what_id);
	owner_car->m_trunk_items.push_back(what_id);
	//CGameObject* GO = smart_cast<CGameObject*>(itm);
	//Actor()->inventory().DropItem(GO, false, true);
	//Actor()->callback(GameObject::eOnItemDrop)(GO->lua_game_object());
	itm->H_SetParent(owner_car);
	itm->setVisible(FALSE);
	itm->setEnabled(FALSE);
}

void CUICarWnd::move_item_to_actor(u16 what_id)
{
	CObject* itm = Level().Objects.net_Find(what_id);
	xr_vector<u16>::iterator it;
	it = std::find(owner_car->m_trunk_items.begin(), owner_car->m_trunk_items.end(), what_id); 
	//VERIFY(it != owner_car->m_trunk_items.end());
	owner_car->m_trunk_items.erase(it);
	itm->H_SetParent(NULL, true);
	//itm->H_SetParent(smart_cast<CObject*>(Actor()));
	//CGameObject* GO = smart_cast<CGameObject*>(itm);
	//Actor()->inventory().Take(GO, false, true);
	//Actor()->callback(GameObject::eInvBoxItemTake)(GO->lua_game_object());
}*/

void CUICarWnd::move_item_from_to(u16 from_id, u16 to_id, u16 what_id)
{
	NET_Packet P;
	CGameObject::u_EventGen(P, GE_OWNERSHIP_REJECT, from_id);
	P.w_u16(what_id);
	CGameObject::u_EventSend(P);

	//другому инвентарю - взять вещь 
	CGameObject::u_EventGen(P, GE_OWNERSHIP_TAKE, to_id);
	P.w_u16(what_id);
	CGameObject::u_EventSend(P);
}

bool CUICarWnd::OnItemDrop(CUICellItem* itm)
{
	//OnItemSelected(itm);
	CUIDragDropListEx*	old_owner = itm->OwnerList();
	CUIDragDropListEx*	new_owner = CUIDragDropListEx::m_drag_item->BackList();

	bool b = old_owner == new_owner;

	if (old_owner&&new_owner && !b)
	{
		CUICellItem* i = old_owner->RemoveItem(itm, (old_owner == new_owner));
		while (i->ChildsCount())
		{
			CUICellItem* _chld = i->PopChild(NULL);
			new_owner->SetItem(_chld, old_owner->GetDragItemPosition());
		}
		new_owner->SetItem(i, old_owner->GetDragItemPosition());
		PIItem item = (PIItem)itm->m_pData;
		/*if (old_owner == d_list_actor_items)
			move_item_from_actor(item->object_id());
		else if (old_owner == d_list_trunk)
			move_item_to_actor(item->object_id());*/
		if (old_owner == d_list_actor_items)
			move_item_from_to(Actor()->ID(), owner_car->ID(), item->object_id());
		else if (old_owner == d_list_trunk)
			move_item_from_to(owner_car->ID(), Actor()->ID(), item->object_id());
		return true;
	}

	return false;
}

bool CUICarWnd::OnItemStartDrag(CUICellItem* itm)
{
	return false;
}

bool CUICarWnd::OnItemDbClick(CUICellItem* itm)
{
	CUIDragDropListEx*	old_owner = itm->OwnerList();
	return true;
}

bool CUICarWnd::OnItemSelected(CUICellItem* itm)
{
	return false;
}

bool CUICarWnd::OnItemRButtonClick(CUICellItem* itm)
{
	return false;
}

bool CUICarWnd::OnItemFocusReceive(CUICellItem* itm)
{
	if (itm)
	{
		itm->m_selected = true;
	}
	return true;
}

bool CUICarWnd::OnItemFocusLost(CUICellItem* itm)
{
	if (itm)
	{
		itm->m_selected = false;
	}

	return true;
}

bool CUICarWnd::OnItemFocusedUpdate(CUICellItem* itm)
{
	if (itm)
	{
		itm->m_selected = true;
	}
	return true;
}
