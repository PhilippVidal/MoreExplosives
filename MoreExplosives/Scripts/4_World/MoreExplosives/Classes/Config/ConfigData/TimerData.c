class MOE_TimerData : MOE_TriggerData
{
	float MinTimer;
	float MaxTimer;
	float InitialTickrate; 
	float TickSpeedIncreaseFactor; 
	
	string BeepingSoundset;	
	
	ref array<float> TimerStages; 	
	
	ref NoiseParams NoiseParameters; 
	
	override void Init(string type)
	{
		super.Init(type);
		
		//Set default values
		MinTimer = 10;
		MaxTimer = 600;
		InitialTickrate = 4; 
		TickSpeedIncreaseFactor = 2; 	
		
		BeepingSoundset = "MOE_SoundSet_Beeping";	
		
		TimerStages = new array<float>(); 
		
		NoiseParameters = new NoiseParams();
		
		//Load values from config if they exist	
		string basePath = GetBasePath(type);		
		string path = basePath + "maxTimer";
		if(GetGame().ConfigIsExisting(path)) MaxTimer = GetGame().ConfigGetFloat(path);
		
		path = basePath + "minTimer";
		if(GetGame().ConfigIsExisting(path)) MinTimer = GetGame().ConfigGetFloat(path);
		
		path = basePath + "initialTickrate";
		if(GetGame().ConfigIsExisting(path)) InitialTickrate = GetGame().ConfigGetFloat(path);
		
		path = basePath + "tickSpeedIncreaseFactor";
		if(GetGame().ConfigIsExisting(path)) TickSpeedIncreaseFactor = GetGame().ConfigGetFloat(path);

		
		path = basePath + "timerStages";
		if(GetGame().ConfigIsExisting(path)) GetGame().ConfigGetFloatArray(path, TimerStages);

		
		path = basePath + "beepingSoundset";
		if(GetGame().ConfigIsExisting(path)) BeepingSoundset = GetGame().ConfigGetTextOut(path);
		
		
		NoiseParameters.LoadFromPath(basePath + "Noise");	
	}

#ifdef MOE_DEBUG_CONFIG	
	override void PrintLoadedData(string type)
	{	
		super.PrintLoadedData(type);
		Print(string.Format("MinTimer: %1", MinTimer));
		Print(string.Format("MaxTimer: %1", MaxTimer));
		Print(string.Format("InitialTickrate: %1", InitialTickrate));
		Print(string.Format("TickSpeedIncreaseFactor: %1", TickSpeedIncreaseFactor));
		
		Print(string.Format("BeepingSoundset: %1", BeepingSoundset));
		
		Print(string.Format("TimerStages: Count = %1", TimerStages.Count()));
		for(int i = 0; i < TimerStages.Count(); i++)
		{
			Print(string.Format(" [%1]: %2", i, TimerStages[i]));
		}
	}
#endif	
}