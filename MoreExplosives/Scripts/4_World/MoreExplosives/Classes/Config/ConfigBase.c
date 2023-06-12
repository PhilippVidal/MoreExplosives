class MOE_ConfigBase 
{
	protected ref MOE_GeneralSettings m_GeneralSettings;
	
	// These could in theory be handled using only a single map for MOE_ConfigDataBase 
	// but since loading/caching has to be handled separately anyways and 
	// the amount of "types" is quite limited this will be more convinient 
	// for targeted access of specific type data
	//protected ref map<string, ref MOE_ConfigDataExplosive> 		m_CachedExplosiveData;		
	//protected ref map<string, ref MOE_ConfigDataSignalSource> 	m_CachedSignalSourceData;	
	//protected ref map<string, ref MOE_ConfigDataTriggerBase> 	m_CachedTriggerData;
	
	protected ref map<string, ref MOE_ConfigDataBase> m_CachedConfigData;

	void MOE_ConfigBase()
	{
		Init();
		LoadGeneralSettings();
	}
	
	protected void Init()
	{	
		m_CachedConfigData = new map<string, ref MOE_ConfigDataBase>();
		//m_CachedExplosiveData 	= new map<string, ref MOE_ConfigDataExplosive>();	
		//m_CachedSignalSourceData 	= new map<string, ref MOE_ConfigDataSignalSource>();
		//m_CachedTriggerData 		= new map<string, ref MOE_ConfigDataTriggerBase>();	
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
	
	MOE_ConfigDataBase GetConfigData(string type)
	{
		return m_CachedConfigData.Get(type);
	}

	MOE_ConfigDataExplosive GetExplosiveData(string type)	
	{	
		MOE_ConfigDataBase data; 
		if(!m_CachedConfigData.Find(type, data))
		{
			return LoadExplosiveData(type);
		}

		return MOE_ConfigDataExplosive.Cast(data);		
	}
	
	MOE_ConfigDataTimer GetTimerData(string type)
	{		
		MOE_ConfigDataBase data; 
		if(!m_CachedConfigData.Find(type, data))
		{
			return LoadTimerData(type);
		}

		return MOE_ConfigDataTimer.Cast(data);
	}
	
	MOE_ConfigDataReceiver GetReceiverData(string type)
	{	
		MOE_ConfigDataBase data; 
		if(!m_CachedConfigData.Find(type, data))
		{
			return LoadReceiverData(type);
		}

		return MOE_ConfigDataReceiver.Cast(data);		
	}
	
	MOE_ConfigDataSignalSource GetSignalSourceData(string type)
	{
		MOE_ConfigDataBase data; 
		if(!m_CachedConfigData.Find(type, data))
		{
			return LoadSignalSourceData(type);
		}

		return MOE_ConfigDataSignalSource.Cast(data);
	}
	
	MOE_ConfigDataDetonator GetDetonatorData(string type)
	{
		MOE_ConfigDataBase data; 
		if(!m_CachedConfigData.Find(type, data))
		{
			return LoadDetonatorData(type);
		}

		return MOE_ConfigDataDetonator.Cast(data);
	}
	
	bool GetCategoryData(string type, out MOE_ConfigDataCategory category)	
	{	
		MOE_ConfigDataBase cachedData;
		return m_CachedConfigData.Find("C " + type, cachedData) && CastTo(category, cachedData);	
	}

	bool GetDestroyableObjectData(string type, out MOE_ConfigDataDestroyableObject destroyableObject)	
	{	
		MOE_ConfigDataBase cachedData;
		return m_CachedConfigData.Find("D " + type, cachedData) && CastTo(destroyableObject, cachedData);	
	}
	
	/////////////////////////
	/// Loading Functions ///
	/////////////////////////
	
	MOE_ConfigDataExplosive LoadExplosiveData(string type)
	{
		MOE_ConfigDataExplosive data = new MOE_ConfigDataExplosive(type);
		m_CachedConfigData.Insert(type, data);
		return data;
	}

	MOE_ConfigDataTimer LoadTimerData(string type)
	{

		MOE_ConfigDataTimer data = new MOE_ConfigDataTimer(type);
		m_CachedConfigData.Insert(type, data);
		return data;
	}

	MOE_ConfigDataReceiver LoadReceiverData(string type)
	{
		MOE_ConfigDataReceiver data = new MOE_ConfigDataReceiver(type);
		m_CachedConfigData.Insert(type, data);
		return data;
	}
	
	MOE_ConfigDataSignalSource LoadSignalSourceData(string type)
	{
		MOE_ConfigDataSignalSource data = new MOE_ConfigDataSignalSource(type);
		m_CachedConfigData.Insert(type, data);
		return data;
	}
	
	MOE_ConfigDataDetonator LoadDetonatorData(string type)
	{
		MOE_ConfigDataDetonator data = new MOE_ConfigDataDetonator(type);
		m_CachedConfigData.Insert(type, data);
		return data;		
	}

	MOE_ConfigDataCategory LoadCategoryData(string type)
	{
		MOE_ConfigDataCategory data = new MOE_ConfigDataCategory(type);
		m_CachedConfigData.Insert("C " + type, data);
		return data;
	}

	MOE_ConfigDataDestroyableObject LoadDestroyableObjectData(string type)
	{
		MOE_ConfigDataDestroyableObject data = new MOE_ConfigDataDestroyableObject(type);
		m_CachedConfigData.Insert("D " + type, data);
		return data;
	}
}