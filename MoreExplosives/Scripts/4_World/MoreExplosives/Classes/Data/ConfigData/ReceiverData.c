class MOE_ReceiverDataBase : MOE_TriggerDataBase
{
	int MaxConnectedSignalSources;
	bool CanBeConnectedToWhenExplosiveArmed;
}

class MOE_ReceiverDataConfig : MOE_ReceiverDataBase
{
	override void Init(string type)
	{	
		//Set default values
		LightRadius = 10;
		LightBrightness = 10;
		LightColors = new array<vector>();
			
		//Load values from config if they exist	
		string basePath = GetBasePath(type);	
		string path = basePath + "Light radius";
		if(GetGame().ConfigIsExisting(path)) LightRadius = GetGame().ConfigGetFloat(path);
		
		path = basePath + "Light brightness";
		if(GetGame().ConfigIsExisting(path)) LightBrightness = GetGame().ConfigGetFloat(path);
		
		array<float> rgbValues = new array<float>;
		path = basePath + "Light colors";
		if(GetGame().ConfigIsExisting(path))
		{
			GetGame().ConfigGetFloatArray(path, rgbValues);	
			LightColors = RGBValuesToVectors(rgbValues);
		}
	}
	
#ifdef MOE_DEBUG_CONFIG		
	override void PrintLoadedData(string type)
	{
		super.PrintLoadedData(type);
		Print(string.Format("Light Radius: %1", LightRadius));
		Print(string.Format("Light Brightness: %1", LightBrightness));
		Print(string.Format("Light Colors: Count = %1", LightColors.Count()));
		for(int i = 0; i < LightColors.Count(); i++)
		{
			Print(string.Format(" [%1]: %2", i, LightColors[i].ToString()));
		}	
	}
#endif
	
	
	
	
	override void Init(string type)
	{
		super.Init(type);
		
		//Set trigger base values
		LightRadius = 10;
		LightBrightness = 10;
		LightColors = new array<vector>();
			
		//Load values from config if they exist	
		string basePath = GetBasePath(type);	
		string path = basePath + "Light radius";
		if(GetGame().ConfigIsExisting(path)) LightRadius = GetGame().ConfigGetFloat(path);
		
		path = basePath + "Light brightness";
		if(GetGame().ConfigIsExisting(path)) LightBrightness = GetGame().ConfigGetFloat(path);
		
		array<float> rgbValues = new array<float>;
		path = basePath + "Light colors";
		if(GetGame().ConfigIsExisting(path))
		{
			GetGame().ConfigGetFloatArray(path, rgbValues);	
			LightColors = RGBValuesToVectors(rgbValues);
		}
		
		//Set receiver values
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
		Print(string.Format("Light Radius: %1", LightRadius));
		Print(string.Format("Light Brightness: %1", LightBrightness));
		Print(string.Format("Light Colors: Count = %1", LightColors.Count()));
		for(int i = 0; i < LightColors.Count(); i++)
		{
			Print(string.Format(" [%1]: %2", i, LightColors[i].ToString()));
		}
		
		
		Print(string.Format("MaxConnectedSignalSources: %1", MaxConnectedSignalSources));
		Print(string.Format("CanBeConnectedToWhenExplosiveArmed: %1", CanBeConnectedToWhenExplosiveArmed));
	}
#endif
}