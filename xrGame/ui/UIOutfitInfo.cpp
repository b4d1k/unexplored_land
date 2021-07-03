#include "StdAfx.h"
#include "UIOutfitInfo.h"
#include "UIXmlInit.h"
#include "UIStatic.h"
#include "UIDoubleProgressBar.h"
#include "UIHelper.h"

#include "..\CustomOutfit.h"
#include "..\ActorHelmet.h"
#include "..\string_table.h"
#include "..\actor.h"
#include "..\ActorCondition.h"
#include "..\player_hud.h"


LPCSTR immunity_names[]=
{
	"burn_immunity",
	"shock_immunity",
	"chemical_burn_immunity",
	"radiation_immunity",
	"telepatic_immunity",
	"wound_immunity",		
	"fire_wound_immunity",
	"strike_immunity", // skif : (взрыв, удар в описании бронежилетов)
	"explosion_immunity", // skif : (взрыв, удар в описании бронежилетов) 
	"helmet_fire_wound_immunity", // skif : (защита головы в описании бронежилетов)
	"camouflage_coeff", // skif : (маскировка в описании бронежилетов)
	"power_loss_coeff", // skif : (затраты сил в описании бронежилетов)
	"artefact_count", // skif : (кол-во контейнеров под арты в описании бронежилетов)
	"additional_weight", // skif : (доп переносимый вес в описании бронежилетов)
	//skif : (доп св-ва броников)
	"health_restore_speed",			
	"satiety_restore_speed",	
	"power_restore_speed",			
	"bleeding_restore_speed",		
	"radiation_restore_speed",		
	"thirst_restore_speed",		
	"sleep_restore_speed",		
	"psy_health_restore_speed",	
	"helmet_available",
	"sprint_allowed",
	"nearest_enemy_show",
	"nightvision_installed",
	"walk_accel",
	"jump_speed"
	// end
};

LPCSTR immunity_st_names[]=
{
	"ui_inv_outfit_burn_protection",
	"ui_inv_outfit_shock_protection",
	"ui_inv_outfit_chemical_burn_protection",
	"ui_inv_outfit_radiation_protection",
	"ui_inv_outfit_telepatic_protection",
	"ui_inv_outfit_wound_protection",
	"ui_inv_outfit_fire_wound_protection",
 	"ui_inv_outfit_strike_protection", // skif : (взрыв, удар в описании бронежилетов)
 	"ui_inv_outfit_explosion_protection", // skif : (взрыв, удар в описании бронежилетов)
	"ui_inv_outfit_helmet_fire_wound_protection", // skif : (защита головы в описании бронежилетов)
	"ui_inv_outfit_camouflage_coeff", // skif : (маскировка в описании бронежилетов)
	"ui_inv_outfit_power_loss_coeff", // skif : (затраты сил в описании бронежилетов)
	"ui_inv_outfit_artefact_count", // skif : (кол-во контейнеров под арты в описании бронежилетов)
	"ui_inv_outfit_additional_weight", // skif : (доп переносимый вес в описании бронежилетов)
	//skif : (доп св-ва броников)
	"ui_inv_health",
	"ui_inv_satiety",
	"ui_inv_power",
	"ui_inv_bleeding",
	"ui_inv_radiation",
	"ui_inv_thirst",
	"ui_inv_sleep",
	"ui_inv_psy_health",
	"ui_inv_helmet_available",
	"ui_inv_sprint_allowed",
	"ui_inv_nearest_enemy_show",
	"ui_inv_nightvision_installed",
	"ui_inv_walk_accel",
	"ui_inv_jump_speed"
	// end
};

CUIOutfitImmunity::CUIOutfitImmunity()
{
	m_caption = NULL;
	m_value = NULL;
	m_sign_inverse = false;
	m_with_plus = false;

	m_unit_str._set("");
	m_texture_minus._set("");
	m_texture_plus._set("");
}

CUIOutfitImmunity::~CUIOutfitImmunity()
{
}

void CUIOutfitImmunity::Init(CUIXml& xml, LPCSTR section)
{
	CUIXmlInit::InitWindow(xml, section, 0, this);
	xml.SetLocalRoot(xml.NavigateToNode(section));

	m_caption = UIHelper::CreateStatic(xml, "caption", this);
	m_value = UIHelper::CreateTextWnd(xml, "value", this);
	m_sign_inverse = (xml.ReadAttribInt("value", 0, "sign_inverse", 0) == 1);
	m_with_plus = (xml.ReadAttribInt("value", 0, "with_plus", 0) == 1);

	LPCSTR unit_str = xml.ReadAttrib("value", 0, "unit_str", "");
	m_unit_str._set(CStringTable().translate(unit_str));

	LPCSTR texture_minus = xml.Read("texture_minus", 0, "");
	if (texture_minus && xr_strlen(texture_minus))
	{
		m_texture_minus._set(texture_minus);

		LPCSTR texture_plus = xml.Read("caption:texture", 0, "");
		m_texture_plus._set(texture_plus);
		VERIFY(m_texture_plus.size());
	}
}

void CUIOutfitImmunity::SetCaption(LPCSTR name)
{
	m_caption->TextItemControl()->SetText(name);
}

void CUIOutfitImmunity::SetValue(float value)
{
	string32	buf;
	if (m_with_plus)
	{
		if (value > 0)
			xr_sprintf(buf, "%+.0f", value);
		else
			xr_sprintf(buf, "%-.0f", value);
	}
	else
	{
		xr_sprintf(buf, "%.0f", value);
	}

	LPSTR		str;
	if (m_unit_str.size())
	{
		STRCONCAT(str, buf, " ", m_unit_str.c_str());
	}
	else 
	{
		STRCONCAT(str, buf);
	}
	m_value->SetText(str);

	bool positive = (value >= 0.0f);
	positive = (m_sign_inverse) ? !positive : positive;
	if (value == 0)
		m_value->SetTextColor(color_rgba(175, 175, 175, 255));
	else if (value > 0)
		m_value->SetTextColor(positive ? color_rgba(0, 255, 0, 255) : color_rgba(255, 0, 0, 255));
	else
		m_value->SetTextColor(positive ? color_rgba(255, 0, 0, 255) : color_rgba(0, 255, 0, 255));

	if (m_texture_minus.size())
	{
		if (positive)
		{
			m_caption->InitTexture(m_texture_plus.c_str());
		}
		else
		{
			m_caption->InitTexture(m_texture_minus.c_str());
		}
	}
}

void CUIOutfitImmunity::SetValue(LPCSTR value)
{
	m_value->SetText(value);
}

// ===========================================================================================

CUIOutfitInfo::CUIOutfitInfo()
{
	for ( u32 i = 0; i < max_count; ++i )
	{
		m_items[i] = NULL;
	}
}

CUIOutfitInfo::~CUIOutfitInfo()
{
	for ( u32 i = 0; i < max_count; ++i )
	{
		xr_delete( m_items[i] );
	}
}

void CUIOutfitInfo::InitFromXml( CUIXml& xml_doc )
{
	LPCSTR base = "outfit_info";

	XML_NODE* stored_root = xml_doc.GetLocalRoot();
	XML_NODE* base_node = xml_doc.NavigateToNode(base, 0);
	if (!base_node)
	{
		return;
	}
	CUIXmlInit::InitWindow(xml_doc, base, 0, this);
	xml_doc.SetLocalRoot(base_node);

	m_Prop_line = xr_new<CUIStatic>();
	AttachChild(m_Prop_line);
	m_Prop_line->SetAutoDelete(false);
	CUIXmlInit::InitStatic(xml_doc, "prop_line", 0, m_Prop_line);

	for (u32 i = 0; i < max_count; i++)
	{
		m_items[i] = xr_new<CUIOutfitImmunity>();
		m_items[i]->Init(xml_doc, immunity_names[i]);
		m_items[i]->SetAutoDelete(false);

		LPCSTR name = CStringTable().translate(immunity_st_names[i]).c_str();
		m_items[i]->SetCaption(name);

		xml_doc.SetLocalRoot(base_node);
	}
	xml_doc.SetLocalRoot(stored_root);
}

void CUIOutfitInfo::UpdateInfo( CCustomOutfit* cur_outfit, CCustomOutfit* slot_outfit )
{
	DetachAll();
	AttachChild(m_Prop_line);

	CActor* actor = smart_cast<CActor*>( Level().CurrentViewEntity() );
	if ( !actor || !cur_outfit )
	{
		return;
	}

	// skif : (защита головы в описании бронежилетов), (маскировка в описании бронежилетов), (затраты сил в описании бронежилетов), (кол-во контейнеров под арты в описании бронежилетов), (доп переносимый вес в описании бронежилетов)
	Fvector2 pos;
	float h = m_Prop_line->GetWndPos().y + m_Prop_line->GetWndSize().y;

	for ( u32 i = 0; i < 9; ++i )
	{
		ALife::EHitType hit_type = (ALife::EHitType)i;
		float val = (cur_outfit->GetDefHitTypeProtection(hit_type) * cur_outfit->GetCondition()) * 100;
		if (!fis_zero(val))
		{
			m_items[i]->SetValue(val);
			pos.set(m_items[i]->GetWndPos());
			pos.y = h;
			m_items[i]->SetWndPos(pos);

			h += m_items[i]->GetWndSize().y;
			AttachChild(m_items[i]);
		}
	}
	m_items[9]->SetValue("0");
	if(!cur_outfit->bIsHelmetAvaliable)
	{
		IKinematics* ikv = smart_cast<IKinematics*>(actor->Visual());
		VERIFY(ikv);
		u16 bone = ikv->LL_BoneID("bip01_head");
		float val = cur_outfit->GetBoneArmor(bone);
		if (!fis_zero(val))
		{
			m_items[9]->SetValue(val * 100);
			pos.set(m_items[9]->GetWndPos());
			pos.y = h;
			m_items[9]->SetWndPos(pos);

			h += m_items[9]->GetWndSize().y;
			AttachChild(m_items[9]);
		}

	}
	if (cur_outfit->OutfitCamuflageCoeff() > 0.0f)
	{
		m_items[10]->SetValue(cur_outfit->OutfitCamuflageCoeff());
		pos.set(m_items[10]->GetWndPos());
		pos.y = h;
		m_items[10]->SetWndPos(pos);

		h += m_items[10]->GetWndSize().y;
		AttachChild(m_items[10]);
	}
	if (cur_outfit->m_fPowerLoss > 0.0f)
	{
		m_items[11]->SetValue(cur_outfit->m_fPowerLoss * 100); 
		pos.set(m_items[11]->GetWndPos());
		pos.y = h;
		m_items[11]->SetWndPos(pos);

		h += m_items[11]->GetWndSize().y;
		AttachChild(m_items[11]);
	}
	m_items[12]->SetValue(cur_outfit->m_artefact_count); 
	pos.set(m_items[12]->GetWndPos());
	pos.y = h;
	m_items[12]->SetWndPos(pos);

	h += m_items[12]->GetWndSize().y;
	AttachChild(m_items[12]);
	if (cur_outfit->m_additional_weight > 0.0f)
	{
		m_items[13]->SetValue(cur_outfit->m_additional_weight); 
		pos.set(m_items[13]->GetWndPos());
		pos.y = h;
		m_items[13]->SetWndPos(pos);

		h += m_items[13]->GetWndSize().y;
		AttachChild(m_items[13]);
	}
	if (cur_outfit->m_fHealthRestoreSpeed > 0.0f)
	{
		m_items[14]->SetValue(cur_outfit->m_fHealthRestoreSpeed);
		pos.set(m_items[14]->GetWndPos());
		pos.y = h;
		m_items[14]->SetWndPos(pos);

		h += m_items[14]->GetWndSize().y;
		AttachChild(m_items[14]);
	}
	if (cur_outfit->m_fSatietyRestoreSpeed > 0.0f)
	{
		m_items[15]->SetValue(cur_outfit->m_fSatietyRestoreSpeed);
		pos.set(m_items[15]->GetWndPos());
		pos.y = h;
		m_items[15]->SetWndPos(pos);

		h += m_items[15]->GetWndSize().y;
		AttachChild(m_items[15]);
	}
	if (cur_outfit->m_fPowerRestoreSpeed > 0.0f)
	{
		m_items[16]->SetValue(cur_outfit->m_fPowerRestoreSpeed);
		pos.set(m_items[16]->GetWndPos());
		pos.y = h;
		m_items[16]->SetWndPos(pos);

		h += m_items[16]->GetWndSize().y;
		AttachChild(m_items[16]);
	}
	if (cur_outfit->m_fBleedingRestoreSpeed > 0.0f)
	{
		m_items[17]->SetValue(cur_outfit->m_fBleedingRestoreSpeed);
		pos.set(m_items[17]->GetWndPos());
		pos.y = h;
		m_items[17]->SetWndPos(pos);

		h += m_items[17]->GetWndSize().y;
		AttachChild(m_items[17]);
	}
	if (cur_outfit->m_fRadiationRestoreSpeed > 0.0f)
	{
		m_items[18]->SetValue(cur_outfit->m_fRadiationRestoreSpeed);
		pos.set(m_items[18]->GetWndPos());
		pos.y = h;
		m_items[18]->SetWndPos(pos);

		h += m_items[18]->GetWndSize().y;
		AttachChild(m_items[18]);
	}
	if (cur_outfit->m_fThirstRestoreSpeed > 0.0f)
	{
		m_items[19]->SetValue(cur_outfit->m_fThirstRestoreSpeed);
		pos.set(m_items[19]->GetWndPos());
		pos.y = h;
		m_items[19]->SetWndPos(pos);

		h += m_items[19]->GetWndSize().y;
		AttachChild(m_items[19]);
	}
	if (cur_outfit->m_fSleepRestoreSpeed > 0.0f)
	{
		m_items[20]->SetValue(cur_outfit->m_fSleepRestoreSpeed);
		pos.set(m_items[20]->GetWndPos());
		pos.y = h;
		m_items[20]->SetWndPos(pos);

		h += m_items[20]->GetWndSize().y;
		AttachChild(m_items[20]);
	}
	if (cur_outfit->m_fPsyHealthRestoreSpeed > 0.0f)
	{
		m_items[21]->SetValue(cur_outfit->m_fPsyHealthRestoreSpeed);
		pos.set(m_items[21]->GetWndPos());
		pos.y = h;
		m_items[21]->SetWndPos(pos);

		h += m_items[21]->GetWndSize().y;
		AttachChild(m_items[21]);
	}

	if (!cur_outfit->bIsHelmetAvaliable)
	{
		pos.set(m_items[22]->GetWndPos());
		pos.y = h;
		m_items[22]->SetWndPos(pos);

		h += m_items[22]->GetWndSize().y;
		AttachChild(m_items[22]);
	}

	if (cur_outfit->m_NightVisionSect != "")
	{
		m_items[25]->SetCaption(CStringTable().translate(cur_outfit->m_NightVisionSect).c_str());
		pos.set(m_items[25]->GetWndPos());
		pos.y = h;
		m_items[25]->SetWndPos(pos);

		h += m_items[25]->GetWndSize().y;
		AttachChild(m_items[25]);
	}

	if (!cur_outfit->IsSprintAllowed())
	{
		pos.set(m_items[23]->GetWndPos());
		pos.y = h;
		m_items[23]->SetWndPos(pos);

		h += m_items[23]->GetWndSize().y;
		AttachChild(m_items[23]);
	}

	if (cur_outfit->m_fShowNearestEnemiesDistance > 0.0f)
	{
		m_items[24]->SetValue(cur_outfit->m_fShowNearestEnemiesDistance);
		pos.set(m_items[24]->GetWndPos());
		pos.y = h;
		m_items[24]->SetWndPos(pos);

		h += m_items[24]->GetWndSize().y;
		AttachChild(m_items[24]);
	}

	if (cur_outfit->m_JumpSpeed > 0.0f)
	{
		m_items[26]->SetValue(cur_outfit->m_JumpSpeed);
		pos.set(m_items[26]->GetWndPos());
		pos.y = h;
		m_items[26]->SetWndPos(pos);

		h += m_items[26]->GetWndSize().y;
		AttachChild(m_items[26]);
	}

	if (cur_outfit->m_WalkAccel > 0.0f)
	{
		m_items[27]->SetValue(cur_outfit->m_WalkAccel);
		pos.set(m_items[27]->GetWndPos());
		pos.y = h;
		m_items[27]->SetWndPos(pos);

		h += m_items[27]->GetWndSize().y;
		AttachChild(m_items[27]);
	}

	SetHeight(h);
	// end
}


void CUIOutfitInfo::UpdateInfo( CHelmet* cur_helmet, CHelmet* slot_helmet )
{
	DetachAll();
	AttachChild(m_Prop_line);
	CActor* actor = smart_cast<CActor*>( Level().CurrentViewEntity() );
	if ( !actor || !cur_helmet )
	{
		return;
	}
	// skif : (защита головы в описании бронежилетов), (маскировка в описании бронежилетов), (затраты сил в описании бронежилетов)
	Fvector2 pos;
	float h = m_Prop_line->GetWndPos().y + m_Prop_line->GetWndSize().y;
	
	for ( u32 i = 0; i < 9; ++i )
	{
		ALife::EHitType hit_type = (ALife::EHitType)i;
		float val = (cur_helmet->GetDefHitTypeProtection(hit_type) * cur_helmet->GetCondition()) * 100;
		if (i==6)
		{
			if (!fis_zero(val))
			{
				m_items[9]->SetValue(val);
				pos.set(m_items[9]->GetWndPos());
				pos.y = h;
				m_items[9]->SetWndPos(pos);

				h += m_items[9]->GetWndSize().y;
				AttachChild(m_items[9]);
			}
		}
		else
		{
			if (!fis_zero(val))
			{
				m_items[i]->SetValue(val);
				pos.set(m_items[i]->GetWndPos());
				pos.y = h;
				m_items[i]->SetWndPos(pos);

				h += m_items[i]->GetWndSize().y;
				AttachChild(m_items[i]);
			}
		}
	}
	if (cur_helmet->m_fPowerLoss > 0.0f)
	{
		m_items[11]->SetValue(cur_helmet->m_fPowerLoss * 100);
		pos.set(m_items[11]->GetWndPos());
		pos.y = h;
		m_items[11]->SetWndPos(pos);

		h += m_items[11]->GetWndSize().y;
		AttachChild(m_items[11]);
	}
	if (cur_helmet->m_fHealthRestoreSpeed > 0.0f)
	{
		m_items[14]->SetValue(cur_helmet->m_fHealthRestoreSpeed);
		pos.set(m_items[14]->GetWndPos());
		pos.y = h;
		m_items[14]->SetWndPos(pos);

		h += m_items[14]->GetWndSize().y;
		AttachChild(m_items[14]);
	}
	if (cur_helmet->m_fSatietyRestoreSpeed > 0.0f)
	{
		m_items[15]->SetValue(cur_helmet->m_fSatietyRestoreSpeed);
		pos.set(m_items[15]->GetWndPos());
		pos.y = h;
		m_items[15]->SetWndPos(pos);

		h += m_items[15]->GetWndSize().y;
		AttachChild(m_items[15]);
	}
	if (cur_helmet->m_fPowerRestoreSpeed > 0.0f)
	{
		m_items[16]->SetValue(cur_helmet->m_fPowerRestoreSpeed);
		pos.set(m_items[16]->GetWndPos());
		pos.y = h;
		m_items[16]->SetWndPos(pos);

		h += m_items[16]->GetWndSize().y;
		AttachChild(m_items[16]);
	}
	if (cur_helmet->m_fBleedingRestoreSpeed > 0.0f)
	{
		m_items[17]->SetValue(cur_helmet->m_fBleedingRestoreSpeed);
		pos.set(m_items[17]->GetWndPos());
		pos.y = h;
		m_items[17]->SetWndPos(pos);

		h += m_items[17]->GetWndSize().y;
		AttachChild(m_items[17]);
	}
	if (cur_helmet->m_fRadiationRestoreSpeed > 0.0f)
	{
		m_items[18]->SetValue(cur_helmet->m_fRadiationRestoreSpeed);
		pos.set(m_items[18]->GetWndPos());
		pos.y = h;
		m_items[18]->SetWndPos(pos);

		h += m_items[18]->GetWndSize().y;
		AttachChild(m_items[18]);
	}
	if (cur_helmet->m_fThirstRestoreSpeed > 0.0f)
	{
		m_items[19]->SetValue(cur_helmet->m_fThirstRestoreSpeed);
		pos.set(m_items[19]->GetWndPos());
		pos.y = h;
		m_items[19]->SetWndPos(pos);

		h += m_items[19]->GetWndSize().y;
		AttachChild(m_items[19]);
	}
	if (cur_helmet->m_fSleepRestoreSpeed > 0.0f)
	{
		m_items[20]->SetValue(cur_helmet->m_fSleepRestoreSpeed);
		pos.set(m_items[20]->GetWndPos());
		pos.y = h;
		m_items[20]->SetWndPos(pos);

		h += m_items[20]->GetWndSize().y;
		AttachChild(m_items[20]);
	}
	if (cur_helmet->m_fPsyHealthRestoreSpeed > 0.0f)
	{
		m_items[21]->SetValue(cur_helmet->m_fPsyHealthRestoreSpeed);
		pos.set(m_items[21]->GetWndPos());
		pos.y = h;
		m_items[21]->SetWndPos(pos);

		h += m_items[21]->GetWndSize().y;
		AttachChild(m_items[21]);
	}

	if (cur_helmet->m_NightVisionSect != "")
	{
		m_items[25]->SetCaption(CStringTable().translate(cur_helmet->m_NightVisionSect).c_str());
		pos.set(m_items[25]->GetWndPos());
		pos.y = h;
		m_items[25]->SetWndPos(pos);

		h += m_items[25]->GetWndSize().y;
		AttachChild(m_items[25]);
	}

	if (cur_helmet->m_fShowNearestEnemiesDistance > 0.0f)
	{
		m_items[24]->SetValue(cur_helmet->m_fShowNearestEnemiesDistance);
		pos.set(m_items[24]->GetWndPos());
		pos.y = h;
		m_items[24]->SetWndPos(pos);

		h += m_items[24]->GetWndSize().y;
		AttachChild(m_items[24]);
	}

	SetHeight(h);
	// end
}
