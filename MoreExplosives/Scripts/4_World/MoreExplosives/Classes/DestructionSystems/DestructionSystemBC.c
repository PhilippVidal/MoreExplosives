
class MOE_DestructionSystemBC : MOE_DestructionSystemBase
{
	
	
	override void CreateDamageSystem()
	{
		m_DamageSystem = new MOE_DamageSystemBC();
	}
	
	override void CreateConfig()
	{
		if(m_Config)
		{
			delete m_Config;
		}
		
		m_Config = new MOE_ConfigBC();
	}

	override bool IsDestructionSystemBC()
	{
		return true;
	}

	bool UseTargetForHitDetection()
	{
		return true;
	}

	override void InitBaseBuildingObject(BaseBuildingBase baseBuildingBase)
	{
		baseBuildingBase.InitHealth_MOE();        
	}

	override void OnConstructionPartChanged(BaseBuildingBase baseBuildingBase, ConstructionPart part)
	{
		MOE_ConfigDataDestroyableObject destroyableObject;
		if(!GetMOE().FindDestroyableObject(baseBuildingBase.GetType(), destroyableObject))
		{
			return;
		}

		baseBuildingBase.UpdateHealthValues_MOE(destroyableObject, part.GetMainPartName());
	}

	override bool CanBePlacedOn(MOE_ExplosiveBase explosive, Object target, int hitComponent, vector position)
	{
		return IsExplosiveCompatible(explosive, target, hitComponent, position);		
	}


	int CalculateMaxHealth(BaseBuildingBase target, string mainPart)
	{
		MOE_ConfigDataDestroyableObject destroyableObject = GetDestroyableObjectData(target.GetType());
		if(!destroyableObject)
		{
			return 0;
		}

		MOE_ConfigDataCategory baseCategory = GetCategoryData(destroyableObject.BaseCategory);
		if(!baseCategory)
		{
			return 0;
		}


		if(destroyableObject.SeparateBasedOnMainPart)
		{
			return baseCategory.HealthIncrease + CalculateMaxHealthBasedOnMainPart(explosiveType, target, destroyableObject, mainPart);
		}

		return baseCategory.HealthIncrease + CalculateMaxHealthAllParts(explosiveType, target, destroyableObject);
	}
	
	int CalculateMaxHealthAllParts(BaseBuildingBase target, MOE_ConfigDataDestroyableObject destroyableObject)
	{
		Construction construction = target.GetConstruction();
		if(!construction)
		{
			return 0;
		}

		string mainPart = target.GetMainPartFromComponent_MOE(component);
		array<string> parts = destroyableObject.MainPartSections.Get(mainPart);
		if(!parts)
		{
			return 0;
		}

		MOE_ConfigDataCategory partCategory;
		ConstructionPart constructionPart; 
		int partHealth;
		foreach(string partName, string partCategory : destroyableObject.PartCategories)
		{
			constructionPart = construction.GetConstructionPart(partName);
			if(!constructionPart || !constructionPart.IsBuilt())
			{
				continue;
			}

			partCategory = GetCategoryData(partCategory);
			if(!partCategory)
			{
				continue;
			}

			partHealth += partCategory.HealthIncrease;
		}

		return partHealth;
	}

	int CalculateMaxHealthBasedOnMainPart(BaseBuildingBase target, MOE_ConfigDataDestroyableObject destroyableObject, string mainPart)
	{
		Construction construction = target.GetConstruction();
		if(!construction)
		{
			return 0;
		}

		array<string> parts = destroyableObject.MainPartSections.Get(mainPart);
		if(!parts)
		{
			return 0;
		}

		MOE_ConfigDataCategory partCategory;
		ConstructionPart constructionPart; 
		int partHealth;
		foreach(string part : parts)
		{
			constructionPart = construction.GetConstructionPart(part);
			if(!constructionPart || !constructionPart.IsBuilt())
			{
				continue;
			}

			partCategory = GetCategoryData(destroyableObject.PartCategories.Get(part));
			if(!partCategory)
			{
				continue;
			}

			partHealth += partCategory.HealthIncrease;
		}

		return partHealth;
	}


	override bool IsExplosiveCompatible(MOE_ExplosiveBase explosive, Object target, int component, vector position)
	{
		MOE_ConfigDataDestroyableObject destroyableObject = GetDestroyableObjectData(target.GetType());
		if(!destroyableObject)
		{
			return false;
		}

		string explosiveType = explosive.GetType();
		MOE_ConfigDataCategory baseCategory = GetCategoryData(destroyableObject.BaseCategory);
		if(!baseCategory || baseCategory.IncompatibleExplosiveTypes.Contains(explosiveType))
		{
			return false;
		}

		BaseBuildingBase baseBuildingBase = BaseBuildingBase.Cast(target);
		if(!baseBuildingBase)
		{		
			return true;
		}

		if(destroyableObject.SeparateBasedOnMainPart)
		{
			return IsExplosiveCompatibleWithMainPart(explosiveType, baseBuildingBase, destroyableObject, mainPart);
		}

		return IsExplosiveCompatibleWithParts(explosiveType, baseBuildingBase, destroyableObject);
	}

	bool IsExplosiveCompatibleWithParts(string explosiveType, BaseBuildingBase target, MOE_ConfigDataDestroyableObject destroyableObject)
	{
		Construction construction = target.GetConstruction();
		if(!construction)
		{
			return true;
		}

		string mainPart = target.GetMainPartFromComponent_MOE(component);
		array<string> parts = destroyableObject.MainPartSections.Get(mainPart);
		if(!parts)
		{
			return true;
		}

		string partCategoryName;
		MOE_ConfigDataCategory partCategory;
		ConstructionPart constructionPart; 
		set<string> checkedCategories = new set<string>();
		foreach(string part, string category : destroyableObject.PartCategories)
		{
			constructionPart = construction.GetConstructionPart(part);
			if(!constructionPart || !constructionPart.IsBuilt())
			{
				continue;
			}

			if(checkedCategories.Find(category))
			{
				continue;
			}

			checkedCategories.Insert(category);
			partCategory = GetCategoryData(category);
			if(!partCategory || !partCategory.IncompatibleExplosiveTypes || !partCategory.IncompatibleExplosiveTypes.Count())
			{
				continue;
			}

			if(partCategory.IncompatibleExplosiveTypes.Find(explosiveType))
			{
				return false;
			}
		}

		return true;
	}

	bool IsExplosiveCompatibleWithMainPart(string explosiveType, BaseBuildingBase target, MOE_ConfigDataDestroyableObject destroyableObject, string mainPart)
	{
		Construction construction = target.GetConstruction();
		if(!construction)
		{
			return true;
		}

		array<string> parts = destroyableObject.MainPartSections.Get(mainPart);
		if(!parts)
		{
			return true;
		}

		string partCategoryName;
		MOE_ConfigDataCategory partCategory;
		ConstructionPart constructionPart; 
		set<string> checkedCategories = new set<string>();
		foreach(string part : parts)
		{
			constructionPart = construction.GetConstructionPart(part);
			if(!constructionPart || !constructionPart.IsBuilt())
			{
				continue;
			}

			partCategoryName = destroyableObject.PartCategories.Get(part);
			if(checkedCategories.Find(partCategoryName))
			{
				continue;
			}

			checkedCategories.Insert(partCategoryName);
			partCategory = GetCategoryData(partCategoryName);
			if(!partCategory || !partCategory.IncompatibleExplosiveTypes || !partCategory.IncompatibleExplosiveTypes.Count())
			{
				continue;
			}

			if(partCategory.IncompatibleExplosiveTypes.Find(explosiveType))
			{
				return false;
			}
		}

		return true;
	}
}