class MOE_ActionMountExplosiveInvalid : ActionContinuousBase 
{	
	void MOE_ActionMountExplosiveInvalid()
	{		
		m_CallbackClass	= ActionContinuousBaseCB;
		m_FullBody = true;
		m_Text = "";
	}	
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		return player.IsPlacingLocal() && GetFailureReason(GetMOE().MountingFailureState, m_Text);
	}
	
	override bool ActionConditionContinue(ActionData action_data)
	{	
		return false;
	}

	bool GetFailureReason(int failureReason, out string failureText)
	{
		if(failureReason == MOE_EMountingFailureReasons.SUCCESS)
		{
			return false;
		}

		switch(failureReason)
		{
			case MOE_EMountingFailureReasons.EXPLOSIVE_MOUNTABLE:
				failureText = "#STR_MOE_MountingFailure_ExplosiveMountable";
			break;
			case MOE_EMountingFailureReasons.RAYCAST_HIT:
				failureText = "#STR_MOE_MountingFailure_RaycastHit";
			break;
			case MOE_EMountingFailureReasons.EXPLOSIVE_PLACEABLE:
				failureText = "#STR_MOE_MountingFailure_ExplosivePlaceable";
			break;
			case MOE_EMountingFailureReasons.RAID_SCHEDULE:
				int days, hours, minutes; 
				if(!GetMOE().GetRaidScheduling().GetTimeTillNextTimeSlot(days, hours, minutes))
				{
					failureText = "No valid time slots set!";
				}
			
				failureText = string.Format("#STR_MOE_MountingFailure_RaidSchedule [%1d %2h %3m]", days, hours, minutes);
			break;
			case MOE_EMountingFailureReasons.TARGET_MOUNTABLE:
				failureText = "#STR_MOE_MountingFailure_TargetMountable";
			break;
			default: 
				failureText = "#STR_MOE_MountingFailure_Other";
		}

		return true;
	}
}