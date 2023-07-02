class MOE_ActionConvertItemCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{	
		m_ActionData.m_ActionComponent = new CAContinuousTime(MOE_Constants.TIME_CONVERT_ITEM);
	}
	
	override void InitActionComponent()
	{
		super.InitActionComponent();
		RegisterAnimationEvent("CraftingAction", UA_IN_CRAFTING);
	}
}

class MOE_ActionConvertItem : ActionContinuousBase
{
	void MOE_ActionConvertItem()
	{
		m_CallbackClass = MOE_ActionConvertItemCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_CRAFTING;
		m_FullBody = true;
		m_Sound = "craft_universal_0";
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
	
	override void OnActionInfoUpdate( PlayerBase player, ActionTarget target, ItemBase item )
	{
		string displayName;
		
		MOE_ComponentTriggerBase trigger;	
		MOE_DetonatorBase detonator;
		if(CastTo(trigger, item) && trigger.CanBeConvertedToItem())
		{
			displayName = MiscGameplayFunctions.GetItemDisplayName(trigger.GetItemConversionType());
		}
		else if(CastTo(detonator, item) && detonator.CanBeConvertedToItem())
		{
			displayName = MiscGameplayFunctions.GetItemDisplayName(detonator.GetItemConversionType());
		}
		
		m_Text = string.Format("#STR_MOE_Convert_Item [%1]",displayName);
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{		
		if(item.GetCompEM() && item.GetCompEM().IsSwitchedOn())
		{
			return false;
		}
		
		MOE_ComponentTriggerBase trigger;	
		if(CastTo(trigger, item))
		{
			return trigger.CanBeConvertedToItem();
		}
		
		MOE_DetonatorBase detonator;	
		if(CastTo(detonator, item))
		{
			return detonator.CanBeConvertedToItem() && !detonator.GetConnectionCount();
		}
		
		return false;
	}
	
	override void OnFinishProgressServer(ActionData action_data)
	{	
		MOE_ComponentTriggerBase trigger;	
		if(CastTo(trigger, action_data.m_MainItem))
		{
			SpawnEntityInPlayerHands(action_data.m_Player, trigger.GetItemConversionType());
			return; 
		}
		
		MOE_DetonatorBase detonator;		
		if(CastTo(detonator, action_data.m_MainItem))
		{
			SpawnEntityInPlayerHands(action_data.m_Player, detonator.GetItemConversionType());
			return; 
		}
	}
	
	protected EntityAI SpawnEntityInPlayerHands(notnull EntityAI pEntity, string type)
	{	
		if(!GetGame().IsServer() || type == "")
		{
			return null; 
		}
		
		PlayerBase player = PlayerBase.Cast(pEntity);
		if (!player)
		{
			return null;
		}
		
		EntityAI createdEntity;
		ItemBase itemInHands = player.GetItemInHands();
		if(itemInHands)
		{
			MiscGameplayFunctions.TurnItemIntoItem(itemInHands, type, player);
			createdEntity = player.GetItemInHands();
		}
		else
		{
			createdEntity = EntityAI.Cast(player.GetHumanInventory().CreateInHands(type));
		}
		
		if (player.GetItemInHands())
		{
			player.GetItemAccessor().OnItemInHandsChanged();
		}
		
		return createdEntity;
	}
}