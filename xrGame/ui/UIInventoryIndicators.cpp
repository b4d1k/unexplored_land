// skif : (индикаторы)
// Панель с отображением всех свойств защиты и состояний актора
#include "stdafx.h"
#include "UIInventoryIndicators.h"
#include "UIXmlInit.h"
#include "xrUIXmlParser.h"
#include "UIItemInfo.h"
#include "UIFrameLineWnd.h"
#include "UIMessageBoxEx.h"
#include "UIPropertiesBox.h"
#include "UI3tButton.h"
#include "../inventoryOwner.h"
#include "UIInventoryUtilities.h"
#include "../Level.h"
#include "UIActorMenu.h"

#include "ai_space.h"
#include "alife_space.h"
#include "../actor_defs.h"
#include "alife_simulator.h"
#include "object_broker.h"
#include "UIWndCallback.h"
#include "UIHelper.h"
#include "UIProgressBar.h"
#include "ui_base.h"
#include "../string_table.h"
#include "../ActorCondition.h"
#include "../EntityCondition.h"
#include "../CustomOutfit.h"
#include "../ActorHelmet.h"
#include "../Inventory.h"
#include "../Artefact.h"
#include "../Actor.h"
#include "../Include/xrRender/Kinematics.h"

const u8 protections_cnt = 10; //кол-во параметров иммунитета
const u8 restores_cnt = 8; // кол-во параметров восстановления состояний актора

LPCSTR protections_sects[protections_cnt] =
{
	"indicator_burn_protection",
	"indicator_shock_protection",
	"indicator_acid_protection",
	"indicator_radio_protection",
	"indicator_psy_protection",
	"indicator_wound_protection",
	"indicator_fwound_protection",
	"indicator_strike_protection",
	"indicator_expl_protection",
	"indicator_head_fwound_protection"
};

LPCSTR restores_sects[restores_cnt] =
{
	"indicator_health_restore",
	"indicator_bleed_restore",
	"indicator_psy_health_restore",
	"indicator_radio_restore",
	"indicator_satiety_restore",
	"indicator_thirst_restore",
	"indicator_sleep_restore",
	"indicator_power_restore"
};

LPCSTR states_sects[restores_cnt] =
{
	"indicator_health",
	"indicator_bleed",
	"indicator_psy_health",
	"indicator_radio",
	"indicator_satiety",
	"indicator_thirst",
	"indicator_sleep",
	"indicator_power"
};


CUIInventoryIndicators::CUIInventoryIndicators()
{
	Initialize();
}

CUIInventoryIndicators::~CUIInventoryIndicators()
{
	
}


void CUIInventoryIndicators::Initialize()
{
	CUIXml								uiXml;
	uiXml.Load							(CONFIG_PATH, UI_PATH, "actor_menu.xml");
	CUIXmlInit							xml_init;
	LPCSTR base	= "indicators_wnd";
	XML_NODE* stored_root = uiXml.GetLocalRoot();
	XML_NODE* base_node   = uiXml.NavigateToNode(base, 0);
	if (!base_node) return;
	xml_init.InitWindow					(uiXml, base, 0, this);
	
	m_capt_outfit = UIHelper::CreateStatic(uiXml, "indicators_wnd:indicator_caption_outfit", this);
	m_capt_afs = UIHelper::CreateStatic(uiXml, "indicators_wnd:indicator_caption_artefacts", this);
	m_capt_booster = UIHelper::CreateStatic(uiXml, "indicators_wnd:indicator_caption_booster", this);
	m_capt_total = UIHelper::CreateStatic(uiXml, "indicators_wnd:indicator_caption_total", this);
	
	uiXml.SetLocalRoot(base_node);
	for (u8 i=0; i<protections_cnt; i++)
	{
		m_ind_protections[i] = xr_new<UIInventoryIndicatorItem>();
		m_ind_protections[i]->Init(uiXml, protections_sects[i]);
		m_ind_protections[i]->SetAutoDelete(false);
		AttachChild(m_ind_protections[i]);
		uiXml.SetLocalRoot(base_node);
	}
	
	for (u8 i=0; i<restores_cnt; i++)
	{
		m_ind_restores[i] = xr_new<UIInventoryIndicatorItem>();
		m_ind_restores[i]->Init(uiXml, restores_sects[i]);
		m_ind_restores[i]->SetAutoDelete(false);
		AttachChild(m_ind_restores[i]);
		uiXml.SetLocalRoot(base_node);
		
		m_ind_states[i] = xr_new<UIInventoryIndicatorItem>();
		m_ind_states[i]->Init(uiXml, states_sects[i]);
		m_ind_states[i]->SetAutoDelete(false);
		AttachChild(m_ind_states[i]);
		uiXml.SetLocalRoot(base_node);
	}
	
	m_ind_weight = xr_new<UIInventoryIndicatorItem>();
	m_ind_weight->Init(uiXml, "indicator_weight");
	m_ind_weight->SetAutoDelete(false);
	AttachChild(m_ind_weight);
	uiXml.SetLocalRoot(base_node);
	
	m_ind_walk_accel = xr_new<UIInventoryIndicatorItem>();
	m_ind_walk_accel->Init(uiXml, "indicator_walk_accel");
	m_ind_walk_accel->SetAutoDelete(false);
	AttachChild(m_ind_walk_accel);
	uiXml.SetLocalRoot(base_node);
	
	m_ind_jump_speed = xr_new<UIInventoryIndicatorItem>();
	m_ind_jump_speed->Init(uiXml, "indicator_jump_speed");
	m_ind_jump_speed->SetAutoDelete(false);
	AttachChild(m_ind_jump_speed);
	uiXml.SetLocalRoot(base_node);
}

void CUIInventoryIndicators::FillIndicators()
{
	CActor* actor = smart_cast<CActor*>(Level().CurrentViewEntity());	
	if (!actor) return;
	
	CCustomOutfit* outfit = actor->GetOutfit();
	PIItem itm = actor->inventory().ItemFromSlot(HELMET_SLOT);
	CHelmet* helmet = smart_cast<CHelmet*>(itm);
	
	// заполняем параметры иммунитета
	for (u8 i=0; i<protections_cnt; i++)
	{
		float val = 0.0f, sum = 0.0f;
		u32 delta = 100;
		{	// заполнение защиты броников
			if (i==6) // значение пулестойкости броников получается отдельно
			{
				if (outfit)
				{
					IKinematics* ikv = smart_cast<IKinematics*>(actor->Visual());
					VERIFY(ikv);
					u16 spine_bone = ikv->LL_BoneID("bip01_spine");
					val += outfit->GetBoneArmor(spine_bone)*outfit->GetCondition()*delta;
				if (val < 0)
					val += 100;
				}
			}
			else if (i==9)
			{
				if (outfit && !outfit->bIsHelmetAvaliable)
				{
					IKinematics* ikv = smart_cast<IKinematics*>(actor->Visual());
					VERIFY(ikv);				
					u16 head_bone = ikv->LL_BoneID("bip01_head");
					val += outfit->GetBoneArmor(head_bone)*outfit->GetCondition()*delta;
				}
				if (helmet)
				{
					IKinematics* ikv = smart_cast<IKinematics*>(actor->Visual());
					VERIFY(ikv);
					u16 spine_bone = ikv->LL_BoneID("bip01_head");
					val += helmet->GetBoneArmor(spine_bone)*helmet->GetCondition()*delta;
				}
			}
			else // здесь читаем все остальные иммунитеты
			{
				if (outfit)
					val = outfit->GetDefHitTypeProtection((ALife::EHitType)i)*delta;
				if (helmet)
					val += helmet->GetDefHitTypeProtection((ALife::EHitType)i)*delta;
			}
			sum += val;
			string64 str = "";
			if (val == 0) 
			{
				m_ind_protections[i]->m_val_outfit->SetTextColor(color_rgba(175,175,175,255));
				m_ind_protections[i]->m_val_outfit->SetText("0");
			}
			else if (val > 0)
			{
				xr_sprintf( str, "+%.0f", val );
				m_ind_protections[i]->m_val_outfit->SetTextColor(color_rgba(0,255,0,255));
				m_ind_protections[i]->m_val_outfit->SetText(str);
			}
			else if (val < 0)
		    {
				xr_sprintf( str, "%.0f", val );
				m_ind_protections[i]->m_val_outfit->SetTextColor(color_rgba(255,0,0,255));
				m_ind_protections[i]->m_val_outfit->SetText(str);
			}
		
		}
		
		if (i != 9)
		{   // заполнение защиты артефактов
			val = actor->GetProtection_ArtefactsOnBelt((ALife::EHitType)i)*delta;
			sum += val;
			{
				string64 str = "";
				if (val == 0)
				{
					m_ind_protections[i]->m_val_af->SetTextColor(color_rgba(175, 175, 175, 255));
					m_ind_protections[i]->m_val_af->SetText("0");
				}
				else if (val > 0)
				{
					xr_sprintf(str, "+%.0f", val);
					m_ind_protections[i]->m_val_af->SetTextColor(color_rgba(0, 255, 0, 255));
					m_ind_protections[i]->m_val_af->SetText(str);
				}
				else if (val < 0)
				{
					xr_sprintf(str, "%.0f", val);
					m_ind_protections[i]->m_val_af->SetTextColor(color_rgba(255, 0, 0, 255));
					m_ind_protections[i]->m_val_af->SetText(str);
				}
				// заполненение защиты бустеров
				CEntityCondition::BOOSTER_MAP cur_booster_influences = actor->conditions().GetCurBoosterInfluences();
				CEntityCondition::BOOSTER_MAP::const_iterator it;

				val = 0.0f;
				switch (i)  //пока бустеры только для термо- электро- радио- хим- пси- защит
				{
				case 0: {it = cur_booster_influences.find(eBoostBurnProtection); if (it != cur_booster_influences.end()) val = it->second.fBoostValue*delta * 10; break; }
				case 1: {it = cur_booster_influences.find(eBoostElectroProtection); if (it != cur_booster_influences.end()) val = it->second.fBoostValue*delta * 10; break; }
				case 2: {it = cur_booster_influences.find(eBoostChemicalBurnProtection); if (it != cur_booster_influences.end()) val = it->second.fBoostValue*delta * 10; break; }
				case 3: {it = cur_booster_influences.find(eBoostRadiationProtection); if (it != cur_booster_influences.end()) val = it->second.fBoostValue*delta * 10; break; }
				case 4: {it = cur_booster_influences.find(eBoostTelepaticProtection); if (it != cur_booster_influences.end()) val = it->second.fBoostValue*delta * 10; break; }
				default: break;
				}

				sum += val;
			}
			{
				string64 str = "";
				if (val == 0)
				{
					m_ind_protections[i]->m_val_booster->SetTextColor(color_rgba(175, 175, 175, 255));
					m_ind_protections[i]->m_val_booster->SetText("0");
				}
				else if (val > 0)
				{
					xr_sprintf(str, "+%.0f", val);
					m_ind_protections[i]->m_val_booster->SetTextColor(color_rgba(0, 255, 0, 255));
					m_ind_protections[i]->m_val_booster->SetText(str);
				}
				else if (val < 0)
				{
					xr_sprintf(str, "%.0f", val);
					m_ind_protections[i]->m_val_booster->SetTextColor(color_rgba(255, 0, 0, 255));
					m_ind_protections[i]->m_val_booster->SetText(str);
				}
			}
		}
		
		{	// заполняем суммарную защиту
			string64 str = "";
				if (sum == 0) 
				{
					m_ind_protections[i]->m_val_total->SetTextColor(color_rgba(175,175,175,255));
					m_ind_protections[i]->m_val_total->SetText("0");
				}
				else if (sum > 0)
				{
					xr_sprintf( str, "+%.0f", sum );
					m_ind_protections[i]->m_val_total->SetTextColor(color_rgba(0,255,0,255));
					m_ind_protections[i]->m_val_total->SetText(str);
				}
				else if (sum < 0)
				{
					xr_sprintf( str, "%.0f", sum );
					m_ind_protections[i]->m_val_total->SetTextColor(color_rgba(255,0,0,255));
					m_ind_protections[i]->m_val_total->SetText(str);
				}
		}
	}
	
	//  заполняем параметры восстановлений состояний актора
	for (u8 i=0; i<restores_cnt; i++)
	{
		float val = 0.0f, sum = 0.0f;
		bool invers = (i==3)?true:false;
		
		{ // броники
			if (outfit)
				switch (i)
				{
					case 0: val = outfit->m_fHealthRestoreSpeed; break;
					case 1: val = outfit->m_fBleedingRestoreSpeed; break;
					case 2: val = outfit->m_fPsyHealthRestoreSpeed; break;
					case 3: val = outfit->m_fRadiationRestoreSpeed; break;
					case 4: val = outfit->m_fSatietyRestoreSpeed; break;
					case 5: val = outfit->m_fThirstRestoreSpeed; break;
					case 6: val = outfit->m_fSleepRestoreSpeed; break;
					case 7: val = outfit->m_fPowerRestoreSpeed; break;
				}
			if (helmet)
				switch (i)
				{
					case 0: val += helmet->m_fHealthRestoreSpeed; break;
					case 1: val += helmet->m_fBleedingRestoreSpeed; break;
					case 2: val += helmet->m_fPsyHealthRestoreSpeed; break;
					case 3: val += helmet->m_fRadiationRestoreSpeed; break;
					case 4: val += helmet->m_fSatietyRestoreSpeed; break;
					case 5: val += helmet->m_fThirstRestoreSpeed; break;
					case 6: val += helmet->m_fSleepRestoreSpeed; break;
					case 7: val += helmet->m_fPowerRestoreSpeed; break;
				}
				
			sum += val;
			string64 str = "";
			if (val == 0) 
			{
				m_ind_restores[i]->m_val_outfit->SetTextColor(color_rgba(175,175,175,255));
				m_ind_restores[i]->m_val_outfit->SetText("0");
			}
			else if (val > 0)
			{
				xr_sprintf( str, "+%.0f", val );
				if (!invers)
					m_ind_restores[i]->m_val_outfit->SetTextColor(color_rgba(0,255,0,255));
				else
					m_ind_restores[i]->m_val_outfit->SetTextColor(color_rgba(255,0,0,255));
				m_ind_restores[i]->m_val_outfit->SetText(str);
			}
			else if (val < 0)
		    {
				xr_sprintf( str, "%.0f", val );
				if (!invers)
					m_ind_restores[i]->m_val_outfit->SetTextColor(color_rgba(255,0,0,255));
				else
					m_ind_restores[i]->m_val_outfit->SetTextColor(color_rgba(0,255,0,255));
				m_ind_restores[i]->m_val_outfit->SetText(str);
			}
		}
		
		{ //артефакты
			val = 0.0f;
			for(TIItemContainer::const_iterator it = actor->inventory().m_belt.begin(); actor->inventory().m_belt.end() != it; ++it) 
			{
				CArtefact*	artefact = smart_cast<CArtefact*>(*it);
				if (artefact)
				{
					if (i == 0) val += artefact->m_fHealthRestoreSpeed;
					else if (i == 1) val += artefact->m_fBleedingRestoreSpeed;
					else if (i == 2)  val += artefact->m_fPsyHealthRestoreSpeed;
					else if (i == 3)  val += artefact->m_fRadiationRestoreSpeed;
					else if (i == 4)  val += artefact->m_fSatietyRestoreSpeed;
					else if (i == 5)  val += artefact->m_fThirstRestoreSpeed;
					else if (i == 6)  val += artefact->m_fSleepRestoreSpeed;
					else if (i == 7)  val += artefact->m_fPowerRestoreSpeed;
				}
			}
				
			sum += val;
			string64 str = "";
			if (val == 0) 
			{
				m_ind_restores[i]->m_val_af->SetTextColor(color_rgba(175,175,175,255));
				m_ind_restores[i]->m_val_af->SetText("0");
			}
			else if (val > 0)
			{
				xr_sprintf( str, "+%.0f", val );
				if (!invers)
					m_ind_restores[i]->m_val_af->SetTextColor(color_rgba(0,255,0,255));
				else
					m_ind_restores[i]->m_val_af->SetTextColor(color_rgba(255,0,0,255));
				m_ind_restores[i]->m_val_af->SetText(str);
			}
			else if (val < 0)
		    {
				xr_sprintf( str, "%.0f", val );
				if (!invers)
					m_ind_restores[i]->m_val_af->SetTextColor(color_rgba(255,0,0,255));
				else
					m_ind_restores[i]->m_val_af->SetTextColor(color_rgba(0,255,0,255));
				m_ind_restores[i]->m_val_af->SetText(str);
			}
		}
		
		{ //бустеры
			CEntityCondition::BOOSTER_MAP cur_booster_influences = actor->conditions().GetCurBoosterInfluences();
			CEntityCondition::BOOSTER_MAP::const_iterator it;
			
			val = 0.0f;
			switch (i)
					{
						case 0: { it = cur_booster_influences.find(eBoostHpRestore); 
							if(it!=cur_booster_influences.end()) val = it->second.fBoostValue*1000; break;}
						case 1: { it = cur_booster_influences.find(eBoostBleedingRestore); 
							if(it!=cur_booster_influences.end()) val = it->second.fBoostValue*1000; break;}
						case 2: { it = cur_booster_influences.find(eBoostPsyHpRestore); 
							if(it!=cur_booster_influences.end()) val = it->second.fBoostValue*1000; break;}
						case 3: { it = cur_booster_influences.find(eBoostRadiationRestore); 
							if(it!=cur_booster_influences.end()) val = it->second.fBoostValue*(-1000); break;}
						default: break;
					}
			
				
			sum += val;
			string64 str = "";
			if (val == 0) 
			{
				m_ind_restores[i]->m_val_booster->SetTextColor(color_rgba(175,175,175,255));
				m_ind_restores[i]->m_val_booster->SetText("0");
			}
			else if (val > 0)
			{
				xr_sprintf( str, "+%.0f", val );
				m_ind_restores[i]->m_val_booster->SetTextColor(color_rgba(0,255,0,255));
				m_ind_restores[i]->m_val_booster->SetText(str);
			}
			else if (val < 0)
		    {
				xr_sprintf( str, "%.0f", val );
				m_ind_restores[i]->m_val_booster->SetTextColor(color_rgba(255,0,0,255));
				m_ind_restores[i]->m_val_booster->SetText(str);
			}
		}
		
		{	// заполняем суммарную защиту
			string64 str = "";
				if (sum == 0) 
				{
					m_ind_restores[i]->m_val_total->SetTextColor(color_rgba(175,175,175,255));
					m_ind_restores[i]->m_val_total->SetText("0");
				}
				else if (sum > 0)
				{
					xr_sprintf( str, "+%.0f", sum );
					if (!invers)
						m_ind_restores[i]->m_val_total->SetTextColor(color_rgba(0,255,0,255));
					else
						m_ind_restores[i]->m_val_total->SetTextColor(color_rgba(255,0,0,255));
					m_ind_restores[i]->m_val_total->SetText(str);
				}
				else if (sum < 0)
				{
					xr_sprintf( str, "%.0f", sum );
					if (!invers)
						m_ind_restores[i]->m_val_total->SetTextColor(color_rgba(255,0,0,255));
					else
						m_ind_restores[i]->m_val_total->SetTextColor(color_rgba(0,255,0,255));
					m_ind_restores[i]->m_val_total->SetText(str);
				}
		}
		
	}
	
	//заполняем состояния актора
	for (u8 i=0; i<restores_cnt; i++)
	{
		float val = 0.0f;
		bool invers = (i==1||i==3)?true:false;
		switch (i)
		{
			case 0: val = actor->conditions().GetHealth()*100; break;
			case 1: val = actor->conditions().BleedingSpeed()*100; break;
			case 2: val = actor->conditions().GetPsyHealth()*100; break;
			case 3: val = actor->conditions().GetRadiation()*100; break;
			case 4: val = actor->conditions().GetSatiety()*100; break;
			case 5: val = actor->conditions().GetThirst()*100; break;
			case 6: val = actor->conditions().GetSleep()*100; break;
			case 7: val = actor->conditions().GetPower()*100; break;
		}
		
		string64 str;
		xr_sprintf( str, "%.0f%c", val, char(37) );
		if (!invers)
		{
			if (val >= 75)
				m_ind_states[i]->m_val_outfit->SetTextColor(color_rgba(0,255,0,255));
			else if (val < 75 && val >= 40)
				m_ind_states[i]->m_val_outfit->SetTextColor(color_rgba(255,255,0,255));
			else if (val < 40)
				m_ind_states[i]->m_val_outfit->SetTextColor(color_rgba(255,0,0,255));
		}
		else
		{
			if (val == 0)
				m_ind_states[i]->m_val_outfit->SetTextColor(color_rgba(0,255,0,255));
			else if (val > 0 && val <= 40)
				m_ind_states[i]->m_val_outfit->SetTextColor(color_rgba(255,255,0,255));
			else if (val > 40 && val <= 75)
				m_ind_states[i]->m_val_outfit->SetTextColor(color_rgba(244,89,0,255));
			else if (val > 75)
				m_ind_states[i]->m_val_outfit->SetTextColor(color_rgba(255,0,0,255));
		}
		
		m_ind_states[i]->m_val_outfit->SetText(str);
		m_ind_states[i]->m_val_af->SetText("");
		m_ind_states[i]->m_val_booster->SetText("");
		m_ind_states[i]->m_val_total->SetText("");
	}
	
	{ // доп переносимый вес
		float val = 0.0f, sum = 0.0f;
		if (outfit) val = outfit->m_additional_weight;
		sum += val;
		{ //броник
			string64 str;
			if (val == 0) 
			{
				m_ind_weight->m_val_outfit->SetTextColor(color_rgba(175,175,175,255));
				m_ind_weight->m_val_outfit->SetText("0");
			}
			else if (val > 0)
			{
				m_ind_weight->m_val_outfit->SetTextColor(color_rgba(0,255,0,255));
				xr_sprintf(str, "+%.0f", val);
				m_ind_weight->m_val_outfit->SetText(str);
			}
			else if (val < 0)
			{
				m_ind_weight->m_val_outfit->SetTextColor(color_rgba(255,0,0,255));
				xr_sprintf(str, "%.0f", val);
				m_ind_weight->m_val_outfit->SetText(str);
			}
		}
		
		// артефакты
		val = 0.0f;
		for(TIItemContainer::const_iterator it = actor->inventory().m_belt.begin(); actor->inventory().m_belt.end() != it; ++it) 
		{
			CArtefact*	artefact = smart_cast<CArtefact*>(*it);
			if (artefact) val += artefact->AdditionalInventoryWeight();
		}
		sum += val;
		{
			string64 str;
			if (val == 0) 
			{
				m_ind_weight->m_val_af->SetTextColor(color_rgba(175,175,175,255));
				m_ind_weight->m_val_af->SetText("0");
			}
			else if (val > 0)
			{
				m_ind_weight->m_val_af->SetTextColor(color_rgba(0,255,0,255));
				xr_sprintf(str, "+%.0f", val);
				m_ind_weight->m_val_af->SetText(str);
			}
			else if (val < 0)
			{
				m_ind_weight->m_val_af->SetTextColor(color_rgba(255,0,0,255));
				xr_sprintf(str, "%.0f", val);
				m_ind_weight->m_val_af->SetText(str);
			}
		}
		
		// бустеры
		{
			val = 0.0f;
			CEntityCondition::BOOSTER_MAP cur_booster_influences = actor->conditions().GetCurBoosterInfluences();
			CEntityCondition::BOOSTER_MAP::const_iterator it;
			it = cur_booster_influences.find(eBoostMaxWeight); 
			if (it!=cur_booster_influences.end()) val = it->second.fBoostValue;
			sum += val;
			{
				string64 str;
				if (val == 0) 
				{
					m_ind_weight->m_val_booster->SetTextColor(color_rgba(175,175,175,255));
					m_ind_weight->m_val_booster->SetText("0");
				}
				else if (val > 0)
				{
					m_ind_weight->m_val_booster->SetTextColor(color_rgba(0,255,0,255));
					xr_sprintf(str, "+%.0f", val);
					m_ind_weight->m_val_booster->SetText(str);
				}
				else if (val < 0)
				{
					m_ind_weight->m_val_booster->SetTextColor(color_rgba(255,0,0,255));
					xr_sprintf(str, "%.0f", val);
					m_ind_weight->m_val_booster->SetText(str);
				}
			}
		}
		
		{
			string64 str;
			if (sum == 0) 
			{
				m_ind_weight->m_val_total->SetTextColor(color_rgba(175,175,175,255));
				m_ind_weight->m_val_total->SetText("0");
			}
			else if (sum > 0)
			{
				m_ind_weight->m_val_total->SetTextColor(color_rgba(0,255,0,255));
				xr_sprintf(str, "+%.0f", sum);
				m_ind_weight->m_val_total->SetText(str);
			}
			else if (sum < 0)
			{
				m_ind_weight->m_val_total->SetTextColor(color_rgba(255,0,0,255));
				xr_sprintf(str, "%.0f", sum);
				m_ind_weight->m_val_total->SetText(str);
			}
		}
	}

	{ // скорость перемещения
		float val = 0.0f, sum = 0.0f;
		if (outfit) val = outfit->m_WalkAccel;
		sum += val;
		{ //броник
			string64 str;
			if (val == 0)
			{
				m_ind_walk_accel->m_val_outfit->SetTextColor(color_rgba(175, 175, 175, 255));
				m_ind_walk_accel->m_val_outfit->SetText("0");
			}
			else if (val > 0)
			{
				m_ind_walk_accel->m_val_outfit->SetTextColor(color_rgba(0, 255, 0, 255));
				xr_sprintf(str, "+%.0f", val);
				m_ind_walk_accel->m_val_outfit->SetText(str);
			}
			else if (val < 0)
			{
				m_ind_walk_accel->m_val_outfit->SetTextColor(color_rgba(255, 0, 0, 255));
				xr_sprintf(str, "%.0f", val);
				m_ind_walk_accel->m_val_outfit->SetText(str);
			}
		}

		// артефакты
		val = 0.0f;
		for (TIItemContainer::const_iterator it = actor->inventory().m_belt.begin(); actor->inventory().m_belt.end() != it; ++it)
		{
			CArtefact*	artefact = smart_cast<CArtefact*>(*it);
			if (artefact) val += artefact->m_WalkAccel;
		}
		sum += val;
		{
			string64 str;
			if (val == 0)
			{
				m_ind_walk_accel->m_val_af->SetTextColor(color_rgba(175, 175, 175, 255));
				m_ind_walk_accel->m_val_af->SetText("0");
			}
			else if (val > 0)
			{
				m_ind_walk_accel->m_val_af->SetTextColor(color_rgba(0, 255, 0, 255));
				xr_sprintf(str, "+%.0f", val);
				m_ind_walk_accel->m_val_af->SetText(str);
			}
			else if (val < 0)
			{
				m_ind_walk_accel->m_val_af->SetTextColor(color_rgba(255, 0, 0, 255));
				xr_sprintf(str, "%.0f", val);
				m_ind_walk_accel->m_val_af->SetText(str);
			}
		}

		{
			string64 str;
			if (sum == 0)
			{
				m_ind_walk_accel->m_val_total->SetTextColor(color_rgba(175, 175, 175, 255));
				m_ind_walk_accel->m_val_total->SetText("0");
			}
			else if (sum > 0)
			{
				m_ind_walk_accel->m_val_total->SetTextColor(color_rgba(0, 255, 0, 255));
				xr_sprintf(str, "+%.0f", sum);
				m_ind_walk_accel->m_val_total->SetText(str);
			}
			else if (sum < 0)
			{
				m_ind_walk_accel->m_val_total->SetTextColor(color_rgba(255, 0, 0, 255));
				xr_sprintf(str, "%.0f", sum);
				m_ind_walk_accel->m_val_total->SetText(str);
			}
		}
	}
	

	{ // высота прыжка
		float val = 0.0f, sum = 0.0f;
		if (outfit) val = outfit->m_JumpSpeed;
		sum += val;
		{ //броник
			string64 str;
			if (val == 0)
			{
				m_ind_jump_speed->m_val_outfit->SetTextColor(color_rgba(175, 175, 175, 255));
				m_ind_jump_speed->m_val_outfit->SetText("0");
			}
			else if (val > 0)
			{
				m_ind_jump_speed->m_val_outfit->SetTextColor(color_rgba(0, 255, 0, 255));
				xr_sprintf(str, "+%.0f", val);
				m_ind_jump_speed->m_val_outfit->SetText(str);
			}
			else if (val < 0)
			{
				m_ind_jump_speed->m_val_outfit->SetTextColor(color_rgba(255, 0, 0, 255));
				xr_sprintf(str, "%.0f", val);
				m_ind_jump_speed->m_val_outfit->SetText(str);
			}
		}

		// артефакты
		val = 0.0f;
		for (TIItemContainer::const_iterator it = actor->inventory().m_belt.begin(); actor->inventory().m_belt.end() != it; ++it)
		{
			CArtefact*	artefact = smart_cast<CArtefact*>(*it);
			if (artefact) val += artefact->m_JumpSpeed;
		}
		sum += val;
		{
			string64 str;
			if (val == 0)
			{
				m_ind_jump_speed->m_val_af->SetTextColor(color_rgba(175, 175, 175, 255));
				m_ind_jump_speed->m_val_af->SetText("0");
			}
			else if (val > 0)
			{
				m_ind_jump_speed->m_val_af->SetTextColor(color_rgba(0, 255, 0, 255));
				xr_sprintf(str, "+%.0f", val);
				m_ind_jump_speed->m_val_af->SetText(str);
			}
			else if (val < 0)
			{
				m_ind_jump_speed->m_val_af->SetTextColor(color_rgba(255, 0, 0, 255));
				xr_sprintf(str, "%.0f", val);
				m_ind_jump_speed->m_val_af->SetText(str);
			}
		}

		// бустеры
		{
			val = 0.0f;
			CEntityCondition::BOOSTER_MAP cur_booster_influences = actor->conditions().GetCurBoosterInfluences();
			CEntityCondition::BOOSTER_MAP::const_iterator it;
			it = cur_booster_influences.find(eBoostJumpSpeed);
			if (it != cur_booster_influences.end()) val = it->second.fBoostValue * 10;
			sum += val;
			{
				string64 str;
				if (val == 0)
				{
					m_ind_jump_speed->m_val_booster->SetTextColor(color_rgba(175, 175, 175, 255));
					m_ind_jump_speed->m_val_booster->SetText("0");
				}
				else if (val > 0)
				{
					m_ind_jump_speed->m_val_booster->SetTextColor(color_rgba(0, 255, 0, 255));
					xr_sprintf(str, "+%.0f", val);
					m_ind_jump_speed->m_val_booster->SetText(str);
				}
				else if (val < 0)
				{
					m_ind_jump_speed->m_val_booster->SetTextColor(color_rgba(255, 0, 0, 255));
					xr_sprintf(str, "%.0f", val);
					m_ind_jump_speed->m_val_booster->SetText(str);
				}
			}
		}

		{
			string64 str;
			if (sum == 0)
			{
				m_ind_jump_speed->m_val_total->SetTextColor(color_rgba(175, 175, 175, 255));
				m_ind_jump_speed->m_val_total->SetText("0");
			}
			else if (sum > 0)
			{
				m_ind_jump_speed->m_val_total->SetTextColor(color_rgba(0, 255, 0, 255));
				xr_sprintf(str, "+%.0f", sum);
				m_ind_jump_speed->m_val_total->SetText(str);
			}
			else if (sum < 0)
			{
				m_ind_jump_speed->m_val_total->SetTextColor(color_rgba(255, 0, 0, 255));
				xr_sprintf(str, "%.0f", sum);
				m_ind_jump_speed->m_val_total->SetText(str);
			}
		}
	}
}





UIInventoryIndicatorItem::UIInventoryIndicatorItem()
{
}

UIInventoryIndicatorItem::~UIInventoryIndicatorItem()
{	
}

void UIInventoryIndicatorItem::Init(CUIXml& xml, LPCSTR section)
{
	CUIXmlInit::InitWindow(xml, section, 0, this);
	xml.SetLocalRoot(xml.NavigateToNode(section));

	m_icon   		= UIHelper::CreateStatic(xml, "icon", this);
	m_val_outfit    = UIHelper::CreateTextWnd(xml, "outfit_value", this);
	m_val_af	    = UIHelper::CreateTextWnd(xml, "artefact_value", this);
	m_val_booster   = UIHelper::CreateTextWnd(xml, "booster_value", this);
	m_val_total     = UIHelper::CreateTextWnd(xml, "total_value", this);
	
	m_icon->InitTexture(xml.Read("icon:texture", 0, ""));
	SetZero();
}

void UIInventoryIndicatorItem::SetZero()
{
	m_val_outfit->SetText("0");
	m_val_af->SetText("0");
	m_val_booster->SetText("0");
	m_val_total->SetText("0");
	m_val_outfit->SetTextColor(color_rgba(175,175,175,255));
	m_val_af->SetTextColor(color_rgba(175,175,175,255));
	m_val_booster->SetTextColor(color_rgba(175,175,175,255));
	m_val_total->SetTextColor(color_rgba(175,175,175,255));
}