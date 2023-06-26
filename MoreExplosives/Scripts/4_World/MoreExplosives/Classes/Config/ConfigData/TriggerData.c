class MOE_TriggerData : MOE_ConfigDataBase
{
	float LightRadius;
	float LightBrightness;
	ref array<vector> LightColors;
	
	override void Init(string type)
	{	
		//Set default values
		LightRadius = 10;
		LightBrightness = 10;
		LightColors = new array<vector>();
			
		//Load values from config if they exist	
		string basePath = GetBasePath(type);	
		string path = basePath + "Light radius";
		if(GetGame().ConfigIsExisting(path)) LightRadius = GetGame().ConfigGetFloat(path);
		
		path = basePath + "Light brightness";
		if(GetGame().ConfigIsExisting(path)) LightBrightness = GetGame().ConfigGetFloat(path);
		
		array<float> rgbValues = new array<float>;
		path = basePath + "Light colors";
		if(GetGame().ConfigIsExisting(path))
		{
			GetGame().ConfigGetFloatArray(path, rgbValues);	
			LightColors = RGBValuesToVectors(rgbValues);
		}
	}
	
	array<vector> RGBValuesToVectors(notnull array<float> rgbValues)
	{
		int count = rgbValues.Count();
		for(int difference = count % 3; difference > 0; difference--)
		{
			rgbValues.Insert(0);
		}
		
		count = rgbValues.Count();
		
		array<vector> colors = new array<vector>();
		vector color;
		for(int i = 0; i < count; i = i + 3)
		{
			color[0] = rgbValues[i];
			color[1] = rgbValues[i + 1];
			color[2] = rgbValues[i + 2];
			colors.Insert(color);
		}
		
		return colors;
	}
	
#ifdef MOE_DEBUG_CONFIG		
	override void PrintLoadedData(string type)
	{
		super.PrintLoadedData(type);
		Print(string.Format("Light Radius: %1", LightRadius));
		Print(string.Format("Light Brightness: %1", LightBrightness));
		Print(string.Format("Light Colors: Count = %1", LightColors.Count()));
		for(int i = 0; i < LightColors.Count(); i++)
		{
			Print(string.Format(" [%1]: %2", i, LightColors[i].ToString()));
		}	
	}
#endif	
}