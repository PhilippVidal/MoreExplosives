class MOE_DamageSystemBC : MOE_DamageSystemBase
{
	override bool CanDealDamage(EntityAI target, int component, string dmgZone, MOE_ExplosiveBase explosive, MOE_ExplosionObject explosiveObject, string ammo)
	{
		BaseBuildingBase baseBuildingBase;
		if(!super.CanDealDamage(target, component, dmgZone, explosive, explosiveObject, ammo) || !CastTo(baseBuildingBase, target))
		{
			return false;
		}

		MOE_ConfigBC config;
		MOE_DestroyableObjectData_Internal destroyableObject;
		if(!CastTo(config, GetMOE().GetConfig()) || !config.GetDestroyableObject(target.GetType(), destroyableObject))
		{
			return false;
		}

		
	}
	

	
	
	override bool DealDamage(EntityAI target, int component, string dmgZone, MOE_ExplosiveBase explosive, MOE_ExplosionObject explosiveObject, string ammo)
	{
		BaseBuildingBase baseBuilding = BaseBuildingBase.Cast(target);
		if(!baseBuilding)
		{
			//TODO: error message
			return true;
		}

		DealDamageBaseBuildingBase(baseBuilding, component, dmgZone, explosive, explosiveObject, ammo);
		return true;
	}
	
	//returns whether damage has been dealt or not 
	bool DealDamageBaseBuildingBase(BaseBuildingBase target, int component, string dmgZone, MOE_ExplosiveBase explosive, MOE_ExplosionObject explosiveObject, string ammo)
	{
		int currentHealth = target.GetHealth_MOE();
		int damageToDeal = 1; // explosive.GetDamage(); //TODO
		int newHealth = Math.Clamp(currentHealth - damageToDeal, 0, currentHealth);
		target.SetHealth_MOE(newHealth);	
		return true;
	}
	
	override int GetMOEType()
	{
		return MOE_EDamageSystemTypes.BC;
	}
}