class MOE_DamageSystemMOE : MOE_DamageSystemDayZ
{
	override bool CanDealDamage(EntityAI target, int component, string dmgZone, MOE_ExplosiveBase explosive, MOE_ExplosionObject explosiveObject, string ammo, vector position)
	{
		return target && target.IsInherited(BaseBuildingBase) && super.CanDealDamage(target, component, dmgZone, explosive, explosiveObject, ammo);
	}
	
	override bool DealDamage(EntityAI target, int component, string dmgZone, MOE_ExplosiveBase explosive, MOE_ExplosionObject explosiveObject, string ammo, vector position)
	{
		//No need to check if null or not -> already checked in CanDealDamage
		BaseBuildingBase baseBuildingBase = BaseBuildingBase.Cast(target);
		
		EntityAI lock = baseBuildingBase.GetLock_MOE();
		
		GetMOE().DealDamageToEntity(ammo, explosive, target, explosiveObject.GetPosition());
		
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
	
	override int GetMOEType()
	{
		return MOE_EDamageSystemTypes.MOE;
	}
	
	
	bool DealDamageToEntity(string ammo, EntityAI source, EntityAI entity, vector explosionPosition)
	{		
		MOE_AmmoData ammoData;
		if(!HasAmmoDataCached(ammo, ammoData)) 
		{ 
			ammoData = CacheAmmoData(ammo); 
		}
		
		if(!ammoData)
		{
			return false;
		}
		
		float damage 			= ammoData.Damage;
		float maxDamageRange 	= ammoData.MaxDamageRange;
		float fullDamageRange	= ammoData.FullDamageRange;	
		
		if(damage <= 0)
		{
			return false;
		}

		string entityType = entity.GetType();
		MOE_EntityData entityData;
		if(!m_CachedEntityData.Find(entityType, entityData))
		{
			entityData = CacheEntityData(entityType, true, entity.GetEntityDamageZoneMap().GetKeyArray());
		}		
		
		string explosiveType = source.GetType();
		
		float entityMultiplier = entityData.GetEntityMultiplier(explosiveType);
		if(entityMultiplier == 0.0)
		{
			return false;
		}
		

		
		float distanceMultiplier, dmgZoneMultiplier, multiplier;
		MOE_DamageZoneData zoneData;
		string zoneName;
		bool wasDamageDealt = false;	
		int dmgZoneCount = entityData.GetDamageZoneCount();		
		
#ifdef MOE_DEBUG_DAMAGE
		Log_MOE(string.Format("[MOE Damage Debug]	Damage dealt to %1 by %2:", entityType, explosiveType), MOE_ELogTypes.DEBUG);
#endif
		
		for(int i = 0; i < dmgZoneCount; i++)
		{
			zoneData = entityData.GetDamageZone(i);
			if(!zoneData)
			{
				continue;
			}
			
			zoneName = zoneData.GetName();
			distanceMultiplier = GetDistanceMultiplier(entity, source, explosionPosition, maxDamageRange, fullDamageRange, zoneName);
			if(distanceMultiplier == 0.0)
			{
				continue;
			}
			
			if(!CanDealDamageToDmgZone(source, entity, zoneName))
			{
				continue;
			}
			
			dmgZoneMultiplier = zoneData.GetDamageMultiplier(explosiveType, entityType);
			if(dmgZoneMultiplier == 0.0)
			{
				continue;
			}
			
			if(entity.IsInherited(BaseBuildingBase) && !CanDealDamageBaseBuildingBase(source, BaseBuildingBase.Cast(entity), zoneData))
			{
				continue;
			}
			
			if(DealDamageToDamageZone(source, entity, zoneName, ammo, ammoData.Damage, entityMultiplier, dmgZoneMultiplier, distanceMultiplier))
			{
				wasDamageDealt = true;
			}		
		}
		
		return wasDamageDealt;
	}
	
	bool DealDamageToDamageZone(EntityAI source, EntityAI target, string dmgZone, string ammo, float damage, float entityMultiplier, float dmgZoneMultiplier, float distanceMultiplier)
	{
		//target.ProcessDirectDamage(damageType, source, dmgZone, ammo, modelPos, multiplier);
		float dealtDamage = CalculateDamage(source, target, dmgZone, ammo, damage, entityMultiplier, dmgZoneMultiplier, distanceMultiplier);
		
		if(dealtDamage <= 0)
		{
			return false;
		}
		
		float healthBefore = target.GetHealth(dmgZone, "");
		float newHealth = Math.Clamp(healthBefore - dealtDamage, 0, healthBefore);
		target.SetHealth(dmgZone, "", newHealth);
	
#ifdef MOE_DEBUG_DAMAGE
		string outStr = string.Format(
			"Damage Results for %1::%2 => [DM: %3, EnM: %4, DzM: %5, DealtDamage: %6] Before: %7, After: %8",
			target,
			dmgZone,
			distanceMultiplier,
			entityMultiplier,
			dmgZoneMultiplier,
			dealtDamage,
			healthBefore,
			newHealth);
			
		Print("" + outStr);
#endif
		
		return true;
	}
	
	float CalculateDamage(EntityAI source, EntityAI target, string dmgZone, string ammo, float damage, float entityMultiplier, float dmgZoneMultiplier, float distanceMultiplier)
	{
		return damage * entityMultiplier * dmgZoneMultiplier * distanceMultiplier;	
	}
	
	//Check if damage can be dealt to specific damage zone 
	bool CanDealDamageToDmgZone(EntityAI source, EntityAI entity, string dmgZone)
	{
		return true; 
	}
	
	//Check if damage can be dealt based on list of parts that must be destroyed beforehand
	bool CanDealDamageBaseBuildingBase(EntityAI source, BaseBuildingBase entity, MOE_DamageZoneData zoneData)
	{		
		Construction constr = entity.GetConstruction();
		if(!constr)
		{
			return true;
		}			
		
		array<string> requiredDestroyedParts = zoneData.GetRequiredDestroyedParts(entity.GetType());
		if(!requiredDestroyedParts || requiredDestroyedParts.Count() == 0)
		{		
			return true;
		}


		
		//If any of the parts that must be destroyed are build, then damage can not be dealt 
		foreach(string part : requiredDestroyedParts)
		{
			if(constr.IsPartConstructed(part))
			{
				return false;
			}
		}
		
		return true;
	}
	
	float GetDamageMultiplierEntity(EntityAI entity, string sourceType)
	{		
		string path = string.Format("%1 %2 MOE_Settings DamageMultipliers %3", GetConfigBasePath(entity), entity.GetType(), sourceType);
		if(!GetGame().ConfigIsExisting(path)) 
		{
			return 1.0;
		}
		
		return GetGame().ConfigGetFloat(path);
	}

	float GetDamageMultiplierDamageZone(EntityAI entity, string dmgZone, string sourceType)
	{
		if(dmgZone == "")
		{
			return 1.0;
		}
		
		string path = string.Format("%1 %2 DamageSystem DamageZones %3 MOE_Settings DamageMultipliers %4", GetConfigBasePath(entity), entity.GetType(), dmgZone, sourceType);
		if(!GetGame().ConfigIsExisting(path))
		{
			return 1.0;
		}
		
		return GetGame().ConfigGetFloat(path);
	}
	
	float GetDistanceMultiplier(EntityAI entity, EntityAI source, vector explosionPosition, float maxRange, float fullDamageRange, string zoneName = "")
	{
		vector targetPos; 
		if(zoneName != "")
		{
			targetPos = entity.GetDamageZonePos(zoneName);
		}
		else 
		{
			targetPos = entity.GetPosition();
		}
		
		
		//float range = vector.Distance(explosionPosition, targetPos);		
		//if(range <= fullDamageRange || maxRange <= fullDamageRange)
		//{
		//	return 1.0;
		//}
		
		//maxRange -= fullDamageRange;
		//range -= fullDamageRange;
		//return Math.Clamp(1 - (range / maxRange), 0.0, 1.0);
		
		float range = vector.Distance(explosionPosition, targetPos);	
		float multiplier = InterpolateDistance(maxRange, fullDamageRange, range, entity, source);
		return Math.Clamp(multiplier, 0.0, 1.0);
	}	
	
	//Override this if you want to have different damage dropoffs (e.g., non-linear)
	protected float InterpolateDistance(float maxRange, float fullDamageRange, float range, EntityAI entity, EntityAI source)
	{
		return Math.InverseLerp(maxRange, fullDamageRange, range);
	}
}