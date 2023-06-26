class MOE_Config 
{
    protected ref MOE_ModSettings m_ModSettings;
    protected ref map<string, ref MOE_ConfigDataBase> m_CachedData;

    void MOE_Config()
    {
        Init();
    }

    protected void Init()
    {
        m_ModSettings = new MOE_ModSettings();
        m_CachedData = new map<string, ref MOE_ConfigDataBase>();

        PreloadBaseBuildingEntities();
    }

    protected void PreloadBaseBuildingEntities()
    {
#ifdef SERVER
		array<string> preloadList = new array<string>();
		string cfgPath = MoreExplosives.CFG_MOE + "baseBuildingEntitiesToPreload";
		if(GetGame().ConfigIsExisting(cfgPath))
		{
			GetGame().ConfigGetTextArray(cfgPath, preloadList);
		}

		foreach(string entityType : preloadList)
		{
			if(!LoadEntityData(entityType, null))
			{
				Log_MOE(string.Format("Could not preload %1!", entityType), MOE_ELogTypes.ERROR);
			}
			else 
			{
				Log_MOE(string.Format("Preloaded %1!", entityType), MOE_ELogTypes.CONFIG);
			}
		}	
#endif
    }

    ///////////////
    /// Getters ///
    ///////////////

	MOE_ModSettings GetModSettings()
	{
		return m_ModSettings;
	}

    MOE_ConfigDataBase GetCachedData(string type)
	{
		return m_CachedData.Get(type);
	}

    bool FindCachedData(string type, out MOE_ConfigDataBase itemData)
    {
        return m_CachedData.Find(type, itemData);
    }

    MOE_ExplosiveData GetExplosiveData(string type)	
	{	
		MOE_ConfigDataBase data; 
		if(!FindCachedData(type, data))
		{
			return LoadExplosiveData(type);
		}

		return MOE_ExplosiveData.Cast(data);		
	}

    MOE_TimerData GetTimerData(string type)
	{		
		MOE_ConfigDataBase data; 
		if(!FindCachedData(type, data))
		{
			return LoadTimerData(type);
		}

		return MOE_TimerData.Cast(data);
	}

    MOE_ReceiverData GetReceiverData(string type)
	{	
		MOE_ConfigDataBase data; 
		if(!FindCachedData(type, data))
		{
			return LoadReceiverData(type);
		}

		return MOE_ReceiverData.Cast(data);		
	}
	
	MOE_SignalSourceData GetSignalSourceData(string type)
	{
		MOE_ConfigDataBase data; 
		if(!FindCachedData(type, data))
		{
			return LoadSignalSourceData(type);
		}

		return MOE_SignalSourceData.Cast(data);
	}
	
	MOE_DetonatorData GetDetonatorData(string type)
	{
		MOE_ConfigDataBase data; 
		if(!FindCachedData(type, data))
		{
			return LoadDetonatorData(type);
		}

		return MOE_DetonatorData.Cast(data);
	}

	MOE_EntityData GetEntityData(string type)
	{
		MOE_ConfigDataBase data; 
		if(!FindCachedData(type, data))
		{
			return LoadEntityData(type, null);
		}

		return MOE_EntityData.Cast(data);
	}

	MOE_AmmoData GetAmmoData(string type)
	{
		MOE_ConfigDataBase data; 
		if(!FindCachedData("AMMO " + type, data))
		{
			return LoadAmmoData(type);
		}

		return MOE_AmmoData.Cast(data);
	}


    ///////////////
    /// Loaders ///
    ///////////////

    MOE_ExplosiveData LoadExplosiveData(string type)
	{
		MOE_ExplosiveData data = new MOE_ExplosiveData(type);
		m_CachedData.Insert(type, data);
		return data;
	}

	MOE_TimerData LoadTimerData(string type)
	{

		MOE_TimerData data = new MOE_TimerData(type);
		m_CachedData.Insert(type, data);
		return data;
	}

	MOE_ReceiverData LoadReceiverData(string type)
	{
		MOE_ReceiverData data = new MOE_ReceiverData(type);
		m_CachedData.Insert(type, data);
		return data;
	}
	
	MOE_SignalSourceData LoadSignalSourceData(string type)
	{
		MOE_SignalSourceData data = new MOE_SignalSourceData(type);
		m_CachedData.Insert(type, data);
		return data;
	}
	
	MOE_DetonatorData LoadDetonatorData(string type)
	{
		MOE_DetonatorData data = new MOE_DetonatorData(type);
		m_CachedData.Insert(type, data);
		return data;		
	}

	MOE_EntityData LoadEntityData(string type, array<string> damageZones)
	{
		MOE_EntityData data = new MOE_EntityData(type, damageZones);
		m_CachedData.Insert(type, data);
		return data;
	}

	MOE_AmmoData LoadAmmoData(string type)
	{
		MOE_AmmoData data = new MOE_AmmoData(type);
		m_CachedData.Insert("AMMO " + type, data);
		return data;		
	}
}