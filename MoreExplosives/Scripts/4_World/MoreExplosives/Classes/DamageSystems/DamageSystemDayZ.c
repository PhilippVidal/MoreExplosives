class MOE_DamageSystemDayZ : MOE_DamageSystemBase
{
	override bool CanDealDamage(MOE_ExplosiveBase explosive, MOE_ExplosionObject explosiveObject, vector explosionPosition, Object target, int component, string dmgZone, string ammo)
	{
		if(!target)
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
		
		BaseBuildingBase baseBuildingBase;	
		if( (GetMOESettings().IsDoorRaidOnlyEnabled || explosive.CanOnlyRaidDoors()) && (!CastTo(baseBuildingBase, target) || !baseBuildingBase.HasGate_MOE()) )
		{
			return false;		
		}
		
		return super.CanDealDamage(explosive, explosiveObject, explosionPosition, target, component, dmgZone, ammo);
	}
}