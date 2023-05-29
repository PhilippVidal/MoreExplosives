//Classes that hold relevant ("static") config information for each object type -> prevents duplicate data on each object 
//Cached & accessed through MOE_Functions class
class MOE_ConfigDataBase
{
	void MOE_ConfigDataBase(string type)
	{
		Init(type);
		OnAfterInit(type);
	}
	
	//Loads values from config on data creation based on object type
	protected void Init(string type)
	{	
		
	}
	
	protected void OnAfterInit(string type)
	{
#ifdef MOE_DEBUG_CONFIG
		PrintLoadedData(type);
#endif	
	}
	
	//where to find config values -> e.g., CfgVehicles MOE_ExplosiveHomemade MOE_Settings VARIABLE
	string GetBasePath(string type)
	{
		return string.Format("%1 %2 MOE_Settings ", CFG_VEHICLESPATH, type);	
	}
	
#ifdef MOE_DEBUG_CONFIG	
	void PrintLoadedData(string type)
	{
		Print(string.Format("MOE_DEBUG	Loaded Config Data for %1:", type));
	}
#endif	
}


class MOE_ConfigDataTriggerBase : MOE_ConfigDataBase
{
	float LightRadius;
	float LightBrightness;
	ref array<vector> LightColors;
	
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
	
	array<vector> RGBValuesToVectors(notnull array<float> rgbValues)
	{
		int count = rgbValues.Count();
		for(int difference = count % 3; difference > 0; difference--)
		{
			rgbValues.Insert(0);
		}
		
		count = rgbValues.Count();
		
		array<vector> colors = new array<vector>();
		vector color;
		for(int i = 0; i < count; i = i + 3)
		{
			color[0] = rgbValues[i];
			color[1] = rgbValues[i + 1];
			color[2] = rgbValues[i + 2];
			colors.Insert(color);
		}
		
		return colors;
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
}

class MOE_ConfigDataExplosive : MOE_ConfigDataBase
{
	float TimeToArm;		
	float TimeToDisarm;		
	float TimeToDefuse;	
	float TimeToMount;		
	float TimeToDismount;		
	
	float ExplodeOnRuinedChance;			//Percentage -> 0 = never explode, 1 = always explode
	float RuinAttachmentsOnRuinedChance;	//Percentage -> 0 = attachments don't get ruined, 1 = attachments always get ruined
	
	bool HasToBeMounted;
	bool CanTakeDamageWhileMounted;
	bool CanTakeDamageWhileArmed;
	bool CanOnlyRaidDoors;
	bool CanOnlyDamagePlacementTarget;
	bool CanOnlyMountDuringSchedule;
	
	//0 = Everywhere, 
	//1 = Only on Base Building Objects, 
	//2 = Only on selected objects
	int MountingMode;
	
	string AmmoType;
	string ExplosionBehaviourType;
	ref array<string> AttachableTriggers;
	
	vector PlacementOffsetPosition;
	vector PlacementOffsetRotation;
	
	int ExplosionParticleIndex;
	ref array<string> ArmTools;
	ref array<string> DisarmTools;
	ref array<string> DefuseTools;

	override void Init(string type)
	{
		super.Init(type);
		
		//Set default values
		TimeToArm 		= 5;		
		TimeToDisarm 	= 5;		
		TimeToDefuse 	= 20;	
		TimeToMount 	= 5;		
		TimeToDismount 	= 5;		
		
		ExplodeOnRuinedChance 			= 0;			
		RuinAttachmentsOnRuinedChance 	= 0;	
		
		HasToBeMounted 				= false;
		CanTakeDamageWhileMounted 	= false;
		CanTakeDamageWhileArmed	 	= true;
		CanOnlyRaidDoors 			= false;
		CanOnlyDamagePlacementTarget = false;
		CanOnlyMountDuringSchedule 	= false;
		
		MountingMode = MOE_EExplosiveMountingModes.EVERYWHERE;
			
		AmmoType 				= "Explosion_NonLethal";
		ExplosionBehaviourType 	= "MOE_ExplosionBehaviourBasic";
		
		AttachableTriggers 		= new array<string>();
		
		ArmTools 	= new array<string>();
		DisarmTools = new array<string>();
		DefuseTools = new array<string>();
			
		//Load values from config if they exist	
		string basePath = GetBasePath(type);	
		string path = basePath + "timeToArm";
		if(GetGame().ConfigIsExisting(path)) 
			TimeToArm = GetGame().ConfigGetFloat(path);
		
		path = basePath + "timeToDisarm";
		if(GetGame().ConfigIsExisting(path)) 
			TimeToDisarm = GetGame().ConfigGetFloat(path);

		path = basePath + "timeToDefuse";
		if(GetGame().ConfigIsExisting(path)) 
			TimeToDefuse = GetGame().ConfigGetFloat(path);
		
		path = basePath + "timeToMount";
		if(GetGame().ConfigIsExisting(path)) 
			TimeToMount = GetGame().ConfigGetFloat(path);
		
		path = basePath + "timeToDismount";
		if(GetGame().ConfigIsExisting(path)) 
			TimeToDismount = GetGame().ConfigGetFloat(path);

		
		path = basePath + "explodeOnRuinedChance";
		if(GetGame().ConfigIsExisting(path)) 
			ExplodeOnRuinedChance = GetGame().ConfigGetFloat(path);

		path = basePath + "ruinAttachmentsOnRuinedChance";
		if(GetGame().ConfigIsExisting(path)) 
			RuinAttachmentsOnRuinedChance = GetGame().ConfigGetFloat(path);
		
		
		path = basePath + "hasToBeMounted";
		if(GetGame().ConfigIsExisting(path)) 
			HasToBeMounted = GetGame().ConfigGetInt(path) != 0;
		
		path = basePath + "canTakeDamageWhileMounted";
		if(GetGame().ConfigIsExisting(path)) 
			CanTakeDamageWhileMounted = GetGame().ConfigGetInt(path) != 0;
		
		path = basePath + "canTakeDamageWhileArmed";
		if(GetGame().ConfigIsExisting(path)) 
			CanTakeDamageWhileArmed = GetGame().ConfigGetInt(path) != 0;
			
		path = basePath + "canOnlyRaidDoors";
		if(GetGame().ConfigIsExisting(path)) 
			CanOnlyRaidDoors = GetGame().ConfigGetInt(path) != 0;
			
		
		path = basePath + "canOnlyDamagePlacementTarget";
		if(GetGame().ConfigIsExisting(path)) 
			CanOnlyDamagePlacementTarget = GetGame().ConfigGetInt(path) != 0;
		
		path = basePath + "canOnlyMountDuringSchedule";
		if(GetGame().ConfigIsExisting(path)) 
			CanOnlyMountDuringSchedule = GetGame().ConfigGetInt(path) != 0;
		
		
		path = basePath + "mountingMode";
		if(GetGame().ConfigIsExisting(path)) 
			MountingMode = GetGame().ConfigGetInt(path);
		
		
		path = basePath + "explosionBehaviour";
		if(GetGame().ConfigIsExisting(path)) 
			ExplosionBehaviourType = GetGame().ConfigGetTextOut(path);	

		
		path = string.Format("%1 %2 %3", CFG_VEHICLESPATH, type, "ammoType");	
		if(GetGame().ConfigIsExisting(path)) 
			AmmoType = GetGame().ConfigGetTextOut(path);
		
		
		path = basePath + "attachableTriggers";
		if(GetGame().ConfigIsExisting(path)) 
			GetGame().ConfigGetTextArray(path, AttachableTriggers);
		
		
		path = basePath + "armTools";
		if(GetGame().ConfigIsExisting(path)) 
			GetGame().ConfigGetTextArray(path, ArmTools);
		
		path = basePath + "disarmTools";
		if(GetGame().ConfigIsExisting(path)) 
			GetGame().ConfigGetTextArray(path, DisarmTools);
		
		path = basePath + "defuseTools";
		if(GetGame().ConfigIsExisting(path)) 
			GetGame().ConfigGetTextArray(path, DefuseTools);
	}

#ifdef MOE_DEBUG_CONFIG		
	override void PrintLoadedData(string type)
	{	
		super.PrintLoadedData(type);
		Print(string.Format("TimeToArm: %1", TimeToArm));
		Print(string.Format("TimeToDisarm: %1", TimeToDisarm));
		Print(string.Format("TimeToDefuse: %1", TimeToDefuse));
		Print(string.Format("TimeToMount: %1", TimeToMount));
		Print(string.Format("TimeToDismount: %1", TimeToDismount));
		
		Print(string.Format("ExplodeOnRuinedChance: %1", ExplodeOnRuinedChance));
		Print(string.Format("RuinAttachmentsOnRuinedChance: %1", RuinAttachmentsOnRuinedChance));
		
		Print(string.Format("HasToBeMounted: %1", HasToBeMounted));
		Print(string.Format("CanTakeDamageWhileMounted: %1", CanTakeDamageWhileMounted));
		Print(string.Format("CanTakeDamageWhileArmed: %1", CanTakeDamageWhileArmed));
		
		Print(string.Format("AmmoType: %1", AmmoType));
		Print(string.Format("ExplosionBehaviourType: %1", ExplosionBehaviourType));
		Print(string.Format("AttachableTriggers: Count = %1", AttachableTriggers.Count()));
		int i;
		for(i = 0; i < AttachableTriggers.Count(); i++)
		{
			Print(string.Format(" [%1]: %2", i, AttachableTriggers[i]));
		}
		
		Print(string.Format("ArmTools: Count = %1", ArmTools.Count()));
		for(i = 0; i < ArmTools.Count(); i++)
		{
			Print(string.Format(" [%1]: %2", i, ArmTools[i]));
		}	
		
		Print(string.Format("DisarmTools: Count = %1", DisarmTools.Count()));
		for(i = 0; i < DisarmTools.Count(); i++)
		{
			Print(string.Format(" [%1]: %2", i, DisarmTools[i]));
		}
		
		Print(string.Format("DefuseTools: Count = %1", DefuseTools.Count()));
		for(i = 0; i < DefuseTools.Count(); i++)
		{
			Print(string.Format(" [%1]: %2", i, DefuseTools[i]));
		}
	}
#endif	
}

class MOE_ConfigDataTimer : MOE_ConfigDataTriggerBase
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


class MOE_ConfigDataReceiver : MOE_ConfigDataTriggerBase
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

class MOE_ConfigDataSignalSource : MOE_ConfigDataBase
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

class MOE_ConfigDataDetonator : MOE_ConfigDataSignalSource
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