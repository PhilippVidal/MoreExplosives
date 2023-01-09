//Enables basic syncing between server and client 
//only sends required data (i.e., the changes that occured since the last state update)
//has several functions that have to be implemented for it to work properly, 
//examples are the MOE_SignalSource and MOE_DetonatorBase classes 

class MOE_StateChangeData
{
	int StateNumber;
	ref Param Data;
	
	void MOE_StateChangeData(int stateNumber, Param data)
	{
		StateNumber = stateNumber;
		Data = data;
	}
}

class MOE_StateSyncedItemBase : ItemBase
{
	int m_ServerState; 
	int m_ClientState;
	bool m_HasUpdatePending;
	ref array<ref MOE_StateChangeData> m_StateChanges;
		
	void MOE_StateSyncedItemBase()
	{
		RegisterNetSyncVariableInt("m_ServerState");
		Init_MOE();
	}
	
	void ~MOE_StateSyncedItemBase()
	{
		Fini_MOE();
	}
	
	protected void Init_MOE()
	{
		m_StateChanges = new array<ref MOE_StateChangeData>();
	}
	
	protected void Fini_MOE()
	{

	}
	
	protected bool RequiresUpdate()
	{
		return (m_ClientState != m_ServerState) && !m_HasUpdatePending;
	}
	
	protected bool IsClientUpToDate(int clientState)
	{
		return clientState == m_ServerState;
	}
	
	protected void OnStateUpdateRequired()
	{
#ifndef SERVER
#ifdef MOE_DEBUG_SYNCING
			Log_MOE(string.Format("State update required (%1 != %2)! Requesting update data from server... [%3]", m_ClientState, m_ServerState, this), MOE_ELogTypes.DEBUG);
#endif
			//Request update for components from server			
			GetGame().RPCSingleParam(this, MOE_ERPCTypes.SYNC, new Param1<int>(m_ClientState), true, NULL);
			m_HasUpdatePending = true;
#endif	
	}

	protected int FindRelatedStateChange(Param changeData)
	{
		return -1;
	}
	
	protected void AddStateChange(notnull Param changeData)
	{
		int idx = FindRelatedStateChange(changeData);
		if(idx != -1)
		{
			m_StateChanges.RemoveOrdered(idx);
		}
		
		m_StateChanges.Insert(new MOE_StateChangeData(++m_ServerState, changeData));
		SetSynchDirty();
	}

	protected int FindFirstRelevantUpdate(int clientState)
	{
		int startState = clientState + 1;
		int low, mid;
		int high = m_StateChanges.Count();
		while(low < high)
		{
			mid = low + (high - low) * 0.5;

			if(m_StateChanges[mid].StateNumber < startState)
			{ 
				low = mid + 1; 
			}
			else 
			{
				high = mid;
			}
		}
		
		//not found
		if(high == m_StateChanges.Count())
		{
			return 0;
		}
		
		return high;		
	}

	protected bool ShouldDoFullSync(int changesToSend)
	{
		return false;
	}
	
	protected array<ref Param> BuildClientUpdate(int firstRelevantChange, bool isFullSync)
	{
		array<ref Param> updateData = new array<ref Param>();
		updateData.Insert(new Param2<int, bool>(m_ServerState, isFullSync));
		return updateData;
	}
	
	protected void SendUpdateDataToClient(PlayerIdentity client, int clientState)
	{
		if(IsClientUpToDate(clientState))
		{
			return;
		}
		
		int startIndex = FindFirstRelevantUpdate(clientState);		
		array<ref Param> rpcContent = BuildClientUpdate(startIndex, ShouldDoFullSync(m_StateChanges.Count() - startIndex));
		GetGame().RPC(this, MOE_ERPCTypes.SYNC, rpcContent, true, client);
	}

	protected bool ReadObjectUpdateData(ParamsReadContext ctx, bool isFullSync)
	{
		return false;
	}
	
	int ReadUpdateDataFromServer(ParamsReadContext ctx)
	{
#ifdef MOE_DEBUG_SYNCING
		Log_MOE(string.Format("Received update data from server. [%1]", this), MOE_ELogTypes.DEBUG);		
#endif
		          
		Param2<int, bool> updateInfo;
        if(!ctx.Read(updateInfo))
        {         
			Log_MOE(string.Format("%1::ReadUpdateDataFromServer -> Failed to read received update data info from server!", this), MOE_ELogTypes.ERROR);
            return m_ClientState;
        }
		
		
		int serverState = updateInfo.param1;
		bool isFullSync = updateInfo.param2;		
		
		if(!ReadObjectUpdateData(ctx, isFullSync))
		{
			Log_MOE(string.Format("%1::ReadUpdateDataFromServer -> Failed to read object update data!!", this), MOE_ELogTypes.ERROR);
		}
		
		
#ifdef MOE_DEBUG_SYNCING
		Log_MOE(string.Format("Updated client data successfully! New State = %1 [%2] ", serverState, this), MOE_ELogTypes.DEBUG);
#endif
		
		return serverState;
	}
	
	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();		
		CheckForUpdate();			
	}

	override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
    {
        super.OnRPC(sender, rpc_type, ctx);

        if(rpc_type != MOE_ERPCTypes.SYNC)
		{
			return;
		}

#ifdef SERVER			
#ifdef MOE_DEBUG_SYNCING
			Log_MOE(string.Format("%1::OnRPC -> A client (%2) wants to update their data!", this, sender.GetPlainId()), MOE_ELogTypes.DEBUG);
#endif		
            Param1<int> clientState;
            if(!ctx.Read(clientState))
			{			
				Log_MOE(string.Format("%1::OnRPC -> Failed to read received RPC data from client.", sender.GetPlainId()), MOE_ELogTypes.ERROR);
				return;
			}
			            
#ifdef MOE_DEBUG_SYNCING
			Log_MOE(string.Format("%1::OnRPC -> Received client state is %2, sending update data...", this, clientState),MOE_ELogTypes.DEBUG);
#endif	 
            SendUpdateDataToClient(sender, clientState.param1);
			return;
#endif	
  
    	m_ClientState = ReadUpdateDataFromServer(ctx);  
		m_HasUpdatePending = false;	
		CheckForUpdate();       
    }
	
	void CheckForUpdate()
	{
		if(RequiresUpdate())
		{
			OnStateUpdateRequired();
		}       
	}
}