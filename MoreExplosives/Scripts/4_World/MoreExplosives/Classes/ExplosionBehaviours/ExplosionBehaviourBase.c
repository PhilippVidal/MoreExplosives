//ExplosionBehaviours determine what happens when a MOE_Explosive detonates 
//and are supposed to delete the 'parent' explosive when the behaviour has been executed
class MOE_ExplosionBehaviourBase
{
	void Detonate(notnull MOE_ExplosiveBase explosive)
	{
		
	}
	
	void OnBehaviourTriggered(notnull MOE_ExplosiveBase explosive)
	{
		
	}

	void NotifyExplosive(notnull MOE_ExplosiveBase explosive, notnull MOE_ExplosionObject explosionObject, string ammo, bool canDamagePlacementTargetDirectly)
	{
		explosive.OnExplosionObjectDetonated_MOE(explosionObject, explosionObject.GetPosition(), ammo, canDamagePlacementTargetDirectly);
	}
	
	//Has to be called to initiate deletion of the 'parent' explosive (default behaviour)
	void OnBehaviourFinished(MOE_ExplosiveBase explosive)
	{
#ifndef MOE_DEBUG_PARTICLE
		if(explosive)
		{
			explosive.OnExplosionBehaviourFinished();
		}
#endif
	}
	
}

