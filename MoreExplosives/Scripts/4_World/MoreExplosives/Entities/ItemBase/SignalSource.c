//Items that can act as a signal source 
//have a list of connected items (in the form of network ids <int, int>)
//get synced when a change occurs (linked item removed/added)
//functionallity mainly used for detonators but can in theory 
//also be used for any kind of detonation device that isn't a 
//traditional detonator (e.g., a pressure plate or explosive clearing devices)

class MOE_SignalSource : MOE_StateSyncedItemBase 
{
	protected ref array<ref MOE_NetworkID> m_ConnectedReceivers;	
	protected ref MOE_SignalSourceData m_ConfigData;
	
	override void Init_MOE()
	{
		super.Init_MOE();
		m_ConnectedReceivers = new array<ref MOE_NetworkID>();
		//m_StateChanges = new  map<int, ref MOE_SignalSourceSyncData>;	
		//m_StateChanges = new array<>
	}
	
	override void Fini_MOE()
	{
		super.Fini_MOE();
#ifdef SERVER
		if(GetGame())
		{		
			for(int i = m_ConnectedReceivers.Count() - 1; i > -1; i--)
			{		
				MOE_ComponentReceiverBase receiver;
				if(!GetReceiverFromNetworkID(m_ConnectedReceivers[i].param1, m_ConnectedReceivers[i].param2, receiver))
				{
					continue;
				}
				
				DisconnectReceiver(receiver);
			}
		}
#endif
	}
	
	void LoadConfigData()
	{
		m_ConfigData = GetMOEConfig().GetSignalSourceData(GetType());
	}
	
		bool IsTurnedOn()
	{
		return true;
	}
	
	float GetDistanceToReceiver(int idx)
	{
		if(!m_ConnectedReceivers || m_ConnectedReceivers.Count() == 0) 
		{
			return -1;
		}
		
		if(!m_ConnectedReceivers.IsValidIndex(idx)) 
		{
			return -2;		
		}
		
		Object obj = GetGame().GetObjectByNetworkId(m_ConnectedReceivers[idx].param1, m_ConnectedReceivers[idx].param2);
		if(!obj) 
		{
			return -3;
		}
		
		return vector.Distance(obj.GetPosition(), GetPosition());
	}
	
	
	float GetAngleToReceiver(int idx, vector viewDirection)
	{
		if(!m_ConnectedReceivers || m_ConnectedReceivers.Count() == 0) 
		{
			return 0;
		}
		
		if(!m_ConnectedReceivers.IsValidIndex(idx)) 
		{
			return 0;		
		}
		
		Object obj = GetGame().GetObjectByNetworkId(m_ConnectedReceivers[idx].param1, m_ConnectedReceivers[idx].param2);
		if(!obj) 
		{
			return 0;
		}
		
		vector receiverPosition = obj.GetPosition();
		vector ownPosition = GetPosition();		
		if(ownPosition == receiverPosition)
		{
			return 0;
		}
		
		receiverPosition[1] = 0;
		ownPosition[1] = 0;
		viewDirection[1] = 0;
		
		return HDSN_Math.SignedAngleBetweenVectorsDEG(viewDirection.Normalized(), (receiverPosition - ownPosition).Normalized(), vector.Up);
	}
	
	bool GetReceiverFromNetworkID(int lowBits, int highBits, out MOE_ComponentReceiverBase receiver)
	{	
		return CastTo(receiver, GetGame().GetObjectByNetworkId(lowBits, highBits));
	}
	
	int GetConnectionIndexByNetworkID(int lowBits, int highBits)
	{
		for(int i = 0; i < m_ConnectedReceivers.Count(); i++)
		{
			if(m_ConnectedReceivers[i].param1 == lowBits && m_ConnectedReceivers[i].param2 == highBits)
			{
				return i;
			}
		}	
			
		return -1;
	}
	
	int GetConnectionCount()
	{
		return m_ConnectedReceivers.Count();
	}
	
	void TriggerSignal(PlayerBase player = null)
	{
		SendSignalToAll(player);
	}
	
	void SendSignalToAll(PlayerBase player = null)
	{
		if(m_ConnectedReceivers.Count() == 0)
		{
			return;
		}
			
		for(int i = 0; i < m_ConnectedReceivers.Count(); i++)
		{
			SendSignal(i, player);
		}  
	}
	
	bool SendSignal(int index, PlayerBase player = null)
	{
		if(!m_ConnectedReceivers.IsValidIndex(index))
		{
			return false;
		}
		
		MOE_ComponentReceiverBase receiver;
		if(!GetReceiverFromNetworkID(m_ConnectedReceivers[index].param1, m_ConnectedReceivers[index].param2, receiver))	
		{
			return false;
		}
		
		if(!CheckSignalConditions(receiver))
		{
			return false;
		}
		
		receiver.OnSignalReceived(this, player);
		return true;	
	}
	
	bool ConnectReceiver(notnull MOE_ComponentReceiverBase receiver)
	{				
		int lowBits, highBits;
		receiver.GetNetworkID(lowBits, highBits);
		if(!(lowBits | highBits))
		{
			return false;
		}
		
		DisconnectReceiver(receiver);
		
		m_ConnectedReceivers.Insert(new MOE_NetworkID(lowBits, highBits));
		
#ifdef SERVER
		receiver.OnSourceConnected(this);
		AddStateChange(new MOE_SignalSourceSyncData(true, lowBits, highBits));
#endif
		
		return true;
	}
	
	bool DisconnectReceiver(notnull MOE_ComponentReceiverBase receiver)	
	{
		int lowBits, highBits;
		receiver.GetNetworkID(lowBits, highBits);		
		if(!(lowBits | highBits))
		{
			return false;
		}
		
		int index = GetConnectionIndexByNetworkID(lowBits, highBits);
		if(index < 0)
		{
			return false;
		}
		
		RemoveReceiverConnection(index);
		
#ifdef SERVER
		receiver.OnSourceDisconnected(this);
		AddStateChange(new MOE_SignalSourceSyncData(false, lowBits, highBits));
#endif	
		return true;
	}
	
	protected void RemoveReceiverConnection(int idx)
	{
		m_ConnectedReceivers.RemoveOrdered(idx);
	}
	
	void DisconnectAllReceivers()
	{
		MOE_ComponentReceiverBase receiver; 
		for(int i = m_ConnectedReceivers.Count() - 1; i > -1; i--)
		{
			if(GetReceiverFromNetworkID(m_ConnectedReceivers[i].param1, m_ConnectedReceivers[i].param2, receiver))
			{
				DisconnectReceiver(receiver);	
			}	
		}
	}
	
	bool DisconnectReceiverByIndex(int idx)
	{
		if(!m_ConnectedReceivers.IsValidIndex(idx))
		{
			return false;
		}
		
		MOE_ComponentReceiverBase receiver; 		
		return GetReceiverFromNetworkID(m_ConnectedReceivers[idx].param1, m_ConnectedReceivers[idx].param2, receiver) && DisconnectReceiver(receiver);
	}
	
	override int FindRelatedStateChange(Param changeData)
	{
		MOE_SignalSourceSyncData data = MOE_SignalSourceSyncData.Cast(changeData);
		int lowBits = data.param2; 
		int highBits = data.param3;
		
		MOE_SignalSourceSyncData stateData;
		for(int idx = 0; idx < m_StateChanges.Count(); idx++)
		{
			stateData = MOE_SignalSourceSyncData.Cast(m_StateChanges[idx].Data);
			if( stateData && stateData.param2 == lowBits && stateData.param3 == highBits )
			{
				return idx;
			}
		}
		
		return -1;
	}
	
	override bool ShouldDoFullSync(int changesToSend)
	{
		return changesToSend > m_ConnectedReceivers.Count();
	}
	
	override array<ref Param> BuildClientUpdate(int firstRelevantChange, bool isFullSync)
	{
		array<ref Param> content = super.BuildClientUpdate(firstRelevantChange, isFullSync);

		
		if(isFullSync)
		{
			content.Insert(new Param1<int>(m_ConnectedReceivers.Count()));
			foreach(MOE_NetworkID id : m_ConnectedReceivers)
			{
				content.Insert(id);
			}
			
			return content;
		}
		
		//array<ref MOE_SignalSourceSyncData> syncData = new array<ref MOE_SignalSourceSyncData>();
		content.Insert(new Param1<int>(m_StateChanges.Count() - firstRelevantChange));
		for(int state = firstRelevantChange; state < m_StateChanges.Count(); state++)
		{
			content.Insert(MOE_SignalSourceSyncData.Cast(m_StateChanges[state].Data));
		}

		return content;
	}
	
	override bool ReadObjectUpdateData(ParamsReadContext ctx, bool isFullSync)
	{
		//Completely replace client data
		//array contains int-pairs (MOE_NetworkID)
		Param1<int> countParam;
		if(!ctx.Read(countParam))
        {         
			Log_MOE(string.Format("%1::ReadObjectUpdateData -> Failed to read countParam from received update data!", this), MOE_ELogTypes.ERROR);
            return false;
        }
		
		int entryCount = countParam.param1;
		int i;
		if(isFullSync)
		{
			delete m_ConnectedReceivers;
			m_ConnectedReceivers = new array<ref MOE_NetworkID>();
			
			MOE_NetworkID id;
			for(i = 0; i < entryCount; i++)
			{
				if(!ctx.Read(id))
				{		
					Log_MOE(string.Format("%1::ReadObjectUpdateData -> Failed to read network ID (Full Sync) [%2, %3]!", this, i, entryCount), MOE_ELogTypes.ERROR);
					return false;
				}
		
				m_ConnectedReceivers.Insert(id);		
			}

			return true;
		}
		
		MOE_SignalSourceSyncData changeData;
		bool add; 
		int lowBits, highBits, index;		
		for(i = 0; i < entryCount; i++)
		{
			if(!ctx.Read(changeData))
			{
				Log_MOE(string.Format("%1::ReadObjectUpdateData -> Failed to read update data (Partial Sync) [%2, %3]!", this, i, entryCount), MOE_ELogTypes.ERROR);
				return false;
			}
			
			add = changeData.param1;
			lowBits = changeData.param2;
			highBits = changeData.param3;
			index = GetConnectionIndexByNetworkID(lowBits, highBits);
			if(index > -1)
			{
				m_ConnectedReceivers.RemoveOrdered(index);
			}
			
			if(add)
			{
#ifdef MOE_DEBUG_SYNCING
				Log_MOE(string.Format("Adding receiver (%1 | %2) to connections. [%3]", lowBits, highBits, this), MOE_ELogTypes.DEBUG);
#endif
				m_ConnectedReceivers.Insert(new MOE_NetworkID(lowBits, highBits));
			}
		}		
		
		return true;		
	}
	
	//========================================
	//========== Conditional Functions =======
	//========================================
	
	bool CheckSignalConditions(notnull MOE_ComponentReceiverBase receiver)
	{
		if(!IsInRange(receiver.GetPosition()))
		{
			return false;
		}
		
		return true;
	}
	
	bool IsReceiverConnected(notnull MOE_ComponentReceiverBase receiver)
	{
		int lowBits, highBits;
		receiver.GetNetworkID(lowBits, highBits);
		
		if(!(lowBits | highBits))
		{
			return false;
		}
		
		foreach(MOE_NetworkID id : m_ConnectedReceivers)
		{
			if(id.param1 == lowBits && id.param2 == highBits)
			{
				return true;
			}
		}
		
		return false;
	}
	
	bool CanConnect()
	{		
		return IsTurnedOn() && (GetConnectionCount() < GetMaxConnectedReceivers());
	}
	
	bool CanConnectTo(notnull MOE_ComponentReceiverBase receiver)
	{	
		if(!receiver.CanBeConnectedTo())
		{
			return false;
		}
		
		if(IsReceiverConnected(receiver))
		{
			return false;
		}
		
		array<string> allowedReceivers = GetConnectableReceivers();
		foreach(string allowedReceiver : allowedReceivers)
		{
			if(allowedReceiver == receiver.GetType())
			{
				return true;
			}
		}

		return false;
	}
	
	bool IsInRange(vector pos)
	{
		float maxRange = GetMaxRange();
		return vector.DistanceSq(pos, GetPosition()) < (maxRange * maxRange);
	}
	
	bool CanBeTriggered()
	{
		return IsTurnedOn();
	}
	
	//========================================
	//========== Modded Functions ============
	//========================================
	
	override void EEHealthLevelChanged(int oldLevel, int newLevel, string zone)
	{
		super.EEHealthLevelChanged(oldLevel, newLevel, zone);

#ifndef SERVER
			return;
#endif
		
		if (newLevel != GameConstants.STATE_RUINED)
		{
			return;
		}
		
		if(Math.RandomFloat01() < GetTriggerOnRuinedChance())
		{				
			TriggerSignal();
		}		
	}
	
	override void EEDelete(EntityAI parent)
	{
#ifdef SERVER
		for(int i = m_ConnectedReceivers.Count() - 1; i > -1; i--)
		{		
			MOE_ComponentReceiverBase receiver;
			if(!GetReceiverFromNetworkID(m_ConnectedReceivers[i].param1, m_ConnectedReceivers[i].param2, receiver))
			{
				continue;
			}
			
			DisconnectReceiver(receiver);
		}
#endif
		
		super.EEDelete(parent);
	}
	
	//========================================
	//======= Access Cached Config Data ======
	//========================================
	
	MOE_SignalSourceData GetConfigData()
	{
		if(!m_ConfigData)
		{
			LoadConfigData();
		}
		
		return m_ConfigData;
	}
	
	int GetMaxConnectedReceivers()
	{
		return GetConfigData().MaxConnectedReceivers;
	}
	
	float GetMaxRange()
	{
		return GetConfigData().MaxRange;
	}
	
	array<string> GetConnectableReceivers()
	{
		return GetConfigData().ConnectableReceivers;
	}
	
	float GetFailureChance()
	{
		return GetConfigData().FailureChance;
	}
	
	float GetTriggerOnRuinedChance()
	{
		return GetConfigData().TriggerOnRuinedChance;
	}
}