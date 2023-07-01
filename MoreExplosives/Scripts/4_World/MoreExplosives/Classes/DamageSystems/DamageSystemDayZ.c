class MOE_DamageSystemDayZ : MOE_DamageSystemBase 
{
	override bool DamageTargetDirectly()
	{
		return false;
	}
	
	//override void OnDamageDealingEnded(MOE_HitInfo hitInfo)
	//{
	//	Log_MOE(m_DamageLogStrings, MOE_ELogTypes.RAID);		
	//	delete m_DamageLogStrings;
	//}
}