class MOE_ModSettings
{
	
    bool IsMOERaidingOnlyEnabled;
   	bool IsRaidSchedulingEnabled;
    bool IsDoorRaidOnlyEnabled;
    bool IsDeleteLocksEnabled;
    bool IsDestroyBaseAfterDestructionEnabled;
	bool IsParentExplosivesToTargetEnabled;
	int AreaDamageMode; 
    
    void MOE_ModSettings()
    {
        Init();
		AfterInit();
    }

    void Init()
    {
		string cfgPath = MoreExplosives.CFG_MOE + " ";
        string path = cfgPath + "raidOnlyWithMOE";
		if(GetGame().ConfigIsExisting(path))
		{
			IsMOERaidingOnlyEnabled = GetGame().ConfigGetInt(path) != 0;
		}
		
		path = cfgPath + "raidSchedulingEnabled";
		if(GetGame().ConfigIsExisting(path))
		{
			IsRaidSchedulingEnabled = GetGame().ConfigGetInt(path) != 0;
		}
			
		path = cfgPath + "doorRaidOnlyEnabled";
		if(GetGame().ConfigIsExisting(path))
		{
			IsDoorRaidOnlyEnabled = GetGame().ConfigGetInt(path) != 0;
		}
		
		path = cfgPath + "deleteLocks";
		if(GetGame().ConfigIsExisting(path))
		{
			IsDeleteLocksEnabled = GetGame().ConfigGetInt(path) != 0;
		}
		
		path = cfgPath + "destroyBaseAfterDestruction";
		if(GetGame().ConfigIsExisting(path))
		{
			IsDestroyBaseAfterDestructionEnabled = GetGame().ConfigGetInt(path) != 0;
		}
		
		path = cfgPath + "parentExplosivesToTarget";
		if(GetGame().ConfigIsExisting(path))
		{
			IsParentExplosivesToTargetEnabled = GetGame().ConfigGetInt(path) != 0;
		}
		
		path = cfgPath + "areaDamageMode";
		if(GetGame().ConfigIsExisting(path))
		{
			AreaDamageMode = GetGame().ConfigGetInt(path);
		}
    }

	void AfterInit()
	{
		Log_MOE(GetLogString(), MOE_ELogTypes.CONFIG);
	}
	
	string GetLogString()
	{
		return string.Format("MOE instance has been created!\n	GENERAL SETTINGS:\n	%1\n %2\n %3\n %4\n %5\n %6\n %7",
							"\n		IsMOERaidingOnlyEnabled = " + IsMOERaidingOnlyEnabled,
							"\n		IsRaidSchedulingEnabled = " + IsRaidSchedulingEnabled,
							"\n		IsDoorRaidOnlyEnabled = " + IsDoorRaidOnlyEnabled,
							"\n		IsDeleteLocksEnabled = " + IsDeleteLocksEnabled,
							"\n		IsDestroyBaseAfterDestructionEnabled = " + IsDestroyBaseAfterDestructionEnabled,
							"\n		IsParentExplosivesToTargetEnabled = " + IsParentExplosivesToTargetEnabled,
							"\n		AreaDamageMode = " + AreaDamageMode);
	}

}