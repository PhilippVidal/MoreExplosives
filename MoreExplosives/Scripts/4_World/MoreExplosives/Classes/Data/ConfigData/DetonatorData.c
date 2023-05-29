class MOE_DetonatorDataBase : MOE_SignalSourceDataBase
{
	ref array<int> DetonatorModes;
}

class MOE_DetonatorDataConfig : MOE_DetonatorDataBase
{
	override void Init(string type)
	{
		super.Init(type);
	
		//Signal source values
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
		
		
		//Detonator values
		//Set default values
		DetonatorModes = new array<int>();
		
		//Load values from config if they exist
		string basePath = GetBasePath(type);	
		string path = basePath + "detonatorModes";	
		if(GetGame().ConfigIsExisting(path)) GetGame().ConfigGetIntArray(path, DetonatorModes);			
	}	
	
#ifdef MOE_DEBUG_CONFIG	
	override void PrintLoadedData(string type)
	{	
		super.PrintLoadedData(type);
		
		//Signal source values
		Print(string.Format("MaxConnectedReceivers: %1", MaxConnectedReceivers));
		Print(string.Format("MaxRange: %1", MaxRange));
		
		Print(string.Format("ConnectableReceivers: Count = %1", ConnectableReceivers.Count()));
		for(int i = 0; i < ConnectableReceivers.Count(); i++)
		{
			Print(string.Format(" [%1]: %2", i, ConnectableReceivers[i]));
		}
		
		Print(string.Format("FailureChance: %1", FailureChance));		
		Print(string.Format("TriggerOnRuinedChance: %1", TriggerOnRuinedChance));
			
		//Detonator values
		Print(string.Format("DetonatorModes: Count = %1", DetonatorModes.Count()));
		for(int i = 0; i < DetonatorModes.Count(); i++)
		{
			Print(string.Format(" [%1]: %2", i, DetonatorModes[i]));
		}
	}
#endif	
}