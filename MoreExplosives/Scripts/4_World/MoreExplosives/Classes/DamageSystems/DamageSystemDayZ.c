class MOE_DamageSystemDayZ : MOE_DamageSystemBase
{
	override bool CanDealDamage(EntityAI target, int component, string dmgZone, MOE_ExplosiveBase explosive, MOE_ExplosionObject explosiveObject, string ammo)
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
		if( (GetMOE().IsDoorRaidOnlyEnabled() || explosive.CanOnlyRaidDoors()) && (!CastTo(baseBuildingBase, target) || !baseBuildingBase.HasGate_MOE()) )
		{
			return false;		
		}
		
		return super.CanDealDamage(target, component, dmgZone, explosive, explosiveObject, ammo);
	}
	
	override int GetMOEType()
	{
		return MOE_EDamageSystemTypes.DAYZ;
	}
}