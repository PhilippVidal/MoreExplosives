class MOE_CycleDetonatorModeActionInput : DefaultActionInput
{	
	void MOE_CycleDetonatorModeActionInput(PlayerBase player)
	{
		SetInput("UADetonatorCycleMode");	
	}
};

class MOE_ActionCycleDetonatorModeCB : ActionSingleUseBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CASingleUse();
	}
}

class MOE_ActionCycleDetonatorMode : ActionSingleUseBase
{
	void MOE_ActionCycleDetonatorMode()
	{
		m_CallbackClass 	= MOE_ActionCycleDetonatorModeCB;
		m_CommandUID        = DayZPlayerConstants.CMD_ACTIONMOD_ITEM_OFF;
		m_CommandUIDProne	= DayZPlayerConstants.CMD_ACTIONFB_ITEM_OFF;
		m_StanceMask        = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_Text = "#STR_MOE_Cycle_Detonator_Mode";
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionItem 	= new CCINonRuined();
		m_ConditionTarget 	= new CCTNone();
	}

	override typename GetInputType()
	{
		return MOE_CycleDetonatorModeActionInput;
	} 
	
	override bool HasTarget()
	{
		return false;
	}
	
	override bool HasProneException()
	{
		return true;
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		MOE_DetonatorBase detonator; 
		if(!CastTo(detonator, item))
		{
			return false;
		}

		return detonator.CanSwitchDetonatorModes();
	}
	
	override void OnExecuteServer(ActionData action_data)
	{
		super.OnExecuteServer(action_data);
		
		MOE_DetonatorBase detonator; 
		if(CastTo(detonator, action_data.m_MainItem))
		{
			detonator.SelectNextDetonatorMode();
		}
	}
}

