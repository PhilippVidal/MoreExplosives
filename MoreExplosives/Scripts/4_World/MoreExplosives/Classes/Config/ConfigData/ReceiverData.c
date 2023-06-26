class MOE_ReceiverData : MOE_TriggerData
{
	int MaxConnectedSignalSources;
	bool CanBeConnectedToWhenExplosiveArmed;
	
	override void Init(string type)
	{
		super.Init(type);
		
		//Set default values
		MaxConnectedSignalSources = 1;
		CanBeConnectedToWhenExplosiveArmed = true;
		
		//Load values from config if they exist
		string basePath = GetBasePath(type);	
		string path = basePath + "maxConnectedSignalSources";
		if(GetGame().ConfigIsExisting(path)) MaxConnectedSignalSources = GetGame().ConfigGetInt(path);
		
		path = basePath + "canBeConnectedToWhenExplosiveArmed";
		if(GetGame().ConfigIsExisting(path)) CanBeConnectedToWhenExplosiveArmed = GetGame().ConfigGetInt(path) != 0;
	}

#ifdef MOE_DEBUG_CONFIG		
	override void PrintLoadedData(string type)
	{	
		super.PrintLoadedData(type);
		Print(string.Format("MaxConnectedSignalSources: %1", MaxConnectedSignalSources));
		Print(string.Format("CanBeConnectedToWhenExplosiveArmed: %1", CanBeConnectedToWhenExplosiveArmed));
	}
#endif	
}