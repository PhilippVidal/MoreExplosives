class MOE_DamageSystemBase 
{
	bool CanDealDamage(EntityAI target, int component, string dmgZone, MOE_ExplosiveBase explosive, MOE_ExplosionObject explosiveObject, string ammo)
	{
		if(!target)
		{
			return false;
		}
		
		if(explosive.CanOnlyDamagePlacementTarget() && (target != explosive.GetPlacementTarget()))
		{
			return false;
		}
		
		if(GetMOE().IsRaidSchedulingEnabled() && !GetMOE().IsInRaidSchedule())
		{
			return false;
		}
		
		BaseBuildingBase baseBuildingBase;
		if( (GetMOE().IsDoorRaidOnlyEnabled() || explosive.CanOnlyRaidDoors()) && (!CastTo(baseBuildingBase) || baseBuildingBase.HasGate_MOE()) )
		{
			return false;		
		}
		
		return true;
	}
	
	
	//True = custom damage dealt, don't deal vanilla damage 
	//False = deal vanilla damage (mainly used for MOE_DamageSystemDayZ)
	bool DealDamage(EntityAI target, int component, string dmgZone, MOE_ExplosiveBase explosive, MOE_ExplosionObject explosiveObject, string ammo)
	{
		return false;
	}
}