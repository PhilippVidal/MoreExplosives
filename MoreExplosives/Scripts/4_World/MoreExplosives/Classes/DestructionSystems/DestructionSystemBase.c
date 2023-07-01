class MOE_DestructionSystemBase 
{
	protected ref MOE_DamageSystemBase m_DamageSystem;
	
	void MOE_DestructionSystemBase()
	{
		Init();
	}
	
	void Init()
	{
		CreateDamageSystem();
	}
	
	protected void CreateDamageSystem()
    {
		m_DamageSystem = new MOE_DamageSystemBase();
    }
	
	MOE_DamageSystemBase GetDamageSystem()
    {
        return m_DamageSystem;
    }
	
	//bool IsPlacementAllowed(MOE_HitInfo hitInfo)
	//{
	//	return !GetMOESettings().IsRaidSchedulingEnabled || GetMOE().GetRaidScheduling().IsInSchedule();
	//}
	
	bool IsExplosiveCompatible(MOE_HitInfo hitInfo)
	{
		return true;
	}
	
	void DealAreaDamage(MOE_ExplosiveBase explosive, MOE_ExplosionObject explosionObject, string ammo)
	{
		
	}
	
	bool HandleExplosionHit(MOE_HitInfo hitInfo)
	{
		return HandleDirectDamage(hitInfo);
	}
			
    bool HandleDirectDamage(MOE_HitInfo hitInfo)
    {
        return !m_DamageSystem.CanDealDamage(hitInfo) || m_DamageSystem.DealDamage(hitInfo);
    }
}