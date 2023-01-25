//Potential fix for players being able to punch/hit a mounted explosive and clip their head through a base wall
modded class DayZPlayerImplementMeleeCombat
{
	override void Init(DayZPlayerImplement player)
	{
		super.Init(player);
		m_NonAlignableObjects.Insert(MOE_ExplosiveBase);
	}
}