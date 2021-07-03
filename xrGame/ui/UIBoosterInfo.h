#pragma once
#include "UIWindow.h"
#include "../EntityCondition.h"

class CUIXml;
class CUIStatic;
class CUITextWnd;
class UIBoosterInfoItem;

class CUIBoosterInfo : public CUIWindow
{
public:
					CUIBoosterInfo		();
	virtual			~CUIBoosterInfo		();
			void	InitFromXml				(CUIXml& xml);
			void	SetInfo					(const shared_str& section);

protected:
	UIBoosterInfoItem*	m_booster_items[eBoostExplImmunity];
	UIBoosterInfoItem*	m_booster_satiety;
	//skif : (потребности актора)
	UIBoosterInfoItem*	m_booster_thirst;
	UIBoosterInfoItem*	m_booster_sleep;
	//end
	//skif : (иконки у бустеров)
	UIBoosterInfoItem*	m_booster_alcohol;
	UIBoosterInfoItem*	m_booster_health_add;
	UIBoosterInfoItem*	m_booster_radio_add;
	UIBoosterInfoItem*	m_booster_portions_num;
	UIBoosterInfoItem*	m_booster_nausea;
	UIBoosterInfoItem*	m_booster_unnausea;
	UIBoosterInfoItem*	m_booster_power_add;
	UIBoosterInfoItem*	m_booster_bleed_add;
	//end
	UIBoosterInfoItem*	m_booster_anabiotic;
	UIBoosterInfoItem*	m_booster_time;

	CUIStatic*			m_Prop_line;

}; // class CUIBoosterInfo

// -----------------------------------

class UIBoosterInfoItem : public CUIWindow
{
public:
				UIBoosterInfoItem	();
	virtual		~UIBoosterInfoItem();
		
		void	Init				( CUIXml& xml, LPCSTR section );
		void	SetCaption			( LPCSTR name );
		void	SetValue			( float value );
		void	SetUnsignedValue	( float value ); //skif : (иконки у бустеров)
		// skif : (батарейка для фонарика) - перенос из private в public
		CUITextWnd*	m_value;
		// end
private:
	CUIStatic*	m_caption;
	float		m_magnitude;
	bool		m_show_sign;
	bool		m_sign_inverse;
	shared_str	m_unit_str;
	shared_str	m_texture_minus;
	shared_str	m_texture_plus;

}; // class UIBoosterInfoItem
