class MOE_PowderBase : ItemBase
{
	override void InitItemVariables()
	{
		super.InitItemVariables();
		can_this_be_combined = true;
	}
}

class MOE_Gunpowder : MOE_PowderBase {}

class MOE_Sulfurpowder : MOE_PowderBase {} 

class MOE_Charcoalpowder : MOE_PowderBase {} 
