modded class PluginRecipesManagerBase extends PluginBase
{	
	override void RegisterRecipies()
	{
		super.RegisterRecipies();						
		RegisterRecipe(new MOE_CraftGunpowder);
		RegisterRecipe(new MOE_CraftExplosiveHomemade);
		RegisterRecipe(new MOE_CraftExplosiveMilitary);
	}
}