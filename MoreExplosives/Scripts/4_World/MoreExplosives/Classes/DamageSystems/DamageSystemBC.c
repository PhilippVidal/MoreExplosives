class MOE_DamageSystemBC : MOE_DamageSystemBase
{
	override bool CanDealDamage(EntityAI target, int component, string dmgZone, MOE_ExplosiveBase explosive, MOE_ExplosionObject explosiveObject, string ammo, vector position)
	{
		BaseBuildingBase baseBuildingBase;
		if(!super.CanDealDamage(target, component, dmgZone, explosive, explosiveObject, ammo) || !CastTo(baseBuildingBase, target))
		{
			return false;
		}

		MOE_ConfigBC config;
		MOE_DestroyableObjectData_Internal destroyableObject;
		if(!CastTo(config, GetMOE().GetConfig()) || !config.GetDestroyableObject(target.GetType(), destroyableObject))
		{
			return false;
		}

		return true;	
	}
	
	override bool DealDamage(EntityAI target, int component, string dmgZone, MOE_ExplosiveBase explosive, MOE_ExplosionObject explosiveObject, string ammo, vector position)
	{
		BaseBuildingBase baseBuildingBase = BaseBuildingBase.Cast(target);
		if(!baseBuildingBase)
		{
			//TODO: error message
			return true;
		}

		DealDamageBaseBuildingBase(baseBuildingBase, component, dmgZone, explosive, explosiveObject, ammo, position);
		return true;
	}
	
	//returns whether damage has been dealt or not 
	bool DealDamageBaseBuildingBase(BaseBuildingBase target, int component, string dmgZone, MOE_ExplosiveBase explosive, MOE_ExplosionObject explosiveObject, string ammo, vector position)
	{
		//Get Destroyable Object 
		MOE_ConfigDataDestroyableObject destroyableObject;
		if(!GetMOE().FindDestroyableObject(target.GetType(), destroyableObject))
		{
			return false;
		}

		//Get MainPart ID 
		string mainPart = target.GetMainPartFromComponent_MOE(component);
		MOE_MainPartData mainPartData;
		if(!destroyableObject.MainParts.Find(mainPart, mainPartData))
		{
			return false;
		}

		int mainPartID = mainPartData.ID;
		if(target.GetMaxHealth_MOE(mainPartID) == -1)
		{
			target.InitHealthValues_MOE(destroyableObject, mainPart);
		}

		int currentHealth = target.GetHealth_MOE(mainPartID);
		int damageToDeal = 1; // explosive.GetDamage(); //TODO
		int newHealth = Math.Clamp(currentHealth - damageToDeal, 0, currentHealth);
		target.SetHealth_MOE(mainPartID, newHealth);	
		return true;
	}
	
	override int GetMOEType()
	{
		return MOE_EDamageSystemTypes.BC;
	}
}