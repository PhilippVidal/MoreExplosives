class MOE_ModSettings
{
    bool IsMOERaidingOnlyEnabled;
    bool IsRaidSchedulingEnabled;
    bool IsDoorRaidOnlyEnabled;
    bool IsDeleteLocksEnabled;
    bool IsDestroyBaseAfterDestructionEnabled;
	bool IsDamagePlacementTargetDirectlyEnabled;
	bool IsParentExplosivesToTargetEnabled;
	int AreaDamageMode; 
    
    void MOE_ModSettings()
    {
        Init();
		AfterInit();
    }

    void Init()
    {
        string cfgPath = MoreExplosives.CFG_MOE + "raidOnlyWithMOE";
		if(GetGame().ConfigIsExisting(cfgPath))
		{
			IsMOERaidingOnlyEnabled = GetGame().ConfigGetInt(cfgPath) != 0;
		}
		
		cfgPath = MoreExplosives.CFG_MOE + "raidSchedulingEnabled";
		if(GetGame().ConfigIsExisting(cfgPath))
		{
			IsRaidSchedulingEnabled = GetGame().ConfigGetInt(cfgPath) != 0;
		}
			
		cfgPath = MoreExplosives.CFG_MOE + "doorRaidOnlyEnabled";
		if(GetGame().ConfigIsExisting(cfgPath))
		{
			IsDoorRaidOnlyEnabled = GetGame().ConfigGetInt(cfgPath) != 0;
		}
		
		cfgPath = MoreExplosives.CFG_MOE + "deleteLocks";
		if(GetGame().ConfigIsExisting(cfgPath))
		{
			IsDeleteLocksEnabled = GetGame().ConfigGetInt(cfgPath) != 0;
		}
		
		cfgPath = MoreExplosives.CFG_MOE + "destroyBaseAfterDestruction";
		if(GetGame().ConfigIsExisting(cfgPath))
		{
			IsDestroyBaseAfterDestructionEnabled = GetGame().ConfigGetInt(cfgPath) != 0;
		}

		cfgPath = MoreExplosives.CFG_MOE + "damagePlacementTargetDirectly";
		if(GetGame().ConfigIsExisting(cfgPath))
		{
			IsDamagePlacementTargetDirectlyEnabled = GetGame().ConfigGetInt(cfgPath) != 0;
		}
		
		cfgPath = MoreExplosives.CFG_MOE + "parentExplosivesToTarget";
		if(GetGame().ConfigIsExisting(cfgPath))
		{
			IsParentExplosivesToTargetEnabled = GetGame().ConfigGetInt(cfgPath) != 0;
		}
		
		cfgPath = MoreExplosives.CFG_MOE + "areaDamageMode";
		if(GetGame().ConfigIsExisting(cfgPath))
		{
			AreaDamageMode = GetGame().ConfigGetInt(cfgPath);
		}
    }

	void AfterInit()
	{
		string logStr = string.Format("MOE instance has been created!\n	GENERAL SETTINGS:\n	%1\n %2\n %3\n %4\n %5\n %6\n %7\n %8",
							"\n		IsMOERaidingOnlyEnabled = " + IsMOERaidingOnlyEnabled,
							"\n		IsRaidSchedulingEnabled = " + IsRaidSchedulingEnabled,
							"\n		IsDoorRaidOnlyEnabled = " + IsDoorRaidOnlyEnabled,
							"\n		IsDeleteLocksEnabled = " + IsDeleteLocksEnabled,
							"\n		IsDestroyBaseAfterDestructionEnabled = " + IsDestroyBaseAfterDestructionEnabled,
							"\n		IsDamagePlacementTargetDirectlyEnabled = " + IsDamagePlacementTargetDirectlyEnabled,
							"\n		IsParentExplosivesToTargetEnabled = " + IsParentExplosivesToTargetEnabled,
							"\n		AreaDamageMode = " + AreaDamageMode);

		Log_MOE(logStr, MOE_ELogTypes.CONFIG);
	}
}