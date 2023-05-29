class MOE_TriggerDataBase : MOE_ConfigDataBase
{
	float LightRadius;
	float LightBrightness;
	ref array<vector> LightColors;
	
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
}