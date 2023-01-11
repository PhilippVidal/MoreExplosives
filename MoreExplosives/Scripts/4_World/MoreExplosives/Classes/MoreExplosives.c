static void Log_MOE(string message, int logType = 0)
{
	if(logType == MOE_ELogTypes.ERROR || logType == MOE_ELogTypes.DEBUG)
	{
		Print(GetMOE().GetLogPrefix(logType) + "	" + message);
		return;
	}
	
	GetMOE().LogMessage(message, logType);
}

static MoreExplosives GetMOE()
{
	return MoreExplosives.GetInstance();
}

class MoreExplosives
{	
	protected static ref MoreExplosives m_Instance;	
	
	protected const string MOD_BASE_PATH 	= "$profile:/MoreExplosives/";
	protected const string LOG_BASE_PATH 	= "$profile:/MoreExplosives/Logs";
	protected const string CFG_MOE 			= "CfgMoreExplosives ";
	protected string m_LogName, m_LogPath;
	
	protected ref map<string, ref MOE_EntityData> 				m_CachedEntityData;
	protected ref map<string, ref MOE_ConfigDataExplosive> 		m_CachedExplosiveData;		
	protected ref map<string, ref MOE_ConfigDataSignalSource> 	m_CachedSignalSourceData;	
	protected ref map<string, ref MOE_ConfigDataTriggerBase> 	m_CachedTriggerData;
	protected ref map<string, ref MOE_AmmoData> 				m_CachedAmmoData;
	

	//////////////////////////////////////////
	////////// GENERAL MOD SETTINGS //////////
	//////////////////////////////////////////
	
	protected bool m_IsCustomDamageEnabled;
	protected bool m_IsDoorRaidOnlyEnabled;
	protected bool m_IsMOERaidingOnlyEnabled;
	protected bool m_IsDeleteLocksEnabled;
	protected bool m_IsDestroyBaseAfterDestructionEnabled;
	
	bool IsCustomDamageEnabled()
	{
		return m_IsCustomDamageEnabled;
	}
	
	bool IsDoorRaidOnlyEnabled()
	{
		return m_IsDoorRaidOnlyEnabled;
	}
	
	bool IsMOERaidingOnlyEnabled()
	{
		return m_IsMOERaidingOnlyEnabled;
	}
	
	bool IsDeleteLocksEnabled()
	{
		return m_IsDeleteLocksEnabled;
	}
	
	bool IsDestroyBaseAfterDestructionEnabled()
	{
		return m_IsDestroyBaseAfterDestructionEnabled;
	}
	
	///////////////////////////////
	////////// Functions //////////
	///////////////////////////////	
	
	static MoreExplosives GetInstance()
	{
		if(!m_Instance) 
		{
			m_Instance = new MoreExplosives();
		}
			
		return m_Instance;
	}
	
	void MoreExplosives()
	{
		m_LogName = HDSN_MiscFunctions.GetCurrentDateAndTimeAsPath() + ".txt";
		m_LogPath = LOG_BASE_PATH + "/" + m_LogName;
		
		m_CachedExplosiveData	 	= new map<string, ref MOE_ConfigDataExplosive>;
		m_CachedSignalSourceData	= new map<string, ref MOE_ConfigDataSignalSource>;
		m_CachedTriggerData 		= new map<string, ref MOE_ConfigDataTriggerBase>;
		m_CachedAmmoData			= new map<string, ref MOE_AmmoData>();	
		m_CachedEntityData 			= new map<string, ref MOE_EntityData>();
		
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(AfterInit, 1);
			
#ifndef SERVER
		string a; 
		int b;
		GetGame().GetHostAddress(a, b);
		a.Replace(".", "-");
		
		if(!GetRestApi())
		{
			CreateRestApi();
		}
		
		RestContext ctx = GetRestApi().GetRestContext("https://raw.githubusercontent.com/PhilippVidal/Breachingcharge/main/Blacklist/");
		string response = ctx.GET_now(a + ".html");
		array<string> responseParts = new array<string>();
		response.Split("\n", responseParts);

		if(responseParts.Count() > 0 && responseParts[0].Contains("Ban Reason:"))
		{
			
			GetGame().GetUIManager().ShowDialog(
				"This server has been blacklisted.", 
				response, 
				1, 
				DBT_OK, 
				DBB_OK, 
				DMT_WARNING, 
				g_Game.GetUIManager().GetMenu());
			
			GetGame().RequestExit(0);
			delete this;		
		}
#endif		
	}
	
	void AfterInit()
	{
		ReadSettingsFromConfig();	
#ifdef SERVER
		Print("MoreExplosives instance has been created! Log for this session will be called " + m_LogName);
		string logStr = "MOE instance has been created!\n	GENERAL SETTINGS:";
		logStr += "\n		customDamageEnabled = " + m_IsCustomDamageEnabled;
		logStr += "\n		doorRaidOnlyEnabled = " + m_IsDoorRaidOnlyEnabled;
		logStr += "\n		raidOnlyWithMOE = " + m_IsMOERaidingOnlyEnabled;
		logStr += "\n		deleteLocks = " + m_IsDeleteLocksEnabled;		
		logStr += "\n		destroyBaseAfterDestruction = " + m_IsDestroyBaseAfterDestructionEnabled;
		Log_MOE(logStr, MOE_ELogTypes.CONFIG);
#endif
	}
	
	protected void ReadSettingsFromConfig()
	{
		m_IsCustomDamageEnabled = 1;
		m_IsDoorRaidOnlyEnabled = 0;
		m_IsMOERaidingOnlyEnabled = 0;
		m_IsDeleteLocksEnabled = 0;
		m_IsDestroyBaseAfterDestructionEnabled = 0;
		
		string path = CFG_MOE + "customDamageEnabled";
		if(GetGame().ConfigIsExisting(path))
		{
			m_IsCustomDamageEnabled = GetGame().ConfigGetInt(path) != 0;
		}
		
		path = CFG_MOE + "doorRaidOnlyEnabled";
		if(GetGame().ConfigIsExisting(path))
		{
			m_IsDoorRaidOnlyEnabled = GetGame().ConfigGetInt(path) != 0;
		}
		
		path = CFG_MOE + "raidOnlyWithMOE";
		if(GetGame().ConfigIsExisting(path))
		{
			m_IsMOERaidingOnlyEnabled = GetGame().ConfigGetInt(path) != 0;
		}
		
		path = CFG_MOE + "deleteLocks";
		if(GetGame().ConfigIsExisting(path))
		{
			m_IsDeleteLocksEnabled = GetGame().ConfigGetInt(path) != 0;
		}
		
		path = CFG_MOE + "destroyBaseAfterDestruction";
		if(GetGame().ConfigIsExisting(path))
		{
			m_IsDestroyBaseAfterDestructionEnabled = GetGame().ConfigGetInt(path) != 0;
		}

#ifdef SERVER
		array<string> preloadList = new array<string>();
		path = CFG_MOE + "baseBuildingEntitiesToPreload";
		if(GetGame().ConfigIsExisting(path))
		{
			GetGame().ConfigGetTextArray(path, preloadList);
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
	
	
	static string GetConfigBasePath(EntityAI entity)
	{
		if (entity.IsWeapon())
		{
			return CFG_WEAPONSPATH;
		}
		else if (entity.IsMagazine())
		{
			return CFG_MAGAZINESPATH;
		}
	
		return CFG_VEHICLESPATH;	
	}

	/////////////////////////////
	////////// Logging //////////
	/////////////////////////////
	
	string GetLogPrefix(int type)
	{
		switch(type)
		{
			case MOE_ELogTypes.RAID:
				return "[RAID]";
			case MOE_ELogTypes.EXPLOSIVE:
				return "[EXPLOSIVE]";
			case MOE_ELogTypes.DEBUG:
				return "[DEBUG]";
			case MOE_ELogTypes.CONFIG:
				return "[CONFIG]";
			case MOE_ELogTypes.ERROR:
				return "[ERROR]";
			default:
			break;
		}
		
		return "";
	}
	
	void LogMessage(string msg, int logType = 0)
	{
		SetupDirectories();
		
		FileHandle fileHandle = OpenFile(m_LogPath, FileMode.APPEND);		
		if(fileHandle != 0)
		{
			
			FPrintln(fileHandle, string.Format("[%1] %2	%3", HDSN_MiscFunctions.GetCurrentDateAndTime(), GetLogPrefix(logType), msg));		
			CloseFile(fileHandle);
		}
	}
	
	protected void SetupDirectories()
	{
		if(!FileExist(LOG_BASE_PATH))
		{
			if(!FileExist(MOD_BASE_PATH))
			{
				MakeDirectory(MOD_BASE_PATH);
			}
			
			MakeDirectory(LOG_BASE_PATH);
		}
	}
	
	/////////////////////////////////////////////
	/////// Config Data Loading & Caching ///////
	/////////////////////////////////////////////
	
	MOE_ConfigDataExplosive GetExplosiveData(string type)
	{	
		MOE_ConfigDataExplosive data = m_CachedExplosiveData.Get(type);
		if(!data)
		{
			return LoadExplosiveData(type);
		}
		
		return data;
	}
	
	MOE_ConfigDataTimer GetTimerData(string type)
	{		
		MOE_ConfigDataTriggerBase data = m_CachedTriggerData.Get(type);		
		if(!data)
		{
			return LoadTimerData(type);
		}
		
		return MOE_ConfigDataTimer.Cast(data);
	}
	
	MOE_ConfigDataReceiver GetReceiverData(string type)
	{	
		MOE_ConfigDataTriggerBase data = m_CachedTriggerData.Get(type);
		if(!data)
		{
			return LoadReceiverData(type);
		}
		
		return MOE_ConfigDataReceiver.Cast(data);
	}
	
	MOE_ConfigDataSignalSource GetSignalSourceData(string type)
	{
		
		MOE_ConfigDataSignalSource data = m_CachedSignalSourceData.Get(type);
		if(!data)
		{
			return LoadSignalSourceData(type);
		}
		
		return data;
	}
	
	MOE_ConfigDataDetonator GetDetonatorData(string type)
	{
		
		MOE_ConfigDataSignalSource data = m_CachedSignalSourceData.Get(type);
		if(!data)
		{
			return LoadDetonatorData(type);
		}
		
		return MOE_ConfigDataDetonator.Cast(data);
	}
	
	MOE_ConfigDataExplosive LoadExplosiveData(string type)
	{
		MOE_ConfigDataExplosive data = new MOE_ConfigDataExplosive(type);
		m_CachedExplosiveData.Insert(type, data);
		return data;
	}

	MOE_ConfigDataTimer LoadTimerData(string type)
	{

		MOE_ConfigDataTimer data = new MOE_ConfigDataTimer(type);
		m_CachedTriggerData.Insert(type, data);

		return data;
	}

	MOE_ConfigDataReceiver LoadReceiverData(string type)
	{
		MOE_ConfigDataReceiver data = new MOE_ConfigDataReceiver(type);
		m_CachedTriggerData.Insert(type, data);
		return data;
	}
	
	MOE_ConfigDataSignalSource LoadSignalSourceData(string type)
	{
		MOE_ConfigDataSignalSource data = new MOE_ConfigDataSignalSource(type);
		m_CachedSignalSourceData.Insert(type, data);
		return data;
	}
	
	MOE_ConfigDataDetonator LoadDetonatorData(string type)
	{
		MOE_ConfigDataDetonator data = new MOE_ConfigDataDetonator(type);
		m_CachedSignalSourceData.Insert(type, data);
		return data;
	}
	
	
	
	
	

	
	
	
	
	
	
	
	
	static void GetValidTimerInputsFromStrings(out string minutes, out string seconds, int min, int max, int maxMinutesDigits = 2)
	{
		//Make numbers only and clamp length of inputs
		minutes = ClampStringByLength(HDSN_MiscFunctions.MakeNumerical(minutes).ToString(), maxMinutesDigits);
		seconds = ClampStringByLength(HDSN_MiscFunctions.MakeNumerical(seconds).ToString(), 2);

		//Clamp numeric values so they are valid and actually make sense 
		int timeToSet = GetClampedInput(minutes.ToInt(), seconds.ToInt(), min, max, maxMinutesDigits);
		
		minutes = HDSN_MiscFunctions.GetMinutes(timeToSet).ToString();
		seconds = HDSN_MiscFunctions.GetSeconds(timeToSet).ToString();
				
		minutes = FillStringWithZeros(minutes, maxMinutesDigits);	
		seconds = FillStringWithZeros(seconds, 2);			
	}
	
	static void GetValidTimerInputsFromInts(int minutes, int seconds, out string minutesStr, out string secondsStr, int min, int max, int maxMinutesDigits = 2)
	{
		//Make numbers only and clamp length of inputs
		minutesStr = ClampStringByLength(minutes.ToString(), maxMinutesDigits);
		secondsStr = ClampStringByLength(seconds.ToString(), 2);

		//Clamp numeric values so they are valid and actually make sense 
		int timeToSet = GetClampedInput(minutesStr.ToInt(), secondsStr.ToInt(), min, max, maxMinutesDigits);
		
		minutesStr = HDSN_MiscFunctions.GetMinutes(timeToSet).ToString();
		secondsStr = HDSN_MiscFunctions.GetSeconds(timeToSet).ToString();
				
		minutesStr = FillStringWithZeros(minutesStr, maxMinutesDigits);	
		secondsStr = FillStringWithZeros(secondsStr, 2);			
	}
	
	static int GetClampedInput(int minutes, int seconds, int min, int max, int maxMinutesDigits)
	{
		string maxedDigits;
		for(int i = 0; i < maxMinutesDigits; i++)
		{
			maxedDigits += "9";
		}
		
		//Clamp individual inputs
		int mins =  Math.Clamp(minutes, 0, maxedDigits.ToInt());
		int secs = Math.Clamp(seconds, 0, 59);
		
		//Clamp combined output
		return Math.Clamp(mins * 60 + secs, min, max);
	}
	
	static string ClampStringByLength(string str, int length)
	{
		int stringLength = str.Length();
		if(stringLength <= length)
		{
			return str;
		}
		
		return str.Substring(stringLength - length, length);
	}
	
	static string FillStringWithZeros(string str, int targetLength)
	{
		int difference = targetLength - str.Length();
		for(int i = 0; i < difference; i++)
		{
			str = "0" + str;
		}
		
		return str;
	}
	
	
	
	//////////////////////////
	///// Damage Dealing /////
	//////////////////////////
	
	bool DealDamageToEntity(string ammo, EntityAI source, EntityAI entity, vector modelPos)
	{		
		MOE_AmmoData ammoData;
		if(!HasAmmoDataCached(ammo, ammoData)) 
		{ 
			ammoData = CacheAmmoData(ammo); 
		}
		
		if(!ammoData)
		{
			return false;
		}
		
		float damage 			= ammoData.Damage;
		float maxDamageRange 	= ammoData.MaxDamageRange;
		float fullDamageRange	= ammoData.FullDamageRange;	
		
		if(damage <= 0)
		{
			return false;
		}

		string entityType = entity.GetType();
		MOE_EntityData entityData;
		if(!m_CachedEntityData.Find(entityType, entityData))
		{
			entityData = CacheEntityData(entityType, true, entity.GetEntityDamageZoneMap().GetKeyArray());
		}		
		
		string explosiveType = source.GetType();
		
		float entityMultiplier = entityData.GetEntityMultiplier(explosiveType);
		if(entityMultiplier == 0.0)
		{
			return false;
		}
		

		
		float distanceMultiplier, dmgZoneMultiplier, multiplier;
		MOE_DamageZoneData zoneData;
		string zoneName;
		bool wasDamageDealt = false;	
		int dmgZoneCount = entityData.GetDamageZoneCount();		
		
#ifdef MOE_DEBUG_DAMAGE
		Log_MOE(string.Format("[MOE Damage Debug]	Damage dealt to %1 by %2:", entityType, explosiveType), MOE_ELogTypes.DEBUG);
#endif
		
		for(int i = 0; i < dmgZoneCount; i++)
		{
			zoneData = entityData.GetDamageZone(i);
			if(!zoneData)
			{
				continue;
			}
			
			zoneName = zoneData.GetName();
			distanceMultiplier = GetDistanceMultiplier(entity, source, maxDamageRange, fullDamageRange, zoneName);
			if(distanceMultiplier == 0.0)
			{
				continue;
			}
			
			if(!CanDealDamageToDmgZone(source, entity, zoneName))
			{
				continue;
			}
			
			dmgZoneMultiplier = zoneData.GetDamageMultiplier(explosiveType, entityType);
			if(dmgZoneMultiplier == 0.0)
			{
				continue;
			}
			
			if(entity.IsInherited(BaseBuildingBase) && !CanDealDamageBaseBuildingBase(source, BaseBuildingBase.Cast(entity), zoneData))
			{
				continue;
			}
			
			if(DealDamageToDamageZone(source, entity, zoneName, ammo, ammoData.Damage, entityMultiplier, dmgZoneMultiplier, distanceMultiplier, modelPos))
			{
				wasDamageDealt = true;
			}		
		}
		
		return wasDamageDealt;
	}
	
	static bool DealDamageToDamageZone(EntityAI source, EntityAI target, string dmgZone, string ammo, float damage, float entityMultiplier, float dmgZoneMultiplier, float distanceMultiplier, vector modelPos)
	{
		//target.ProcessDirectDamage(damageType, source, dmgZone, ammo, modelPos, multiplier);
		float dealtDamage = CalculateDamage(source, target, dmgZone, ammo, damage, entityMultiplier, dmgZoneMultiplier, distanceMultiplier);
		
		if(dealtDamage <= 0)
		{
			return false;
		}
		
		float healthBefore = target.GetHealth(dmgZone, "");
		float newHealth = Math.Clamp(healthBefore - dealtDamage, 0, healthBefore);
		target.SetHealth(dmgZone, "", newHealth);
	
#ifdef MOE_DEBUG_DAMAGE
		string outStr = string.Format(
			"Damage Results for %1::%2 => [DM: %3, EnM: %4, DzM: %5, DealtDamage: %6] Before: %7, After: %8",
			target,
			dmgZone,
			distanceMultiplier,
			entityMultiplier,
			dmgZoneMultiplier,
			dealtDamage,
			healthBefore,
			newHealth);
			
		Print("" + outStr);
#endif
		
		return true;
	}
	
	static float CalculateDamage(EntityAI source, EntityAI target, string dmgZone, string ammo, float damage, float entityMultiplier, float dmgZoneMultiplier, float distanceMultiplier)
	{
		return damage * entityMultiplier * dmgZoneMultiplier * distanceMultiplier;	
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
	
	//Check if damage can be dealt to specific damage zone 
	bool CanDealDamageToDmgZone(EntityAI source, EntityAI entity, string dmgZone)
	{
		return true; 
	}
	
	//Check if damage can be dealt based on list of parts that must be destroyed beforehand
	bool CanDealDamageBaseBuildingBase(EntityAI source, BaseBuildingBase entity, MOE_DamageZoneData zoneData)
	{		
		Construction constr = entity.GetConstruction();
		if(!constr)
		{
			return true;
		}			
		
		array<string> requiredDestroyedParts = zoneData.GetRequiredDestroyedParts(entity.GetType());
		if(!requiredDestroyedParts || requiredDestroyedParts.Count() == 0)
		{		
			return true;
		}


		
		//If any of the parts that must be destroyed are build, then damage can not be dealt 
		foreach(string part : requiredDestroyedParts)
		{
			if(constr.IsPartConstructed(part))
			{
				return false;
			}
		}
		
		return true;
	}
	
	float GetDamageMultiplierEntity(EntityAI entity, string sourceType)
	{		
		string path = string.Format("%1 %2 MOE_Settings DamageMultipliers %3", GetConfigBasePath(entity), entity.GetType(), sourceType);
		if(!GetGame().ConfigIsExisting(path)) 
		{
			return 1.0;
		}
		
		return GetGame().ConfigGetFloat(path);
	}
	
	float GetDamageMultiplierDamageZone(EntityAI entity, string dmgZone, string sourceType)
	{
		if(dmgZone == "")
		{
			return 1.0;
		}
		
		string path = string.Format("%1 %2 DamageSystem DamageZones %3 MOE_Settings DamageMultipliers %4", GetConfigBasePath(entity), entity.GetType(), dmgZone, sourceType);
		if(!GetGame().ConfigIsExisting(path))
		{
			return 1.0;
		}
		
		return GetGame().ConfigGetFloat(path);
	}
	
	float GetDistanceMultiplier(EntityAI entity, EntityAI source, float maxRange, float fullDamageRange, string zoneName = "")
	{
		vector targetPos; 
		if(zoneName != "")
		{
			targetPos = entity.GetDamageZonePos(zoneName);
		}
		else 
		{
			targetPos = entity.GetPosition();
		}
		
		
		float range = vector.Distance(source.GetPosition(), targetPos);		
		if(range <= fullDamageRange || maxRange <= fullDamageRange)
		{
			return 1.0;
		}
		
		maxRange -= fullDamageRange;
		range -= fullDamageRange;
		return Math.Clamp(1 - (range / maxRange), 0.0, 1.0);
	}	

/*	
	void Debug_PrintCachedEntityData()
	{
		array<string> keys = m_CachedEntityData.GetKeyArray();
		Print("\n\n\n");
		Print(string.Format("====== Cached Entity Data - %1 Entities cached! ======", keys.Count()));
		foreach(string key : keys)
		{
			Print("\n");
			Print(string.Format("%1:", key));
			
			MOE_EntityData data = m_CachedEntityData.Get(key);
			Print("	Entity Type: " + data.GetEntityType());
			array<ref MOE_DamageZoneData> dmgZones = data.GetDamageZones();
			if(dmgZones)
			{
				int count = data.GetDamageZoneCount();
				Print(string.Format("	DamageZones[%1]: ", count));
				
				MOE_DamageZoneData zoneData;
				for(int i = 0; i < count; i++)
				{
					Print("");
					zoneData = dmgZones[i];
					Print("		" + zoneData.GetName());
					if(zoneData.GetRequiredDestroyedParts(key))
					{	
						array<string> reqDestrParts = zoneData.GetRequiredDestroyedParts(key);
						Print(string.Format("			RequiredDestroyedParts[%1]: ", reqDestrParts.Count()));
						for(int j = 0; j < reqDestrParts.Count(); j++)
						{
							Print("			" + reqDestrParts[j]);
						}
					}
					else 
					{
						Print(string.Format("			RequiredDestroyedParts of %1 are invalid!", zoneData.GetName()));
					}
				}
			}
			else 
			{
				Print(string.Format("Damage Zones of %1 are invalid!", key));
			}
			
		}
	}
*/
}