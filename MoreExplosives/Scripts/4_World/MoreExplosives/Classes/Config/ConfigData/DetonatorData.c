class MOE_DetonatorData : MOE_SignalSourceData
{
	ref array<int> DetonatorModes;	

	override void Init(string type)
	{
		super.Init(type);
	
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
		Print(string.Format("DetonatorModes: Count = %1", DetonatorModes.Count()));
		for(int i = 0; i < DetonatorModes.Count(); i++)
		{
			Print(string.Format(" [%1]: %2", i, DetonatorModes[i]));
		}
	}
#endif	
}