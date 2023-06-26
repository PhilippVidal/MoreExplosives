class MOE_SignalSourceData : MOE_ConfigDataBase
{
	int MaxConnectedReceivers;
	float MaxRange;
	ref array<string> ConnectableReceivers; 
	float FailureChance;
	float TriggerOnRuinedChance;

    override void Init(string type)
	{
		super.Init(type);
		
		//Set default values
		MaxConnectedReceivers = 1;
		MaxRange = 50;
		ConnectableReceivers  = new array<string>(); 
		
		//Load values from config if they exist
		string basePath = GetBasePath(type);		
		string path = basePath + "maxConnectedReceivers";
		if(GetGame().ConfigIsExisting(path)) MaxConnectedReceivers = GetGame().ConfigGetInt(path);
		
		path = basePath + "maxRange";
		if(GetGame().ConfigIsExisting(path)) MaxRange = GetGame().ConfigGetFloat(path);	
		
		path = basePath + "connectableReceivers";
		if(GetGame().ConfigIsExisting(path)) GetGame().ConfigGetTextArray(path, ConnectableReceivers);

		path = basePath + "failureChance";
		if(GetGame().ConfigIsExisting(path)) FailureChance = GetGame().ConfigGetFloat(path);	
		
		path = basePath + "triggerOnRuinedChance";
		if(GetGame().ConfigIsExisting(path)) TriggerOnRuinedChance = GetGame().ConfigGetFloat(path);
	}

#ifdef MOE_DEBUG_CONFIG
	override void PrintLoadedData(string type)
	{	
		super.PrintLoadedData(type);
		Print(string.Format("MaxConnectedReceivers: %1", MaxConnectedReceivers));
		Print(string.Format("MaxRange: %1", MaxRange));
		
		Print(string.Format("ConnectableReceivers: Count = %1", ConnectableReceivers.Count()));
		for(int i = 0; i < ConnectableReceivers.Count(); i++)
		{
			Print(string.Format(" [%1]: %2", i, ConnectableReceivers[i]));
		}
		
		Print(string.Format("FailureChance: %1", FailureChance));		
		Print(string.Format("TriggerOnRuinedChance: %1", TriggerOnRuinedChance));
	}
#endif
}