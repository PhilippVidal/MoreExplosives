//Simple ExplosionBehaviour that spawns a single ExplosionObject at the 
//location of the explosive and then deletes the explosive 
class MOE_SingleExplosion : MOE_ExplosionBehaviourBase
{
	override void Detonate(notnull MOE_ExplosiveBase explosive)
	{
		super.Detonate(explosive);
		
		//Notify that behaviour has been triggered
		OnBehaviourTriggered();
		
		//Spawn explosion wherever you want them 
		//in this case a single explosion at the position of the explosive
		vector position = explosive.GetExplosionPosition();
		string ammo = explosive.GetAmmoType();
		MOE_ExplosionObject explosionObject = MOE_ExplosionObject.Cast(GetGame().CreateObject("MOE_ExplosionObject", position));
		explosionObject.SetOrientation(explosive.GetExplosionOrientation());		
		explosionObject.Detonate(explosive, ammo);
		NotifyExplosive(explosive, explosionObject, ammo, true);

		//Notify that the behaviour has finished executing,
		//i.e., that the explosive can now be safely deleted 
		OnBehaviourFinished(explosive);				
	}
}