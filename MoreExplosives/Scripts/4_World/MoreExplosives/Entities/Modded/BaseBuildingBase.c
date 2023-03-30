modded class BaseBuildingBase
{	
	override bool EEOnDamageCalculated(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{		
		if(!super.EEOnDamageCalculated(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef))
		{
			return false;
		}	
		
		return !GetMOE().TryHandleDamage(this, component, dmgZone, source, ammo);
	}
	
	
	////////////////////////////
	///// Helper Functions /////
	////////////////////////////
	
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

