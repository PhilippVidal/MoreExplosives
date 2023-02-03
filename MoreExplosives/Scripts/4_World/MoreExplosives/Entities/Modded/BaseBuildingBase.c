#ifdef SERVER
modded class BaseBuildingBase
{	
	override bool EEOnDamageCalculated(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{		
		if(!super.EEOnDamageCalculated(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef))
		{
			return false;
		}	
		
		return CanDealDamage_MOE(source, component, dmgZone, ammo) && !DealCustomDamage_MOE(source, component, dmgZone, ammo, modelPos);
	}

	//True = custom damage dealt, don't deal vanilla damage 
	//False = deal vanilla damage
	bool DealCustomDamage_MOE(EntityAI source, int component, string dmgZone, string ammo, vector modelPos)
	{
		MOE_ExplosionObject explosiveObject;
		if(!CastTo(explosiveObject, source))
		{
			return false;
		}
		
		MOE_ExplosiveBase explosive;
		if(!CastTo(explosive, explosiveObject.GetSourceExplosive()))
		{
			Log_MOE(string.Format("%1::DealCustomDamage_MOE -> MOE_ExplosionObject does not have a valid source explosive (%2)", this, explosiveObject.GetSourceExplosive()), MOE_ELogTypes.ERROR);
			return false;
		}
		
		if(!GetMOE().IsCustomDamageEnabled())
		{
			return false;
		}	
		
		EntityAI lock = GetLock_MOE();
		GetMOE().DealDamageToEntity(ammo, explosive, this, explosiveObject.GetPosition(), modelPos);
		
		if(lock && GetMOE().IsDeleteLocksEnabled() && !GetLock_MOE())
		{
			lock.Delete();
		}
		
		bool wasFullyDestroyed = IsConstructionDestroyed_MOE();	
		
		
		string logStr = string.Format("Object = %1 [%2, wasFullyDestroyed: %3], Source = %4", this, GetPosition().ToString(), wasFullyDestroyed, source);	
		string playerName, playerSteam64;
		if(explosive.GetInteractingPlayer(playerName, playerSteam64))
		{
			logStr += string.Format(", Player: %1 [%2]", playerName, playerSteam64);
		}
		
		Log_MOE(logStr, MOE_ELogTypes.RAID);
		
		if(wasFullyDestroyed && GetMOE().IsDestroyBaseAfterDestructionEnabled())
		{
			Delete();
		}
		
		return true;
	}
	
	bool CanDealDamage_MOE(EntityAI source, int component, string dmgZone, string ammo)
	{
		MOE_ExplosionObject explosiveObject;
		if(!CastTo(explosiveObject, source))
		{
			return !GetMOE().IsMOERaidingOnlyEnabled();
		}
		
		MOE_ExplosiveBase explosive;
		if(!CastTo(explosive, explosiveObject.GetSourceExplosive()))
		{
			Log_MOE(string.Format("%1::CanDealDamage_MOE -> MOE_ExplosionObject does not have a valid source explosive (%2)", this, explosiveObject.GetSourceExplosive()), MOE_ELogTypes.ERROR);
			return true;
		}
		
		if(explosive.CanOnlyDamagePlacementTarget() && (this != explosive.GetPlacementTarget()))
		{
			return false;
		}
		
		if((GetMOE().IsDoorRaidOnlyEnabled() || explosive.CanOnlyRaidDoors()) && !HasGate_MOE())
		{
			return false;		
		}
		
		if(GetMOE().IsRaidSchedulingEnabled() && !GetMOE().IsInRaidSchedule())
		{
			return false;
		}
		
		return true;
	}
	
	bool HasGate_MOE()
	{
		Construction constr = GetConstruction();
		if(constr)
		{
			ConstructionPart gate = constr.GetGateConstructionPart();	
			return gate && gate.IsBuilt();
		}
		
		return false;
	}
	
	bool IsConstructionDestroyed_MOE()
	{
		Construction constr = GetConstruction();
		if(constr)
		{
			string key;
			ConstructionPart part;
			map<string, ref ConstructionPart> constrParts = constr.GetConstructionParts();
			for( int i = 0; i < constrParts.Count(); ++i )
			{
				key = constrParts.GetKey(i);
				part = constrParts.Get(key);
				if(part.IsBuilt() && !part.IsBase())
				{
					return false;
				}
			}
		}
		
		return true;
	}
	
	EntityAI GetLock_MOE()
	{
		return FindAttachmentBySlotName("Att_CombinationLock");
	}
}
#endif

