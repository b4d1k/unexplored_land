// skif : (разделы в ПДА) - раздел "Отношения"
#include "stdafx.h"
#include "pch_script.h"
#include "UIRelationWnd.h"

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

#define  PDA_RELATION_XML		"pda_relation.xml"

CUIRelationWnd::CUIRelationWnd()
{	
	m_rel_list = xr_new<CUIListBox>();
	m_rel_list->SetAutoDelete(true);
	AttachChild(m_rel_list);
}

CUIRelationWnd::~CUIRelationWnd()
{
}

void CUIRelationWnd::Show( bool status )
{
	inherited::Show( status );
}

void CUIRelationWnd::Init()
{
	CUIXml xml;
	xml.Load( CONFIG_PATH, UI_PATH, PDA_RELATION_XML );

	CUIXmlInit::InitWindow( xml, "main_wnd", 0, this );
	m_background				= UIHelper::CreateFrameWindow(xml, "background", this);
	m_down_background			= UIHelper::CreateFrameWindow(xml, "down_background", this);
	m_center_caption			= UIHelper::CreateTextWnd( xml, "center_caption", this );
	string256 buf;
	xr_strcpy( buf, sizeof(buf), m_center_caption->GetText() );
	xr_strcat( buf, sizeof(buf), CStringTable().translate("ui_relation_center_caption").c_str() );
	m_center_caption->SetText( buf );
	CUIXmlInit::InitListBox(xml, "relation_list", 0, m_rel_list);
}

void CUIRelationWnd::UpdateInfo()
{
	string_path ini_path;
	FS.update_path(ini_path, CONFIG_PATH, "creatures\\game_relations.ltx");
	CInifile* RelIni = xr_new<CInifile>(ini_path);
	xr_vector<shared_str>	m_communities;
	m_communities.clear();
	LPCSTR communities = RelIni->r_string("game_relations", "community_names");
	m_rel_list->Clear();
	if (communities && communities[0])
	{
		string128 _community;
		for (int it = 0; it < _GetItemCount(communities); ++it)
		{
			_GetItem(communities, it, _community);
			m_communities.push_back(_community);
		}
	}
		
	for (u8 i = 0; i < m_communities.size(); i++)
	{
		CHARACTER_COMMUNITY c;
		c.set(m_communities[i]);
		if (!Actor())
			return;
		/*luabind::functor<u8>	m_functor;
		R_ASSERT( ai().script_engine().functor( "main.get_community_goodwill", m_functor ) );
		u8 val = m_functor(m_communities[i].c_str());*/
		int val = RELATION_REGISTRY().GetCommunityGoodwill( c.index(), Actor()->ID() );
		LPCSTR icon;
		STRCONCAT(icon, "ui_faction_",m_communities[i],"_emblem");
		LPCSTR relation;
		if (val > -1000 && val < 1000)
		{
			string64 str = "";
			xr_sprintf( str, "%d", val );
			STRCONCAT(relation, CStringTable().translate(m_communities[i]).c_str(), " ", CStringTable().translate("st_ui_pda_legend_neutrals").c_str(), " ", str);
			m_rel_list->AddRelationItem(relation,color_rgba(255,255,0,255),icon);
		}
		else if (val <= -1000)
		{
			string64 str = "";				
			xr_sprintf( str, "%d", val );
			STRCONCAT(relation, CStringTable().translate(m_communities[i]).c_str(), " ", CStringTable().translate("st_ui_pda_legend_enemies").c_str(), " ", str);
			m_rel_list->AddRelationItem(relation,color_rgba(255,0,0,255),icon);
		}
		else if (val >= 1000)
		{
			string64 str = "";
			xr_sprintf( str, "%d", val );
			STRCONCAT(relation, CStringTable().translate(m_communities[i]).c_str(), " ", CStringTable().translate("st_ui_pda_legend_friends").c_str(), " ", str);
			m_rel_list->AddRelationItem(relation,color_rgba(0,255,0,255),icon);
		}
	}
}

void CUIRelationWnd::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
	CUIWndCallback::OnEvent	(pWnd, msg, pData);
	inherited::SendMessage	(pWnd, msg, pData);
}

void CUIRelationWnd::ResetAll()
{
	inherited::ResetAll();
}