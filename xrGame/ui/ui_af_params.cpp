#include "stdafx.h"
#include "ui_af_params.h"
#include "UIStatic.h"

#include "..\actor.h"
#include "..\ActorCondition.h"
#include "object_broker.h"
#include "UIXmlInit.h"
#include "UIHelper.h"
#include "../string_table.h"

u32 const red_clr   = color_argb(255,210,50,50);
u32 const green_clr = color_argb(255,170,170,170);

CUIArtefactParams::CUIArtefactParams()
{
	for ( u32 i = 0; i < ALife::infl_max_count; ++i )
	{
		m_immunity_item[i] = NULL;
	}
	for ( u32 i = 0; i < ALife::eRestoreTypeMax; ++i )
	{
		m_restore_item[i] = NULL;
	}
	m_additional_weight = NULL;
	//skif : (доп иконки х-к у артов)
	m_wound_imm = NULL;
	m_fwound_imm = NULL;
	m_expl_imm = NULL;
	m_strk_imm = NULL;
	m_psy_heal_restore = NULL;
	m_walk_accel = NULL;
	m_jump_speed = NULL;
	//end
	
}

CUIArtefactParams::~CUIArtefactParams()
{
	delete_data	( m_immunity_item );
	delete_data	( m_restore_item );
	xr_delete	( m_additional_weight );
	//skif : (доп иконки х-к у артов)
	xr_delete	( m_wound_imm );
	xr_delete	( m_fwound_imm );
	xr_delete	( m_expl_imm );
	xr_delete	( m_strk_imm );
	xr_delete	(m_psy_heal_restore);
	xr_delete	(m_walk_accel);
	xr_delete	(m_jump_speed);
	//end
	xr_delete	( m_Prop_line );
}

LPCSTR af_immunity_section_names[] = // ALife::EInfluenceType
{
	"radiation_immunity",		// infl_rad=0
	"burn_immunity",			// infl_fire=1
	"chemical_burn_immunity",	// infl_acid=2
	"telepatic_immunity",		// infl_psi=3
	"shock_immunity",			// infl_electra=4

//	"strike_immunity",
//	"wound_immunity",		
//	"explosion_immunity",
//	"fire_wound_immunity",
};

LPCSTR af_restore_section_names[] = // ALife::EConditionRestoreType
{
	"health_restore_speed",			// eHealthRestoreSpeed=0
	"satiety_restore_speed",		// eSatietyRestoreSpeed=1
	"power_restore_speed",			// ePowerRestoreSpeed=2
	"bleeding_restore_speed",		// eBleedingRestoreSpeed=3
	"radiation_restore_speed",		// eRadiationRestoreSpeed=4
	//skif : (потребности актора)
	"thirst_restore_speed",		// eThirstRestoreSpeed=5
	"sleep_restore_speed",		// eSleepRestoreSpeed=6
	//end
};

LPCSTR af_immunity_caption[] =  // ALife::EInfluenceType
{
	"ui_inv_outfit_radiation_protection",		// "(radiation_imm)",
	"ui_inv_outfit_burn_protection",			// "(burn_imm)",
	"ui_inv_outfit_chemical_burn_protection",	// "(chemical_burn_imm)",
	"ui_inv_outfit_telepatic_protection",		// "(telepatic_imm)",
	"ui_inv_outfit_shock_protection",			// "(shock_imm)",

//	"ui_inv_outfit_strike_protection",			// "(strike_imm)",
//	"ui_inv_outfit_wound_protection",			// "(wound_imm)",
//	"ui_inv_outfit_explosion_protection",		// "(explosion_imm)",
//	"ui_inv_outfit_fire_wound_protection",		// "(fire_wound_imm)",
};

LPCSTR af_restore_caption[] =  // ALife::EConditionRestoreType
{
	"ui_inv_health",
	"ui_inv_satiety",
	"ui_inv_power",
	"ui_inv_bleeding",
	"ui_inv_radiation",
	//skif : (потребности актора)
	"ui_inv_thirst",
	"ui_inv_sleep",
	//end
};

/*
LPCSTR af_actor_param_names[]=
{
	"satiety_health_v",
	"radiation_v",
	"satiety_v",
	"satiety_power_v",
	"wound_incarnation_v",
};
*/

void CUIArtefactParams::InitFromXml( CUIXml& xml )
{
	LPCSTR base	= "af_params";

	XML_NODE* stored_root = xml.GetLocalRoot();
	XML_NODE* base_node   = xml.NavigateToNode( base, 0 );
	if ( !base_node )
	{
		return;
	}
	CUIXmlInit::InitWindow( xml, base, 0, this );
	xml.SetLocalRoot( base_node );
	
	m_Prop_line = xr_new<CUIStatic>();
	AttachChild( m_Prop_line );
	m_Prop_line->SetAutoDelete( false );	
	CUIXmlInit::InitStatic( xml, "prop_line", 0, m_Prop_line );

	for ( u32 i = 0; i < ALife::infl_max_count; ++i )
	{
		m_immunity_item[i] = xr_new<UIArtefactParamItem>();
		m_immunity_item[i]->Init( xml, af_immunity_section_names[i] );
		m_immunity_item[i]->SetAutoDelete(false);

		LPCSTR name = CStringTable().translate(af_immunity_caption[i]).c_str();
		m_immunity_item[i]->SetCaption( name );

		xml.SetLocalRoot( base_node );
	}

	for ( u32 i = 0; i < ALife::eRestoreTypeMax; ++i )
	{
		m_restore_item[i] = xr_new<UIArtefactParamItem>();
		m_restore_item[i]->Init( xml, af_restore_section_names[i] );
		m_restore_item[i]->SetAutoDelete(false);

		LPCSTR name = CStringTable().translate(af_restore_caption[i]).c_str();
		m_restore_item[i]->SetCaption( name );

		xml.SetLocalRoot( base_node );
	}
	
	{
		m_additional_weight = xr_new<UIArtefactParamItem>();
		m_additional_weight->Init( xml, "additional_weight" );
		m_additional_weight->SetAutoDelete(false);
		LPCSTR name = CStringTable().translate( "ui_inv_weight" ).c_str();
		m_additional_weight->SetCaption( name );
		xml.SetLocalRoot( base_node );
	}
	//skif : (доп иконки х-к у артов)
	{
		// разрыв
		m_wound_imm = xr_new<UIArtefactParamItem>();
		m_wound_imm->Init( xml, "wound_immunity" );
		m_wound_imm->SetAutoDelete(false);
		LPCSTR name = CStringTable().translate( "ui_inv_outfit_wound_protection" ).c_str();
		m_wound_imm->SetCaption( name );
		xml.SetLocalRoot( base_node );
		
		// пулестойкость
		m_fwound_imm = xr_new<UIArtefactParamItem>();
		m_fwound_imm->Init( xml, "fire_wound_immunity" );
		m_fwound_imm->SetAutoDelete(false);
		name = CStringTable().translate( "ui_inv_outfit_fire_wound_protection" ).c_str();
		m_fwound_imm->SetCaption( name );
		xml.SetLocalRoot( base_node );
		
		// взрыв
		m_expl_imm = xr_new<UIArtefactParamItem>();
		m_expl_imm->Init( xml, "explosion_immunity" );
		m_expl_imm->SetAutoDelete(false);
		name = CStringTable().translate( "ui_inv_outfit_explosion_protection" ).c_str();
		m_expl_imm->SetCaption( name );
		xml.SetLocalRoot( base_node );
		
		// удар 
		m_strk_imm = xr_new<UIArtefactParamItem>();
		m_strk_imm->Init( xml, "strike_immunity" );
		m_strk_imm->SetAutoDelete(false);
		name = CStringTable().translate( "ui_inv_outfit_strike_protection" ).c_str();
		m_strk_imm->SetCaption( name );
		xml.SetLocalRoot( base_node );
		
		// восстановление пси-здоровья 
		m_psy_heal_restore = xr_new<UIArtefactParamItem>();
		m_psy_heal_restore->Init( xml, "psy_health_restore_speed" );
		m_psy_heal_restore->SetAutoDelete(false);
		name = CStringTable().translate( "ui_inv_psy_health" ).c_str();
		m_psy_heal_restore->SetCaption( name );
		xml.SetLocalRoot( base_node );

		m_walk_accel = xr_new<UIArtefactParamItem>();
		m_walk_accel->Init( xml, "walk_accel" );
		m_walk_accel->SetAutoDelete(false);
		name = CStringTable().translate( "ui_inv_walk_accel" ).c_str();
		m_walk_accel->SetCaption( name );
		xml.SetLocalRoot( base_node );

		m_jump_speed = xr_new<UIArtefactParamItem>();
		m_jump_speed->Init( xml, "jump_speed" );
		m_jump_speed->SetAutoDelete(false);
		name = CStringTable().translate( "ui_inv_jump_speed" ).c_str();
		m_jump_speed->SetCaption( name );
		xml.SetLocalRoot( base_node );
	}
	//end

	xml.SetLocalRoot( stored_root );
}

bool CUIArtefactParams::Check(const shared_str& af_section)
{
	return !!pSettings->line_exist(af_section, "af_actor_properties");
}

void CUIArtefactParams::SetInfo( shared_str const& af_section )
{
	DetachAll();
	AttachChild( m_Prop_line );

	CActor* actor = smart_cast<CActor*>( Level().CurrentViewEntity() );
	if ( !actor )
	{
		return;
	}

	float val = 0.0f;
	Fvector2 pos;
	float h = m_Prop_line->GetWndPos().y+m_Prop_line->GetWndSize().y;

	for ( u32 i = 0; i < ALife::infl_max_count; ++i )
	{
		shared_str const& sect = pSettings->r_string( af_section, "hit_absorbation_sect" );
		int value = pSettings->r_float( sect, af_immunity_section_names[i] );
		if (fis_zero(value)) continue;
		m_immunity_item[i]->SetValue(value);

		pos.set( m_immunity_item[i]->GetWndPos() );
		pos.y = h;
		m_immunity_item[i]->SetWndPos( pos );

		h += m_immunity_item[i]->GetWndSize().y;
		AttachChild( m_immunity_item[i] );
	}

	{
		val	= pSettings->r_float( af_section, "additional_inventory_weight" );
		if ( !fis_zero(val) )
		{
			m_additional_weight->SetValue( val );

			pos.set( m_additional_weight->GetWndPos() );
			pos.y = h;
			m_additional_weight->SetWndPos( pos );

			h += m_additional_weight->GetWndSize().y;
			AttachChild( m_additional_weight );
		}
		//skif : (доп иконки х-к у артов)
		shared_str const& sect = pSettings->r_string( af_section, "hit_absorbation_sect" );
		
		// разрыв
		val	= pSettings->r_float( sect, "wound_immunity" );
		if ( !fis_zero(val) )
		{
			//max_val = actor->conditions().GetZoneMaxPower( (ALife::EHitType)5 );
			//val /= max_val;
			m_wound_imm->SetValue( val );

			pos.set( m_wound_imm->GetWndPos() );
			pos.y = h;
			m_wound_imm->SetWndPos( pos );

			h += m_wound_imm->GetWndSize().y;
			AttachChild( m_wound_imm );
		}
		
		// пулестойкость
		val	= pSettings->r_float( sect, "fire_wound_immunity" );
		if ( !fis_zero(val) )
		{
			//max_val = actor->conditions().GetZoneMaxPower( (ALife::EHitType)6 );
			//val /= max_val;
			m_fwound_imm->SetValue( val );

			pos.set( m_fwound_imm->GetWndPos() );
			pos.y = h;
			m_fwound_imm->SetWndPos( pos );

			h += m_fwound_imm->GetWndSize().y;
			AttachChild( m_fwound_imm );
		}
		
		// удар
		val	= pSettings->r_float( sect, "strike_immunity" );
		if ( !fis_zero(val) )
		{
			//max_val = actor->conditions().GetZoneMaxPower( (ALife::EHitType)7 );
			//val /= max_val;
			m_strk_imm->SetValue( val );

			pos.set( m_strk_imm->GetWndPos() );
			pos.y = h;
			m_strk_imm->SetWndPos( pos );

			h += m_strk_imm->GetWndSize().y;
			AttachChild( m_strk_imm );
		}
		
		// взрыв
		val	= pSettings->r_float( sect, "explosion_immunity" );
		if ( !fis_zero(val) )
		{
			//max_val = actor->conditions().GetZoneMaxPower( (ALife::EHitType)8 );
			//val /= max_val;
			m_expl_imm->SetValue( val );

			pos.set( m_expl_imm->GetWndPos() );
			pos.y = h;
			m_expl_imm->SetWndPos( pos );

			h += m_expl_imm->GetWndSize().y;
			AttachChild( m_expl_imm );
		}
		
		// восстановление пси-здоровья
		val	= pSettings->r_float( af_section, "psy_health_restore_speed" );
		if ( !fis_zero(val) )
		{
			m_psy_heal_restore->SetValue( val );

			pos.set( m_psy_heal_restore->GetWndPos() );
			pos.y = h;
			m_psy_heal_restore->SetWndPos( pos );

			h += m_psy_heal_restore->GetWndSize().y;
			AttachChild( m_psy_heal_restore );
		}
		//end
	}

	for ( u32 i = 0; i < ALife::eRestoreTypeMax; ++i )
	{
		val	= pSettings->r_float( af_section, af_restore_section_names[i] );
		if ( fis_zero(val) )
		{
			continue;
		}
		m_restore_item[i]->SetValue( val );

		pos.set( m_restore_item[i]->GetWndPos() );
		pos.y = h;
		m_restore_item[i]->SetWndPos( pos );

		h += m_restore_item[i]->GetWndSize().y;
		AttachChild( m_restore_item[i] );
	}

	//skif : (доп иконки х-к у артов)
	val = pSettings->r_float(af_section, "walk_accel");
	if (!fis_zero(val))
	{
		m_walk_accel->SetValue(val);

		pos.set(m_walk_accel->GetWndPos());
		pos.y = h;
		m_walk_accel->SetWndPos(pos);

		h += m_walk_accel->GetWndSize().y;
		AttachChild(m_walk_accel);
	}

	val = pSettings->r_float(af_section, "jump_speed");
	if (!fis_zero(val))
	{
		m_jump_speed->SetValue(val);

		pos.set(m_jump_speed->GetWndPos());
		pos.y = h;
		m_jump_speed->SetWndPos(pos);

		h += m_jump_speed->GetWndSize().y;
		AttachChild(m_jump_speed);
	}
	//end
	
	SetHeight( h );
}

/// ----------------------------------------------------------------

UIArtefactParamItem::UIArtefactParamItem()
{
	m_caption   = NULL;
	m_value     = NULL;
	m_magnitude = 1.0f;
	m_sign_inverse = false;
	
	m_unit_str._set( "" );
	m_texture_minus._set( "" );
	m_texture_plus._set( "" );
}

UIArtefactParamItem::~UIArtefactParamItem()
{
}

void UIArtefactParamItem::Init( CUIXml& xml, LPCSTR section )
{
	CUIXmlInit::InitWindow( xml, section, 0, this );
	xml.SetLocalRoot( xml.NavigateToNode( section ) );

	m_caption   = UIHelper::CreateStatic( xml, "caption", this );
	m_value     = UIHelper::CreateTextWnd( xml, "value",   this );
	m_magnitude = xml.ReadAttribFlt( "value", 0, "magnitude", 1.0f );
	m_sign_inverse = (xml.ReadAttribInt( "value", 0, "sign_inverse", 0 ) == 1);
	
	LPCSTR unit_str = xml.ReadAttrib( "value", 0, "unit_str", "" );
	m_unit_str._set( CStringTable().translate( unit_str ) );
	
	LPCSTR texture_minus = xml.Read( "texture_minus", 0, "" );
	if ( texture_minus && xr_strlen(texture_minus) )
	{
		m_texture_minus._set( texture_minus );
		
		LPCSTR texture_plus = xml.Read( "caption:texture", 0, "" );
		m_texture_plus._set( texture_plus );
		VERIFY( m_texture_plus.size() );
	}
}

void UIArtefactParamItem::SetCaption( LPCSTR name )
{
	m_caption->TextItemControl()->SetText( name );
}

void UIArtefactParamItem::SetValue( float value )
{
	//value *= m_magnitude;
	string32	buf;
	xr_sprintf( buf, "%+.0f", value );
	
	LPSTR		str;
	if ( m_unit_str.size() )
	{
		STRCONCAT( str, buf, " ", m_unit_str.c_str() );
	}
	else // = ""
	{
		STRCONCAT( str, buf );
	}
	m_value->SetText( str );

	bool positive = (value >= 0.0f);
	positive      = (m_sign_inverse)? !positive : positive;
	u32 color     = (positive      )? green_clr : red_clr;
	m_value->SetTextColor( color );

	if ( m_texture_minus.size() )
	{
		if ( positive )
		{
			m_caption->InitTexture( m_texture_plus.c_str() );
		}
		else
		{
			m_caption->InitTexture( m_texture_minus.c_str() );
		}
	}

}
