modded class BaseBuildingBase
{	
	
	protected int m_Health_MOE = -1;
	
	void BaseBuildingBase()
	{
#ifdef SERVER	
		if(GetHealth_MOE() < 1) 
		{
			InitHealth_MOE(); 
		}
#endif	
	}
	
	override bool EEOnDamageCalculated(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{		
		if(!super.EEOnDamageCalculated(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef))
		{
			return false;
		}	
		
		return !GetMOE().TryHandleDamage(this, component, dmgZone, source, ammo);
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
	
	EntityAI GetLock_MOE()
	{
		return FindAttachmentBySlotName("Att_CombinationLock");
	}
	
	
	//////////////////////////////////////////
	/// Functionality for BC damage system ///
	//////////////////////////////////////////
	
	int GetHealth_MOE()
	{
		return m_Health_MOE;
	}
	
	int GetMaxHealth_MOE()
	{
		//TODO: get base health from config 
		int maxHealth = 1;

		Construction constr = GetConstruction();
		if(!constr)
		{
			return maxHealth;
		}

		//TODO
		//configData.DestroyableObjectData...

		foreach(ConstructionPart part : constr.GetConstructionParts())
		{
			if(!part.IsBuilt())
			{
				continue;
			}
			
			

			int partId = part.GetId();
			int categoryId;
			if(!destroyableObjectData.PartCategories.Find(partId, categoryId))
			{
				continue;
			}

			MOE_CategoryData_Internal categoryData;
			if(!configData.Categories.Find(categoryId, categoryData))
			{
				continue;
			}

			maxHealth += categoryData.HealthIncrease;

		}

		return 2; //TODO
	}
	
	void SetHealth_MOE(int newHealth)
	{
		int oldHealth = m_Health_MOE;
		m_Health_MOE = newHealth;
		OnHealthChanged_MOE(oldHealth);
	}
	
	protected void OnHealthChanged_MOE(int oldHealth)
	{
		Print(string.Format("Health has been changed(%1): newHealth = %2, oldHealth = %3, Change = %4", this, m_Health_MOE, oldHealth, oldHealth - m_Health_MOE));
	}
	
	protected void InitHealth_MOE()
	{
		m_Health_MOE = GetMaxHealth_MOE();
	}
	
	string GetPartMask_MOE()
	{
		Construction constr = GetConstruction();
		if(!constr)
		{
			return "";
		}
		
		int mask1, mask2, mask3;
		
		int partID = 0; 
		foreach(ConstructionPart part : constr.GetConstructionParts())
		{
			MoreExplosives.AddToBuildMask(part.GetId() * part.IsBuilt(), mask1, mask2, mask3);
		}
		
		return string.Format(
			"%1 %2 %3", 
			HDSN_MiscFunctions.IntToHexString(mask1), 
			HDSN_MiscFunctions.IntToHexString(mask2), 
			HDSN_MiscFunctions.IntToHexString(mask3));
	}


	array<int> GetBuildPartIDs_MOE()
	{
		Construction constr = GetConstruction();
		if(!constr)
		{
			return null;
		}

		array<int> partIDs = new array<int>();
		foreach(ConstructionPart part : constr.GetConstructionParts())
		{
			if(part.IsBuilt())
			{
				partIDs.Insert(part.GetId());
			}
		}
	}
}

