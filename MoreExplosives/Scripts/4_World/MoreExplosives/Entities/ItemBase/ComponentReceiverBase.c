//Base class for all receivers that can interact with explosives and detonators 
//Can receive signal from signal sources (-> default behaviour: trigger explosive)
class MOE_ComponentReceiverBase : MOE_ComponentTriggerBase
{
	protected int m_PrevConnectedSignalSourcesCount;
	protected int m_ConnectedSignalSourcesCount; 	

	protected ref array<ref MOE_NetworkID> m_ConnectedSources;
	
	override void Init_MOE() 
	{
		super.Init_MOE();
		RegisterNetSyncVariableInt("m_ConnectedSignalSourcesCount");
		m_ConnectedSources = new array<ref MOE_NetworkID>();
	}	
	
	override void Fini_MOE()
	{
		super.Fini_MOE();
		
		if(GetGame()) 
		{
			DisconnectAllSources();
		}	
	}	
	
	override void Deactivate(EntityAI usedTool = null)
	{
		if(GetGame().IsServer())
		{
			DisconnectAllSources();
		}
		
		super.Deactivate(usedTool);
	}
	
	void DisconnectAllSources()
	{		
		MOE_SignalSource signalSource;
		foreach(MOE_NetworkID id : m_ConnectedSources)
		{
			if(GetSourceFromNetworkID(id.param1, id.param2, signalSource))
			{
				signalSource.DisconnectReceiver(this);
			}
		}
	}
	
	bool GetSourceFromNetworkID(int lowBits, int highBits, out MOE_SignalSource signalSource)
	{	
		return CastTo(signalSource, GetGame().GetObjectByNetworkId(lowBits, highBits));
	}
	
	int GetConnectionIndexByNetworkID(int lowBits, int highBits)
	{
		for(int i = 0; i < m_ConnectedSources.Count(); i++)
		{
			if(m_ConnectedSources[i].param1 == lowBits && m_ConnectedSources[i].param2 == highBits)
			{
				return i;
			}
		}	
			
		return -1;
	}
	
	protected void EvaluateConnectedSignalSourceCount()
	{		
		if(m_PrevConnectedSignalSourcesCount != m_ConnectedSignalSourcesCount)
		{
			OnConnectedSourcesCountChanged(m_PrevConnectedSignalSourcesCount, m_ConnectedSignalSourcesCount);
			m_PrevConnectedSignalSourcesCount = m_ConnectedSignalSourcesCount;
		}	
		
	}

	protected void EvaluateLightState()
	{
		if(!ValidateConnectedExplosive())
		{	
			return;
		}		
		
		if(m_ConnectedExplosive.IsArmed() && !m_ConnectedExplosive.IsTriggerLightOn())
		{	
			MOE_TriggerLight light = m_ConnectedExplosive.CreateTriggerLight(this);		
			if(light)
			{
				light.TurnOn();
			}
		}
	}
	
	override void EvaluateState()
	{
		super.EvaluateState();
		if(GetGame().IsClient())
		{
			EvaluateLightState();
		}
	}
		
	//========================================
	//========== Conditional Functions =======
	//========================================
	
	override bool AllowsArming()
	{
		return true;
	}
	
	override bool IsReceiver()
	{
		return true;
	}
	
	override bool IsActive()
	{
		return HasSourcesConnected();
		//return false;
	}
	
	bool HasSourcesConnected()
	{
		return m_ConnectedSignalSourcesCount > 0;
	}
	

	bool CanBeConnectedTo()
	{
		if(m_ConnectedSignalSourcesCount >= GetMaxConnectedSignalSources())
		{
			return false;
		}
		
		if(ValidateConnectedExplosive())
		{
			if(m_ConnectedExplosive.IsArmed() && !GetCanBeConnectedToWhenExplosiveArmed())
			{
				return false;
			}
		}
		
		return true;
	}	
	
	override vector GetLightColor()
	{
		array<vector> lightColors = GetLightColors();	
		if(!lightColors || lightColors.Count() == 0)
		{
			return "0 0 1";
		}
		
		return lightColors[0];
	}
	
	//========================================
	//============= "Events" =================
	//========================================
	
	void OnSourceConnected(notnull MOE_SignalSource source)
	{
		EstablishConnection(source);
	}
	
	void OnSourceDisconnected(notnull MOE_SignalSource source)
	{
		BreakConnection(source);
	}
	
	void OnSignalReceived(ItemBase source, PlayerBase player)
	{
		SetInteractingPlayerOnExplosive(player);
		OnActivatedByItem(source);
	}

	protected void OnConnectedSourcesCountChanged(int prevCount, int newCount)
	{
		
	}
	
	override void OnConnectedExplosiveArmed(notnull MOE_ExplosiveBase explosive)
	{
		super.OnConnectedExplosiveArmed(explosive);
		if(GetGame().IsClient())
		{
			MOE_TriggerLight light = explosive.CreateTriggerLight(this);		
			if(light)
			{
				light.TurnOn();
			}
		}	
	}
	
	override void OnConnectedExplosiveDisarmed(notnull MOE_ExplosiveBase explosive)
	{
		super.OnConnectedExplosiveDisarmed(explosive);
		explosive.DestroyTriggerLight();
	}
	
	//========================================
	//========== Modded Functions ============
	//========================================
	
	override void EEDelete(EntityAI parent)
	{
		DisconnectAllSources();	
		super.EEDelete(parent);
	}
	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();

		EvaluateConnectedSignalSourceCount();
		EvaluateLightState();		
	}
	
	override bool DescriptionOverride(out string output)
	{		
		output = ConfigGetString("descriptionShort");
		
		MOE_ConfigDataReceiver receiverData = GetConfigData_Receiver();
		
		output += string.Format("\n%1:\t%2", "#STR_MOE_Receiver_MaxConnectedSources", receiverData.MaxConnectedSignalSources);
		
		string canBeConnectedToWhenArmed; 
		if(receiverData.CanBeConnectedToWhenExplosiveArmed)
		{
			canBeConnectedToWhenArmed = "#STR_MOE_Yes";
		}
		else 
		{
			canBeConnectedToWhenArmed = "#STR_MOE_No";
		}
		
		output += string.Format("\n%1: %2", "#STR_MOE_Receiver_CanBeConnectedToWhenArmed", canBeConnectedToWhenArmed);
		
		return true;
	}
	
	//Used to create a connection to a given signal source 
	//Only takes care of connection creation on this receiver
	//--> to connect a source and a receiver use the functions provided by the signal source instead
	protected void EstablishConnection(notnull MOE_SignalSource source)
	{
		int lowBits, highBits;
		source.GetNetworkID(lowBits, highBits);		
		if(!(lowBits | highBits))
		{
			return;
		}
		
		int oldCount = m_ConnectedSources.Count();	
		int idx = GetConnectionIndexByNetworkID(lowBits, highBits);
		if(idx > -1)
		{
			m_ConnectedSources.RemoveOrdered(idx);
		}
		
		m_ConnectedSources.Insert(new MOE_NetworkID(lowBits, highBits));
		
		m_ConnectedSignalSourcesCount = m_ConnectedSources.Count();	
		OnConnectedSourcesCountChanged(oldCount, m_ConnectedSignalSourcesCount);
		SetSynchDirty();
	}
	
	//Used to remove a connection to a given signal source 
	//Only takes care of connection clean-up on this receiver
	//--> to disconnect a source from a receiver use the functions provided by the signal source instead
	protected void BreakConnection(notnull MOE_SignalSource source)
	{
		int lowBits, highBits;
		source.GetNetworkID(lowBits, highBits);		
		if(!(lowBits | highBits))
		{
			return;
		}
		
		int idx = GetConnectionIndexByNetworkID(lowBits, highBits);
		if(idx < 0)
		{
			return;
		}
		
		int oldCount = m_ConnectedSources.Count();	
		m_ConnectedSources.RemoveOrdered(idx);
		m_ConnectedSignalSourcesCount = m_ConnectedSources.Count();	
		OnConnectedSourcesCountChanged(oldCount, m_ConnectedSignalSourcesCount);
		SetSynchDirty();
	}
	
	//========================================
	//======= Access Cached Config Data ======
	//========================================
	
	override void LoadConfigData()
	{
		m_ConfigData = GetMOE().GetReceiverData(GetType());
	}
	
	MOE_ConfigDataReceiver GetConfigData_Receiver()
	{
		return MOE_ConfigDataReceiver.Cast(GetConfigData());
	}
	
	float GetMaxConnectedSignalSources()
	{
		return GetConfigData_Receiver().MaxConnectedSignalSources;
	}
	
	float GetCanBeConnectedToWhenExplosiveArmed()
	{
		return GetConfigData_Receiver().CanBeConnectedToWhenExplosiveArmed;
	}
}
