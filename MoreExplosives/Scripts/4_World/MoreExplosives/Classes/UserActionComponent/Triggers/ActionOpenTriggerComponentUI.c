class MOE_ActionOpenTriggerComponentUI: ActionInteractBase
{	
	void MOE_ActionOpenTriggerComponentUI()
	{
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_INTERACTONCE;
		m_Text = "#STR_MOE_Configure_Trigger";
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{	
		if(!target)
		{
			return false;
		}
		
		MOE_ComponentTriggerBase trigger;
		if(!GetTriggerFromItem(trigger, ItemBase.Cast(target.GetObject())))
		{
			return false;
		}

		return trigger.HasUI() && trigger.CanOpenUI();
	}

	override void OnExecuteClient(ActionData action_data)
	{		
		MOE_ComponentTriggerBase trigger;
		if(GetTriggerFromItem(trigger, ItemBase.Cast(action_data.m_Target.GetObject())))
		{
			trigger.OpenUI();
		}
	}
	
	bool GetTriggerFromItem(out MOE_ComponentTriggerBase trigger, ItemBase item)
	{
		if(!CastTo(trigger, item))
		{
			MOE_ExplosiveBase explosive;
			if(!CastTo(explosive, item))
			{
				return false;
			}
			
			if(!explosive.ValidateTrigger())
			{
				return false;
			}
			
			trigger = explosive.GetTrigger();
		}
		
		return true;
	}
};