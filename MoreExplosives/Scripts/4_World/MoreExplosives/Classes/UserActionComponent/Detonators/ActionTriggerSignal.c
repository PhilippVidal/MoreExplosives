//Action that triggers a signal from a signal source (e.g., a detonator) and activates the connected receiver (e.g., on an explosive)
//should be added to a signal source (e.g., a detonator)
class MOE_ActionTriggerSignal : ActionSingleUseBase
{	
	void MOE_ActionTriggerSignal()
	{
		m_CommandUID        = DayZPlayerConstants.CMD_ACTIONMOD_PRESS_TRIGGER;
		m_CommandUIDProne 	= DayZPlayerConstants.CMD_ACTIONFB_PRESS_TRIGGER;
		m_Text 			= "#STR_MOE_Trigger_Detonator";
	}
	
	override void CreateConditionComponents()
	{	
		m_ConditionTarget	= new CCTNone();
		m_ConditionItem		= new CCINonRuined();
	}

	override bool HasProneException()
	{
		return true;
	}
	
	override bool HasProgress()
	{
		return true;
	}
	
	override bool HasTarget()
	{
		return false;
	}
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
 		MOE_SignalSource source;
        if(!CastTo(source, item))
		{		
			return false;
		}      

		if(!source.CanBeTriggered())
		{
			return false;
		}

        return true;
	}
	
	override void OnStartClient(ActionData action_data)
	{		
		MOE_DetonatorBase detonator;
        if(!CastTo(detonator, action_data.m_MainItem))
		{	
			return;
		}  
            
        detonator.PlayTriggerAnimation();		
	}
	
	override void OnExecuteServer(ActionData action_data)
	{
		MOE_SignalSource source;
        if(!CastTo(source, action_data.m_MainItem))
		{				
			return;
		}  

		if(Math.RandomFloat01() < source.GetFailureChance())
		{
			return;
		}
		
        source.TriggerSignal(action_data.m_Player);
	}
}
