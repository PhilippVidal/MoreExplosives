class MOE_DestructionSystem : MOE_DestructionSystemBase
{
    override bool IsExplosiveCompatible(MOE_HitInfo hitInfo)
	{		
		int mode = hitInfo.Explosive.GetMountingMode();		
		switch(mode)
		{
			case MOE_EExplosiveMountingModes.BASEBUILDINGONLY:
				return hitInfo.Target && CanMountOnComponent(hitInfo.Explosive, hitInfo.Target, hitInfo.HitComponent);
			
			case MOE_EExplosiveMountingModes.SELECTEDONLY:
				return hitInfo.Target && IsTargetMountable(hitInfo.Explosive, hitInfo.Target) && CanMountOnComponent(hitInfo.Explosive, hitInfo.Target, hitInfo.HitComponent);
		}
					
		return true;
	}
	
	override void DealAreaDamage(MOE_ExplosiveBase explosive, MOE_ExplosionObject explosionObject, string ammo)
	{
		MOE_AmmoData ammoData = GetMOEConfig().GetAmmoData(ammo);
		vector position = explosionObject.GetPosition();
		array<Object> hitObjects = GetObjectsAtPosition(position, ammoData.MaxDamageRange);
		
		BaseBuildingBase baseBuildingBase;
		foreach(Object hitObject : hitObjects)
		{		
			if(!hitObject.CanUseConstruction() || !CastTo(baseBuildingBase, hitObject))
			{
				continue;
			}
			
			MOE_HitInfo hitInfo = new MOE_HitInfo();
			hitInfo.Explosive = explosive;
			hitInfo.ExplosionObject = explosionObject;
			hitInfo.Target = baseBuildingBase;
			hitInfo.Ammo = ammo;
			hitInfo.HitPosition = position;
			hitInfo.HitComponent = -1;
			hitInfo.TargetZone = "";
			hitInfo.IsAreaHit = true;
			
			HandleExplosionHit(hitInfo);
		}
	}
	
	override bool HandleExplosionHit(MOE_HitInfo hitInfo)
	{
		if(hitInfo.IsAreaHit && m_DamageSystem.DamageTargetDirectly() && (hitInfo.Target == hitInfo.Explosive.GetPlacementTarget()))
		{
			return true;
		}
		
		return HandleDirectDamage(hitInfo);
	}
	
	override void CreateDamageSystem()
    {
        int damageSystemType = GetGame().ConfigGetInt(MoreExplosives.CFG_MOE + " " + "selectedDamageSystem");
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
	
	array<Object> GetObjectsAtPosition(vector position, float radius)
	{
		array<Object> hitObjects = new array<Object>();
		array<CargoBase> proxyCargos = new array<CargoBase>();
		GetGame().GetObjectsAtPosition3D(position, radius, hitObjects, proxyCargos);
		
		return hitObjects;
	}
	
	bool CanMountOnComponent(MOE_ExplosiveBase explosive, BaseBuildingBase target, int component)
	{
		ConstructionPart part;
		if(!target.GetPartFromComponent_MOE(component, part))
		{
			return false;
		}
		
		
		string path = string.Format("%1 %2 MOE_Settings MountableParts %3", CFG_VEHICLESPATH, target.GetType(), explosive.GetType());			
		return !GetGame().ConfigIsExisting(path) || (GetGame().ConfigGetInt(string.Format("%1 %2", path, part.GetPartName())) != 0);
	}
	
	bool IsTargetMountable(MOE_ExplosiveBase explosive, BaseBuildingBase target)
	{
		return GetGame().ConfigGetInt(string.Format("%1 %2 MOE_Settings MountableExplosives %3", CFG_VEHICLESPATH, target.GetType(), explosive.GetType())) != 0;
	}
}