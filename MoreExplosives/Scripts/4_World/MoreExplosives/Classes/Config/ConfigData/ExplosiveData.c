class MOE_ExplosiveData : MOE_ConfigDataBase
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