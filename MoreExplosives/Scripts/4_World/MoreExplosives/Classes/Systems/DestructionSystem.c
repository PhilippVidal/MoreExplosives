class MOE_DestructionSystem
{
    protected ref MOE_DamageSystemBase m_DamageSystem;

    void MOE_DestructionSystem()
    {
        Init();
    }

    void Init()
    {
        CreateDamageSystem();
    }

    bool IsPlacementAllowed(MOE_ExplosiveBase explosive, Object target, int component, vector position)
	{
		return !GetMOESettings().IsRaidSchedulingEnabled || GetMOE().GetRaidScheduling().IsInSchedule();
	}

    bool IsExplosiveCompatible(MOE_ExplosiveBase explosive, Object target, int component, vector position)
	{
		int mode = explosive.GetMountingMode();
		switch(mode)
		{
			case MOE_EExplosiveMountingModes.BASEBUILDINGONLY:
				return target && target.IsInherited(BaseBuildingBase);

			case MOE_EExplosiveMountingModes.SELECTEDONLY:
				return target && IsMountable(target.GetType(), explosive.GetType());

            case MOE_EExplosiveMountingModes.ANYTARGET:
                return target != null;
        }
		
		return true;
	}
	
	bool IsMountable(string targetType, string explosiveType)
	{
		return GetGame().ConfigGetInt(string.Format("%1 %2 MOE_Settings Mountable %3", CFG_VEHICLESPATH, targetType, explosiveType)) != 0;
	}
	
	void DealAreaDamage(MOE_ExplosiveBase explosive, MOE_ExplosionObject explosionObject, vector position, string ammo)
	{
		MOE_AmmoData ammoData = GetMOEConfig().GetAmmoData(ammo);
		array<Object> hitObjects = GetObjectsAtPosition(position, ammoData.MaxDamageRange);
		
		BaseBuildingBase baseBuildingBase;
		foreach(Object hitObject : hitObjects)
		{
			if(!hitObject.CanUseConstruction() || !CastTo(baseBuildingBase, hitObject))
			{
				continue;
			}
			
			HandleExplosionHit(explosive, explosionObject, baseBuildingBase, -1, "", ammo);
		}
	}
	
	array<Object> GetObjectsAtPosition(vector position, float radius)
	{
		array<Object> hitObjects = new array<Object>();
		array<CargoBase> proxyCargos = new array<CargoBase>();
		GetGame().GetObjectsAtPosition3D(position, radius, hitObjects, proxyCargos);
		
		return hitObjects;
	}

    bool HandleExplosionHit(MOE_ExplosiveBase explosive, MOE_ExplosionObject explosionObject, EntityAI target, int component, string dmgZone, string ammo)
	{
		if(GetMOESettings().IsDamagePlacementTargetDirectlyEnabled && (target == explosive.GetPlacementTarget()))
		{
            // Damage to placement target will be handled directly, 
		    // do not deal any damage here to prevent dealing damage twice
			return true;
		}

		//return !m_DamageSystem.CanDealDamage(target, component, dmgZone, explosive, explosionObject, ammo) || m_DamageSystem.DealDamage(target, component, dmgZone, explosive, explosionObject, ammo);
		return HandleDirectDamage(explosive, explosionObject, explosionObject.GetPosition(), target, component, dmgZone, ammo);
	}

    bool HandleDirectDamage(MOE_ExplosiveBase explosive, MOE_ExplosionObject explosionObject, vector explosionPosition, Object target, int component, string dmgZone, string ammo)
    {
        return !m_DamageSystem.CanDealDamage(explosive, explosionObject, explosionPosition, target, component, dmgZone, ammo) || m_DamageSystem.DealDamage(explosive, explosionObject, explosionPosition, target, component, dmgZone, ammo);
    }

    void CreateDamageSystem()
    {
        int damageSystemType = GetSelectedDamageSystemType();
        switch(damageSystemType)
        {
            case MOE_EDamageSystemTypes.DAYZ:
                m_DamageSystem = new MOE_DamageSystemDayZ();
            break;

            case MOE_EDamageSystemTypes.MOE:
                m_DamageSystem = new MOE_DamageSystemMOE();
            break;

            default: 
                m_DamageSystem = new MOE_DamageSystemBase();
            break;
        }
    }

    MOE_DamageSystemBase GetDamageSystem()
    {
        return m_DamageSystem;
    }

    int GetSelectedDamageSystemType()
	{
		return GetGame().ConfigGetInt(MoreExplosives.CFG_MOE + "selectedDamageSystem");
	}
	
	bool UseExplosionForHitDetection()
	{
		return true;
	}
}