//Base class for all trigger components of MOE_Explosives 
//These trigger componenets can detonate the explosive 
//as well as arm the explosive whenever required
class MOE_ComponentTriggerBase : MOE_ComponentBase 
{	
	protected ref MOE_ConfigDataTriggerBase m_ConfigData;
	
	override void Fini_MOE()
	{
		super.Fini_MOE();
		if(m_ConnectedExplosive)
		{
			m_ConnectedExplosive.DestroyTriggerLight();
		}
	}
	
	void TriggerExplosive()
	{
		if(ValidateConnectedExplosive())
		{
			m_ConnectedExplosive.OnActivatedByItem(this);
		}
	}	
	
	void SetInteractingPlayerOnExplosive(PlayerBase player)
	{
		if(ValidateConnectedExplosive())
		{
			m_ConnectedExplosive.SetInteractingPlayer(player);
		}
	}
	
	void Deactivate(EntityAI usedTool = null)
	{
		OnDeactivated(usedTool);
	}
	
	void OnDeactivated(EntityAI usedTool)
	{
		
	}

	void LoadConfigData()
	{
		
	}
		
	string GetItemConversionType(int typeIdentifier = 0)
	{
		return "";
	}
	
	//Called by connected explosive once it got fully loaded 
	void EvaluateState()
	{
	
	}
	
	//========================================
	//============= Light Functions ==========
	//========================================
	
	vector GetLightColor()
	{
		return "1 1 1";
	}
	
	//========================================
	//========== Conditional Functions =======
	//========================================
	
	override bool IsTrigger()
	{
		return true;
	}
	
	bool IsReceiver()
	{
		return false;
	}
	
	bool IsTimer()
	{
		return false;
	}
	
	bool AllowsArming()
	{
		return !IsActive();
	}

	
	//Determines if trigger/connected explosive has to be defused 
	bool IsActive()
	{
		return false;
	}
	
	bool CanBeDeactivated(EntityAI usedTool = null)
	{
		return IsActive();
	}
	
	bool CanBeConvertedToItem(int typeIdentifier = 0)
	{
		return false;
	}
	
	//========================================
	//============= "Events" =================
	//========================================
	
	void OnConnectedExplosiveArmed(notnull MOE_ExplosiveBase explosive)
	{
		
	}
	
	void OnConnectedExplosiveDisarmed(notnull MOE_ExplosiveBase explosive)
	{

	}
	
	void OnTriggerLightTurnedOn()
	{
	
	}
	
	void OnTriggerLightTurnedOff()
	{
	
	}
	
	//========================================
	//========== Modded Functions ============
	//========================================
	
	override void OnActivatedByItem(notnull ItemBase item)
	{
		super.OnActivatedByItem(item);	
		
#ifdef SERVER
		TriggerExplosive();
#endif
	}
	
	override void OnWasDetached(EntityAI parent, int slot_id)
	{
		super.OnWasDetached(parent, slot_id);
			
		if(ValidateConnectedExplosive())
		{
			m_ConnectedExplosive.DestroyTriggerLight();
		}
		
		m_ConnectedExplosive = null;
	}
	
	//========================================
	//======= Access Cached Config Data ======
	//========================================
	
	MOE_ConfigDataTriggerBase GetConfigData()
	{
		if(!m_ConfigData)
		{
			LoadConfigData();
		}
		
		return m_ConfigData;
	}
	
	float GetLightRadius()
	{
		return GetConfigData().LightRadius;
	}
	
	float GetLightBrightness()
	{
		return GetConfigData().LightBrightness;
	}
	
	array<vector> GetLightColors()
	{
		return GetConfigData().LightColors;
	}
} 