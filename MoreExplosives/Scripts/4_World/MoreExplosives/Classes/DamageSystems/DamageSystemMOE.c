class MOE_DamageSystemMOE : MOE_DamageSystemBase
{
	override bool CanDealDamage(EntityAI target, int component, string dmgZone, MOE_ExplosiveBase explosive, MOE_ExplosionObject explosiveObject, string ammo)
	{
		return target && target.IsInherited(BaseBuildingBase) && super.CanDealDamage(target, component, dmgZone, explosive, explosiveObject, ammo);
	}
	
	override bool DealDamage(EntityAI target, int component, string dmgZone, MOE_ExplosiveBase explosive, MOE_ExplosionObject explosiveObject, string ammo)
	{
		//No need to check if null or not -> already checked in CanDealDamage
		BaseBuildingBase baseBuildingBase = BaseBuildingBase.Cast(target);
		
		EntityAI lock = baseBuildingBase.GetLock_MOE();
		
		GetMOE().DealDamageToEntity(ammo, explosive, target, explosiveObject.GetPosition(), modelPos);
		
		//Did the target have a lock before damage was dealt that is now not attached anymore?
		if(lock && GetMOE().IsDeleteLocksEnabled() && !baseBuildingBase.GetLock_MOE())
		{
			lock.Delete();
		}
		
		bool wasFullyDestroyed = baseBuildingBase.IsConstructionDestroyed_MOE();
		
		string logStr = string.Format("Object = %1 [%2, wasFullyDestroyed: %3], Source = %4", this, target.GetPosition().ToString(), wasFullyDestroyed, explosive);	
		string playerName, playerSteam64;
		if(explosive.GetInteractingPlayer(playerName, playerSteam64))
		{
			logStr += string.Format(", Player: %1 [%2]", playerName, playerSteam64);
		}
		
		Log_MOE(logStr, MOE_ELogTypes.RAID);
		
		if(wasFullyDestroyed && GetMOE().IsDestroyBaseAfterDestructionEnabled())
		{
			target.Delete();
		}
		
		return true;
	}
}