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
	
	//protected ref map<string, ref MOE_ConfigDataExplosive> 		m_CachedExplosiveData;		
	//protected ref map<string, ref MOE_ConfigDataSignalSource> 	m_CachedSignalSourceData;	
	//protected ref map<string, ref MOE_ConfigDataTriggerBase> 	m_CachedTriggerData;
	
	protected ref map<string, ref MOE_ConfigDataBase> m_CachedConfigData;

	void CreateDamageSystem()
	{
		m_DamageSystem = new MOE_DamageSystemBase();
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

	void InitBaseBuildingObject(BaseBuildingBase baseBuildingBase)
	{
		
	}

	void OnConstructionPartChanged(BaseBuildingBase baseBuildingBase, ConstructionPart part)
	{

	}

	bool HandleExplosionHit(MOE_ExplosionObject explosionObject, EntityAI target, int component, string dmgZone, string ammo)
	{
		MOE_ExplosiveBase explosive;
		if(!CastTo(explosive, explosionObject.GetSourceExplosive()))
		{
			Log_MOE(string.Format("MoreExplosives::HandleExplosionHitDamage -> MOE_ExplosionObject does not have a valid source explosive (%1)", explosionObject.GetSourceExplosive()), MOE_ELogTypes.ERROR);
			return true;
		}

		// Damage to placement target will be handled directly, 
		// do not deal any damage here to prevent dealing damage twice
		if(CanDamageTargetDirectly() && (target == explosive.GetPlacementTarget()))
		{
			return true;
		}

		//return !m_DamageSystem.CanDealDamage(target, component, dmgZone, explosive, explosionObject, ammo) || m_DamageSystem.DealDamage(target, component, dmgZone, explosive, explosionObject, ammo);
		return HandleDirectDamage(explosive, explosionObject, explosionObject.GetPosition(), target, component, dmgZone, ammo);
	}

	bool HandlePlacementTargetDamage(MOE_ExplosiveBase explosive, MOE_ExplosionObject explosionObject, vector explosionPosition, Object placementTarget, int component, string dmgZone, string ammo) 
	{
		return !CanDamageTargetDirectly() || HandleDirectDamage(explosive, explosionObject, explosionObject.GetPosition(), placementTarget, component, dmgZone, ammo);
	}

	//return true -> deal custom damage only or no damage at all
	//return false -> deal vanilla damage 
	bool HandleDirectDamage(MOE_ExplosiveBase explosive, MOE_ExplosionObject explosionObject, vector explosionPosition, Object target, int component, string dmgZone, string ammo)
	{
		return !m_DamageSystem.CanDealDamage(explosive, explosionObject, explosionPosition, target, component, dmgZone, ammo) || m_DamageSystem.DealDamage(explosive, explosionObject, explosionPosition, target, component, dmgZone, ammo);
	}


	bool CanBePlacedOn(MOE_ExplosiveBase explosive, Object target, int selection, vector position)
	{
		return true;
	}

	bool IsExplosiveCompatible(MOE_ExplosiveBase explosive, Object target, int component, vector position)
	{
		return true;
	}

	bool IsDestructionSystemDayZ()
	{
		return false;
	}

	bool IsDestructionSystemMOE()
	{
		return false;
	}

	bool IsDestructionSystemBC()
	{
		return false;
	}


	////////////////////////////	
	/// Target/Hit detection ///
	////////////////////////////

	//"Hits" get caught when OnDamageCalculated event gets called on target
	bool UseExplosionForHitDetection()
	{
		return true;
	}

	//Target will be damaged directly when main explosion of explosion behaviour goes off
	bool CanDamageTargetDirectly()
	{
		return false;
	}
}
