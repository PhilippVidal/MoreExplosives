class MOE_DamageZoneData
{
	protected string m_Name;
	protected ref map<string, float> m_DamageMultipliers;
	protected ref array<string> m_RequiredDestroyedParts;
	
	void MOE_DamageZoneData(string name)
	{
		m_Name = name;
	}
	
	string GetName()
	{
		return m_Name;
	}
	
	array<string> GetRequiredDestroyedParts(string entityType)
	{
		if(!m_RequiredDestroyedParts)
		{		
			m_RequiredDestroyedParts = LoadRequiredDestroyedParts(entityType);
		}	
		
		return m_RequiredDestroyedParts;
	}
	
	float GetDamageMultiplier(string explosiveType, string entityType)
	{
		if(!m_DamageMultipliers) m_DamageMultipliers = new map<string, float>();
		
		float multiplier;
		if(m_DamageMultipliers.Find(explosiveType, multiplier))
		{
			return multiplier;
		}

		multiplier = LoadDamageMultiplierFromConfig(explosiveType, entityType);
		m_DamageMultipliers.Insert(explosiveType, multiplier); 
		return multiplier;
	}
	
	
	
	protected float LoadDamageMultiplierFromConfig(string explosiveType, string entityType)
	{
		string cfgPath = string.Format("%1 %2 DamageSystem DamageZones %3 MOE_Settings DamageMultipliers %4", CFG_VEHICLESPATH, entityType, m_Name, explosiveType);
		if(!GetGame().ConfigIsExisting(cfgPath))
		{
			return 1;
		}
		
		return GetGame().ConfigGetFloat(cfgPath);
	}
	
	protected array<string> LoadRequiredDestroyedParts(string entityType)
	{
		array<string> requiredDestroyedParts = new array<string>();
		
		string cfgPath = string.Format("%1 %2 DamageSystem DamageZones %3 MOE_Settings requiredDestroyedParts", CFG_VEHICLESPATH, entityType, m_Name);	
		if(GetGame().ConfigIsExisting(cfgPath))
		{
			GetGame().ConfigGetTextArray(cfgPath, requiredDestroyedParts);
		}
		
		return requiredDestroyedParts;
	}
}