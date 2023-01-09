class MOE_ActionToggleExplosivePlacement : ActionSingleUseBase
{
	void MOE_ActionToggleExplosivePlacement()
	{
		m_Text = "#toggle_placing";
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
	
	override bool IsLocal()
	{
		return true;
	}
	
	override bool IsInstant()
	{
		return true;
	}
	
	override bool IsDeploymentAction()
	{
		return true;
	}
	
	override bool RemoveForceTargetAfterUse()
	{
		return false;
	}
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		MOE_ExplosiveBase explosive;
		if(!CastTo(explosive, item))
		{
			return false;
		}
		
		return explosive.CanBeMounted();
	}
	
	override void Start(ActionData action_data)
	{
		super.Start( action_data );
		action_data.m_Player.TogglePlacingLocal();
	}
}