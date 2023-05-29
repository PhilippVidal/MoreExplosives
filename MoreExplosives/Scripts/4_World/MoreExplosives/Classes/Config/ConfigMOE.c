class MOE_ConfigMOE : MOE_ConfigBase
{
	void MOE_ConfigMOE()
	{		
		HandlePreloading();
	}
	
	override void LoadGeneralSettings()
	{
		m_GeneralSettings = new MOE_GeneralSettings();

		string cfgPath = MoreExplosives.CFG_MOE + "raidOnlyWithMOE";
		if(GetGame().ConfigIsExisting(cfgPath))
		{
			m_GeneralSettings.IsMOERaidingOnlyEnabled = GetGame().ConfigGetInt(cfgPath) != 0;
		}
		
		cfgPath = MoreExplosives.CFG_MOE + "raidSchedulingEnabled";
		if(GetGame().ConfigIsExisting(cfgPath))
		{
			m_GeneralSettings.IsRaidSchedulingEnabled = GetGame().ConfigGetInt(cfgPath) != 0;
		}
		
		
		cfgPath = MoreExplosives.CFG_MOE + "doorRaidOnlyEnabled";
		if(GetGame().ConfigIsExisting(cfgPath))
		{
			m_GeneralSettings.IsDoorRaidOnlyEnabled = GetGame().ConfigGetInt(cfgPath) != 0;
		}
		
		cfgPath = MoreExplosives.CFG_MOE + "deleteLocks";
		if(GetGame().ConfigIsExisting(cfgPath))
		{
			m_GeneralSettings.IsDeleteLocksEnabled = GetGame().ConfigGetInt(cfgPath) != 0;
		}
		
		cfgPath = MoreExplosives.CFG_MOE + "destroyBaseAfterDestruction";
		if(GetGame().ConfigIsExisting(cfgPath))
		{
			m_GeneralSettings.IsDestroyBaseAfterDestructionEnabled = GetGame().ConfigGetInt(cfgPath) != 0;
		}
	}
	
	
	
	protected void HandlePreloading()
	{
#ifdef SERVER
		array<string> preloadList = new array<string>();
		string cfgPath = MoreExplosives.CFG_MOE + "baseBuildingEntitiesToPreload";
		if(GetGame().ConfigIsExisting(cfgPath))
		{
			GetGame().ConfigGetTextArray(cfgPath, preloadList);
		}

		foreach(string entityType : preloadList)
		{
			if(!PreloadBaseBuildingEntity(entityType))
			{
				Log_MOE(string.Format("Could not preload %1!", entityType), MOE_ELogTypes.ERROR);
			}
			else 
			{
				Log_MOE(string.Format("Preloaded %1!", entityType), MOE_ELogTypes.CONFIG);
			}
		}	
#endif	
	}
	
	MOE_EntityData CacheEntityData(string entityType, bool trySorting, array<string> dmgZones = null)
	{
		MOE_EntityData data = new MOE_EntityData(entityType, dmgZones);
		m_CachedEntityData.Insert(entityType, data);
		return data;
	}
	
	bool PreloadBaseBuildingEntity(string entityType)
	{
		typename typeName = entityType.ToType();
		if(!typeName || !typeName.IsInherited(BaseBuildingBase))
		{
			return false;
		}

		array<string> dmgZones = new array<string>();
		string basePath = string.Format("%1 %2 DamageSystem DamageZones", CFG_VEHICLESPATH, entityType);	
		if(GetGame().ConfigIsExisting(basePath))
		{
			int childCount = GetGame().ConfigGetChildrenCount(basePath);	
			if(childCount > 0)
			{
				string childName;		
				for(int idx = 0; idx < childCount; idx++)
				{
					GetGame().ConfigGetChildName(basePath, idx, childName);			
					dmgZones.Insert(childName);
				}
			}		
		}
		
		if(dmgZones.Count() == 0)
		{
			dmgZones.Insert("");
		}	
		
		MOE_EntityData entityData = CacheEntityData(entityType, true, dmgZones);
		
		if(!entityData)
		{
			return false;
		}
		
		MOE_DamageZoneData zoneData;
		int count = entityData.GetDamageZoneCount();
		for(int i = 0; i < count; i++)
		{
			zoneData = entityData.GetDamageZone(i);
			if(zoneData)
			{
				zoneData.GetRequiredDestroyedParts(entityType);
			}
		}
		 
		return true;
	}
	
	bool HasAmmoDataCached(string ammo, out MOE_AmmoData ammoData)
	{
		return m_CachedAmmoData.Find(ammo, ammoData);
	}
	
	MOE_AmmoData CacheAmmoData(string ammo)
	{
		if(!GetGame().ConfigIsExisting(CFG_AMMO + " " + ammo)) 
		{
			return null;
		}
		
		MOE_AmmoData ammoData = new MOE_AmmoData(ammo);
		m_CachedAmmoData.Insert(ammo, ammoData);
		return ammoData;
	}
	
	MOE_AmmoData GetAmmoData(string ammo)
	{
		MOE_AmmoData ammoData;
		if(m_CachedAmmoData.Find(ammo, ammoData))
		{
			return ammoData;
		}
		
		return CacheAmmoData(ammo);		
	}

}
