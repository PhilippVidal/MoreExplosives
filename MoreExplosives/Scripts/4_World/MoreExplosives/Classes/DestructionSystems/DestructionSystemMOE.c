class MOE_DestructionSystemMOE : MOE_DestructionSystemBase
{
	protected ref map<string, ref MOE_EntityData> 	m_CachedEntityData;
	protected ref map<string, ref MOE_AmmoData> 	m_CachedAmmoData;
	
	override void CreateDamageSystem()
	{
		m_DamageSystem = new MOE_DamageSystemMOE();
	}

	override void CreateConfig()
	{
		if(m_Config)
		{
			delete m_Config;
		}
		
		m_Config = new MOE_ConfigMOE();
	}

	override bool IsDestructionSystemMOE()
	{
		return true;
	}

    override bool IsExplosiveCompatible(MOE_ExplosiveBase explosive, Object target, int component, vector position)
	{
		int mode = explosive.GetMountingMode();
		switch(mode)
		{
			case MOE_EExplosiveMountingModes.BASEBUILDINGONLY:
				return target && target.IsInherited(BaseBuildingBase);

			case MOE_EExplosiveMountingModes.SELECTEDONLY:
				return target && GetGame().ConfigGetInt(string.Format("%1 %2 MOE_Settings Mountable %3", CFG_VEHICLESPATH, target.GetType(), explosive.GetType()));

            case MOE_EExplosiveMountingModes.ANYTARGET:
                return target;
        }
		
		return true;
	}
}
