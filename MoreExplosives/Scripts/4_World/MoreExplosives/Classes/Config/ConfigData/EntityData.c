class MOE_EntityData : MOE_ConfigDataBase
{
    string Type;
	ref map<string, float> DamageMultipliers;
	ref array<ref MOE_DamageZoneData> DamageZones;

	
    void MOE_EntityData(string type, array<string> dmgZones)
    {		
		//It's unintuitive but this constructor calls MOE_ConfigDataBase::MOE_ConfigDataBase(string type)
        SetupDamageZones(dmgZones);
    }

    override void Init(string type)
    {
        Type = type;
        DamageMultipliers = new map<string, float>();
    }
    
    float GetEntityMultiplier(string explosiveType)
	{
		//Multiplier for explosive already cached
		float multiplier;
		if(DamageMultipliers.Find(explosiveType, multiplier))
		{
			return multiplier;
		}
		
		//Multiplier not cached, get it from config and cache it
		multiplier = LoadEntityMultiplier(explosiveType);
		
		//Cache multiplier
		DamageMultipliers.Insert(explosiveType, multiplier);
		
		return multiplier;
	}

    protected float LoadEntityMultiplier(string explosiveType)
	{
		string cfgPath = string.Format("%1 %2 MOE_Settings DamageMultipliers %4", CFG_VEHICLESPATH, Type, explosiveType);
		if(!GetGame().ConfigIsExisting(cfgPath))
		{
			return 1;
		}
		
		return GetGame().ConfigGetFloat(cfgPath);
	}

    protected void SetupDamageZones(array<string> dmgZones)
    {	
        if(!dmgZones)
		{
			
			SetDamageZones(LoadDamageZonesFromConfig());
			return;
			//dmgZones = LoadDamageZonesFromConfig();
			//Print("DamageZones valid after load? " + (dmgZones != null));
		}	

        SetDamageZones(dmgZones);
    }

    protected void SetDamageZones(array<string> dmgZones)
	{
		array<string> zones = FilterExceptedDamageZones(dmgZones);
		zones = GetSortedDamageZones(zones); 

		if(DamageZones)
		{
			delete DamageZones; 
		} 
		
		DamageZones = new array<ref MOE_DamageZoneData>();
		
		foreach(string zone : zones)
		{
			DamageZones.Insert(new MOE_DamageZoneData(zone));
		}
	}

    protected array<string> LoadDamageZonesFromConfig()
	{		
		array<string> dmgZones = new array<string>();
		string basePath = string.Format("%1 %2 DamageSystem DamageZones", CFG_VEHICLESPATH, Type);	
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
		
        return dmgZones;
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
			string path = string.Format("%1 %2 DamageSystem DamageZones %3 ArmorType FragGrenade Health damage", CFG_VEHICLESPATH, Type, dmgZones[i]);
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
		string path = string.Format("%1 %2 MOE_Settings damageZonePriorities", CFG_VEHICLESPATH, Type);
		if(GetGame().ConfigIsExisting(path))
		{
			GetGame().ConfigGetTextArray(path, priorities);
		}

		return priorities;
	}
}