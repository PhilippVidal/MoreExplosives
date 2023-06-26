//Classes that hold relevant ("static") config information for each object type -> prevents duplicate data on each object 
class MOE_ConfigDataBase
{
	void MOE_ConfigDataBase(string type)
	{		
		Init(type);
		OnAfterInit(type);
	}

	//Loads values from config on data creation based on object type
	protected void Init(string type)
	{	
		
	}
	
	protected void OnAfterInit(string type)
	{
#ifdef MOE_DEBUG_CONFIG
		PrintLoadedData(type);
#endif	
	}
	
	//where to find config values -> e.g., CfgVehicles MOE_ExplosiveHomemade MOE_Settings VARIABLE
	string GetBasePath(string type)
	{
		return string.Format("%1 %2 MOE_Settings ", CFG_VEHICLESPATH, type);	
	}
	
#ifdef MOE_DEBUG_CONFIG	
	void PrintLoadedData(string type)
	{
		Print(string.Format("MOE_DEBUG	Loaded Config Data for %1:", type));
	}
#endif
}