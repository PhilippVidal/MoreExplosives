class MOE_AmmoData : MOE_ConfigDataBase
{
	float Damage;
	float MaxDamageRange; 
	float FullDamageRange;	
	vector ParticleOrientation;

	override void Init(string type)
	{
		string basePath = string.Format("%1 %2 ", CFG_AMMO, type);
		float indirectHitRange 				= GetGame().ConfigGetFloat(basePath + "indirectHitRange"); 
		float indirectHitRangeMultiplier 	= GetGame().ConfigGetFloat(basePath + "indirectHitRangeMultiplier");
		
		MaxDamageRange = indirectHitRange * indirectHitRangeMultiplier;
		
		basePath += "MOE_Settings ";
		
		Damage				= GetGame().ConfigGetFloat(basePath + "damage"); 
		FullDamageRange 	= GetGame().ConfigGetFloat(basePath + "fullDamageRange"); 
		ParticleOrientation = GetGame().ConfigGetTextOut(basePath + "particleOrientation").ToVector();
	}

#ifdef MOE_DEBUG_CONFIG		
	override void PrintLoadedData(string type)
	{	
		super.PrintLoadedData(type);
		string debugStr = string.Format("AmmoData [%1]:", type);
			debugStr += string.Format("\n	Damage: %1", Damage);
			debugStr += string.Format("\n	indirectHitRange: %1", indirectHitRange);
			debugStr += string.Format("\n	indirectHitRangeMultiplier: %1", indirectHitRangeMultiplier);
			debugStr += string.Format("\n	FullDamageRange: %1", FullDamageRange);
			debugStr += string.Format("\n	ParticleOrientation: %1", ParticleOrientation);
		Print("" + debugStr);
	}
#endif
}