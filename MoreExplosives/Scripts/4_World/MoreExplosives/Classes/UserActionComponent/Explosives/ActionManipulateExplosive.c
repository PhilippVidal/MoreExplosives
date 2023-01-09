//////////////////////////////
////// Callback classes //////
//////////////////////////////

class MOE_ActionManipulateExplosiveBaseCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		float time = GetManipulationTime(GetExplosiveFromActionData());
		m_ActionData.m_ActionComponent = new CAContinuousTime(time);
	}
	
	override void InitActionComponent()
	{
		super.InitActionComponent();
		RegisterAnimationEvent("CraftingAction", UA_IN_CRAFTING);
	}

	MOE_ExplosiveBase GetExplosiveFromActionData()
	{
		return MOE_ExplosiveBase.Cast(m_ActionData.m_Target.GetObject());
	}

	float GetManipulationTime(MOE_ExplosiveBase explosive)
	{
		float time = 15;

		if(explosive)
		{
			int state = explosive.GetState();
			switch(state)
			{
				case MOE_EExplosiveStates.ACTIVE:
					time = explosive.GetTimeToDefuse();
				break;
				case MOE_EExplosiveStates.ARMED:
					time = explosive.GetTimeToDisarm();
				break;
				case MOE_EExplosiveStates.DISARMED:
					time = explosive.GetTimeToArm();
				break;
			}
		}
		return time;
	}
}

class MOE_ActionManipulateExplosiveInHandsCB : MOE_ActionManipulateExplosiveBaseCB
{
	override MOE_ExplosiveBase GetExplosiveFromActionData()
	{
		return MOE_ExplosiveBase.Cast(m_ActionData.m_MainItem);
	}
}




/////////////////////
////// Actions //////
/////////////////////

class MOE_ActionManipulateExplosiveBase : ActionContinuousBase
{
	void MOE_ActionManipulateExplosiveBase()
	{
		m_CallbackClass = MOE_ActionManipulateExplosiveBaseCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_CRAFTING;
		m_FullBody = true;
		m_Sound = "craft_universal_0";
	}
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{		
        return GetGame().IsDedicatedServer() || !player.IsPlacingLocal();
	}
	
	void ManipulateExplosive(ItemBase item, ItemBase usedTool = null)
	{
		MOE_ExplosiveBase explosive;
		if(!CastTo(explosive, item))
		{
			return;
		}	
		
		int state = explosive.GetState();
		switch(state)
		{
			case MOE_EExplosiveStates.ACTIVE:
				explosive.Defuse(usedTool);
			break;
			case MOE_EExplosiveStates.ARMED:
				explosive.Disarm(usedTool);
			break;
			case MOE_EExplosiveStates.DISARMED:
				explosive.Arm(usedTool);
			break;
		}			
	}	
	
	bool CanManipulateExplosive(ItemBase item, ItemBase usedTool = null)
	{
		MOE_ExplosiveBase explosive;
		if(CastTo(explosive, item))
		{
			int state = explosive.GetState();
			switch(state)
			{
				case MOE_EExplosiveStates.ACTIVE:
                    m_Text = "#STR_MOE_Defuse_Explosive";
				
					MOE_ComponentTimerBase timer;
					if(explosive.GetTimer(timer))
					{
						m_Text += string.Format(" [%1]", timer.GetTimeAsFormattedString(timer.GetRemainingTime()));	
					}
				
					return explosive.CanBeDefused() && explosive.CanBeDefusedWith(usedTool);
				case MOE_EExplosiveStates.ARMED:
                    m_Text = "#STR_MOE_Disarm_Explosive";
					return explosive.CanBeDisarmed() && explosive.CanBeDisarmedWith(usedTool);
				case MOE_EExplosiveStates.DISARMED:
                    m_Text = "#STR_MOE_Arm_Explosive";
					return explosive.CanBeArmed() && explosive.CanBeArmedWith(usedTool);
			}
		}	
		
		return false;
	}
}

class MOE_ActionManipulateExplosive : MOE_ActionManipulateExplosiveBase
{
	override void CreateConditionComponents()  
	{	
		m_ConditionTarget = new CCTNonRuined(MOE_Constants.DISTANCE_MANIPULATE_EXPLOSIVE);
		m_ConditionItem = new CCINone();
	}
	
	override typename GetInputType()
	{
		return ContinuousInteractActionInput;
	}
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{				
		return super.ActionCondition(player, target, item) && CanManipulateExplosive(ItemBase.Cast(target.GetObject()), item);
	}
	
	override void OnFinishProgressServer(ActionData action_data)
	{	
		ManipulateExplosive(ItemBase.Cast(action_data.m_Target.GetObject()), action_data.m_MainItem);
	}	

}

class MOE_ActionManipulateExplosiveInHands : MOE_ActionManipulateExplosiveBase
{
	void MOE_ActionManipulateExplosiveInHands()
	{
		m_CallbackClass = MOE_ActionManipulateExplosiveInHandsCB;
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionTarget = new CCTNone();
		m_ConditionItem = new CCINonRuined();		
	}
	
	override bool HasTarget()
	{
		return false; 
	}
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{				
		return super.ActionCondition(player, target, item) && CanManipulateExplosive(item, null);
	}
	
	override void OnFinishProgressServer(ActionData action_data)
	{	
		ManipulateExplosive(action_data.m_MainItem, null);
	}
}

class MOE_ActionManipulateExplosiveInHandsWithTool : MOE_ActionManipulateExplosiveBase
{	
	void MOE_ActionManipulateExplosiveInHandsWithTool()
	{
		m_CallbackClass = MOE_ActionManipulateExplosiveInHandsCB;
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionTarget = new CCTNonRuined(0); 
		m_ConditionItem = new CCINonRuined();		
	}
	
	override bool HasTarget()
	{
		return true; 
	}
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{		
		return super.ActionCondition(player, target, item) && CanManipulateExplosive(item, ItemBase.Cast(target.GetObject()));
	}
	
	override void OnFinishProgressServer(ActionData action_data)
	{	
		ManipulateExplosive(action_data.m_MainItem, ItemBase.Cast(action_data.m_Target.GetObject()));
	}	
}