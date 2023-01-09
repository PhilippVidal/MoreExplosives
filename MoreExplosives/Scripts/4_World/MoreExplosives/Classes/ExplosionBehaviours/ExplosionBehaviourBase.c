//ExplosionBehaviours determine what happens when a MOE_Explosive detonates 
//and are supposed to delete the 'parent' explosive when the behaviour has been executed
class MOE_ExplosionBehaviourBase
{
	void Detonate(notnull MOE_ExplosiveBase explosive)
	{
		OnDetonate(explosive);
		OnBehaviourFinished(explosive);
	}
	
	void OnDetonate(notnull MOE_ExplosiveBase explosive)
	{
		
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

