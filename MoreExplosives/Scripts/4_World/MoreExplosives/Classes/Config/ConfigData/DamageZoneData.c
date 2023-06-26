class MOE_DamageZoneData : MOE_ConfigDataBase
{
	string Name;
	ref map<string, float> DamageMultipliers;
	ref array<string> RequiredDestroyedParts;

	override void Init(string type)
	{
		Name = type;
		DamageMultipliers = new map<string, float>();
	}
	
	array<string> GetRequiredDestroyedParts(string entityType)
	{
		if(!RequiredDestroyedParts)
		{		
			RequiredDestroyedParts = LoadRequiredDestroyedParts(entityType);
		}	
		
		return RequiredDestroyedParts;
	}
	
	float GetDamageMultiplier(string explosiveType, string entityType)
	{
		float multiplier;
		if(DamageMultipliers.Find(explosiveType, multiplier))
		{
			return multiplier;
		}

		multiplier = LoadDamageMultiplierFromConfig(explosiveType, entityType);
		DamageMultipliers.Insert(explosiveType, multiplier); 
		return multiplier;
	}
	
	
	
	protected float LoadDamageMultiplierFromConfig(string explosiveType, string entityType)
	{
		string cfgPath = string.Format("%1 %2 DamageSystem DamageZones %3 MOE_Settings DamageMultipliers %4", CFG_VEHICLESPATH, entityType, Name, explosiveType);
		if(!GetGame().ConfigIsExisting(cfgPath))
		{
			return 1;
		}
		
		return GetGame().ConfigGetFloat(cfgPath);
	}
	
	protected array<string> LoadRequiredDestroyedParts(string entityType)
	{
		array<string> requiredDestroyedParts = new array<string>();
		
		string cfgPath = string.Format("%1 %2 DamageSystem DamageZones %3 MOE_Settings requiredDestroyedParts", CFG_VEHICLESPATH, entityType, Name);	
		if(GetGame().ConfigIsExisting(cfgPath))
		{
			GetGame().ConfigGetTextArray(cfgPath, requiredDestroyedParts);
		}
		
		return requiredDestroyedParts;
	}
}