#include "pch_script.h"

#include "uiiteminfo.h"
#include "uistatic.h"
#include "UIXmlInit.h"

#include "UIProgressBar.h"
#include "UIScrollView.h"
#include "UIFrameWindow.h"

#include "ai_space.h"
#include "alife_simulator.h"
#include "../string_table.h"
#include "../Inventory_Item.h"
#include "UIInventoryUtilities.h"
#include "../PhysicsShellHolder.h"
#include "UIWpnParams.h"
#include "ui_af_params.h"
#include "UIInvUpgradeProperty.h"
#include "UIOutfitInfo.h"
#include "UIBoosterInfo.h"
#include "../Weapon.h"
#include "../CustomOutfit.h"
#include "../ActorHelmet.h"
#include "../eatable_item.h"
#include "../WeaponKnife.h" // skif : (не отображать характеристики оружи€ у ножей)
#include "UICellItem.h"
// skif : (батарейка дл€ фонарика)
#include "Actor.h"
#include "Torch.h"
// end
#include "Car.h"

extern const LPCSTR g_inventory_upgrade_xml;

#define  INV_GRID_WIDTH2  40.0f
#define  INV_GRID_HEIGHT2 40.0f

CUIItemInfo::CUIItemInfo()
{
	UIItemImageSize.set			(0.0f,0.0f);
	
	UICost						= NULL;
	UITradeTip					= NULL;
	UIWeight					= NULL;
	UIItemImage					= NULL;
	UIDesc						= NULL;
//	UIConditionWnd				= NULL;
	UIWpnParams					= NULL;
	UIProperties				= NULL;
	UIOutfitInfo				= NULL;
	UIBoosterInfo				= NULL;
	UIArtefactParams			= NULL;
	UIName						= NULL;
	UIBackground				= NULL;
	m_pInvItem					= NULL;
	m_b_FitToHeight				= false;
	m_complex_desc				= false;
	// skif : (батарейка дл€ фонарика)
	UITorchBatareyInfo			= NULL;
	// end
	UICanisterInfo = NULL;
	UICarToolkitInfo = NULL;
}

CUIItemInfo::~CUIItemInfo()
{
//	xr_delete	(UIConditionWnd);
	xr_delete	(UIWpnParams);
	xr_delete	(UIArtefactParams);
	xr_delete	(UIProperties);
	xr_delete	(UIOutfitInfo);
	xr_delete	(UIBoosterInfo);
	// skif : (батарейка дл€ фонарика)
	xr_delete	(UITorchBatareyInfo);
	// end
	xr_delete(UICanisterInfo);
	xr_delete(UICarToolkitInfo);
}

void CUIItemInfo::InitItemInfo(LPCSTR xml_name)
{
	CUIXml						uiXml;
	uiXml.Load					(CONFIG_PATH, UI_PATH, xml_name);
	CUIXmlInit					xml_init;

	if(uiXml.NavigateToNode("main_frame",0))
	{
		Frect wnd_rect;
		wnd_rect.x1		= uiXml.ReadAttribFlt("main_frame", 0, "x", 0);
		wnd_rect.y1		= uiXml.ReadAttribFlt("main_frame", 0, "y", 0);

		wnd_rect.x2		= uiXml.ReadAttribFlt("main_frame", 0, "width", 0);
		wnd_rect.y2		= uiXml.ReadAttribFlt("main_frame", 0, "height", 0);
		wnd_rect.x2		+= wnd_rect.x1;
		wnd_rect.y2		+= wnd_rect.y1;
		inherited::SetWndRect(wnd_rect);
		
		delay			= uiXml.ReadAttribInt("main_frame", 0, "delay", 500);
	}
	if(uiXml.NavigateToNode("background_frame",0))
	{
		UIBackground				= xr_new<CUIFrameWindow>();
		UIBackground->SetAutoDelete	(true);
		AttachChild					(UIBackground);
		xml_init.InitFrameWindow	(uiXml, "background_frame", 0,	UIBackground);
	}
	m_complex_desc = false;
	if(uiXml.NavigateToNode("static_name",0))
	{
		UIName						= xr_new<CUITextWnd>();	 
		AttachChild					(UIName);		
		UIName->SetAutoDelete		(true);
		xml_init.InitTextWnd		(uiXml, "static_name", 0,	UIName);
		m_complex_desc				= ( uiXml.ReadAttribInt("static_name", 0, "complex_desc", 0) == 1 );
	}
	if(uiXml.NavigateToNode("static_weight",0))
	{
		UIWeight				= xr_new<CUITextWnd>();	 
		AttachChild				(UIWeight);		
		UIWeight->SetAutoDelete(true);
		xml_init.InitTextWnd		(uiXml, "static_weight", 0,			UIWeight);
	}

	if(uiXml.NavigateToNode("static_cost",0))
	{
		UICost					= xr_new<CUITextWnd>();	 
		AttachChild				(UICost);
		UICost->SetAutoDelete	(true);
		xml_init.InitTextWnd		(uiXml, "static_cost", 0,			UICost);
	}

	if(uiXml.NavigateToNode("static_no_trade",0))
	{
		UITradeTip					= xr_new<CUITextWnd>();	 
		AttachChild					(UITradeTip);
		UITradeTip->SetAutoDelete	(true);
		xml_init.InitTextWnd		(uiXml, "static_no_trade", 0,		UITradeTip);
	}

	if(uiXml.NavigateToNode("descr_list",0))
	{
//		UIConditionWnd					= xr_new<CUIConditionParams>();
//		UIConditionWnd->InitFromXml		(uiXml);
		UIWpnParams						= xr_new<CUIWpnParams>();
		UIWpnParams->InitFromXml		(uiXml);

		UIArtefactParams				= xr_new<CUIArtefactParams>();
		UIArtefactParams->InitFromXml	(uiXml);

		UIBoosterInfo					= xr_new<CUIBoosterInfo>();
		UIBoosterInfo->InitFromXml		(uiXml);
		
		// skif : (батарейка дл€ фонарика)
		UITorchBatareyInfo					= xr_new<CUITorchBatareyInfo>();
		UITorchBatareyInfo->InitFromXml		(uiXml);
		// end
		UICanisterInfo = xr_new<CUICanisterInfo>();
		UICanisterInfo->InitFromXml(uiXml);
		UICarToolkitInfo = xr_new<CUICarToolkitInfo>();
		UICarToolkitInfo->InitFromXml(uiXml);

		//UIDesc_line						= xr_new<CUIStatic>();
		//AttachChild						(UIDesc_line);	
		//UIDesc_line->SetAutoDelete		(true);
		//xml_init.InitStatic				(uiXml, "description_line", 0, UIDesc_line);

		if ( ai().get_alife() ) // (-designer)
		{
			UIProperties					= xr_new<UIInvUpgPropertiesWnd>();
			UIProperties->init_from_xml		("actor_menu_item.xml");
		}

		UIDesc							= xr_new<CUIScrollView>(); 
		AttachChild						(UIDesc);		
		UIDesc->SetAutoDelete			(true);
		m_desc_info.bShowDescrText		= !!uiXml.ReadAttribInt("descr_list",0,"only_text_info", 1);
		m_b_FitToHeight					= !!uiXml.ReadAttribInt("descr_list",0,"fit_to_height", 0);
		xml_init.InitScrollView			(uiXml, "descr_list", 0, UIDesc);
		xml_init.InitFont				(uiXml, "descr_list:font", 0, m_desc_info.uDescClr, m_desc_info.pDescFont);
	}	

	if (uiXml.NavigateToNode("image_static", 0))
	{	
		UIItemImage					= xr_new<CUIStatic>();	 
		AttachChild					(UIItemImage);	
		UIItemImage->SetAutoDelete	(true);
		xml_init.InitStatic			(uiXml, "image_static", 0, UIItemImage);
		UIItemImage->TextureOn		();

		UIItemImage->TextureOff			();
		UIItemImageSize.set				(UIItemImage->GetWidth(),UIItemImage->GetHeight());
	}
	if ( uiXml.NavigateToNode( "outfit_info", 0 ) )
	{
		UIOutfitInfo				= xr_new<CUIOutfitInfo>();
		UIOutfitInfo->InitFromXml	(uiXml);
	}

	xml_init.InitAutoStaticGroup	(uiXml, "auto", 0, this);
}

void CUIItemInfo::InitItemInfo(Fvector2 pos, Fvector2 size, LPCSTR xml_name)
{
	inherited::SetWndPos	(pos);
	inherited::SetWndSize	(size);
    InitItemInfo			(xml_name);
}

bool	IsGameTypeSingle();

void CUIItemInfo::InitItem(CUICellItem* pCellItem, CInventoryItem* pCompareItem, u32 item_price, LPCSTR trade_tip)
{
	if(!pCellItem)
	{
		m_pInvItem			= NULL;
		Enable				(false);
		return;
	}

	PIItem pInvItem			= (PIItem)pCellItem->m_pData;
	m_pInvItem				= pInvItem;
	Enable					(NULL != m_pInvItem);
	if(!m_pInvItem)			return;

	Fvector2				pos;	pos.set( 0.0f, 0.0f );
	string256				str;
	if ( UIName )
	{
		UIName->SetText		(pInvItem->NameItem());
		UIName->AdjustHeightToText();
		pos.y = UIName->GetWndPos().y + UIName->GetHeight() + 4.0f;
	}
	if ( UIWeight )
	{
		LPCSTR  kg_str = CStringTable().translate( "st_kg" ).c_str();
		float	weight = pInvItem->Weight();
		
		if ( !weight )
		{
			if ( CWeaponAmmo* ammo = dynamic_cast<CWeaponAmmo*>(pInvItem) )
			{
				// its helper item, m_boxCur is zero, so recalculate via CInventoryItem::Weight()
				weight = pInvItem->CInventoryItem::Weight();
				for( u32 j = 0; j < pCellItem->ChildsCount(); ++j )
				{
					PIItem jitem	= (PIItem)pCellItem->Child(j)->m_pData;
					weight			+= jitem->CInventoryItem::Weight();
				}

			}
		}

		xr_sprintf				(str, "%3.2f %s", weight, kg_str );
		UIWeight->SetText	(str);
		
		pos.x = UIWeight->GetWndPos().x;
		if ( m_complex_desc )
		{
			UIWeight->SetWndPos	(pos);
		}
	}
	if ( UICost && IsGameTypeSingle()) //skif : (цены в описании айтемов)
	{
		xr_sprintf				(str, "%d RU", pInvItem->Cost());// skif : (цены в описании айтемов)  
		UICost->SetText		(str);
		pos.x = UICost->GetWndPos().x;
		if ( m_complex_desc )
		{
			UICost->SetWndPos	(pos);
		}
		UICost->Show(true);
	}
	else
		UICost->Show(false);
	
//	CActor* actor = smart_cast<CActor*>( Level().CurrentViewEntity() );
//	if ( g_pGameLevel && Level().game && actor )
//	{
//		game_cl_Deathmatch* gs_mp = smart_cast<game_cl_Deathmatch*>( Game() );
//		IBuyWnd* buy_menu = gs_mp->pCurBuyMenu->GetItemPrice();
//		GetItemPrice();
//	}
	if (UITradeTip)
	{
		pos.y = UITradeTip->GetWndPos().y;
		if ( UIWeight && m_complex_desc )
		{
			pos.y = UIWeight->GetWndPos().y + UIWeight->GetHeight() + 4.0f;
		}

		if(trade_tip==NULL)
			UITradeTip->Show(false);
		else
		{
			UITradeTip->SetText(CStringTable().translate(trade_tip).c_str());
			UITradeTip->AdjustHeightToText();
			UITradeTip->SetWndPos(pos);
			UITradeTip->Show(true);
		}
	}
	
	if ( UIDesc )
	{
		pos = UIDesc->GetWndPos();
		if ( UIWeight )
			pos.y = UIWeight->GetWndPos().y + UIWeight->GetHeight() + 4.0f;

		if(UITradeTip && trade_tip!=NULL)
			pos.y = UITradeTip->GetWndPos().y + UITradeTip->GetHeight() + 4.0f;

		UIDesc->SetWndPos		(pos);
		UIDesc->Clear			();
		VERIFY					(0==UIDesc->GetSize());
		if(m_desc_info.bShowDescrText)
		{
			CUITextWnd* pItem					= xr_new<CUITextWnd>();
			pItem->SetTextColor					(m_desc_info.uDescClr);
			pItem->SetFont						(m_desc_info.pDescFont);
			pItem->SetWidth						(UIDesc->GetDesiredChildWidth());
			pItem->SetTextComplexMode			(true);
			pItem->SetText						(*pInvItem->ItemDescription());
			pItem->AdjustHeightToText			();
			UIDesc->AddWindow					(pItem, true);
		}
		TryAddConditionInfo					(*pInvItem, pCompareItem);
		TryAddWpnInfo						(*pInvItem, pCompareItem);
		TryAddArtefactInfo					(pInvItem->object().cNameSect());
		TryAddOutfitInfo					(*pInvItem, pCompareItem);
		// skif : (доп св-ва броников)
		CCustomOutfit* outfit = smart_cast<CCustomOutfit*>(pInvItem);
		CHelmet* helmet = smart_cast<CHelmet*>(pInvItem);
		if (!outfit && !helmet)
		{
			TryAddUpgradeInfo(*pInvItem);
		}
		// end
		TryAddBoosterInfo					(*pInvItem);
		// skif : (батарейка дл€ фонарика)
		TryAddTorchBatareyInfo				(pInvItem->object().cNameSect(), *pInvItem);
		// end
		TryAddCanisterInfo					(pInvItem->object().cNameSect(), *pInvItem);
		TryAddCarToolkitInfo				(pInvItem->object().cNameSect(), *pInvItem);

		if(m_b_FitToHeight)
		{
			UIDesc->SetWndSize				(Fvector2().set(UIDesc->GetWndSize().x, UIDesc->GetPadSize().y) );
			Fvector2 new_size;
			new_size.x						= GetWndSize().x;
			new_size.y						= UIDesc->GetWndPos().y+UIDesc->GetWndSize().y+20.0f;
			new_size.x						= _max(105.0f, new_size.x);
			new_size.y						= _max(105.0f, new_size.y);
			
			SetWndSize						(new_size);
			if(UIBackground)
				UIBackground->SetWndSize	(new_size);
		}

		UIDesc->ScrollToBegin				();
	}
	if(UIItemImage)
	{
		// «агружаем картинку
		UIItemImage->SetShader				(InventoryUtilities::GetEquipmentIconsShader());

		Irect item_grid_rect				= pInvItem->GetInvGridRect();
		Frect texture_rect;
		texture_rect.lt.set					(item_grid_rect.x1*INV_GRID_WIDTH,	item_grid_rect.y1*INV_GRID_HEIGHT);
		texture_rect.rb.set					(item_grid_rect.x2*INV_GRID_WIDTH,	item_grid_rect.y2*INV_GRID_HEIGHT);
		texture_rect.rb.add					(texture_rect.lt);
		UIItemImage->GetUIStaticItem().SetTextureRect(texture_rect);
		UIItemImage->TextureOn				();
		UIItemImage->SetStretchTexture		(true);
		Fvector2 v_r						= { item_grid_rect.x2*INV_GRID_WIDTH2,	
												item_grid_rect.y2*INV_GRID_HEIGHT2};
		
		v_r.x								*= UI().get_current_kx();

		UIItemImage->GetUIStaticItem().SetSize	(v_r);
		UIItemImage->SetWidth					(v_r.x);
		UIItemImage->SetHeight					(v_r.y);
	}
}

void CUIItemInfo::TryAddConditionInfo( CInventoryItem& pInvItem, CInventoryItem* pCompareItem )
{
	CWeapon*		weapon = smart_cast<CWeapon*>( &pInvItem );
	CCustomOutfit*	outfit = smart_cast<CCustomOutfit*>( &pInvItem );
	if ( weapon || outfit )
	{
//		UIConditionWnd->SetInfo( pCompareItem, pInvItem );
//		UIDesc->AddWindow( UIConditionWnd, false );
	}
}

void CUIItemInfo::TryAddWpnInfo( CInventoryItem& pInvItem, CInventoryItem* pCompareItem )
{
	// skif : (не отображать характеристики оружи€ у ножей)
	CWeaponKnife* knife = smart_cast<CWeaponKnife*>( &pInvItem );
	if (knife) return;
	// end
	if ( UIWpnParams->Check( pInvItem.object().cNameSect() ) )
	{
		UIWpnParams->SetInfo( pCompareItem, pInvItem );
		UIDesc->AddWindow( UIWpnParams, false );
	}
}

void CUIItemInfo::TryAddArtefactInfo	(const shared_str& af_section)
{
	if ( UIArtefactParams->Check( af_section ) )
	{
		UIArtefactParams->SetInfo( af_section );
		UIDesc->AddWindow( UIArtefactParams, false );
	}
}

void CUIItemInfo::TryAddOutfitInfo( CInventoryItem& pInvItem, CInventoryItem* pCompareItem )
{
	CCustomOutfit* outfit = smart_cast<CCustomOutfit*>(&pInvItem);
	CHelmet* helmet = smart_cast<CHelmet*>(&pInvItem);
	if ( outfit && UIOutfitInfo )
	{
		CCustomOutfit* comp_outfit = smart_cast<CCustomOutfit*>(pCompareItem);
		UIOutfitInfo->UpdateInfo( outfit, comp_outfit );
		UIDesc->AddWindow( UIOutfitInfo, false );
	}
	if ( helmet && UIOutfitInfo )
	{
		CHelmet* comp_helmet = smart_cast<CHelmet*>(pCompareItem);
		UIOutfitInfo->UpdateInfo( helmet, comp_helmet );
		UIDesc->AddWindow( UIOutfitInfo, false );
	}

}

void CUIItemInfo::TryAddUpgradeInfo( CInventoryItem& pInvItem )
{
	if ( pInvItem.upgardes().size() && UIProperties )
	{
		UIProperties->set_item_info( pInvItem );
		UIDesc->AddWindow( UIProperties, false );
	}
}

void CUIItemInfo::TryAddBoosterInfo(CInventoryItem& pInvItem)
{
	CEatableItem* food = smart_cast<CEatableItem*>(&pInvItem);
	if ( food && UIBoosterInfo )
	{
		UIBoosterInfo->SetInfo(pInvItem.object().cNameSect());
		UIDesc->AddWindow( UIBoosterInfo, false );
	}
}

// skif : (батарейка дл€ фонарика)
void CUIItemInfo::TryAddTorchBatareyInfo(const shared_str& section, CInventoryItem& pInvItem)
{
	if ( UITorchBatareyInfo->Check( section ) )
	{
		UITorchBatareyInfo->SetInfo( section, pInvItem );
		UIDesc->AddWindow( UITorchBatareyInfo, false );
	}
}
// end
void CUIItemInfo::TryAddCanisterInfo(const shared_str& section, CInventoryItem& pInvItem)
{
	if (UICanisterInfo->Check( section ) )
	{
		UICanisterInfo->SetInfo( section, pInvItem );
		UIDesc->AddWindow(UICanisterInfo, false );
	}
}
void CUIItemInfo::TryAddCarToolkitInfo(const shared_str& section, CInventoryItem& pInvItem)
{
	if (UICarToolkitInfo->Check( section ) )
	{
		UICarToolkitInfo->SetInfo( section, pInvItem );
		UIDesc->AddWindow(UICarToolkitInfo, false );
	}
}

void CUIItemInfo::Draw()
{
	if(m_pInvItem)
		inherited::Draw();
}

// skif : (батарейка дл€ фонарика)

CUITorchBatareyInfo::CUITorchBatareyInfo()
{
	m_torch_batarey_power = NULL;
	m_torch_batarey_power_reduction_speed = NULL;
}

CUITorchBatareyInfo::~CUITorchBatareyInfo()
{
	xr_delete(m_torch_batarey_power);
	xr_delete(m_torch_batarey_power_reduction_speed);
	xr_delete(m_Prop_line);
}

void CUITorchBatareyInfo::InitFromXml(CUIXml& xml)
{
	LPCSTR base	= "device_params";
	XML_NODE* stored_root = xml.GetLocalRoot();
	XML_NODE* base_node   = xml.NavigateToNode( base, 0 );
	if(!base_node)
		return;

	CUIXmlInit::InitWindow(xml, base, 0, this);
	xml.SetLocalRoot(base_node);
	
	m_Prop_line = xr_new<CUIStatic>();
	AttachChild(m_Prop_line);
	m_Prop_line->SetAutoDelete(false);	
	CUIXmlInit::InitStatic(xml, "prop_line", 0, m_Prop_line);

	m_torch_batarey_power = xr_new<UIBoosterInfoItem>();
	m_torch_batarey_power->Init(xml, "device_torch_batarey_power");
	m_torch_batarey_power->SetAutoDelete(false);
	LPCSTR name = CStringTable().translate("ui_inv_torch_batarey_power").c_str();
	m_torch_batarey_power->SetCaption(name);
	xml.SetLocalRoot( base_node );

	m_torch_batarey_power_reduction_speed = xr_new<UIBoosterInfoItem>();
	m_torch_batarey_power_reduction_speed->Init(xml, "device_torch_batarey_power_reduction_speed");
	m_torch_batarey_power_reduction_speed->SetAutoDelete(false);
	name = CStringTable().translate("ui_inv_torch_batarey_power_reduction_speed").c_str();
	m_torch_batarey_power_reduction_speed->SetCaption(name);
	xml.SetLocalRoot( base_node );

	xml.SetLocalRoot( stored_root );
}

bool CUITorchBatareyInfo::Check(const shared_str& section)
{
	return !!pSettings->line_exist(section, "power_reduction_speed");
}

void CUITorchBatareyInfo::SetInfo( shared_str const& section, CInventoryItem& pInvItem )
{
	DetachAll();
	AttachChild( m_Prop_line );
	
	if (!Actor())
		return;

	Fvector2 pos;
	float val;
	float h = m_Prop_line->GetWndPos().y+m_Prop_line->GetWndSize().y;

	if(pSettings->line_exist(section.c_str(), "power"))
	{
		CTorchBatarey* bt = smart_cast<CTorchBatarey*>(&pInvItem);
		val = bt->GetPower();
		if (!fis_zero(val))
		{
			string64 str;
			xr_sprintf( str, "%.1f%c", val, char(37) );
			m_torch_batarey_power->m_value->SetText(str);
		}
		else
			m_torch_batarey_power->m_value->SetText(CStringTable().translate("ui_inv_torch_batarey_discharged").c_str());
		pos.set(m_torch_batarey_power->GetWndPos());
		pos.y = h;
		m_torch_batarey_power->SetWndPos(pos);
		h += m_torch_batarey_power->GetWndSize().y;
		AttachChild(m_torch_batarey_power);
	}
	if(pSettings->line_exist(section.c_str(), "power_reduction_speed"))
	{
		val = pSettings->r_float(section, "power_reduction_speed");
		string64 str;
		xr_sprintf( str, "%.3f", val);
		m_torch_batarey_power_reduction_speed->m_value->SetText(str);
		pos.set(m_torch_batarey_power_reduction_speed->GetWndPos());
		pos.y = h;
		m_torch_batarey_power_reduction_speed->SetWndPos(pos);
		h += m_torch_batarey_power_reduction_speed->GetWndSize().y;
		AttachChild(m_torch_batarey_power_reduction_speed);
	}
	SetHeight(h);
}
// end

// skif

CUICanisterInfo::CUICanisterInfo()
{
	m_fuel = NULL;
	m_volume = NULL;
}

CUICanisterInfo::~CUICanisterInfo()
{
	xr_delete(m_fuel);
	xr_delete(m_volume);
	xr_delete(m_Prop_line);
}

void CUICanisterInfo::InitFromXml(CUIXml& xml)
{
	LPCSTR base	= "device_params";
	XML_NODE* stored_root = xml.GetLocalRoot();
	XML_NODE* base_node   = xml.NavigateToNode( base, 0 );
	if(!base_node)
		return;

	CUIXmlInit::InitWindow(xml, base, 0, this);
	xml.SetLocalRoot(base_node);
	
	m_Prop_line = xr_new<CUIStatic>();
	AttachChild(m_Prop_line);
	m_Prop_line->SetAutoDelete(false);	
	CUIXmlInit::InitStatic(xml, "prop_line", 0, m_Prop_line);

	m_fuel = xr_new<UIBoosterInfoItem>();
	m_fuel->Init(xml, "canister_fuel");
	m_fuel->SetAutoDelete(false);
	LPCSTR name = CStringTable().translate("ui_inv_canister_fuel").c_str();
	m_fuel->SetCaption(name);
	xml.SetLocalRoot( base_node );

	m_volume = xr_new<UIBoosterInfoItem>();
	m_volume->Init(xml, "canister_volume");
	m_volume->SetAutoDelete(false);
	name = CStringTable().translate("ui_inv_canister_volume").c_str();
	m_volume->SetCaption(name);
	xml.SetLocalRoot( base_node );

	xml.SetLocalRoot( stored_root );
}

bool CUICanisterInfo::Check(const shared_str& section)
{
	return !!pSettings->line_exist(section, "volume");
}

void CUICanisterInfo::SetInfo( shared_str const& section, CInventoryItem& pInvItem )
{
	DetachAll();
	AttachChild( m_Prop_line );
	
	if (!Actor())
		return;

	Fvector2 pos;
	float val;
	float h = m_Prop_line->GetWndPos().y+m_Prop_line->GetWndSize().y;
	CCanister* c = smart_cast<CCanister*>(&pInvItem);

	if(pSettings->line_exist(section.c_str(), "volume"))
	{
		val = c->volume;
		if (!fis_zero(val))
		{
			string64 fuel;
			xr_sprintf(fuel, "%.1f", val);
			LPCSTR fuel_level_text;
			STRCONCAT(fuel_level_text, fuel, CStringTable().translate("ui_litr").c_str());
			m_volume->m_value->SetText(fuel_level_text);
		}
		pos.set(m_volume->GetWndPos());
		pos.y = h;
		m_volume->SetWndPos(pos);
		h += m_volume->GetWndSize().y;
		AttachChild(m_volume);
	}
	val = c->GetFuel();
	if (!fis_zero(val))
	{
		string64 fuel;
		xr_sprintf(fuel, "%.2f", val);
		LPCSTR fuel_level_text;
		STRCONCAT(fuel_level_text, fuel, CStringTable().translate("ui_litr").c_str());
		m_fuel->m_value->SetText(fuel_level_text);
	}
	else
		m_fuel->m_value->SetText(CStringTable().translate("ui_inv_canister_empty").c_str());
	pos.set(m_fuel->GetWndPos());
	pos.y = h;
	m_fuel->SetWndPos(pos);
	h += m_fuel->GetWndSize().y;
	AttachChild(m_fuel);
	SetHeight(h);
}




CUICarToolkitInfo::CUICarToolkitInfo()
{
	m_applying = NULL;
	m_capacity = NULL;
}

CUICarToolkitInfo::~CUICarToolkitInfo()
{
	xr_delete(m_applying);
	xr_delete(m_capacity);
	xr_delete(m_Prop_line);
}

void CUICarToolkitInfo::InitFromXml(CUIXml& xml)
{
	LPCSTR base	= "device_params";
	XML_NODE* stored_root = xml.GetLocalRoot();
	XML_NODE* base_node   = xml.NavigateToNode( base, 0 );
	if(!base_node)
		return;

	CUIXmlInit::InitWindow(xml, base, 0, this);
	xml.SetLocalRoot(base_node);
	
	m_Prop_line = xr_new<CUIStatic>();
	AttachChild(m_Prop_line);
	m_Prop_line->SetAutoDelete(false);	
	CUIXmlInit::InitStatic(xml, "prop_line", 0, m_Prop_line);

	m_applying = xr_new<UIBoosterInfoItem>();
	m_applying->Init(xml, "car_toolkit_applying");
	m_applying->SetAutoDelete(false);
	LPCSTR name = CStringTable().translate("ui_inv_car_toolkit_applying").c_str();
	m_applying->SetCaption(name);
	xml.SetLocalRoot( base_node );

	m_capacity = xr_new<UIBoosterInfoItem>();
	m_capacity->Init(xml, "car_toolkit_capacity");
	m_capacity->SetAutoDelete(false);
	name = CStringTable().translate("ui_inv_car_toolkit_capacity").c_str();
	m_capacity->SetCaption(name);
	xml.SetLocalRoot( base_node );

	xml.SetLocalRoot( stored_root );
}

bool CUICarToolkitInfo::Check(const shared_str& section)
{
	return !!pSettings->line_exist(section, "applying");
}

void CUICarToolkitInfo::SetInfo( shared_str const& section, CInventoryItem& pInvItem )
{
	DetachAll();
	AttachChild( m_Prop_line );
	
	if (!Actor())
		return;

	Fvector2 pos;
	u8 val;
	float h = m_Prop_line->GetWndPos().y+m_Prop_line->GetWndSize().y;
	CCarToolkit* c = smart_cast<CCarToolkit*>(&pInvItem);

	if(pSettings->line_exist(section.c_str(), "capacity"))
	{
		val = c->capacity;
		if (!fis_zero(val))
		{
			string64 str;
			xr_sprintf(str, "%d%c", val, char(37));
			m_capacity->m_value->SetText(str);
		}
		pos.set(m_capacity->GetWndPos());
		pos.y = h;
		m_capacity->SetWndPos(pos);
		h += m_capacity->GetWndSize().y;
		AttachChild(m_capacity);
	}
	val = c->applying_remain;
	string64 text2;
	xr_sprintf(text2, "%d", val);
	m_applying->m_value->SetText(text2);
	pos.set(m_applying->GetWndPos());
	pos.y = h;
	m_applying->SetWndPos(pos);
	h += m_applying->GetWndSize().y;
	AttachChild(m_applying);
	SetHeight(h);
}
// end