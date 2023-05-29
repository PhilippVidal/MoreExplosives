class MOE_CategoryData_External
{
	string Name;
	int Health; 
	array<string> AcceptedExplosiveTypes;
}

class MOE_BuildConfigurationData_External
{
	string Category;
	ref array<string> BuildParts;
	ref array<string> PartsToDestroy;
}

class MOE_DestroyableObjectData_External
{
	string Name;
	array<MOE_BuildConfigurationData_External> BuildConfigurations;
}

class MOE_JsonData_External
{
	const string ConfigPath 				= "$profile:/MoreExplosives/Jsons/"; 
	const string CategoriesName 			= "categories.json";
	const string DestroyableObjectsSubPath 	= "DestroyableObjects/";
	
	ref array<ref MOE_CategoryData_External> Categories;
	ref array<ref MOE_DestroyableObjectData_Json> DestroyableObjects;
	
	void MOE_JsonData_External()
	{
		LoadCategories();
		LoadDestroyableObjects();
	}
	
	protected void LoadCategories()
	{
		Categories = array<ref MOE_CategoryData_External>();
		JsonFileLoader<array<MOE_CategoryData_Json>>.JsonLoadFile(configPath + categoriesName, Categories);
	}
	
	protected void LoadDestroyableObjects()
	{		
		string basePath = ConfigPath + DestroyableObjectsSubPath;
		array<string> fileList = GetDestroyableObjectsFileList();
						
		foreach(string file : fileList)
		{
			MOE_DestroyableObjectData_External destroyableObject = new MOE_DestroyableObjectData_External();
			JsonFileLoader<MOE_DestroyableObjectData_External>.JsonLoadFile(basePath + file, destroyableObject);		
			if(destroyableObject)
			{
				DestroyableObjects.Insert(destroyableObject);
			}
		}
	}
	
	protected array<string> GetDestroyableObjectsFileList()
	{
		array<string> fileList = new array<string>();
		string filePattern = ConfigPath + DestroyableObjectsSubPath + "/*.json";
		
		string fileName;
		int fileAttributes;
		int fileFlags;
		
		FindFileHandle fileHandle = FindFile(filePattern, fileName, fileAttributes, fileFlags);
		
		bool found = true;
		while (found)
		{				
			if (fileAttributes & FileAttr.DIRECTORY)
			{
				fileList.Insert(fileName);
			}
			
			found = FindNextFile(fileHandle, fileName, fileAttributes);
		}
		
		return fileList;
	}
}