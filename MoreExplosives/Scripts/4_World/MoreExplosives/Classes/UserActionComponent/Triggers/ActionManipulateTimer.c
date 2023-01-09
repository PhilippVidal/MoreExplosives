class MOE_ActionManipulateTimer: ActionInteractBase
{	
	void MOE_ActionManipulateTimer()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_INTERACTONCE;
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{				
		MOE_ComponentTimerBase timer;	
		if(!GetTimerFromExplosive(timer, ItemBase.Cast( target.GetObject() )))
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

	override void OnExecuteServer(ActionData action_data)
	{
		MOE_ComponentTimerBase timer;	
		if(GetTimerFromExplosive(timer, ItemBase.Cast( action_data.m_Target.GetObject() )))
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
};
