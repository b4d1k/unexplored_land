// skif : (разделы в ПДА) - раздел "Энциклопедия"
#include "stdafx.h"
#include "pch_script.h"
#include "UIEncWnd.h"

#include "UIFixedScrollBar.h"
#include "UIXmlInit.h"
#include "UIProgressBar.h"
#include "UIFrameLineWnd.h"
#include "UIScrollView.h"
#include "UIHelper.h"
#include "UIListBox.h"
#include "UIInventoryUtilities.h"

#include "../actor.h"
#include "../ai_space.h"
#include "../alife_simulator.h"

#include "../../xrServerEntities/script_engine.h"
#include "../character_community.h"
#include "../character_reputation.h"
#include "../relation_registry.h"
#include "../string_table.h"
#include "ui_base.h"

#define  PDA_ENC_XML		"pda_enc.xml"

CUIEncWnd::CUIEncWnd()
{	
	m_main_list = xr_new<CUIListBox>();
	m_main_list->SetAutoDelete(true);
	AttachChild(m_main_list);
	
	m_items_list = xr_new<CUIListBox>();
	m_items_list->SetAutoDelete(true);
	AttachChild(m_items_list);
}

CUIEncWnd::~CUIEncWnd()
{
}

void CUIEncWnd::Show( bool status )
{
	inherited::Show( status );
}

void CUIEncWnd::Init()
{
	CUIXml xml;
	xml.Load( CONFIG_PATH, UI_PATH, PDA_ENC_XML );

	CUIXmlInit::InitWindow( xml, "main_wnd", 0, this );

	m_background				= UIHelper::CreateFrameWindow(xml, "background", this);
	m_down_background			= UIHelper::CreateFrameWindow(xml, "down_background", this);
	m_center_caption			= UIHelper::CreateTextWnd( xml, "center_caption", this );
	m_icon   					= UIHelper::CreateStatic(xml, "icon", this);
	m_descr    					= UIHelper::CreateTextWnd(xml, "descr", this);
	string256 buf;
	xr_strcpy( buf, sizeof(buf), m_center_caption->GetText() );
	xr_strcat( buf, sizeof(buf), CStringTable().translate("ui_enc_center_caption").c_str() );
	m_center_caption->SetText( buf );
	CUIXmlInit::InitListBox(xml, "main_list", 0, m_main_list);
	CUIXmlInit::InitListBox(xml, "items_list", 0, m_items_list);
	m_main_list->AddTextItem(CStringTable().translate("st_enc_anomaly").c_str());
	m_main_list->AddTextItem(CStringTable().translate("st_enc_artefact").c_str());
	m_main_list->AddTextItem(CStringTable().translate("st_enc_monster").c_str());
	m_main_list->AddTextItem(CStringTable().translate("st_enc_level").c_str());
	m_main_list->AddTextItem(CStringTable().translate("st_enc_faction").c_str());
	m_main_list->AddTextItem(CStringTable().translate("st_enc_note").c_str());
	m_items_list->Clear();
	m_icon->InitTexture("ui_nomap");
	m_descr->SetText("");
}

void CUIEncWnd::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
	/*string_path ini_path;
	FS.update_path(ini_path, CONFIG_PATH, "misc\\pda.ltx");
	CInifile* EncIni = xr_new<CInifile>(ini_path);
	
	LPCSTR artefacts_arr = EncIni->r_string("encyclopedia","artefact_articles");
	LPCSTR levels_arr = EncIni->r_string("encyclopedia","level_articles");
	LPCSTR anomalies_arr = EncIni->r_string("encyclopedia","anomaly_articles");
	LPCSTR monsters_arr = EncIni->r_string("encyclopedia","monster_articles");
	LPCSTR notes_arr = EncIni->r_string("encyclopedia","note_articles");
	LPCSTR factions_arr = EncIni->r_string("encyclopedia","faction_articles");
	
	artefact_articles = FillArray(artefacts_arr);
	level_articles = FillArray(levels_arr);
	anomaly_articles = FillArray(anomalies_arr);
	monster_articles = FillArray(monsters_arr);
	note_articles = FillArray(notes_arr);
	faction_articles = FillArray(factions_arr);*/


	if (pWnd == m_main_list)
	{
		if(msg == LIST_ITEM_CLICKED)
		{
			m_items_list->Clear();
			m_icon->InitTexture("ui_nomap");
			m_descr->SetText("");
			switch (m_main_list->GetSelectedIDX())
			{	
				case 0 :				
					FillItemsList(3);
					break;
				case 1 :
					FillItemsList(1);
					break;
				case 2 :				
					FillItemsList(4);
					break;
				case 3 :				
					FillItemsList(2);
					break;
				case 4 :				
					FillItemsList(5);
					break;
				case 5 :				
					FillItemsList(6);
					break;
			}
		}
	}
	else if (pWnd == m_items_list)
	{
		if(msg == LIST_ITEM_CLICKED)
		{
			switch (m_main_list->GetSelectedIDX())
			{
				case 0:
					FillArticleInfo(3);
					break;
				case 1:
					FillArticleInfo(1);
					break;
				case 2:
					FillArticleInfo(4);
					break;
				case 3:
					FillArticleInfo(2);
					break;
				case 4:
					FillArticleInfo(5);
					break;
				case 5:
					FillArticleInfo(6);
					break;
			}
		}
	}
	CUIWndCallback::OnEvent	(pWnd, msg, pData);
	inherited::SendMessage	(pWnd, msg, pData);
}



xr_vector<shared_str> CUIEncWnd::FillArray(LPCSTR str)
{
	xr_vector<shared_str> arr;
	arr.clear();
	if (str && str[0])
	{
		string128 cur_item;
		int cnt = _GetItemCount(str);
		for (int it = 0; it < cnt; ++it)
		{
			_GetItem(str, it, cur_item);
			arr.push_back(cur_item);
		}
	}
	return arr;
}

void CUIEncWnd::FillItemsList(u8 chapter)
{
	luabind::functor<u16> chapter_func;
	R_ASSERT2(ai().script_engine().functor("pda.get_enc_cnt", chapter_func), "Failed to get chapter");
	u16 cnt = chapter_func(chapter);
	
	luabind::functor<LPCSTR> chapterI_func;
	R_ASSERT2(ai().script_engine().functor("pda.get_enc_chapI", chapterI_func), "Failed to get chapter");
	
	for (u16 i=0; i<cnt; i++)
	{
		LPCSTR item_name;
		STRCONCAT(item_name, "st_", chapterI_func(chapter, i+1));
		m_items_list->AddTextItem(CStringTable().translate(item_name).c_str());
	}
}

void CUIEncWnd::FillArticleInfo(u8 chapter)
{
	luabind::functor<u16> chapter_func;
	R_ASSERT2(ai().script_engine().functor("pda.get_enc_cnt", chapter_func), "Failed to get chapter");
	u16 cnt = chapter_func(chapter);
	
	luabind::functor<LPCSTR> chapterI_func;
	R_ASSERT2(ai().script_engine().functor("pda.get_enc_chapI", chapterI_func), "Failed to get chapter");
	LPCSTR texture;
	STRCONCAT(texture, "ui_", chapterI_func(chapter, m_items_list->GetSelectedIDX()+1), "_icon");
	m_icon->InitTexture(texture);
	LPCSTR item_name;
	STRCONCAT(item_name, "st_", chapterI_func(chapter, m_items_list->GetSelectedIDX()+1), "_descr");
	m_descr->SetText(CStringTable().translate(item_name).c_str());
}

void CUIEncWnd::ResetAll()
{
	inherited::ResetAll();
}