// skif : (индикаторы)

#include "UIDialogWnd.h"
#include "UIWndCallback.h"
#include "../../xrServerEntities/inventory_space.h"
#include "UIHint.h"

class CUIItemInfo;
class CUIFrameLineWnd;
class CUIStatic;
class CUITextWnd;
class CUI3tButton;
class CUIProgressBar;
class UIInventoryIndicatorItem;

class CUIInventoryIndicators: public CUIWindow
{
private:
	typedef CUIWindow inherited;
	
public: 
	CUIInventoryIndicators();
	virtual ~CUIInventoryIndicators();
	CUIStatic*     m_capt_outfit;
	CUIStatic*     m_capt_afs;
	CUIStatic*     m_capt_booster;
	CUIStatic*     m_capt_total;
	
	UIInventoryIndicatorItem* m_ind_protections[10];
	UIInventoryIndicatorItem* m_ind_restores[8];
	UIInventoryIndicatorItem* m_ind_states[8];
	UIInventoryIndicatorItem* m_ind_weight;
	UIInventoryIndicatorItem* m_ind_walk_accel;
	UIInventoryIndicatorItem* m_ind_jump_speed;
	
	void FillIndicators();
	
protected:
	void Initialize();
};


class UIInventoryIndicatorItem: public CUIWindow
{
	public:
					UIInventoryIndicatorItem();
		virtual		~UIInventoryIndicatorItem();
			
		void		Init(CUIXml& xml, LPCSTR section);
		void 		SetZero();
	
		CUIStatic*	m_icon;
		CUITextWnd*	m_val_outfit;
		CUITextWnd*	m_val_af;
		CUITextWnd*	m_val_booster;
		CUITextWnd*	m_val_total;
};