class MOE_ActionMountExplosiveCB : ActiondeployObjectCB
{
	override void CreateActionComponent()
	{
		float time = 20;
		MOE_ExplosiveBase explosive; 
		if(CastTo(explosive, m_ActionData.m_MainItem))
		{
			time = explosive.GetTimeToMount();
		}
		
		m_ActionData.m_ActionComponent = new CAContinuousTime(time);
	}
};

class MOE_ActionMountExplosive : ActionDeployObject 
{	
	void MOE_ActionMountExplosive()
	{		
		m_CallbackClass	= MOE_ActionMountExplosiveCB;
		m_FullBody = true;
		m_Text 	= "#STR_MOE_Mount_Explosive";
	}	
	
	
	override void SetupAnimation(ItemBase item)
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_INTERACT;
	}
	
	override bool HasAlternativeInterrupt()
	{
		return false;
	}
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (player.GetBrokenLegs() == eBrokenLegs.BROKEN_LEGS)
		{
			return false;
		}
			
		MOE_ExplosiveBase explosive;
		if(!CastTo(explosive, item) || !explosive.CanBeMounted())
		{
			return false;
		}

#ifndef SERVER
		if(!GetGame().IsDedicatedServer())
		{
			if(!player.IsPlacingLocal())
			{
				return false;
			}
			
			if(!player.GetHologramLocal().GetRaycastHit_MOE())
			{
				return false;
			}
			
			if(!item.CanBePlaced(player, player.GetHologramLocal().GetProjectionEntity().GetPosition()))
			{
				return false;
			}
		}
#endif
		return true;
	}
	
	override bool ActionConditionContinue(ActionData action_data)
	{
		if(action_data.m_Player.GetBrokenLegs() == eBrokenLegs.BROKEN_LEGS)
		{
			return false;
		}
		
		if(GetGame().IsServer())
		{
			if(!action_data.m_Player.IsPlacingServer())
			{
				return false;
			}
			
			if (!action_data.m_MainItem.CanBePlaced(action_data.m_Player, action_data.m_Player.GetHologramServer().GetProjectionEntity().GetPosition()))
			{
				return false;
			}
		}
		
		return true;
	}
	
	override void OnFinishProgressServer(ActionData action_data)
	{	
		if(!action_data.m_MainItem)
		{ 
			return; 
		}
		
		if(!action_data.m_Player.GetHologramServer())
		{ 
			return; 
		}
		
		PlaceObjectActionData poActionData;		
		if(!CastTo(poActionData, action_data))
		{ 
			return; 
		}
		
		EntityAI entity_for_placing = action_data.m_MainItem;		
		vector position = action_data.m_Player.GetLocalProjectionPosition();
		vector orientation = action_data.m_Player.GetLocalProjectionOrientation();		
		
		action_data.m_Player.GetHologramServer().PlaceEntity(entity_for_placing);
		
		if(GetGame().IsMultiplayer())
		{
			action_data.m_Player.GetHologramServer().CheckPowerSource();
		}

		action_data.m_Player.PlacingCompleteServer();
		entity_for_placing.OnPlacementComplete(action_data.m_Player, position, orientation);
		
		MoveEntityToFinalPosition(action_data, position, orientation);		
		GetGame().ClearJuncture(action_data.m_Player, entity_for_placing);
		action_data.m_MainItem.SetIsBeingPlaced(false);
		poActionData.m_AlreadyPlaced = true;	
		action_data.m_MainItem.SoundSynchRemoteReset();
		
		//required stuff
		MOE_ExplosiveBase explosive;
		if(CastTo(explosive, entity_for_placing))
		{
			explosive.Mount(); 
		}
	}
	
	override void OnStartAnimationLoop(ActionData action_data) {}
}