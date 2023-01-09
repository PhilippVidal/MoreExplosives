class MOE_EntityData
{
	protected string m_Type;
	protected ref map<string, float> m_DamageMultipliers;
	protected ref array<ref MOE_DamageZoneData> m_DamageZones;
	
	void MOE_EntityData(string type, array<string> dmgZones)
	{
		Init(type);	
		
		if(dmgZones)
		{
			SetDamageZones(dmgZones);
		}
		else 
		{
			LoadDamageZonesFromConfig();
		}	
	}
		
	string GetEntityType()
	{
		return m_Type;
	}
	
	int GetDamageZoneCount()
	{
		return m_DamageZones.Count();
	}
	
	float GetEntityMultiplier(string explosiveType)
	{
		//Multiplier for explosive already cached
		float multiplier;
		if(m_DamageMultipliers.Find(explosiveType, multiplier))
		{
			return multiplier;
		}
		
		//Multiplier not cached, get it from config and cache it
		multiplier = LoadEntityMultiplier(explosiveType);
		
		//Cache multiplier
		m_DamageMultipliers.Insert(explosiveType, multiplier);
		
		return multiplier;
	}
		
	MOE_DamageZoneData GetDamageZone(int idx)
	{
		return m_DamageZones[idx];
	}
	
	array<ref MOE_DamageZoneData> GetDamageZones()
	{
		return m_DamageZones;
	}
	
	protected void Init(string type)
	{
		m_Type = type;		
		m_DamageMultipliers = new map<string, float>();
	}
	
	protected float LoadEntityMultiplier(string explosiveType)
	{
		string cfgPath = string.Format("%1 %2 MOE_Settings DamageMultipliers %4", CFG_VEHICLESPATH, m_Type, explosiveType);
		if(!GetGame().ConfigIsExisting(cfgPath))
		{
			return 1;
		}
		
		return GetGame().ConfigGetFloat(cfgPath);
	}
	
	protected void LoadDamageZonesFromConfig()
	{		
		array<string> dmgZones = new array<string>();
		string basePath = string.Format("%1 %2 DamageSystem DamageZones", CFG_VEHICLESPATH, m_Type);	
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
		
		SetDamageZones(dmgZones);
	}
	
	protected void SetDamageZones(array<string> dmgZones)
	{
		array<string> zones = FilterExceptedDamageZones(dmgZones);
		zones = GetSortedDamageZones(zones); 

		if(m_DamageZones)
		{
			delete m_DamageZones; 
		} 
		
		m_DamageZones = new array<ref MOE_DamageZoneData>();
		
		foreach(string zone : zones)
		{
			m_DamageZones.Insert(new MOE_DamageZoneData(zone));
		}
	}
	
	//Some damage zones should not be damaged at all (e.g., the Base or Gate parts of a Fence)
	//This functions tries to automate identifying these exceptions, though it is not a perfect solution 
	//For example: 
	//A damage zone could also have a FragGrenade Armor value of 0 when it's only supposed to be raided 
	//with Projectiles, Melee and MOE Explosives, but not with vanilla DayZ explosives 
	//This is a very niche use-case though and can be fixed for particular items by simply overriding this function 
	protected array<string> FilterExceptedDamageZones(array<string> dmgZones)
	{
		for(int i = dmgZones.Count() - 1; i > -1; i--)
		{
			string path = string.Format("%1 %2 DamageSystem DamageZones %3 ArmorType FragGrenade Health damage", CFG_VEHICLESPATH, m_Type, dmgZones[i]);
			if(GetGame().ConfigIsExisting(path) && ( GetGame().ConfigGetFloat(path) == 0 ))
			{
				dmgZones.Remove(i);
			}
		}
		
		return dmgZones;
	}
	
	protected array<string> GetSortedDamageZones(array<string> dmgZones)
	{
		array<string> sortedDmgZones = GetDamageZonePriorities();

		if(sortedDmgZones.Count() > 0)
		{
			foreach(string zone : dmgZones)
			{
				if(sortedDmgZones.Find(zone) == -1)
				{
					sortedDmgZones.Insert(zone);
				}
			}
		}
		else 
		{
			sortedDmgZones = dmgZones;
		}
	
		return sortedDmgZones;
	}
	
	protected array<string> GetDamageZonePriorities() 
	{		
		array<string> priorities = new array<string>();	
		string path = string.Format("%1 %2 MOE_Settings damageZonePriorities", CFG_VEHICLESPATH, m_Type);
		if(GetGame().ConfigIsExisting(path))
		{
			GetGame().ConfigGetTextArray(path, priorities);
		}

		return priorities;
	}
}