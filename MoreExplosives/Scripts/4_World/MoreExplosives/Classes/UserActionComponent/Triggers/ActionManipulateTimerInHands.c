class MOE_ActionManipulateTimerInHands : ActionSingleUseBase
{	
	override void CreateConditionComponents()  
	{	
		m_ConditionItem		= new CCINonRuined;
		m_ConditionTarget 	= new CCTNone;
	}

	override bool HasTarget()
	{
		return false;
	}
	
	override bool IsInstant()
	{
		return true;
	}

	override bool RemoveForceTargetAfterUse()
	{
		return false;
	}
	
	override bool UseAcknowledgment()
	{
		return true;
	}
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		//Prevent execution of action when player is trying to place explosive on surface
		if(!GetGame().IsDedicatedServer() && player.IsPlacingLocal())
		{
			return false;
		}
		
		MOE_ComponentTimerBase timer;
		if(!GetTimerFromExplosive(timer, item))
		{		
			return false;			
		}
		
		if(timer.CanBeStopped())
		{
			m_Text = string.Format("%1 [%2]", "#STR_MOE_Stop_Timer", timer.GetTimeAsFormattedString(timer.GetRemainingTime()));		
			return true;
		}

		if(timer.CanBeStarted())
		{
			m_Text = string.Format("%1 [%2]", "#STR_MOE_Start_Timer", timer.GetTimeAsFormattedString(timer.GetSelectedTime()));	
			return true;
		}
		
		return false;
	}

	override void OnStartServer(ActionData action_data)
	{
		MOE_ComponentTimerBase timer;	
		if(GetTimerFromExplosive(timer, action_data.m_MainItem))
		{
			if(timer.CanBeStopped())
			{
				timer.StopTimer();
				return;
			}

			if(timer.CanBeStarted())
			{
				timer.StartTimer();
				return;
			}
		}
	}
	
	bool GetTimerFromExplosive(out MOE_ComponentTimerBase timer, ItemBase item)
	{
		MOE_ExplosiveBase explosive;
		if(!CastTo(explosive, item))
		{
			return false;
		}
		
		if(!explosive.GetTimer(timer))
		{
			return false;
		}
		
		return true;
	}
}