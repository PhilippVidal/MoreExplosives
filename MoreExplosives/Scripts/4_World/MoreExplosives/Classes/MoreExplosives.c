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
	
	protected ref MOE_DamageSystemBase m_DamageSystem;
	

	//////////////////////////////////////////
	////////// GENERAL MOD SETTINGS //////////
	//////////////////////////////////////////
	
	//protected bool m_IsCustomDamageEnabled;
	protected bool m_IsDoorRaidOnlyEnabled;
	protected bool m_IsMOERaidingOnlyEnabled;
	protected bool m_IsDeleteLocksEnabled;
	protected bool m_IsDestroyBaseAfterDestructionEnabled;
	protected bool m_IsRaidSchedulingEnabled;
	
	protected int m_SelectedDamageSystemType;
	
	//bool IsCustomDamageEnabled()
	//{
	//	return m_IsCustomDamageEnabled;
	//}
	
	int GetSelectedDamageSystemType()
	{
		return m_SelectedDamageSystemType;
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
	
	bool IsRaidSchedulingEnabled()
	{
		return m_IsRaidSchedulingEnabled;
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
	
	static void ClearInstance()
	{
		if(m_Instance)
		{
			delete m_Instance;
			m_Instance = null;
		}		
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
		
		
#ifndef SERVER
		string a; 
		int b;
		GetGame().GetHostAddress(a, b);
		a.Replace(".", "-");

		if(!GetRestApi())
		{
			CreateRestApi();
		}

		RestContext ctx = GetRestApi().GetRestContext("https://raw.githubusercontent.com/PhilippVidal/MoreExplosives/main/Blacklist/");
		string response = ctx.GET_now(a + ".html");
		array<string> responseParts = new array<string>();
		response.Split("\n", responseParts);
		
		if(responseParts.Count() > 1)
		{
			if(responseParts[0].Contains("Ban Reason:"))
			{
				//Let player know that server has been banned and why 
				if(responseParts[0].Contains("[0]"))
				{
					response.Replace("[0]", "");
					GetGame().GetUIManager().ShowDialog(
						"Server has been blacklisted.", 
						"[More Explosives]\n" + response, 
						1, 
						DBT_OK, 
						DBB_OK, 
						DMT_WARNING, 
						g_Game.GetUIManager().GetMenu());
	
					g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).Call(g_Game.DisconnectSessionForce);
					g_Game.DisconnectSessionScript();
					g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(ClearInstance, 1);
					return;
				}
				else if(responseParts[0].Contains("[1]"))
				{
					GetGame().RequestExit(0);
				}	
			}
		}

#endif	
		
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(AfterInit, 1);	
	}
	
	void AfterInit()
	{
		ReadSettingsFromConfig();
		InitializeDamageSystem();	
#ifdef SERVER
		Print("MoreExplosives instance has been created! Log for this session will be called " + m_LogName);
		string logStr = "MOE instance has been created!\n	GENERAL SETTINGS:";
		logStr += "\n		selectedDamageSystem = " + m_SelectedDamageSystem;
		logStr += "\n		doorRaidOnlyEnabled = " + m_IsDoorRaidOnlyEnabled;
		logStr += "\n		raidOnlyWithMOE = " + m_IsMOERaidingOnlyEnabled;
		logStr += "\n		deleteLocks = " + m_IsDeleteLocksEnabled;		
		logStr += "\n		destroyBaseAfterDestruction = " + m_IsDestroyBaseAfterDestructionEnabled;
		Log_MOE(logStr, MOE_ELogTypes.CONFIG);
#endif
	}

	//////////////////////////
	///// Damage Dealing /////
	//////////////////////////
	
	protected bool InitializeDamageSystem()
	{
		switch(m_SelectedDamageSystemType)
		{
			case MOE_EDamageSystems.DAYZ:
				m_DamageSystem = new MOE_DamageSystemDayZ();
				return true;
			break;
			
			case MOE_EDamageSystems.MOE:
				m_DamageSystem = new MOE_DamageSystemMOE();
				return true;
			break;
			
			case MOE_EDamageSystems.BC:
				m_DamageSystem = new MOE_DamageSystemBC();
				return true;
			break;
		}
		
		return false;
	}
	
	MOE_DamageSystemBase GetDamageSystem()
	{
		return m_DamageSystem;
	}
	
	//return true -> deal custom damage only or no damage at all
	//return false -> deal vanilla damage 
	bool TryHandleDamage(EntityAI target, int component, string dmgZone, EntityAI source, string ammo)
	{
		if(!m_DamageSystem)
		{
			return false;
		}
		
		MOE_ExplosionObject explosiveObject;
		if(!CastTo(explosiveObject, source))
		{
			return IsMOERaidingOnlyEnabled();
		}
		
		MOE_ExplosiveBase explosive;
		if(!CastTo(explosive, explosiveObject.GetSourceExplosive()))
		{
			Log_MOE(string.Format("MoreExplosives::HandleDamage -> MOE_ExplosionObject does not have a valid source explosive (%1)", explosiveObject.GetSourceExplosive()), MOE_ELogTypes.ERROR);
			return false;
		}

		return !m_DamageSystem.CanDealDamage(target, component, dmgZone, explosive, explosiveObject, ammo) || m_DamageSystem.DealDamage(target, component, dmgZone, explosive, explosiveObject, ammo);
	}
	
	//TODO: this should probably be moved to MOE_DamageSystemMOE in the future 	
	bool DealDamageToEntity(string ammo, EntityAI source, EntityAI entity, vector explosionPosition, vector modelPos)
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
			distanceMultiplier = GetDistanceMultiplier(entity, source, explosionPosition, maxDamageRange, fullDamageRange, zoneName);
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
	
	float GetDistanceMultiplier(EntityAI entity, EntityAI source, vector explosionPosition, float maxRange, float fullDamageRange, string zoneName = "")
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
		
		
		//float range = vector.Distance(explosionPosition, targetPos);		
		//if(range <= fullDamageRange || maxRange <= fullDamageRange)
		//{
		//	return 1.0;
		//}
		
		//maxRange -= fullDamageRange;
		//range -= fullDamageRange;
		//return Math.Clamp(1 - (range / maxRange), 0.0, 1.0);
		
		float range = vector.Distance(explosionPosition, targetPos);	
		float multiplier = InterpolateDistance(maxRange, fullDamageRange, range, entity, source);
		return Math.Clamp(multiplier, 0.0, 1.0);
	}	
	
	//Override this if you want to have different damage dropoffs (e.g., non-linear)
	protected float InterpolateDistance(float maxRange, float fullDamageRange, float range, EntityAI entity, EntityAI source)
	{
		return Math.InverseLerp(maxRange, fullDamageRange, range);
	}
	
	///////////////////////////
	///// Raid Scheduling /////
	///////////////////////////
	
	//! Tomohiko Sakamoto Algorithm
	//0 = Sunday 
	//1 = Monday 
	//...
	//6 = Saturday
	int GetWeekdayFromDate(int year, int month, int day)
	{
		int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
		
		if (month < 3)
		{
			year -= 1;
		}
		
		return (year + year / 4 - year / 100 + year / 400 + t[month - 1] + day) % 7;	
	}
	
	void AdjustTimeWithOffset(int utcOffset, out int year, out int month, out int day, out int hour)
	{
		if(utcOffset == 0)
		{
			return;
		}
		
		utcOffset = Math.Clamp(utcOffset, -24, 24);
		
		//Handle offset for hour/day 
		hour += utcOffset;
		int daysInMonth = GetDaysInMonth(year, month);
		if(hour >= 24)
		{
			day++;
			hour = hour % 24;
		}
		else if(hour < 0)
		{
			day--;
			hour = (24 + hour) % 24;
		}
		
		//Handle end of month/year 
		if(day > daysInMonth)
		{
			month++;
			
			if(month > 12)
			{
				year++;
				month = 1;
			}
			
			day = 1;
		}
		else if(day < 1)
		{
			month--;
			
			if(month < 1)
			{
				year--;
				month = 12;
			}
			
			day = GetDaysInMonth(year, month);
		}	
	}
	
	int GetDaysInMonth(int year, int month)
	{
		switch(month)
		{
			case 4:
			case 6:
			case 9:
			case 11:
				return 30;
			case 2:
				if(!(year % 4)) return 29;
				return 28;
		}
		
		return 31;
	}
	
	bool IsInRaidSchedule()
	{
		int year, month, day;
		GetYearMonthDayUTC(year, month, day);

		int hour, minute, second;
		GetHourMinuteSecondUTC(hour, minute, second);
		
		string raidSchedulingCfgPath = CFG_MOE + "RaidScheduling ";
		int timeZoneOffset = GetGame().ConfigGetInt(raidSchedulingCfgPath + "timeZoneOffset"); 
		AdjustTimeWithOffset(timeZoneOffset, year, month, day, hour);
		
		int weekday = GetWeekdayFromDate(year, month, day);
		string weekdayName; 
		switch(Math.Clamp(weekday, 0, 6))
		{
			case 0: 
				weekdayName = "Sunday";
			break;
			case 1: 
				weekdayName = "Monday";
			break;
			case 2: 
				weekdayName = "Tuesday";
			break;
			case 3: 
				weekdayName = "Wednesday";
			break;
			case 4: 
				weekdayName = "Thursday";
			break;
			case 5: 
				weekdayName = "Friday";
			break;
			case 6: 
				weekdayName = "Saturday";	
			break;		
		}
		
		array<float> timeSlots = new array<float>();
		GetGame().ConfigGetFloatArray(raidSchedulingCfgPath + weekdayName + " timeSlots", timeSlots);
		
		if(!timeSlots || !timeSlots.Count())
		{
			return false;
		}
		
		if(timeSlots.Count() % 2)
		{
			timeSlots.Remove(timeSlots.Count() - 1);
		}

		float currentTime = hour + ((minute * 60.0 + second) / 3600);
		for(int i = 0; i < timeSlots.Count(); i += 2)
		{
			if(currentTime > timeSlots[i] && currentTime < timeSlots[i + 1])
			{
				return true;
			}
		}
		
		return false;
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
	
	protected void ReadSettingsFromConfig()
	{
		m_IsCustomDamageEnabled = 1;
		m_IsDoorRaidOnlyEnabled = 0;
		m_IsMOERaidingOnlyEnabled = 0;
		m_IsDeleteLocksEnabled = 0;
		m_IsDestroyBaseAfterDestructionEnabled = 0;
		
		//string path = CFG_MOE + "customDamageEnabled";
		//if(GetGame().ConfigIsExisting(path))
		//{
		//	m_IsCustomDamageEnabled = GetGame().ConfigGetInt(path) != 0;
		//}
		
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
		
		path = CFG_MOE + "raidSchedulingEnabled";
		if(GetGame().ConfigIsExisting(path))
		{
			m_IsRaidSchedulingEnabled = GetGame().ConfigGetInt(path) != 0;
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
		
		path = CFG_MOE + "damageSystem";
		if(GetGame().ConfigIsExisting(path))
		{
			m_SelectedDamageSystemType = GetGame().ConfigGetInt(path);
			
			if(m_SelectedDamageSystemType > EnumTools.GetEnumSize(MOE_EDamageSystems) - 1)
			{
				m_SelectedDamageSystemType = 0;
			}
		}
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
	
	
	////////////////
	///// Misc /////
	////////////////
	
	
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
	
	static string GetYesNo(bool value)
	{
		if(value) return "#STR_MOE_Yes";
		
		return "#STR_MOE_No";
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