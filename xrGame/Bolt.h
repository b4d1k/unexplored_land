// skif : (конечные болты)
#pragma once
#include "missile.h"
#include "../xrphysics/DamageSource.h"
#include "../xrEngine/feel_touch.h"
#include "script_export_space.h"

class CBolt :
	public CMissile,
	public IDamageSource
{
	typedef CMissile		inherited;
public:
	CBolt();
	virtual					~CBolt();
	u16						m_thrower_id;


	virtual void			Load(LPCSTR section);

	virtual BOOL 			net_Spawn(CSE_Abstract* DC);
	virtual void 			net_Destroy();
	virtual void 			net_Relcase(CObject* O);

	virtual void 			OnH_B_Independent(bool just_before_destroy);
	virtual void 			OnH_A_Independent();
	virtual void 			OnH_A_Chield();
	virtual void			DiscardState();

	virtual void 			OnEvent(NET_Packet& P, u16 type);
	virtual bool			DropBolt();

	virtual void 			OnAnimationEnd(u32 state);
	virtual void 			UpdateCL();

	virtual void 			Throw();
	virtual void 			Destroy();

	virtual bool			Action(u16 cmd, u32 flags);
	virtual bool			Useful() const;
	virtual void			State(u32 state);
	
	virtual	void SetInitiator		(u16 id);
	virtual	u16	 Initiator			();

	virtual void			OnH_B_Chield() { inherited::OnH_B_Chield(); }

	void					PutNextToSlot();

	virtual void			DeactivateItem();
	virtual bool			GetBriefInfo(II_BriefInfo& info);

	virtual void			SendHiddenItem();
    virtual void 			activate_physic_shell	();
private:
	bool					m_thrown;

protected:
	virtual	void			UpdateXForm() { CMissile::UpdateXForm(); };
public:

	virtual BOOL			UsedAI_Locations();
	virtual IDamageSource   *cast_IDamageSource()	{return this;}
	virtual CMissile		*cast_missile() { return this; }
	virtual CHudItem		*cast_hud_item() { return this; }
	virtual CGameObject		*cast_game_object() { return this; }

	DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(CBolt)
#undef script_type_list
#define script_type_list save_type_list(CBolt)