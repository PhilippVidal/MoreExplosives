class MOE_DamageInfo
{
	float HealthBefore;
	float HealthAfter;
	float Damage;
	string Zone;
}

class MOE_DamageSystemBase 
{
	protected ref array<string> m_DamageLogStrings;
	
	bool CanDealDamage(MOE_HitInfo hitInfo)
	{
		if(!hitInfo.Target)
		{
			return false;
		}
		
		if(hitInfo.Explosive.CanOnlyDamagePlacementTarget() && (hitInfo.Target != hitInfo.Explosive.GetPlacementTarget()))
		{
			return false;
		}
		
		if(GetMOESettings().IsRaidSchedulingEnabled && !GetMOE().GetRaidScheduling().IsInSchedule())
		{
			return false;
		}
		
		if((GetMOESettings().IsDoorRaidOnlyEnabled || hitInfo.Explosive.CanOnlyRaidDoors()) && !hitInfo.Target.HasGate_MOE())
		{
			return false;		
		}
		
		return true;
	}
	
	//True = custom damage handled, don't deal vanilla damage 
	//False = deal vanilla damage (mainly used for MOE_DamageSystemDayZ)
	bool DealDamage(MOE_HitInfo hitInfo)
	{	
		OnDamageDealingStarted(hitInfo);
		//OnDamageDealt(hitInfo);
		OnDamageDealingEnded(hitInfo);
		return false;
	}
	
	bool DamageTargetDirectly()
	{
		return true;
	}
	
	void OnDamageDealingStarted(MOE_HitInfo hitInfo)
	{
		m_DamageLogStrings = new array<string>();
		
		string playerName, playerSteam64;
		hitInfo.Explosive.GetInteractingPlayer(playerName, playerSteam64);
			
		string logString = string.Format(
							"Object hit:\n------------ Player: %1 [%2], Target: %3 [%4], Source: %6 [%7]\n",
							playerName,
							playerSteam64,
							hitInfo.Target, 
							hitInfo.Target.GetPosition().ToString(), 
							hitInfo.WasFullyDestroyed, 
							hitInfo.Explosive, 
							hitInfo.Explosive.GetPosition().ToString());
		
		m_DamageLogStrings.Insert(logString);
	}
	
	void OnDamageDealt(MOE_DamageInfo damageInfo)
	{	
		string logString = string.Format("------------------ Damage dealt to %1: %2, Health Before: %3, Health After: %4\n",
							damageInfo.Zone,
							damageInfo.Damage,
							damageInfo.HealthBefore,
							damageInfo.HealthAfter);
		
		m_DamageLogStrings.Insert(logString);
	}
	
	void OnDamageDealingEnded(MOE_HitInfo hitInfo)
	{
		//string logString = string.Format("---%1 wasFullyDestroyed = %2", hitInfo.Target, hitInfo.WasFullyDestroyed);
		//m_DamageLogStrings.Insert(logString);
		
		Log_MOE(m_DamageLogStrings, MOE_ELogTypes.RAID);
		
		delete m_DamageLogStrings;
	}
}