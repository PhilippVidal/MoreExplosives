
static MoreExplosives GetMOE()
{
	return MoreExplosives.GetInstance();
}

class MoreExplosives
{	
	protected static ref MoreExplosives m_Instance;	
	protected MOE_DestructionSystemBase m_DestructionSystem;
	
	protected const string MOD_BASE_PATH 	= "$profile:/MoreExplosives/";
	protected const string LOG_BASE_PATH 	= "$profile:/MoreExplosives/Logs";
	protected const string CFG_MOE 			= "CfgMoreExplosives ";
	protected string m_LogName, m_LogPath;


	MOE_DestructionSystemBase GetDestructionSystem()
	{
		return m_DestructionSystem;
	}
	
	MOE_ConfigBase GetConfig()
	{
		return m_DestructionSystem.GetConfig();
	}
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
		logStr += "\n		selectedDamageSystem = " + m_SelectedDamageSystemType;
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
	
	bool IsUsingDestructionSystem(MOE_EDestructionSystemTypes type)
	{
		return m_DestructionSystem && (m_DestructionSystem.GetType() == type);
	}

	protected void InitializeDestructionSystem()
	{
		int destructionSystemType = 0; 
		
		string cfgPath = CFG_MOE + "selectedDestructionSystemType";
		if(GetGame().ConfigIsExisting(cfgPath))
		{
			destructionSystemType = GetGame().ConfigGetInt(cfgPath);
		}
		
		switch(destructionSystemType)
		{
			case MOE_EDestructionSystemTypes.MOE:
				destructionSystemType = new MOE_DestructionSystemMOE();
			break;
			
			case MOE_EDestructionSystemTypes.BC:
				destructionSystemType = new MOE_DestructionSystemBC();
			break;
			
			default:
				destructionSystemType = new MOE_DestructionSystemBase();
			break;
		}
	}
	
	MOE_DamageSystemBase GetDamageSystem()
	{
		return m_DamageSystem;
	}
	
	////////////////
	///// Misc /////
	////////////////
	
	static string GeneratePartKey(int mask1, int mask2, int mask3)
	{
		return string.Format(
					"%1 %2 %3", 
					HDSN_MiscFunctions.IntToHexString(mask1), 
					HDSN_MiscFunctions.IntToHexString(mask2), 
					HDSN_MiscFunctions.IntToHexString(mask3));
	}

	static void GetMasksFromPartKey(string partKey, out int mask1, out int mask2, out int mask3)
	{
		array<string> tokens = new array<string>();
		partKey.Split(" ", tokens);

		if(tokens.Count() != 3)
		{
			return;
		}

		mask1 = tokens[0].HexToInt();
		mask2 = tokens[1].HexToInt();
		mask3 = tokens[2].HexToInt();
	}	

	static void GetMasksFromPartList(array<string> buildParts, map<string, int> potentialParts, out int mask1, out int mask2, out int mask3)
	{
		int id;
		foreach(string part : buildParts)
		{
			if(potentialParts.Find(part, id))
			{
				MoreExplosives.AddToBuildMask(id, mask1, mask2, mask3);
			}
		}
	}

	static map<string, int> GetConstructionPartIDs(string type)
    {
        string constructionPath = string.Format("CfgVehicles %1 Construction", type);	
		if (!GetGame().ConfigIsExisting(constructionPath))
		{
			return null;
		}
		
		map<string, int> partList = new map<string, int>();
		
		string mainPart;
		int subConstrCount = GetGame().ConfigGetChildrenCount(constructionPath);		
		for (int i = 0; i < subConstrCount; ++i)
		{			
			GetGame().ConfigGetChildName(constructionPath, i, mainPart);
			string partPath = string.Format("%1 %2", constructionPath, mainPart);
			
			string part;
			int partCount = GetGame().ConfigGetChildrenCount(partPath);
			for (int j = 0; j < partCount; ++j)
			{
				GetGame().ConfigGetChildName(partPath, j, part);	
				id = GetGame().ConfigGetInt(string.Format("%1 %2 id", partPath, part));		
				partList.Insert(part, id);
			}
		}
		
		return partList;
    }

	static void AddToBuildMask(int partID, out int mask1, out int mask2, out int mask3)
	{
		if(partID < 1)
		{
			return;
		}
		
		if(partID < 33)
		{
			mask1 |= (1 << (partID - 1));
		}
		else if(partID < 65)
		{
			mask2 |= (1 << (partID - 33));
		}
		else if(partID < 97)
		{
			mask3 |= (1 << (partID - 65));
		}
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