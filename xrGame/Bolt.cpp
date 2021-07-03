// skif : (конечные болты)
#include "stdafx.h"
#include "bolt.h"
#include "../xrphysics/PhysicsShell.h"
#include "entity.h"
#include "ParticlesObject.h"
#include "actor.h"
#include "inventory.h"
#include "level.h"
#include "xrmessages.h"
#include "xr_level_controller.h"
#include "game_cl_base.h"
#include "xrserver_objects_alife.h"
#include "ai_object_location.h"
#include "pch_script.h"

CBolt::CBolt(void)
{
}

CBolt::~CBolt(void)
{
}

void CBolt::Load(LPCSTR section)
{
	inherited::Load(section);
}

BOOL CBolt::net_Spawn(CSE_Abstract* DC)
{
	BOOL ret = CHudItemObject::net_Spawn(DC);
	m_thrown = false;
	return ret;
}

void CBolt::net_Destroy()
{
	CHudItemObject::net_Destroy();
}

void CBolt::OnH_B_Independent(bool just_before_destroy)
{
	CHudItemObject::OnH_B_Independent(just_before_destroy);
}

void CBolt::OnH_A_Independent()
{
	inherited::OnH_A_Independent();
}

void CBolt::OnH_A_Chield()
{
	inherited::OnH_A_Chield();
	CObject* o= H_Parent()->H_Parent();
	if (o)
		SetInitiator(o->ID());
}

void CBolt::State(u32 state)
{
	switch (state)
	{
	case eThrowStart:
	{
		Fvector						C;
		Center(C);
	}break;
	case eThrowEnd:
	{
		if (m_thrown)
		{
			if (m_pPhysicsShell)
				m_pPhysicsShell->Deactivate();
			xr_delete(m_pPhysicsShell);
			PutNextToSlot();
			if (Local())
			{
				DestroyObject();
			}

		};
	}break;
	};
	inherited::State(state);
}

bool CBolt::DropBolt()
{
	EMissileStates bolt_state = static_cast<EMissileStates>(GetState());
	if (((bolt_state == eThrowStart) ||
		(bolt_state == eReady) ||
		(bolt_state == eThrow)) &&
		(!m_thrown)
		)
	{
		Throw();
		return true;
	}
	return false;
}

void CBolt::DiscardState()
{
	if (GetState() == eReady || GetState() == eThrow)
		OnStateSwitch(eIdle);
}

void CBolt::SendHiddenItem()
{
	if (GetState() == eThrow)
	{
		Throw();
	}
	CActor* pActor = smart_cast<CActor*>(m_pInventory->GetOwner());
	if (pActor && (GetState() == eReady || GetState() == eThrow))
	{
		return;
	}

	inherited::SendHiddenItem();
}

void CBolt::Throw()
{
	if (m_thrown)
		return;

	if (!m_fake_missile)
		return;

	inherited::Throw();
	m_fake_missile->processing_activate();
	m_thrown = true;
}


void CBolt::Destroy()
{
	
}

bool CBolt::Useful() const
{
	return (TestServerFlag(CSE_ALifeObject::flCanSave));
}

void CBolt::OnEvent(NET_Packet& P, u16 type)
{
	inherited::OnEvent(P, type);
}

void CBolt::PutNextToSlot()
{
	if (OnClient()) return;

	VERIFY(!getDestroy());
	NET_Packet						P;
	if (m_pInventory)
	{
		m_pInventory->Ruck(this);

		this->u_EventGen(P, GEG_PLAYER_ITEM2RUCK, this->H_Parent()->ID());
		P.w_u16(this->ID());
		this->u_EventSend(P);
	}
	else
		Msg("! PutNextToSlot : m_pInventory = NULL [%d][%d]", ID(), Device.dwFrame);

	if (smart_cast<CInventoryOwner*>(H_Parent()) && m_pInventory)
	{
		CBolt *pNext = smart_cast<CBolt*>(m_pInventory->Same(this, true));

		VERIFY(pNext != this);

		if (pNext && m_pInventory->Slot(pNext->BaseSlot(), pNext))
		{
			pNext->u_EventGen(P, GEG_PLAYER_ITEM2SLOT, pNext->H_Parent()->ID());
			P.w_u16(pNext->ID());
			P.w_u16(pNext->BaseSlot());
			pNext->u_EventSend(P);
			m_pInventory->SetActiveSlot(pNext->BaseSlot());
		}
		else
		{
			CActor* pActor = smart_cast<CActor*>(m_pInventory->GetOwner());

			if (pActor)
				pActor->OnPrevWeaponSlot();
		}

		m_thrown = false;
	}
}

void CBolt::OnAnimationEnd(u32 state)
{
	switch (state)
	{
		case eHiding:
		{
			setVisible(FALSE);
			SwitchState(eHidden);
		} break;
		case eShowing:
		{
			setVisible(TRUE);
			SwitchState(eIdle);
		} break;
		case eThrowStart:
		{
			if (!m_fake_missile && !smart_cast<CMissile*>(H_Parent()))
			{
				Level().spawn_item(*cNameSect(),Position(), (g_dedicated_server) ? u32(-1) : ai_location().level_vertex_id(), ID(), false);
			}
			if (m_throw)
				SwitchState(eThrow);
			else
				SwitchState(eReady);
		} break;
		case eThrow:
		{
			SwitchState(eThrowEnd);
		} break;
		case eThrowEnd:
		{
			SwitchState(eHidden);
		} break;
		default:
			inherited::OnAnimationEnd(state);
	}
}


void CBolt::UpdateCL()
{
	inherited::UpdateCL();
}


bool CBolt::Action(u16 cmd, u32 flags)
{
	if (inherited::Action(cmd, flags)) return true;

	switch (cmd)
	{
		//переключение типа болтов
	case kWPN_NEXT:
	{
		if (flags&CMD_START)
		{
			if (m_pInventory)
			{
				TIItemContainer::iterator it = m_pInventory->m_ruck.begin();
				TIItemContainer::iterator it_e = m_pInventory->m_ruck.end();
				for (; it != it_e; ++it)
				{
					CBolt *pBolt = smart_cast<CBolt*>(*it);
					if (pBolt && xr_strcmp(pBolt->cNameSect(), cNameSect()))
					{
						m_pInventory->Ruck(this);
						m_pInventory->SetActiveSlot(NO_ACTIVE_SLOT);
						m_pInventory->Slot(pBolt->BaseSlot(), pBolt);
						return						true;
					}
				}
				return true;
			}
		}
		return true;
	};
	}
	return false;
}

BOOL CBolt::UsedAI_Locations()
{
	return inherited::UsedAI_Locations();
}

void CBolt::net_Relcase(CObject* O)
{
	inherited::net_Relcase(O);
}

void CBolt::DeactivateItem()
{
	//Drop bolt if primed
	StopCurrentAnimWithoutCallback();
	if (!GetTmpPreDestroy() && Local() && (GetState() == eThrowStart || GetState() == eReady || GetState() == eThrow))
	{
		if (m_fake_missile)
		{
			CBolt*		pBolt = smart_cast<CBolt*>(m_fake_missile);
			if (pBolt)
			{
				if (m_pInventory->GetOwner())
				{
					CActor* pActor = smart_cast<CActor*>(m_pInventory->GetOwner());
					if (pActor)
					{
						if (!pActor->g_Alive())
						{
							m_constpower = false;
							m_fThrowForce = 0;
						}
					}
				}
				Throw();
			};
		};
	};

	inherited::DeactivateItem();
}

bool CBolt::GetBriefInfo(II_BriefInfo& info)
{
	VERIFY(m_pInventory);
	info.clear();

	info.name._set(m_nameShort);
	info.icon._set(cNameSect());

	u32 ThisBoltCount = m_pInventory->dwfGetSameItemCount(cNameSect().c_str(), true);

	string16 stmp;
	xr_sprintf(stmp, "%d", ThisBoltCount);
	info.cur_ammo._set(stmp);
	return true;
}

void CBolt::SetInitiator(u16 id)
{
	m_thrower_id = id;
}

u16	CBolt::Initiator()
{
	return m_thrower_id;
}

void CBolt::activate_physic_shell()
{
	inherited::activate_physic_shell	();
	m_pPhysicsShell->SetAirResistance	(.0001f);
}

using namespace luabind;

#pragma optimize("s",on)
void CBolt::script_register(lua_State *L)
{
	module(L)
		[
			class_<CBolt, CGameObject>("CBolt")
			.def(constructor<>())
		];
}
