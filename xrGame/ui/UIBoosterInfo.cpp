#include "stdafx.h"
#include "UIBoosterInfo.h"
#include "UIStatic.h"
#include "object_broker.h"
#include "../EntityCondition.h"
#include "..\actor.h"
#include "../ActorCondition.h"
#include "UIXmlInit.h"
#include "UIHelper.h"
#include "../string_table.h"

CUIBoosterInfo::CUIBoosterInfo()
{
	for(u32 i = 0; i < eBoostExplImmunity; ++i)
	{
		m_booster_items[i] = NULL;
	}
	m_booster_satiety = NULL;
	//skif : (потребности актора)
	m_booster_thirst = NULL;
	m_booster_sleep = NULL;
	//end
	//skif : (иконки у бустеров)
	m_booster_alcohol = NULL;
	m_booster_health_add = NULL;
	m_booster_radio_add = NULL;
	m_booster_portions_num = NULL;
	m_booster_nausea = NULL;
	m_booster_unnausea = NULL;
	m_booster_power_add = NULL;
	m_booster_bleed_add = NULL;
	//end
	m_booster_anabiotic = NULL;
	m_booster_time = NULL;
}

CUIBoosterInfo::~CUIBoosterInfo()
{
	delete_data(m_booster_items);
	xr_delete(m_booster_satiety);
	//skif : (потребности актора)
	xr_delete(m_booster_thirst);
	xr_delete(m_booster_sleep);
	//end
	//skif : (иконки у бустеров)
	xr_delete(m_booster_alcohol);  
	xr_delete(m_booster_health_add);  
	xr_delete(m_booster_radio_add);  
	xr_delete(m_booster_portions_num);  
	xr_delete(m_booster_nausea);  
	xr_delete(m_booster_unnausea);  
	xr_delete(m_booster_power_add);  
	xr_delete(m_booster_bleed_add);  
	//end
	xr_delete(m_booster_anabiotic);
	xr_delete(m_booster_time);
	xr_delete(m_Prop_line);
}

LPCSTR boost_influence_caption[] =
{
	"ui_inv_health",
	"ui_inv_power",
	"ui_inv_radiation",
	"ui_inv_bleeding",
	"ui_inv_outfit_additional_weight",
	"ui_inv_outfit_radiation_protection",
	"ui_inv_outfit_telepatic_protection",
	"ui_inv_outfit_chemical_burn_protection",
	"ui_inv_outfit_burn_protection",  //skif : (термозащита у бустеров)
	"ui_inv_outfit_shock_protection",  //skif : (электрозащита у бустеров)
	"ui_inv_outfit_jump_speed",  //skif : (высота прыжков у бустеров)
	"ui_inv_psy_health",  //skif : (восстановление пси-здоровья у бустеров)
	"ui_inv_outfit_burn_immunity",
	"ui_inv_outfit_shock_immunity",
	"ui_inv_outfit_radiation_immunity",
	"ui_inv_outfit_telepatic_immunity",
	"ui_inv_outfit_chemical_burn_immunity"
};

void CUIBoosterInfo::InitFromXml(CUIXml& xml)
{
	LPCSTR base	= "booster_params";
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

	for(u32 i = 0; i < eBoostExplImmunity; ++i)
	{
		m_booster_items[i] = xr_new<UIBoosterInfoItem>();
		m_booster_items[i]->Init(xml, ef_boosters_section_names[i]);
		m_booster_items[i]->SetAutoDelete(false);

		LPCSTR name = CStringTable().translate(boost_influence_caption[i]).c_str();
		m_booster_items[i]->SetCaption(name);

		xml.SetLocalRoot(base_node);
	}

	m_booster_satiety = xr_new<UIBoosterInfoItem>();
	m_booster_satiety->Init(xml, "boost_satiety");
	m_booster_satiety->SetAutoDelete(false);
	LPCSTR name = CStringTable().translate("ui_inv_satiety").c_str();
	m_booster_satiety->SetCaption(name);
	xml.SetLocalRoot( base_node );
	
	//skif : (потребности актора)
	m_booster_thirst = xr_new<UIBoosterInfoItem>();
	m_booster_thirst->Init(xml, "boost_thirst");
	m_booster_thirst->SetAutoDelete(false);
	name = CStringTable().translate("ui_inv_thirst").c_str();
	m_booster_thirst->SetCaption(name);
	xml.SetLocalRoot( base_node );

	m_booster_sleep = xr_new<UIBoosterInfoItem>();
	m_booster_sleep->Init(xml, "boost_sleep");
	m_booster_sleep->SetAutoDelete(false);
	name = CStringTable().translate("ui_inv_sleep").c_str();
	m_booster_sleep->SetCaption(name);
	xml.SetLocalRoot( base_node );
	//end
	
	//skif : (иконки у бустеров)
	m_booster_alcohol = xr_new<UIBoosterInfoItem>();
	m_booster_alcohol->Init(xml, "boost_alcohol");
	m_booster_alcohol->SetAutoDelete(false);
	name = CStringTable().translate("ui_inv_alcohol").c_str();
	m_booster_alcohol->SetCaption(name);
	xml.SetLocalRoot( base_node );
	
	m_booster_health_add = xr_new<UIBoosterInfoItem>();
	m_booster_health_add->Init(xml, "boost_eat_health");
	m_booster_health_add->SetAutoDelete(false);
	name = CStringTable().translate("ui_inv_health_add").c_str();
	m_booster_health_add->SetCaption(name);
	xml.SetLocalRoot( base_node );
	
	m_booster_radio_add = xr_new<UIBoosterInfoItem>();
	m_booster_radio_add->Init(xml, "boost_eat_radiation");
	m_booster_radio_add->SetAutoDelete(false);
	name = CStringTable().translate("ui_inv_radio_add").c_str();
	m_booster_radio_add->SetCaption(name);
	xml.SetLocalRoot( base_node );
	
	m_booster_portions_num = xr_new<UIBoosterInfoItem>();
	m_booster_portions_num->Init(xml, "boost_portions_num");
	m_booster_portions_num->SetAutoDelete(false);
	name = CStringTable().translate("ui_inv_portions_num").c_str();
	m_booster_portions_num->SetCaption(name);
	xml.SetLocalRoot( base_node );
	
	m_booster_nausea = xr_new<UIBoosterInfoItem>();
	m_booster_nausea->Init(xml, "boost_nausea");
	m_booster_nausea->SetAutoDelete(false);
	name = CStringTable().translate("ui_inv_nausea").c_str();
	m_booster_nausea->SetCaption(name);
	xml.SetLocalRoot( base_node );
	
	m_booster_unnausea = xr_new<UIBoosterInfoItem>();
	m_booster_unnausea->Init(xml, "boost_unnausea");
	m_booster_unnausea->SetAutoDelete(false);
	name = CStringTable().translate("ui_inv_unnausea").c_str();
	m_booster_unnausea->SetCaption(name);
	xml.SetLocalRoot( base_node );
	
	m_booster_power_add = xr_new<UIBoosterInfoItem>();
	m_booster_power_add->Init(xml, "boost_power_add");
	m_booster_power_add->SetAutoDelete(false);
	name = CStringTable().translate("ui_inv_power_add").c_str();
	m_booster_power_add->SetCaption(name);
	xml.SetLocalRoot( base_node );
	
	m_booster_bleed_add = xr_new<UIBoosterInfoItem>();
	m_booster_bleed_add->Init(xml, "boost_bleed_add");
	m_booster_bleed_add->SetAutoDelete(false);
	name = CStringTable().translate("ui_inv_bleed_add").c_str();
	m_booster_bleed_add->SetCaption(name);
	xml.SetLocalRoot( base_node );
	//end

	m_booster_anabiotic = xr_new<UIBoosterInfoItem>();
	m_booster_anabiotic->Init(xml, "boost_anabiotic");
	m_booster_anabiotic->SetAutoDelete(false);
	name = CStringTable().translate("ui_inv_survive_surge").c_str();
	m_booster_anabiotic->SetCaption(name);
	xml.SetLocalRoot( base_node );

	m_booster_time = xr_new<UIBoosterInfoItem>();
	m_booster_time->Init(xml, "boost_time");
	m_booster_time->SetAutoDelete(false);
	name = CStringTable().translate("ui_inv_effect_time").c_str();
	m_booster_time->SetCaption(name);

	xml.SetLocalRoot( stored_root );
}

void CUIBoosterInfo::SetInfo( shared_str const& section )
{
	DetachAll();
	AttachChild( m_Prop_line );

	CActor* actor = smart_cast<CActor*>( Level().CurrentViewEntity() );
	if ( !actor )
	{
		return;
	}

	CEntityCondition::BOOSTER_MAP boosters = actor->conditions().GetCurBoosterInfluences();

	float val = 0.0f, max_val = 1.0f;
	Fvector2 pos;
	float h = m_Prop_line->GetWndPos().y+m_Prop_line->GetWndSize().y;

	for (u32 i = 0; i < eBoostExplImmunity; ++i)
	{
		if(pSettings->line_exist(section.c_str(), ef_boosters_section_names[i]))
		{
			val	= pSettings->r_float(section, ef_boosters_section_names[i]);
			if(fis_zero(val))
				continue;

			/*EBoostParams type = (EBoostParams)i;
			switch(type)
			{
				case eBoostHpRestore: val *= 10000; break;
				case eBoostPsyHpRestore: val *= 10000; break; //skif : (восстановление пси-здоровья у бустеров)	
				case eBoostPowerRestore: val *= 1000; break;
				case eBoostBleedingRestore: val *= 10000; break;
				case eBoostJumpSpeed: val *= 10; break; //skif : (высота прыжков у бустеров)
				case eBoostMaxWeight: break;
				case eBoostRadiationRestore: val *= -10000; break;
				case eBoostBurnImmunity: break;
				case eBoostShockImmunity: break;
				case eBoostRadiationImmunity: break; 
				case eBoostRadiationProtection: val *= 1000; break;
				case eBoostTelepaticImmunity: break;
				case eBoostTelepaticProtection: val *= 100;  break;
				case eBoostChemicalBurnImmunity: break;
				case eBoostChemicalBurnProtection: val *= 1000;  break;
				case eBoostBurnProtection: val *= 1000; break;	//skif : (термозащита у бустеров)
				case eBoostElectroProtection: val *= 1000; break;	//skif : (электрозащита у бустеров)
			}*/
			//val /= max_val;
			m_booster_items[i]->SetValue(val);

			pos.set(m_booster_items[i]->GetWndPos());
			pos.y = h;
			m_booster_items[i]->SetWndPos(pos);

			h += m_booster_items[i]->GetWndSize().y;
			AttachChild(m_booster_items[i]);
		}
	}

	if(pSettings->line_exist(section.c_str(), "eat_satiety"))
	{
		val	= pSettings->r_float(section, "eat_satiety");
		if(!fis_zero(val))
		{
			m_booster_satiety->SetValue(val);
			pos.set(m_booster_satiety->GetWndPos());
			pos.y = h;
			m_booster_satiety->SetWndPos(pos);

			h += m_booster_satiety->GetWndSize().y;
			AttachChild(m_booster_satiety);
		}
	}
	
	//skif : (потребности актора)
	if(pSettings->line_exist(section.c_str(), "eat_thirst"))
	{
		val	= pSettings->r_float(section, "eat_thirst");
		if(!fis_zero(val))
		{
			m_booster_thirst->SetValue(val);
			pos.set(m_booster_thirst->GetWndPos());
			pos.y = h;
			m_booster_thirst->SetWndPos(pos);

			h += m_booster_thirst->GetWndSize().y;
			AttachChild(m_booster_thirst);
		}
	}
	
	if(pSettings->line_exist(section.c_str(), "eat_sleep"))
	{
		val	= pSettings->r_float(section, "eat_sleep");
		if(!fis_zero(val))
		{
			m_booster_sleep->SetValue(val);
			pos.set(m_booster_sleep->GetWndPos());
			pos.y = h;
			m_booster_sleep->SetWndPos(pos);

			h += m_booster_sleep->GetWndSize().y;
			AttachChild(m_booster_sleep);
		}
	}
	//end
	
	//skif : (иконки у бустеров)
	if(pSettings->line_exist(section.c_str(), "eat_alcohol"))
	{
		val	= pSettings->r_float(section, "eat_alcohol");
		if(!fis_zero(val))
		{
			m_booster_alcohol->SetValue(val);
			pos.set(m_booster_alcohol->GetWndPos());
			pos.y = h;
			m_booster_alcohol->SetWndPos(pos);

			h += m_booster_alcohol->GetWndSize().y;
			AttachChild(m_booster_alcohol);
		}
	}
	
	if(pSettings->line_exist(section.c_str(), "eat_health"))
	{
		val	= pSettings->r_float(section, "eat_health");
		if(!fis_zero(val))
		{
			m_booster_health_add->SetValue(val);
			pos.set(m_booster_health_add->GetWndPos());
			pos.y = h;
			m_booster_health_add->SetWndPos(pos);

			h += m_booster_health_add->GetWndSize().y;
			AttachChild(m_booster_health_add);
		}
	}
	
	if(pSettings->line_exist(section.c_str(), "eat_radiation"))
	{
		val	= pSettings->r_float(section, "eat_radiation");
		if(!fis_zero(val))
		{
			m_booster_radio_add->SetValue(val);
			pos.set(m_booster_radio_add->GetWndPos());
			pos.y = h;
			m_booster_radio_add->SetWndPos(pos);

			h += m_booster_radio_add->GetWndSize().y;
			AttachChild(m_booster_radio_add);
		}
	}
	
	if(pSettings->line_exist(section.c_str(), "eat_power"))
	{
		val	= pSettings->r_float(section, "eat_power");
		if(!fis_zero(val))
		{
			m_booster_power_add->SetValue(val);
			pos.set(m_booster_power_add->GetWndPos());
			pos.y = h;
			m_booster_power_add->SetWndPos(pos);

			h += m_booster_power_add->GetWndSize().y;
			AttachChild(m_booster_power_add);
		}
	}
	
	if(pSettings->line_exist(section.c_str(), "eat_bleed"))
	{
		val	= pSettings->r_float(section, "eat_bleed");
		if(!fis_zero(val))
		{
			m_booster_bleed_add->SetValue(val);
			pos.set(m_booster_bleed_add->GetWndPos());
			pos.y = h;
			m_booster_bleed_add->SetWndPos(pos);

			h += m_booster_bleed_add->GetWndSize().y;
			AttachChild(m_booster_bleed_add);
		}
	}
	
	if(pSettings->line_exist(section.c_str(), "portions_remain"))
	{
		val	= pSettings->r_float(section, "portions_remain");
		if(!fis_zero(val))
		{
			m_booster_portions_num->SetUnsignedValue(val);
			pos.set(m_booster_portions_num->GetWndPos());
			pos.y = h;
			m_booster_portions_num->SetWndPos(pos);

			h += m_booster_portions_num->GetWndSize().y;
			AttachChild(m_booster_portions_num);
		}
	}
	
	if(pSettings->line_exist(section.c_str(), "nausea"))
	{
		val	= pSettings->r_float(section, "nausea");
		if(val>0)
		{
			pos.set(m_booster_nausea->GetWndPos());
			pos.y = h;
			m_booster_nausea->SetWndPos(pos);

			h += m_booster_nausea->GetWndSize().y;
			AttachChild(m_booster_nausea);
		}
	}
	
	if(pSettings->line_exist(section.c_str(), "unnausea"))
	{
		val	= pSettings->r_float(section, "unnausea");
		if(!fis_zero(val))
		{
			pos.set(m_booster_unnausea->GetWndPos());
			pos.y = h;
			m_booster_unnausea->SetWndPos(pos);

			h += m_booster_unnausea->GetWndSize().y;
			AttachChild(m_booster_unnausea);
		}
	}
	//end

	if(!xr_strcmp(section.c_str(), "drug_anabiotic"))
	{
		pos.set(m_booster_anabiotic->GetWndPos());
		pos.y = h;
		m_booster_anabiotic->SetWndPos(pos);

		h += m_booster_anabiotic->GetWndSize().y;
		AttachChild(m_booster_anabiotic);
	}

	if(pSettings->line_exist(section.c_str(), "boost_time"))
	{
		val	= pSettings->r_float(section, "boost_time");
		if(!fis_zero(val))
		{
			m_booster_time->SetValue(val);
			pos.set(m_booster_time->GetWndPos());
			pos.y = h;
			m_booster_time->SetWndPos(pos);

			h += m_booster_time->GetWndSize().y;
			AttachChild(m_booster_time);
		}
	}
	SetHeight(h);
}

/// ----------------------------------------------------------------

UIBoosterInfoItem::UIBoosterInfoItem()
{
	m_caption				= NULL;
	m_value					= NULL;
	m_magnitude				= 1.0f;
	m_show_sign				= false;
	m_sign_inverse			= false;
	
	m_unit_str._set			("");
	m_texture_minus._set	("");
	m_texture_plus._set		("");
}

UIBoosterInfoItem::~UIBoosterInfoItem()
{
}

void UIBoosterInfoItem::Init(CUIXml& xml, LPCSTR section)
{
	CUIXmlInit::InitWindow(xml, section, 0, this);
	xml.SetLocalRoot(xml.NavigateToNode(section));

	m_caption   = UIHelper::CreateStatic(xml, "caption", this);
	m_value     = UIHelper::CreateTextWnd(xml, "value",   this);
	m_magnitude = 0;
	m_show_sign = (xml.ReadAttribInt("value", 0, "show_sign", 1) == 1);
	m_sign_inverse = (xml.ReadAttribInt("value", 0, "sign_inverse", 0) == 1);
	
	LPCSTR unit_str = xml.ReadAttrib("value", 0, "unit_str", "");
	m_unit_str._set(CStringTable().translate(unit_str));
	
	LPCSTR texture_minus = xml.Read("texture_minus", 0, "");
	if(texture_minus && xr_strlen(texture_minus))
	{
		m_texture_minus._set(texture_minus);
		
		LPCSTR texture_plus = xml.Read("caption:texture", 0, "");
		m_texture_plus._set(texture_plus);
		VERIFY(m_texture_plus.size());
	}
}

void UIBoosterInfoItem::SetCaption(LPCSTR name)
{
	m_caption->TextItemControl()->SetText(name);
}

void UIBoosterInfoItem::SetValue(float value)
{
	string32 buf;
	if(m_show_sign)
		xr_sprintf(buf, "%+.0f", value);
	else
		xr_sprintf(buf, "%.0f", value);
	
	LPSTR str;
	if(m_unit_str.size())
		STRCONCAT(str, buf, " ", m_unit_str.c_str());
	else
		STRCONCAT(str, buf);

	m_value->SetText(str);

	bool positive = (value >= 0.0f);
	if (value == 0.0f)
	{
		m_value->SetTextColor(color_rgba(170, 170, 170, 255));
	}
	else if (value > 0.0f)
	{
		m_value->SetTextColor(m_sign_inverse ? color_rgba(255, 0, 0, 255) : color_rgba(0, 255, 0, 255));
	}
	else if (value < 0.0f)
	{
		m_value->SetTextColor(m_sign_inverse ? color_rgba(0, 255, 0, 255) : color_rgba(255, 0, 0, 255));
	}

	if(m_texture_minus.size())
	{
		if (positive)
			m_caption->InitTexture(m_texture_plus.c_str());
		else
			m_caption->InitTexture(m_texture_minus.c_str());
	}
}


//skif : (иконки у бустеров)
void UIBoosterInfoItem::SetUnsignedValue(float value)
{
	string32 buf;
	xr_sprintf(buf, "%.0f", value);
	
	LPSTR str;
	if(m_unit_str.size())
		STRCONCAT(str, buf, " ", m_unit_str.c_str());
	else
		STRCONCAT(str, buf);

	m_value->SetText(str);

	bool positive = (value >= 0.0f);
	if (value == 0.0f)
	{
		m_value->SetTextColor(color_rgba(170, 170, 170, 255));
	}
	else if (value > 0.0f)
	{
		m_value->SetTextColor(m_sign_inverse ? color_rgba(255, 0, 0, 255) : color_rgba(0, 255, 0, 255));
	}
	else if (value < 0.0f)
	{
		m_value->SetTextColor(m_sign_inverse ? color_rgba(0, 255, 0, 255) : color_rgba(255, 0, 0, 255));
	}

	if(m_texture_minus.size())
	{
		if(positive)
			m_caption->InitTexture(m_texture_plus.c_str());
		else
			m_caption->InitTexture(m_texture_minus.c_str());
	}
}
//end