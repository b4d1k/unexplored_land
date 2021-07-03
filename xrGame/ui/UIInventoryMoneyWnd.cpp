// skif : (конвертер для денег)
// Конвертер для электронных денег
#include "stdafx.h"
#include "pch_script.h"
#include "UIInventoryMoneyWnd.h"

#include "UIFixedScrollBar.h"
#include "UIXmlInit.h"
#include "UIProgressBar.h"
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

#define  INVENTORY_MONEY_XML		"actor_menu.xml"

CUIInvMoneyWnd::CUIInvMoneyWnd()
{	
}

CUIInvMoneyWnd::~CUIInvMoneyWnd()
{
}

void CUIInvMoneyWnd::Show( bool status )
{
	inherited::Show( status );
}

void CUIInvMoneyWnd::Init()
{
	CUIXml xml;
	xml.Load( CONFIG_PATH, UI_PATH, INVENTORY_MONEY_XML );

	CUIXmlInit::InitWindow( xml, "money_main_wnd", 0, this );

	m_background			= UIHelper::CreateStatic(xml, "money_main_wnd:background", this);
	m_btn_100_rub			= UIHelper::Create3tButton(xml, "money_main_wnd:btn_100_rub", this);
	m_btn_1000_rub			= UIHelper::Create3tButton(xml, "money_main_wnd:btn_1000_rub", this);
	m_btn_5000_rub			= UIHelper::Create3tButton(xml, "money_main_wnd:btn_5000_rub", this);
	m_btn_10000_rub			= UIHelper::Create3tButton(xml, "money_main_wnd:btn_10000_rub", this);
	m_btn_50000_rub			= UIHelper::Create3tButton(xml, "money_main_wnd:btn_50000_rub", this);
	m_btn_100000_rub		= UIHelper::Create3tButton(xml, "money_main_wnd:btn_100000_rub", this);
	m_actor_money_caption   = UIHelper::CreateTextWnd(xml, "money_main_wnd:actor_money_caption", this);
}

void CUIInvMoneyWnd::UpdateInfo()
{
	if (!Actor()) return;
	string64 buf;
	xr_sprintf( buf, sizeof(buf), "%d %s", Actor()->get_money(), "RU" );
	m_actor_money_caption->SetText( buf );
	m_actor_money_caption->AdjustWidthToText();
	if (Actor()->get_money() < 100000)
		m_btn_100000_rub->Enable(false);
	else
		m_btn_100000_rub->Enable(true);
	if (Actor()->get_money() < 50000)
		m_btn_50000_rub->Enable(false);
	else
		m_btn_50000_rub->Enable(true);
	if (Actor()->get_money() < 10000)
		m_btn_10000_rub->Enable(false);
	else
		m_btn_10000_rub->Enable(true);
	if (Actor()->get_money() < 5000)
		m_btn_5000_rub->Enable(false);
	else
		m_btn_5000_rub->Enable(true);
	if (Actor()->get_money() < 1000)
		m_btn_1000_rub->Enable(false);
	else
		m_btn_1000_rub->Enable(true);
	if (Actor()->get_money() < 100)
		m_btn_100_rub->Enable(false);
	else
		m_btn_100_rub->Enable(true);
}

void CUIInvMoneyWnd::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
	if (pWnd == m_btn_100_rub)
		if (msg == BUTTON_CLICKED && Actor()->get_money() >= 100)
		{
			Level().spawn_item("money_rub_100",Actor()->Position(),Actor()->ai_location().level_vertex_id(),Actor()->ID());
			Actor()->set_money( Actor()->get_money() - 100, true);
		}
	if (pWnd == m_btn_1000_rub)
		if (msg == BUTTON_CLICKED && Actor()->get_money() >= 1000)
		{
			Level().spawn_item("money_rub_1000",Actor()->Position(),Actor()->ai_location().level_vertex_id(),Actor()->ID());
			Actor()->set_money( Actor()->get_money() - 1000, true);
		}
	if (pWnd == m_btn_5000_rub)
		if (msg == BUTTON_CLICKED && Actor()->get_money() >= 5000)
		{
			Level().spawn_item("money_rub_5000",Actor()->Position(),Actor()->ai_location().level_vertex_id(),Actor()->ID());
			Actor()->set_money( Actor()->get_money() - 5000, true);
		}
	if (pWnd == m_btn_10000_rub)
		if (msg == BUTTON_CLICKED && Actor()->get_money() >= 10000)
		{
			Level().spawn_item("money_rub_10000",Actor()->Position(),Actor()->ai_location().level_vertex_id(),Actor()->ID());
			Actor()->set_money( Actor()->get_money() - 10000, true);
		}
	if (pWnd == m_btn_50000_rub)
		if (msg == BUTTON_CLICKED && Actor()->get_money() >= 50000)
		{
			Level().spawn_item("money_rub_50000",Actor()->Position(),Actor()->ai_location().level_vertex_id(),Actor()->ID());
			Actor()->set_money( Actor()->get_money() - 50000, true);
		}
	if (pWnd == m_btn_100000_rub)
		if (msg == BUTTON_CLICKED && Actor()->get_money() >= 100000)
		{
			Level().spawn_item("money_rub_100000",Actor()->Position(),Actor()->ai_location().level_vertex_id(),Actor()->ID());
			Actor()->set_money( Actor()->get_money() - 100000, true);
		}
	UpdateInfo();
	CUIWndCallback::OnEvent	(pWnd, msg, pData);
	inherited::SendMessage	(pWnd, msg, pData);
}