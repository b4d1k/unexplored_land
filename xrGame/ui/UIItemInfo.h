#pragma once
#include "uiwindow.h"


class CInventoryItem;
class CUIStatic;
class CUITextWnd;
class CUIScrollView;
class CUIProgressBar;
class CUIConditionParams;
class CUIWpnParams;
class CUIArtefactParams;
class CUIFrameWindow;
class UIInvUpgPropertiesWnd;
class CUIOutfitInfo;
class CUIBoosterInfo;
class CUICellItem;
// skif : (батарейка для фонарика)
class CUITorchBatareyInfo;
class UIBoosterInfoItem;
class CUIXml;
// end
class CUICanisterInfo;
class CUICarToolkitInfo;

extern const char * const 		fieldsCaptionColor;

class CUIItemInfo: public CUIWindow
{
private:
	typedef CUIWindow inherited;
	struct _desc_info
	{
		CGameFont*			pDescFont;
		u32					uDescClr;
		bool				bShowDescrText;
	};
	_desc_info				m_desc_info;
	CInventoryItem* m_pInvItem;
public:
						CUIItemInfo			();
	virtual				~CUIItemInfo		();
	CInventoryItem*		CurrentItem			() const {return m_pInvItem;}
	void				InitItemInfo		(Fvector2 pos, Fvector2 size, LPCSTR xml_name);
	void				InitItemInfo		(LPCSTR xml_name);
	void				InitItem			(CUICellItem* pCellItem, CInventoryItem* pCompareItem = NULL, u32 item_price=u32(-1), LPCSTR trade_tip=NULL);


	void				TryAddConditionInfo	(CInventoryItem& pInvItem, CInventoryItem* pCompareItem);
	void				TryAddWpnInfo		(CInventoryItem& pInvItem, CInventoryItem* pCompareItem);
	void				TryAddArtefactInfo	(const shared_str& af_section);
	void				TryAddOutfitInfo	(CInventoryItem& pInvItem, CInventoryItem* pCompareItem);
	void				TryAddUpgradeInfo	(CInventoryItem& pInvItem);
	void				TryAddBoosterInfo	(CInventoryItem& pInvItem);
	// skif : (батарейка для фонарика)
	void				TryAddTorchBatareyInfo(const shared_str& section, CInventoryItem& pInvItem);
	// end
	void				TryAddCanisterInfo(const shared_str& section, CInventoryItem& pInvItem);
	void				TryAddCarToolkitInfo(const shared_str& section, CInventoryItem& pInvItem);
	
	virtual void		Draw				();
	bool				m_b_FitToHeight;
	u32					delay;
	
	CUIFrameWindow*		UIBackground;
	CUITextWnd*			UIName;
	CUITextWnd*			UIWeight;
	CUITextWnd*			UICost;
	CUITextWnd*			UITradeTip;
//	CUIStatic*			UIDesc_line;
	CUIScrollView*		UIDesc;
	bool				m_complex_desc;

//	CUIConditionParams*		UIConditionWnd;
	CUIWpnParams*			UIWpnParams;
	CUIArtefactParams*		UIArtefactParams;
	UIInvUpgPropertiesWnd*	UIProperties;
	CUIOutfitInfo*			UIOutfitInfo;
	CUIBoosterInfo*			UIBoosterInfo;
	// skif : (батарейка для фонарика)
	CUITorchBatareyInfo*	UITorchBatareyInfo;
	// end
	CUICanisterInfo*		UICanisterInfo;
	CUICarToolkitInfo*		UICarToolkitInfo;

	Fvector2			UIItemImageSize; 
	CUIStatic*			UIItemImage;
};


// skif : (батарейка для фонарика)
class CUITorchBatareyInfo : public CUIWindow
{
public:
					CUITorchBatareyInfo		();
	virtual			~CUITorchBatareyInfo		();
			void	InitFromXml				(CUIXml& xml);
			void	SetInfo					(const shared_str& section, CInventoryItem& pInvItem);
			bool	Check					(const shared_str& section);

protected:
	UIBoosterInfoItem*	m_torch_batarey_power;
	UIBoosterInfoItem*	m_torch_batarey_power_reduction_speed;
	CUIStatic*			m_Prop_line;

};
// end


// skif
class CUICanisterInfo : public CUIWindow
{
public:
					CUICanisterInfo();
	virtual			~CUICanisterInfo();
			void	InitFromXml				(CUIXml& xml);
			void	SetInfo					(const shared_str& section, CInventoryItem& pInvItem);
			bool	Check					(const shared_str& section);

protected:
	UIBoosterInfoItem*	m_fuel;
	UIBoosterInfoItem*	m_volume;
	CUIStatic*			m_Prop_line;

};

class CUICarToolkitInfo : public CUIWindow
{
public:
					CUICarToolkitInfo();
	virtual			~CUICarToolkitInfo();
			void	InitFromXml				(CUIXml& xml);
			void	SetInfo					(const shared_str& section, CInventoryItem& pInvItem);
			bool	Check					(const shared_str& section);

protected:
	UIBoosterInfoItem*	m_capacity;
	UIBoosterInfoItem*	m_applying;
	CUIStatic*			m_Prop_line;

};
// end