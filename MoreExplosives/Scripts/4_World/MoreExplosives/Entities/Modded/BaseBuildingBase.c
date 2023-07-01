modded class BaseBuildingBase
{	
	override bool EEOnDamageCalculated(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{		
		return super.EEOnDamageCalculated(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef) && HandleDamage_MOE(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);
	}

	bool HandleDamage_MOE(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
		MOE_ExplosionObject explosionObject;
		if(!CastTo(explosionObject, source))
		{
			return !GetMOESettings().IsMOERaidingOnlyEnabled;
		}

		if(GetMOESettings().AreaDamageMode != MOE_EAreaDamageModes.DAYZ_EXPLOSION)
		{
			return false;
		}

		MOE_ExplosiveBase explosive;
		if(!CastTo(explosive, explosionObject.GetSourceExplosive()))
		{
			Log_MOE(string.Format("BaseBuildingBase::HandleDamage_MOE -> MOE_ExplosionObject does not have a valid source explosive (%1)", explosionObject.GetSourceExplosive()), MOE_ELogTypes.ERROR);
			return true;
		}
		
		MOE_HitInfo hitInfo = new MOE_HitInfo();
		hitInfo.Explosive = explosive;
		hitInfo.ExplosionObject = explosionObject;
		hitInfo.Target = this;
		hitInfo.HitComponent = component;
		hitInfo.TargetZone = dmgZone;
		hitInfo.Ammo = ammo;
		hitInfo.HitPosition = explosionObject.GetPosition();
		hitInfo.IsAreaHit = true;

		return !GetMOE().GetDestructionSystem().HandleExplosionHit(hitInfo);
	}
	
	bool HasGate_MOE()
	{
		Construction constr = GetConstruction();
		if(constr)
		{		
			map<string, ref ConstructionPart> constructionParts = constr.GetConstructionParts();
			for ( int i = 0; i < constructionParts.Count(); ++i )
			{
				string key = constructionParts.GetKey( i );
				ConstructionPart value = constructionParts.Get( key );
			
				if ( value.IsGate() && value.IsBuilt())
				{
					return true;
				}
			}
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
	
	bool AllowsExplosiveAttaching_MOE(int component)
	{
		return true;
	}
	
	EntityAI GetLock_MOE()
	{
		return FindAttachmentBySlotName("Att_CombinationLock");
	}
	
	
	bool GetPartFromComponent_MOE(int component, out ConstructionPart part)
	{
		string comp = GetActionComponentName(component, "fire");
		part = GetConstruction().GetConstructionPart(comp);
		return part != null;
	}
}

