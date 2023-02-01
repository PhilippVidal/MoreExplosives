//Action that connects the signal source to a receiver, should be added to a signal source (e.g., a detonator)
//Can be done when looking at a receiver or explosive, while having a signal source in hand
//TODO: 
// - update text 

class MOE_ActionConnectReceiverCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(MOE_Constants.TIME_CONNECT_RECEIVER);
	}
}

class MOE_ActionConnectReceiver : ActionContinuousBase
{
	void MOE_ActionConnectReceiver()
	{
		m_CallbackClass		= MOE_ActionConnectReceiverCB;
		m_CommandUID		= DayZPlayerConstants.CMD_ACTIONFB_CRAFTING;
		m_FullBody			= true;
		m_SpecialtyWeight	= UASoftSkillsWeight.PRECISE_LOW;
	}
	
	override void CreateConditionComponents() 
	{
		m_ConditionTarget 	= new CCTCursor(MOE_Constants.DISTANCE_MANIPULATE_EXPLOSIVE);
		m_ConditionItem 	= new CCINonRuined();
	}
	
	override bool ActionConditionContinue(ActionData action_data)
	{
		return true;
	}
	
	override bool CanBeSetFromInventory()
	{
		return true;
	}
	
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		MOE_SignalSource source; 
        if(!CastTo(source, item))
		{
			return false; 
		}
		
		if(!source.CanConnect())
		{
			return false; 
		}
		
		MOE_ComponentReceiverBase receiver;
		if(!GetReceiverFromItem(receiver, ItemBase.Cast(target.GetObject())))
		{
			return false;
		}

		if(!source.CanConnectTo(receiver))
		{
			return false;
		}
		
		m_Text = string.Format("#STR_MOE_Connect_Receiver [%1]",receiver.GetDisplayName());
		return true;
	}
	
	override void OnFinishProgressServer(ActionData action_data)
	{
		MOE_SignalSource source; 
        if(!CastTo(source, action_data.m_MainItem))
		{
			return; 
		}

		MOE_ComponentReceiverBase receiver;
		if(GetReceiverFromItem(receiver, ItemBase.Cast(action_data.m_Target.GetObject())))
		{		
			source.ConnectReceiver(receiver);
		}
		
		source.ConnectReceiver(receiver);
	}
	
	bool GetReceiverFromItem(out MOE_ComponentReceiverBase receiver, ItemBase item)
	{
		if(!CastTo(receiver, item))
		{		
			MOE_ExplosiveBase explosive;
			if(!CastTo(explosive, item))
			{
				return false;
			}
		
			if(!explosive.GetReceiver(receiver))
			{
				return false;
			}
		}
		
		return true;
	}
}