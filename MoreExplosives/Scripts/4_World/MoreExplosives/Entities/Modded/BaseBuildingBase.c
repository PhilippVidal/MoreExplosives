modded class BaseBuildingBase
{	
	protected ref array<int> m_Health_MOE;
	protected ref array<int> m_MaxHealth_MOE;
	protected bool m_IsConstructionFullyInitialized_MOE;

	void BaseBuildingBase()
	{
		MOE_DestructionSystemBase destructionSystem = GetMOE().GetDestructionSystem();
		if(destructionSystem)
		{
			destructionSystem.InitBaseBuildingObject(this);
		}
	}

	override void SetPartFromSyncData(ConstructionPart part)
	{
		super.SetPartFromSyncData(part);		
		GetMOE().GetDestructionSystem().OnConstructionPartChanged(this, part);
	}

	void InitHealth_MOE()
	{
		MOE_ConfigDataDestroyableObject destroyableObject = GetMOE().GetDestroyableObject(GetType());
		if(!destroyableObject)
		{
			return;
		}

		m_Health_MOE = new array<int>();
		m_MaxHealth_MOE = new array<int>();

		int count = destroyableObject.MainParts.Count();
		m_Health_MOE.Reserve(count);
		m_MaxHealth_MOE.Reserve(count);

		for(int i = 0; i < count; ++i)
		{
			m_Health_MOE[i] = -1;
			m_MaxHealth_MOE[i] = -1;
		}		
	}

	void InitHealthValues_MOE(MOE_ConfigDataDestroyableObject destroyableObject, string mainPart)
	{
		int maxHealth = destroyableObject.CalculateHealth(this, mainPart);
		m_MaxHealth_MOE[id] = maxHealth;
		m_Health_MOE[id] = maxHealth;
	}

	void UpdateHealthValues_MOE(MOE_ConfigDataDestroyableObject destroyableObject, string mainPart)
	{
		MOE_MainPartData mainPartData;
		if(!destroyableObject.MainParts.Find(mainPart, mainPartData))
		{
			return;
		}

		int id = mainPartData.ID;
		int oldMaxHealth = m_MaxHealth_MOE[id];
		int oldHealth = m_Health_MOE[id]; 
		int newMaxHealth = destroyableObject.CalculateHealth(this, mainPart);
		if((oldMaxHealth < 0) || (oldHealth < 0))
		{
			m_MaxHealth_MOE[id] = newMaxHealth;
			m_Health_MOE[id] = newMaxHealth;
			return;
		}

		OnMaxHealthChanged_MOE(mainPartData, oldMaxHealth, newMaxHealth);
	}

	int GetMaxHealth_MOE(int mainPartID)
	{
		return m_MaxHealth_MOE[mainPartData.ID];
	}

	int GetMaxHealth_MOE(string mainPart)
	{
		MOE_ConfigDataDestroyableObject destroyableObject;
		if(!GetMOE().FindDestroyableObject(GetType(), destroyableObject))
		{
			return 0;
		}

		MOE_MainPartData mainPartData;
		if(!destroyableObject.MainParts.Find(mainPart, mainPartData))
		{
			return 0;
		}

		return m_MaxHealth_MOE[mainPartData.ID];
	}

	int GetHealth_MOE(int mainPartID)
	{
		return m_Health_MOE[mainPartID];
	}

	int GetHealth_MOE(string mainPart)
	{
		MOE_ConfigDataDestroyableObject destroyableObject;
		if(!GetMOE().FindDestroyableObject(GetType(), destroyableObject))
		{
			return 0;
		}

		MOE_MainPartData mainPartData;
		if(!destroyableObject.MainParts.Find(mainPart, mainPartData))
		{
			return 0;
		}

		return m_Health_MOE[mainPartData.ID];
	}

	array<int> GetHealthValues_MOE()
	{
		return m_Health_MOE;
	}

	array<int> GetMaxHealthValues_MOE()
	{
		return m_MaxHealth_MOE;
	}

	void OnMaxHealthChanged_MOE(MOE_MainPartData mainPartData, int oldHealth, int newHealth)
	{
		float oldPercentage = (float)m_Health_MOE[mainPartData.ID] / oldHealth;
		m_Health_MOE[mainPartData.ID] = Math.Ceil(newHealth * oldPercentage);
	}


	string GetMainPartFromComponent_MOE(int component)
	{
		//string part_name = target_object.GetActionComponentName( target.GetComponentIndex() );
		string partName = GetActionComponentName(component);
		Construction construction = GetConstruction();
		ConstructionPart constructionPart = construction.GetConstructionPart(partName);
		return constructionPart.GetMainPartName();
	}
	
	override bool EEOnDamageCalculated(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{		
		if(!super.EEOnDamageCalculated(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef))
		{
			return false;
		}	
		
		MOE_ExplosionObject explosionObject;
		if(!CastTo(explosionObject, source))
		{
			return !GetMOE().IsMOERaidingOnlyEnabled();
		}
		
		return GetMOE().GetDestructionSystem().UseExplosionForHitDetection() && !GetMOE().GetDestructionSystem().HandleExplosionHit(explosionObject, this, component, dmgZone, ammo);
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
	
	
	void SetHealth_MOE(int newHealth, int mainPartID)
	{
		int oldHealth = m_Health_MOE[mainPartID];
		m_Health_MOE[mainPartID] = newHealth;
		OnHealthChanged_MOE(oldHealth, newHealth);
	}
	
	protected void OnHealthChanged_MOE(int oldHealth, int newHealth)
	{
		Print(string.Format("Health has been changed(%1): newHealth = %2, oldHealth = %3, Change = %4", this, newHealth, oldHealth, oldHealth - m_Health_MOE));
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

