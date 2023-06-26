//Action that open the UI of a trigger component, should be added to a trigger component or explosive
class MOE_ActionOpenTriggerComponentUIInHands : ActionSingleUseBase
{
	void MOE_ActionOpenTriggerComponentUIInHands()
	{
		m_Text = "#STR_MOE_Configure_Trigger";
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionItem		= new CCINonRuined();
		m_ConditionTarget 	= new CCTNone();
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
		if(!GetGame().IsDedicatedServer() && player.IsPlacingLocal())
		{
			return false;
		}
		
		MOE_ComponentTriggerBase trigger;
		if(!GetTriggerFromItem(trigger, item))
		{
			return false;
		}

		return trigger.HasUI() && trigger.CanOpenUI();
	}

	override void OnStartClient(ActionData action_data)
	{
		super.OnStartClient(action_data);

		MOE_ComponentTriggerBase trigger;
		if(GetTriggerFromItem(trigger, action_data.m_MainItem))
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
}