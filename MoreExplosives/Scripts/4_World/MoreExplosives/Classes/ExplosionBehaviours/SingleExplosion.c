//Simple ExplosionBehaviour that spawns a single ExplosionObject at the location of the MOE_Explosive
//and then deletes the explosive 
class MOE_SingleExplosion : MOE_ExplosionBehaviourBase
{
	override void Detonate(notnull MOE_ExplosiveBase explosive)
	{
		MOE_ExplosionObject obj = MOE_ExplosionObject.Cast(GetGame().CreateObject("MOE_ExplosionObject", explosive.GetExplosionPosition()));
		obj.SetOrientation(explosive.GetExplosionOrientation());		
		obj.Detonate(explosive);	
		super.Detonate(explosive);		
	}
}