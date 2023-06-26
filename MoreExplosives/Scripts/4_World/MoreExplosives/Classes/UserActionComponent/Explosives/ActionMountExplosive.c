class MOE_MountExplosiveActionData : PlaceObjectActionData
{
	Object m_PlacementTarget;
	int m_HitComponent;
}

class MOE_MountExplosiveActionReceiveData : PlaceObjectActionReciveData
{
	Object m_PlacementTarget;
	int m_HitComponent;
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
	protected vector m_InitialTargetPosition;
	protected Object m_PlacementTarget;
	protected int m_PlacementComponent;

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

		meActionData.m_PlacementTarget = m_PlacementTarget;
		meActionData.m_HitComponent = m_PlacementComponent;
#endif

		return true;
	}
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{		
		if(player.GetBrokenLegs() == eBrokenLegs.BROKEN_LEGS)
		{
			GetMOE().MountingFailureState = MOE_EMountingFailureReasons.OTHER;
			return false;
		}
			
		MOE_ExplosiveBase explosive;
		if(!CastTo(explosive, item) || !explosive.CanBeMounted())
		{
			GetMOE().MountingFailureState = MOE_EMountingFailureReasons.EXPLOSIVE_MOUNTABLE;
			return false;
		}
		
		if(!GetGame().IsDedicatedServer())
		{
			MOE_Hologram hologram;
			if(!player.IsPlacingLocal() || !CastTo(hologram, player.GetHologramLocal()))
			{			
				GetMOE().MountingFailureState = MOE_EMountingFailureReasons.OTHER;
				return false;
			}
			
			if(!hologram.GetRaycastHit_MOE())
			{
				GetMOE().MountingFailureState = MOE_EMountingFailureReasons.RAYCAST_HIT;
				return false;
			}
			
			vector position = player.GetHologramLocal().GetProjectionEntity().GetPosition();
			if(!item.CanBePlaced(player, position))
			{
				GetMOE().MountingFailureState = MOE_EMountingFailureReasons.EXPLOSIVE_PLACEABLE;
				return false;
			}
			
			if(explosive.CanOnlyMountDuringSchedule() && GetMOESettings().IsRaidSchedulingEnabled && !GetMOE().GetRaidScheduling().IsInSchedule())
			{
				GetMOE().MountingFailureState = MOE_EMountingFailureReasons.RAID_SCHEDULE;
				return false;
			}
			
			m_PlacementTarget = hologram.GetPlacementTarget_MOE();
			m_PlacementComponent = hologram.GetHitComponent_MOE();
			if(!explosive.CanBeMountedOn(m_PlacementTarget, m_PlacementComponent, position))
			{
				GetMOE().MountingFailureState = MOE_EMountingFailureReasons.TARGET_MOUNTABLE;
				return false;
			}
			
			m_InitialTargetPosition = GetPlacementPositionLocal(m_PlacementTarget, m_PlacementComponent, position, player.GetLocalProjectionOrientation());

			m_Text = "#STR_MOE_Mount_Explosive";
			if(m_PlacementTarget)
			{
				m_Text += string.Format(" [%1]", m_PlacementTarget.GetType());
			}
			
		}
		
		GetMOE().MountingFailureState = MOE_EMountingFailureReasons.SUCCESS;
		return true;
	}
	
	override bool ActionConditionContinue(ActionData action_data)
	{
		if(action_data.m_Player.GetBrokenLegs() == eBrokenLegs.BROKEN_LEGS)
		{
			return false;
		}
		
		if(!GetGame().IsServer())
		{
			MOE_MountExplosiveActionData meActionData;
			if(!CastTo(meActionData, action_data))
			{
				return false;
			}

			Object placementTarget = meActionData.m_PlacementTarget;
			if(!placementTarget)
			{
				return true;
			}
			
			vector currentLocalPosition = GetPlacementPositionLocal(placementTarget, meActionData.m_HitComponent, meActionData.m_Position, meActionData.m_Orientation);
			return HDSN_MiscFunctions.ApproximatelyEqual(currentLocalPosition, m_InitialTargetPosition, MOE_Constants.DISTANCE_PLACEMENT_VARIANCE);
		}

		if(!action_data.m_Player.IsPlacingServer())
		{
			return false;
		}
		
		if (!action_data.m_MainItem.CanBePlaced(action_data.m_Player, action_data.m_Player.GetHologramServer().GetProjectionEntity().GetPosition()))
		{
			return false;
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
				
		vector position = player.GetLocalProjectionPosition();
		vector orientation = player.GetLocalProjectionOrientation();	
		
		hologram.PlaceEntity(explosive);
		
		player.PlacingCompleteServer();
		explosive.OnPlacementComplete(player, position, orientation);
		
		MoveEntityToFinalPosition(action_data, position, orientation);
		GetGame().ClearJuncture(player, explosive);
		explosive.SetIsBeingPlaced(false);
		meActionData.m_AlreadyPlaced = true;
		explosive.SoundSynchRemoteReset();
		
		Object placementTarget = meActionData.m_PlacementTarget;
		int hitComponent = meActionData.m_HitComponent;
		vector currentLocalPos = GetPlacementPositionLocal(placementTarget, hitComponent, position, orientation);
		
		if(placementTarget && !HDSN_MiscFunctions.ApproximatelyEqual(currentLocalPos, m_InitialTargetPosition, MOE_Constants.DISTANCE_PLACEMENT_VARIANCE))
		{
			return;
		}
		
		if(explosive.CanOnlyMountDuringSchedule() && GetMOESettings().IsRaidSchedulingEnabled && !GetMOE().GetRaidScheduling().IsInSchedule())
		{
			return;
		}
		
		if(explosive.CanBeMountedOn(placementTarget, hitComponent, position))
		{
			explosive.Mount(placementTarget, hitComponent, position, orientation);
		}
	}
	
	override void WriteToContext(ParamsWriteContext ctx, ActionData action_data)
	{
		super.WriteToContext(ctx, action_data);
		MOE_MountExplosiveActionData actionData = MOE_MountExplosiveActionData.Cast(action_data);
		ctx.Write(actionData.m_PlacementTarget);
		ctx.Write(actionData.m_HitComponent);
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
		
		int hitComponent;	
		if(!ctx.Read(hitComponent))
		{
			return false;
		}
			
		action_data_me.m_PlacementTarget = placementTarget;
		action_data_me.m_HitComponent = hitComponent;
		return true;
	}
	
	override void HandleReciveData(ActionReciveData action_recive_data, ActionData action_data)
	{
		super.HandleReciveData(action_recive_data, action_data);
		
		MOE_MountExplosiveActionReceiveData recive_data_me = MOE_MountExplosiveActionReceiveData.Cast(action_recive_data);
		MOE_MountExplosiveActionData action_data_me = MOE_MountExplosiveActionData.Cast(action_data);
		action_data_me.m_PlacementTarget = recive_data_me.m_PlacementTarget;
		action_data_me.m_HitComponent = recive_data_me.m_HitComponent;
		
		m_InitialTargetPosition = GetPlacementPositionLocal(
										action_data_me.m_PlacementTarget,
										action_data_me.m_HitComponent,
										action_data_me.m_Position,
										action_data_me.m_Orientation);
	}	

	vector GetPlacementPositionLocal(Object placementTarget, int component, vector placementPosition, vector placementOrientation)
	{
		if(!placementTarget)
		{
			return vector.Zero;
		}

		int pivot = HDSN_MiscFunctions.GetPivotFromComponent(placementTarget, component);
		if (pivot == -1)
		{
			return placementTarget.WorldToModel(placementPosition);
		}

		vector rotation_matrix[3];
		Math3D.YawPitchRollMatrix(placementOrientation, rotation_matrix);
		
		vector placementTransform[4]; 
		placementTransform[0] = rotation_matrix[0];
		placementTransform[1] = rotation_matrix[1];
		placementTransform[2] = rotation_matrix[2];
		placementTransform[3] = placementPosition;
		
		vector boneTransform[4];
		placementTarget.GetBoneTransformWS(pivot, boneTransform);
		Math3D.MatrixInvMultiply4(boneTransform, placementTransform, placementTransform);

		return placementTransform[3];
	}
}