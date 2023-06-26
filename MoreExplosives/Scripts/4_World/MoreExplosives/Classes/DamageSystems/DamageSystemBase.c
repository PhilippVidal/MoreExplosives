class MOE_DamageSystemBase 
{
	bool CanDealDamage(MOE_ExplosiveBase explosive, MOE_ExplosionObject explosiveObject, vector explosionPosition, Object target, int component, string dmgZone, string ammo)
	{
		return target != null;
	}
	
	//True = custom damage handled, don't deal vanilla damage 
	//False = deal vanilla damage (mainly used for MOE_DamageSystemDayZ)
	bool DealDamage(MOE_ExplosiveBase explosive, MOE_ExplosionObject explosiveObject, vector explosionPosition, Object target, int component, string dmgZone, string ammo)
	{
		return false;
	}
}