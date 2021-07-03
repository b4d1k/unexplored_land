#pragma once

#include "UIWindow.h"
#include "UIDoubleProgressBar.h"
#include "..\..\xrServerEntities\alife_space.h"

class CCustomOutfit;
class CHelmet;
class CUIStatic;
class CUIDoubleProgressBar;
class CUIXml;

class CUIOutfitImmunity : public CUIWindow
{
public:
					CUIOutfitImmunity	();
	virtual			~CUIOutfitImmunity	();

			void	Init(CUIXml& xml, LPCSTR section);
			void	SetCaption(LPCSTR name);
			void	SetValue(float value);
			void	SetValue(LPCSTR value);

protected:
	CUIStatic*	m_caption;
	CUITextWnd*	m_value;
	bool		m_sign_inverse;
	bool		m_with_plus;
	shared_str	m_unit_str;
	shared_str	m_texture_minus;
	shared_str	m_texture_plus;

}; // class CUIOutfitImmunity

// -------------------------------------------------------------------------------------

class CUIOutfitInfo : public CUIWindow
{
public:
					CUIOutfitInfo		();
	virtual			~CUIOutfitInfo		();

			void 	InitFromXml			( CUIXml& xml_doc );
			void 	UpdateInfo			( CCustomOutfit* cur_outfit, CCustomOutfit* slot_outfit = NULL );	
			void 	UpdateInfo			( CHelmet* cur_helmet, CHelmet* slot_helmet = NULL );	
protected:
	enum				{ max_count = ALife::eHitTypeMax-2 + 19 }; // skif : (взрыв, удар в описании бронежилетов), (защита головы в описании бронежилетов), (маскировка в описании бронежилетов), (затраты сил в описании бронежилетов), (кол-во контейнеров под арты в описании бронежилетов), (доп переносимый вес в описании бронежилетов)
	
//	CUIStatic*			m_caption;
	CUIStatic*			m_Prop_line;
	CUIOutfitImmunity*	m_items[max_count];

}; // class CUIOutfitInfo
