class MOE_ConfigBase 
{
	protected ref MOE_GeneralSettings m_GeneralSettings;
	
	// These could in theory be handled using only a single map for MOE_ConfigDataBase 
	// but since loading/caching has to be handled separately anyways and 
	// the amount of "types" is quite limited this will be more convinient 
	// for targeted access of specific type data
	protected ref map<string, ref MOE_ConfigDataExplosive> 		m_CachedExplosiveData;		
	protected ref map<string, ref MOE_ConfigDataSignalSource> 	m_CachedSignalSourceData;	
	protected ref map<string, ref MOE_ConfigDataTriggerBase> 	m_CachedTriggerData;
	
	void MOE_ConfigBase()
	{
		Init();
		LoadGeneralSettings();
	}
	
	protected void Init()
	{	
		m_CachedExplosiveData 		= new map<string, ref MOE_ConfigDataExplosive>();	
		m_CachedSignalSourceData 	= new map<string, ref MOE_ConfigDataSignalSource>();
		m_CachedTriggerData 		= new map<string, ref MOE_ConfigDataTriggerBase>();	
	}
	
	protected void LoadGeneralSettings()
	{
		
	}
	
	void RequestDataFromServer()
	{
					
	}
	
	void SentDataToClient(notnull PlayerIdentity client)
	{

	}
	
	bool ReceiveDataFromServer(ParamsReadContext ctx)
	{
		
	}
	
	
	////////////////////
	/// Data Getters ///
	////////////////////
	
	MOE_ConfigDataExplosive GetExplosiveData(string type)
	{	
		MOE_ConfigDataExplosive data = m_CachedExplosiveData.Get(type);
		if(!data)
		{
			return LoadExplosiveData(type);
		}
		
		return data;
	}
	
	MOE_ConfigDataTimer GetTimerData(string type)
	{		
		MOE_ConfigDataTriggerBase data = m_CachedTriggerData.Get(type);		
		if(!data)
		{
			return LoadTimerData(type);
		}
		
		return MOE_ConfigDataTimer.Cast(data);
	}
	
	MOE_ConfigDataReceiver GetReceiverData(string type)
	{	
		MOE_ConfigDataTriggerBase data = m_CachedTriggerData.Get(type);
		if(!data)
		{
			return LoadReceiverData(type);
		}
		
		return MOE_ConfigDataReceiver.Cast(data);
	}
	
	MOE_ConfigDataSignalSource GetSignalSourceData(string type)
	{
		
		MOE_ConfigDataSignalSource data = m_CachedSignalSourceData.Get(type);
		if(!data)
		{
			return LoadSignalSourceData(type);
		}
		
		return data;
	}
	
	MOE_ConfigDataDetonator GetDetonatorData(string type)
	{
		
		MOE_ConfigDataSignalSource data = m_CachedSignalSourceData.Get(type);
		if(!data)
		{
			return LoadDetonatorData(type);
		}
		
		return MOE_ConfigDataDetonator.Cast(data);
	}
	
	
	/////////////////////////
	/// Loading Functions ///
	/////////////////////////
	
	MOE_ConfigDataExplosive LoadExplosiveData(string type)
	{
		MOE_ConfigDataExplosive data = new MOE_ConfigDataExplosive(type);
		m_CachedExplosiveData.Insert(type, data);
		return data;
	}

	MOE_ConfigDataTimer LoadTimerData(string type)
	{

		MOE_ConfigDataTimer data = new MOE_ConfigDataTimer(type);
		m_CachedTriggerData.Insert(type, data);
		return data;
	}

	MOE_ConfigDataReceiver LoadReceiverData(string type)
	{
		MOE_ConfigDataReceiver data = new MOE_ConfigDataReceiver(type);
		m_CachedTriggerData.Insert(type, data);
		return data;
	}
	
	MOE_ConfigDataSignalSource LoadSignalSourceData(string type)
	{
		MOE_ConfigDataSignalSource data = new MOE_ConfigDataSignalSource(type);
		m_CachedSignalSourceData.Insert(type, data);
		return data;
	}
	
	MOE_ConfigDataDetonator LoadDetonatorData(string type)
	{
		MOE_ConfigDataDetonator data = new MOE_ConfigDataDetonator(type);
		m_CachedSignalSourceData.Insert(type, data);
		return data;		
	}
}