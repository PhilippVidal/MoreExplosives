class MOE_GeneralSettings
{
	bool 	IsMOERaidingOnlyEnabled;
	bool 	IsRaidSchedulingEnabled;
	bool 	IsDoorRaidOnlyEnabled;
	bool 	IsDeleteLocksEnabled;
	bool 	IsDestroyBaseAfterDestructionEnabled;	
}

class MOE_DestructionSystemBase
{
	protected ref MOE_ConfigBase m_Config;
	protected ref MOE_DamageSystemBase m_DamageSystem;
	
	protected ref map<string, ref MOE_ConfigDataExplosive> 		m_CachedExplosiveData;		
	protected ref map<string, ref MOE_ConfigDataSignalSource> 	m_CachedSignalSourceData;	
	protected ref map<string, ref MOE_ConfigDataTriggerBase> 	m_CachedTriggerData;
	
	void CreateDamageSystem()
	{
		m_DamageSystem = null;
	}
	
	MOE_DamageSystemBase GetDamageSystem()
	{
		return m_DamageSystem;
	}
	
	void CreateConfig()
	{
		if(m_Config)
		{
			delete m_Config;
		}
		
		m_Config = new MOE_ConfigBase();
	}
	
	MOE_ConfigBase GetConfig()
	{
		return m_Config;
	}
}

class MOE_DestructionSystemDayZ : MOE_DestructionSystemBase
{
	override void CreateDamageSystem()
	{
		m_DamageSystem = new MOE_DamageSystemDayz();
	}
}

class MOE_DestructionSystemMOE : MOE_DestructionSystemBase
{
	protected ref map<string, ref MOE_EntityData> 	m_CachedEntityData;
	protected ref map<string, ref MOE_AmmoData> 	m_CachedAmmoData;
	
	
	override void CreateDamageSystem()
	{
		m_DamageSystem = new MOE_DamageSystemMOE();
	}
	
	
	
	override void CreateConfig()
	{
		if(m_Config)
		{
			delete m_Config;
		}
		
		m_Config = new MOE_ConfigMOE();
	}
}

class MOE_DestructionSystemBC : MOE_DestructionSystemBase
{
	override void CreateDamageSystem()
	{
		m_DamageSystem = new MOE_DamageSystemBC();
	}
	
	override void CreateConfig()
	{
		if(m_Config)
		{
			delete m_Config;
		}
		
		m_Config = new MOE_ConfigBC();
	}
}