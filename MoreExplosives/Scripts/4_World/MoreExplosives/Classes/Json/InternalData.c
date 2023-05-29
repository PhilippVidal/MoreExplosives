typedef map<string, int> MOE_BuildConfigurations;
typedef map<string, string> MOE_DestroyList;

class MOE_CategoryData_Internal
{
	int Health;
	ref array<int> AcceptedExplosiveTypes;
}

class MOE_JsonData_Internal 
{
    ref map<string, int> ExplosiveIDs;
	ref map<int, ref MOE_CategoryData_Internal> Categories;
	ref map<string, ref MOE_BuildConfigurations> DestroyableObjects;

	ref map<string, ref MOE_DestroyList> PartsToDestroy;

	void MOE_JsonData_Internal(MOE_JsonData_External externalData)
	{
		map<string, int> categoryIDs = new map<string, int>();
		InitCategories(externalData, categoryIDs);
		InitDestroyableObjects(externalData, categoryIDs);
	}

	array<ref Param> GetClientData()
	{
		array<ref Param> data = new array<Param>();
		data.Insert(new Param1<ref map<string, int>>(ExplosiveIDs));
		data.Insert(new Param1<ref map<int, ref MOE_CategoryData_Internal>>(Categories));
		data.Insert(new Param1<ref map<string, ref MOE_BuildConfigurations>>(DestroyableObjects));
		return data;
	}

	void ReadServerData(ParamsReadContext ctx)
	{
		Param1<ref map<string, int>> explosiveIDParam;
		if(!ctx.Read(explosiveIDParam))	
		{
			return;
		}
		ExplosiveIDs = explosiveIDParam.param1;

		Param1<ref map<int, ref MOE_CategoryData_Internal>> categoriesParam;
		if(!ctx.Read(categoriesParam))	
		{
			return;
		}
		Categories = categoriesParam.param1;

		Param1<ref map<string, ref MOE_BuildConfigurations>> destroyableObjectsParam;
		if(!ctx.Read(destroyableObjectsParam))	
		{
			return;
		}
		DestroyableObjects = destroyableObjectsParam.param1;
	}
	
	protected void InitCategories(MOE_JsonData_External jsonData, out map<string, int> categoryIDs)
	{
		Categories = new map<int, ref MOE_CategoryData_Internal>();

        int categoryCount = 1;
        int explosiveCount = 1;
        ExplosiveIDs = new map<string, int>();

		foreach(MOE_CategoryData_External externalCategory : jsonData.Categories)
		{
			MOE_CategoryData_Internal categoryData = new MOE_CategoryData_Internal();
			categoryData.Health = externalCategory.Health;
			categoryData.AcceptedExplosiveTypes = new array<int>();

			int explosiveID; 
			foreach(string explosive : externalCategory.AcceptedExplosiveTypes)
			{
				
				if(!ExplosiveIDs.Find(explosive, explosiveID))
				{			
					explosiveID = explosiveCount++;		
					ExplosiveIDs.Insert(explosive, explosiveID);
				}

				categoryData.AcceptedExplosiveTypes.Insert(explosiveID);
			}

			int categoryID = categoryCount++;
			Categories.Insert(categoryID, categoryData);
			categoryIDs.Insert(externalCategory.Name, categoryID);  
		}
	}
	
	protected void InitDestroyableObjects(MOE_JsonData_External jsonData, map<string, int> categoryIDs)
	{
		DestroyableObjects = new map<string, ref MOE_BuildConfigurations>();
		PartsToDestroy = new map<string, ref MOE_DestroyList>();

		foreach(MOE_DestroyableObjectData_External externalData : jsonData.DestroyableObjects)
		{	
			//Get all parts listed in the config and their ids
			map<string, int> potentialParts = MoreExplosives.GetConstructionPartIDs(externalData.Name);

			MOE_BuildConfigurations buildConfigurations = new MOE_BuildConfigurations();
			MOE_DestroyList partsToDestroy = new MOE_DestroyList();

			foreach(MOE_BuildConfigurationData_External externalConfiguration : externalData.BuildConfiguartions)
			{
				int mask1, mask2, mask3;

				//Create internal representation of build configuration 
				MoreExplosives.GetMasksFromPartList(externalConfiguration.BuildParts, potentialParts, mask1, mask2, mask3);
				string buildConfigurationKey = MoreExplosives.GeneratePartKey(mask1, mask2, mask3);

				int categoryID;
				if(!categoryIDs.Find(externalConfiguration.Category, categoryID))
				{
					categoryID = -1;
				}

				buildConfigurations.Insert(buildConfigurationKey, categoryID);


				//Create parts to destroy entry
				if(externalConfiguration.PartsToDestroy && externalConfiguration.PartsToDestroy.Count())
				{
					mask1 = 0;
					mask2 = 0;
					mask3 = 0;

					MoreExplosives.GetMasksFromPartList(externalConfiguration.PartsToDestroy, potentialParts, mask1, mask2, mask3);
					string destroyListMask = MoreExplosives.GeneratePartKey(mask1, mask2, mask3);
					partsToDestroy.Insert(buildConfigurationKey, destroyListMask);
				}
			}

			if(!buildConfigurations.Count())
			{
				continue;
			}
			
			DestroyableObjects.Insert(externalData.Name, buildConfigurations);

			if(partsToDestroy.Count())
			{
				PartsToDestroy.Insert(externalData.Name, partsToDestroy);
			}
		}
	}
}