class MOE_ActionDismountExplosiveCB : ActionContinuousBaseCB
{
    override void CreateActionComponent()
	{
		float time = 15;
		
		MOE_ExplosiveBase explosive;
		if(CastTo(explosive, m_ActionData.m_Target.GetObject()))
		{
			time = explosive.GetTimeToDismount();
		}
				
		m_ActionData.m_ActionComponent = new CAContinuousTime(time);
	}
}

class MOE_ActionDismountExplosive : ActionContinuousBase
{
	void MOE_ActionDismountExplosive()
	{
		m_CallbackClass = MOE_ActionDismountExplosiveCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_INTERACT;
		m_FullBody = true;
		m_Text = "#STR_MOE_Dismount_Explosive";
		m_Sound = "craft_universal_0";
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionTarget = new CCTNonRuined(MOE_Constants.DISTANCE_MANIPULATE_EXPLOSIVE);
		m_ConditionItem = new CCINone();		
	}
	
	override bool HasTarget()
	{
		return true; 
	}

    override typename GetInputType()
	{
		return ContinuousInteractActionInput;
	}
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{		
		if(!GetGame().IsDedicatedServer() && player.IsPlacingLocal())
		{
			return false;
		}
				
		MOE_ExplosiveBase explosive;
        if(!CastTo(explosive, target.GetObject()))
        {
            return false;
        }

        return explosive.CanBeDismounted();
	}
	
	override void OnFinishProgressServer(ActionData action_data)
	{	
		MOE_ExplosiveBase explosive;
        if(CastTo(explosive, action_data.m_Target.GetObject()))
        {
            explosive.Dismount();
        }
	}
}