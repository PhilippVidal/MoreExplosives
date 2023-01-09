class MOE_ActionCycleSelectedReceiverCB : ActionSingleUseBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CASingleUseTurnOnPlugged;
	}
}

class MOE_ActionCycleSelectedReceiver : ActionSingleUseBase
{	
	void MOE_ActionCycleSelectedReceiver()
	{
		m_CallbackClass 	= MOE_ActionCycleSelectedReceiverCB;
		m_CommandUID        = DayZPlayerConstants.CMD_ACTIONMOD_ITEM_ON;
		m_CommandUIDProne	= DayZPlayerConstants.CMD_ACTIONFB_ITEM_ON;
		m_Text = "#STR_MOE_Cycle_Selected_Explosive";
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTNone;
	}
	
	override bool HasTarget()
	{
		return false;
	}
		
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if(GetGame().IsServer())
		{
			return true;
		}
			
		MOE_DetonatorBase detonator;
		if(!CastTo(detonator, item))
		{
			return false; 
		}
		
		return detonator.CanSwitchSelectedReceiver();
	}
		
	override void OnExecuteServer(ActionData action_data)
	{
		MOE_DetonatorBase detonator;
		if(CastTo(detonator, action_data.m_MainItem))
		{
			detonator.SelectNextReceiver();
		}	
	}
}
