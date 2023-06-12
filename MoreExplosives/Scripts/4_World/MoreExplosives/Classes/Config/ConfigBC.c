class MOE_MainPartData
{
	int ID;
	ref array<ref MOE_PartData> ConfiguredParts;
	ref array<ref MOE_PartData> UnconfiguredParts;

	void MOE_MainPartData(string objectType, string name)
	{
		Init(objectType, name);
	}

	void Init(string objectType, string name)
	{
		ConfiguredParts = new array<ref MOE_PartData>();
		UnconfiguredParts = new array<ref MOE_PartData>();

		string partPath = string.Format("CfgVehicles %1 Construction %2", objectType, name);

		int partCount = GetGame().ConfigGetChildrenCount(partPath);
		for (int i = 0; i < partCount; ++i)
		{
			string part;
			GetGame().ConfigGetChildName(partPath, i, part);	

			int categoryID;
			string category = GetGame().ConfigGetTextOut(string.Format("CfgMoreExplosives DestroyableObjects %1 PartCategories %2 %3", objectType, name, part));
			
			MOE_ConfigDataCategory category; 
			if(!GetCategoryData(category, category))
			{
				UnconfiguredParts.Insert(new MOE_PartData(part, categoryID));
				continue;		
			}
			
			/////
			if(category)
			{
				categoryID = GetCategoryID(category);
			}		

			if(!categoryID)
			{
				UnconfiguredParts.Insert(new MOE_PartData(part, categoryID));
				continue;
			}

			ConfiguredParts.Insert(new MOE_PartData(part, categoryID));
		}
	}
}

class MOE_PartData
{
	int CategoryID;
	string Name;

	void MOE_PartData(string name, int categoryID)
	{
		Name = name; 
		CategoryID = categoryID;
	}
}

class MOE_ConfigDataCategory : MOE_ConfigDataBase
{
	int HealthIncrease;
	ref array<string> IncompatibleExplosiveTypes;

	override void Init(string type)
	{
		super.Init(type);
		HealthIncrease = 0;
		IncompatibleExplosiveTypes = new array<string>();

		string basePath = GetBasePath(type);
		string path = basePath + "healthIncrease";
		if(GetGame().ConfigIsExisting(path)) 
			HealthIncrease = GetGame().ConfigGetInt(path);

		path = basePath + "incompatibleExplosiveTypes";
		if(GetGame().ConfigIsExisting(path)) 
			GetGame().ConfigGetTextArray(path, IncompatibleExplosiveTypes);
	}

	override string GetBasePath(string type)
	{
		return string.Format("%1 Categories %2 ", CFG_MOE, type);	
	}

	//TODO debug functions
}

class MOE_ConfigDataDestroyableObject : MOE_ConfigDataBase
{
	int BaseCategoryID;
	bool DeleteWithoutDestroyableParts;
	ref array<string> DestroyableParts; //Only these parts can be destroyed, if empty -> destroy all parts
	//ref map<string, string> PartCategories;

	//bool SeparateBasedOnMainPart;
	//ref map<int, ref array<string>> MainPartSections;

	//ref map<string, int> Parts;
	//ref map<string, int> MainParts;

	ref map<string, ref MOE_MainPartData> MainParts;


	override void Init(string type)
	{
		super.Init(type);
		BaseCategory = "";
		DeleteWithoutDestroyableParts = false;
		DestroyableParts = new array<string>();
		PartCategories = new map<string, string>();
		MainParts = new map<string, ref MOE_MainPartData>();

		string basePath = GetBasePath(type);

		string path = basePath + "baseCategory";		
		if(GetGame().ConfigIsExisting(path)) 
			BaseCategoryID = GetCategoryID(GetGame().ConfigGetTextOut(path));

		path = basePath + "deleteWithoutDestroyableParts";
		if(GetGame().ConfigIsExisting(path)) 
			DeleteWithoutDestroyableParts = GetGame().ConfigGetInt(path) != 0;

		path = basePath + "destroyableParts";
		if(GetGame().ConfigIsExisting(path)) 
			GetGame().ConfigGetTextArray(path, DestroyableParts);

		path = basePath + "separateBasedOnMainPart";
		if(GetGame().ConfigIsExisting(path)) 
			SeparateBasedOnMainPart = GetGame().ConfigGetInt(path) != 0;

		if(SeparateBasedOnMainPart)
		{
			MainPartSections = new map<string, ref array<string>>();
		}

		string constructionPath = string.Format("CfgVehicles %1 Construction", type);
		
		int subConstrCount = GetGame().ConfigGetChildrenCount(constructionPath);		
		for (int i = 0; i < subConstrCount; ++i)
		{			
			string mainPart;
			GetGame().ConfigGetChildName(constructionPath, i, mainPart);
			MainParts.Insert(mainPart, new MOE_MainPartData(constructionPath, mainPart)); 
		}
	}

	override string GetBasePath(string type)
	{
		return string.Format("%1 DestroyableObjects %2 ", CFG_MOE, type);	
	}

	//bool IsExplosiveCompatible(string explosiveType, Object target, string mainPart)
	//{
	//	MOE_ConfigDataCategory categoryData;
	//	if(GetCategory(BaseCategoryID, categoryData))
	//	{
	//		if(categoryData.IncompatibleExplosiveTypes.Find(explosiveType))
	//		{
	//			return false;
	//		}
	//	}
	//	BaseBuildingBase baseBuildingBase;
	//	return CastTo(baseBuildingBase, target) && IsExplosiveCompatibleWithMainPart(explosiveType, baseBuildingBase, mainPart);
	//}
	
	bool IsExplosiveCompatible(string explosiveType, Object target, string mainPart)
	{
		MOE_ConfigDataCategory baseCategory = GetMOE().GetCategoryData(destroyableObject.BaseCategory);
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

	bool IsExplosiveCompatibleWithMainPart(string explosiveType, BaseBuildingBase target, string mainPart)
	{
		Cosntruction construction = target.GetConstruction();
		if(!construction)
		{
			return true;
		}

		MOE_MainPartData mainPartData = MainParts.Get(mainPart);
		if(!mainPartData || !mainPartData.ConfiguredParts)
		{
			return true;
		}

		set<int> checkedCategories = new set<int>();
		MOE_ConfigDataCategory categoryData;
		foreach(MOE_PartData part : ConfiguredParts)
		{
			ConstructionPart constructionPart = construction.GetConstructionPart(part.Name);
			if(!constructionPart || !constructionPart.IsBuilt())
			{
				continue;
			}

			if(!CategoryID || !GetCategory(part.CategoryID, categoryData)) 
			{
				//TODO log warning/error
				continue;
			}

			if(categoryData.IncompatibleExplosiveTypes.Find(explosiveType))
			{
				return false;
			}
		}

		return true;
	}


	int CalculateHealth(BaseBuildingBase target, string mainPart)
	{
		int health;
		MOE_ConfigDataCategory categoryData;
		if(GetCategory(BaseCategoryID, categoryData))
		{
			health += categoryData.HealthIncrease;
		}

		return health + CalculateMainPartHealth(target, mainPart);
	}

	int CalculateMainPartHealth(BaseBuildingBase target, string mainPart)
	{
		Cosntruction construction = target.GetConstruction();
		if(!construction)
		{
			return 0;
		}

		MOE_MainPartData mainPartData = MainParts.Get(mainPart);
		if(!mainPartData || !mainPartData.ConfiguredParts)
		{
			return 0;
		}

		int mainPartHealth; 
		MOE_ConfigDataCategory categoryData;
		foreach(MOE_PartData part : ConfiguredParts)
		{
			ConstructionPart constructionPart = construction.GetConstructionPart(part.Name);
			if(!constructionPart || !constructionPart.IsBuilt())
			{
				continue;
			}

			if(!CategoryID || !GetCategory(part.CategoryID, categoryData)) 
			{
				//TODO log warning/error
				continue;
			}

			mainPartHealth += categoryData.HealthIncrease;
		}

		return mainPartHealth;
	}

	int GetCategoryID(string category)
	{

	}

	//TODO
	bool GetCategoryFromID(int id, out MOE_ConfigDataCategory categoryData)
	{
		return true;
	}


	int GetMainPartID(string mainPart)
	{
		return MainPartSections.Get(mainPart);
	}

	int GetPartID(string part)
	{
		return Parts.Get(part);
	}

	//TODO debug functions
}

class MOE_ConfigBC : MOE_ConfigBase
{
	protected MOE_JsonData_Internal m_JsonData;

	const protected string m_ConfigPath = "$profile:/MoreExplosives/Jsons/";
	const protected string m_GeneralSettingsJsonName = "generalsettings.json";

	override void Init()
	{
		InitCategories();
		InitDestroyableObjects();
	}

	void InitCategories()
	{
		string basePath = "CfgMoreExplosives Categories";
		int childCount = GetGame().ConfigGetChildrenCount(basePath);
		for(int i = 0; i < childCount; ++i)
		{
			string categoryName; 
			GetGame().ConfigGetChildName(basePath, i, categoryName);
			//m_CachedConfigData.Insert(string.Format("Category %1", categoryName), new MOE_ConfigDataCategory(categoryName));
			LoadCategoryData(categoryName);
		}
	}

	void InitDestroyableObjects()
	{
		string basePath = "CfgMoreExplosives DestroyableObjects";
		int childCount = GetGame().ConfigGetChildrenCount(basePath);
		for(int i = 0; i < childCount; ++i)
		{
			string destroyableObjectType; 
			GetGame().ConfigGetChildName(basePath, i, destroyableObjectType);
			//m_CachedConfigData.Insert(destroyableObjectName, new MOE_ConfigDataDestroyableObject(destroyableObjectName));
			LoadDestroyableObjectData(destroyableObjectType);
		}
	}
	
	override void LoadGeneralSettings()
	{
		m_GeneralSettings = new MOE_GeneralSettings();	
		JsonFileLoader<MOE_GeneralSettings>.JsonLoadFile(m_ConfigPath + m_GeneralSettingsJsonName, m_GeneralSettings);		
	}	
	

	override void RequestDataFromServer()
	{
		if(GetGame().IsClient())
		{
			GetGame().RPC(null, MOE_ERPCTypes.CONFIG, null, true, null);
		}					
	}
	
	override void SentDataToClient(notnull PlayerIdentity client)
	{
		if(GetGame().IsClient())
		{
			return;
		}
		
		GetGame().RPC(null, MOE_ERPCTypes.CONFIG, m_ConfigData.GetClientData(), true, client);
	}
	
	override bool ReceiveDataFromServer(ParamsReadContext ctx)
	{
		m_ConfigData.ReadServerData(ctx);	
	}
	
	MOE_JsonData_Internal GetJsonData()
	{
		return m_JsonData;
	}
	
	MOE_CategoryData_Internal GetCategory(int id)
	{
		if(!m_JsonData || !m_JsonData.Categories)
		{
			return null;
		}

		return m_JsonData.Categories.Get(id);
	}

	MOE_DestroyableObjectData_Internal GetDestroyableObject(string type)
	{
		if(!m_JsonData || !m_JsonData.DestroyableObjects)
		{
			return null;
		}

		return m_JsonData.DestroyableObjects.Get(type);
	}

	bool GetDestroyableObject(string type, out MOE_DestroyableObjectData_Internal destroyableObject)
	{
		destroyableObject = GetDestroyableObject(type);
		return destroyableObject != null;
	}

	int GetExplosiveID(string type)
	{
		if(!m_JsonData || !m_JsonData.ExplosiveIDs)
		{
			return 0;
		}

		return m_JsonData.ExplosiveIDs.Get(type);
	}

	bool IsExplosiveTypeAccepted(string explosiveType, MOE_DestroyableObjectData_Internal destroyableObject, array<int> builtPartIDs)
	{
		MOE_JsonData_Internal config; //TODO

		int explosiveID = config.GetExplosiveID(explosiveType);
		if(explosiveID < 1)
		{
			return false;
		}

		foreach(int partID : builtPartIDs)
		{
			

			int categoryID = destroyableObject.PartCategories.Get(partID);
			if(categoryID < 1)
			{
				continue;
			}

			MOE_CategoryData_Internal category = config.GetCategory(categoryID);
			if(!category)
			{
				continue;
			}

			if(category.IncompatibleExplosiveTypes.Contains(explosiveID))
			{
				return false;
			}
		}

		return true;
	}
}
