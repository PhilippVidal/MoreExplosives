class MOE_MountExplosiveActionData : PlaceObjectActionData
{
	Object m_PlacementTarget;
}

class MOE_MountExplosiveActionReceiveData : PlaceObjectActionReciveData
{
	Object m_PlacementTarget;
}

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
}

class MOE_ActionMountExplosive : ActionDeployObject 
{	
	void MOE_ActionMountExplosive()
	{		
		m_CallbackClass	= MOE_ActionMountExplosiveCB;
		m_FullBody = true;
		m_Text = "#STR_MOE_Mount_Explosive";
	}	
	
	
	override void SetupAnimation(ItemBase item)
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_INTERACT;
	}
	
	override bool HasAlternativeInterrupt()
	{
		return false;
	}
	
	override ActionData CreateActionData()
	{
		return new MOE_MountExplosiveActionData;
	}
	
	override void OnStartAnimationLoop(ActionData action_data) 
	{
		//...
	}
	
	override bool SetupAction(PlayerBase player, ActionTarget target, ItemBase item, out ActionData action_data, Param extra_data = NULL)
	{			
		if(!super.SetupAction(player, target, item, action_data, extra_data))
		{
			return false;
		}

#ifndef SERVER

		MOE_MountExplosiveActionData meActionData;
        if(!CastTo(meActionData, action_data))
        {
            return false;
        }

		MOE_Hologram hologram;
		if(!CastTo(hologram, player.GetHologramLocal()))
		{
			return false;
		}

		meActionData.m_PlacementTarget = hologram.GetPlacementTarget_MOE();
#endif

		return true;
	}
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if(player.GetBrokenLegs() == eBrokenLegs.BROKEN_LEGS)
		{
			return false;
		}
			
		MOE_ExplosiveBase explosive;
		if(!CastTo(explosive, item) || !explosive.CanBeMounted())
		{
			return false;
		}
		
		if(!GetGame().IsDedicatedServer())
		{
			MOE_Hologram hologram;
			if(!player.IsPlacingLocal() || !CastTo(hologram, player.GetHologramLocal()))
			{
				return false;
			}
			
			if(!hologram.GetRaycastHit_MOE())
			{
				return false;
			}
			
			if(!item.CanBePlaced(player, player.GetHologramLocal().GetProjectionEntity().GetPosition()))
			{
				return false;
			}
			
			if(explosive.CanOnlyMountDuringSchedule() && GetMOE().IsRaidSchedulingEnabled() && !GetMOE().IsInRaidSchedule())
			{
				return false;
			}
			
			Object placementTarget = hologram.GetPlacementTarget_MOE();
			if(!explosive.CanBeMountedOn(placementTarget))
			{
				return false;
			}
			
			m_Text = "#STR_MOE_Mount_Explosive";
			if(placementTarget)
			{
				m_Text += string.Format(" [%1]", placementTarget.GetType());
			}
		}
		
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
		MOE_ExplosiveBase explosive; 
		MOE_MountExplosiveActionData meActionData;
		if(!CastTo(meActionData, action_data) || !CastTo(explosive, action_data.m_MainItem))
		{
			return;
		}			
		
		PlayerBase player = action_data.m_Player;
		Hologram hologram = player.GetHologramServer();
		if(!hologram)
		{
			return;
		}
		
		vector position;
		vector orientation; 
		
		position = player.GetLocalProjectionPosition();
		orientation = player.GetLocalProjectionOrientation();	
		
		hologram.PlaceEntity(explosive);
		
		player.PlacingCompleteServer();
		explosive.OnPlacementComplete(player, position, orientation);
		
		MoveEntityToFinalPosition(action_data, position, orientation);
		GetGame().ClearJuncture(player, explosive);
		explosive.SetIsBeingPlaced(false);
		meActionData.m_AlreadyPlaced = true;
		explosive.SoundSynchRemoteReset();
		
		Object placementTarget = meActionData.m_PlacementTarget;
		if(explosive.CanBeMountedOn(placementTarget))
		{
			explosive.Mount(placementTarget);
		}
	}
	
	override void WriteToContext(ParamsWriteContext ctx, ActionData action_data)
	{
		super.WriteToContext(ctx, action_data);
		ctx.Write(MOE_MountExplosiveActionData.Cast(action_data).m_PlacementTarget);
	}
	
	override bool ReadFromContext(ParamsReadContext ctx, out ActionReciveData action_recive_data)
	{
        if(!action_recive_data)
		{
			action_recive_data = new MOE_MountExplosiveActionReceiveData;
		}

        MOE_MountExplosiveActionReceiveData action_data_me;
        if(!super.ReadFromContext(ctx, action_recive_data) || !CastTo(action_data_me, action_recive_data))
        {
            return false;
        }

        Object placementTarget;	
		if(!ctx.Read(placementTarget))
		{
			return false;
		}
			
		action_data_me.m_PlacementTarget = placementTarget;
		return true;
	}
	
	override void HandleReciveData(ActionReciveData action_recive_data, ActionData action_data)
	{
		super.HandleReciveData(action_recive_data, action_data);
		
		MOE_MountExplosiveActionReceiveData recive_data_me = MOE_MountExplosiveActionReceiveData.Cast(action_recive_data);
		MOE_MountExplosiveActionData action_data_me = MOE_MountExplosiveActionData.Cast(action_data);
		action_data_me.m_PlacementTarget = recive_data_me.m_PlacementTarget;
	}	
}