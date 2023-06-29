class MOE_DamageSystemBase 
{
	bool CanDealDamage(MOE_ExplosiveBase explosive, MOE_ExplosionObject explosiveObject, vector explosionPosition, Object target, int component, string dmgZone, string ammo)
	{
		BaseBuildingBase baseBuildingBase;
		if(!CastTo(baseBuildingBase,target))
		{
			return false;
		}
		
		if(!super.CanDealDamage(explosive, explosiveObject, explosionPosition, target, component, dmgZone, ammo))
		{
			return false;
		}
		
		if(explosive.CanOnlyDamagePlacementTarget() && (target != explosive.GetPlacementTarget()))
		{
			return false;
		}
		
		if(GetMOESettings().IsRaidSchedulingEnabled && !GetMOE().GetRaidScheduling().IsInSchedule())
		{
			return false;
		}
		
		if( (GetMOESettings().IsDoorRaidOnlyEnabled || explosive.CanOnlyRaidDoors()) && !baseBuildingBase.HasGate_MOE())
		{
			return false;		
		}
		
		return true;
	}
	
	//True = custom damage handled, don't deal vanilla damage 
	//False = deal vanilla damage (mainly used for MOE_DamageSystemDayZ)
	bool DealDamage(MOE_ExplosiveBase explosive, MOE_ExplosionObject explosiveObject, vector explosionPosition, Object target, int component, string dmgZone, string ammo)
	{
		return false;
	}
}