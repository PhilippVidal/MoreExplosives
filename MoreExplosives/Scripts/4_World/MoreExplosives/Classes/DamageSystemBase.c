class MOE_DamageSystemBase 
{
	bool CanDealDamage(EntityAI target, int component, string dmgZone, MOE_ExplosiveBase explosive, MOE_ExplosionObject explosiveObject, string ammo)
	{
		return true;
	}
	
	
	//True = custom damage dealt, don't deal vanilla damage 
	//False = deal vanilla damage (mainly used for MOE_DamageSystemDayZ)
	bool DealDamage(EntityAI target, int component, string dmgZone, MOE_ExplosiveBase explosive, MOE_ExplosionObject explosiveObject, string ammo)
	{
		return false;
	}
	
	int GetMOEType()
	{
		return MOE_EDamageSystemTypes.NONE;
	}
}