//This gets spawned by a ExplosionBehaviour when an explosion occurs (due to an MOE_Explosive detonating)
//should delete itself after it exploded 
class MOE_ExplosionObject : House
{
	protected MOE_ExplosiveBase m_ParentExplosive;
	protected ParticleSource m_ExplosionParticle;
	protected int m_ParticleID;
	protected string m_AmmoType;
	
	void Detonate(notnull MOE_ExplosiveBase explosive, string ammoOverride = "")
	{
		m_ParentExplosive = explosive;
		string ammo; 
		if(ammoOverride == "")
		{
			ammo = m_ParentExplosive.GetAmmoType();
		}
		else 
		{
			ammo = ammoOverride;
		}
		
		SetAmmoType(ammo);
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(Explode_MOE, 1);
	}
	
	void Explode_MOE()
	{
#ifdef SERVER
		if(m_AmmoType == "")
		{
			m_AmmoType = "Dummy_Heavy";
		}
		
		SynchExplosion();		
		DamageSystem.ExplosionDamage(this, null, m_AmmoType, GetPosition(), DamageType.EXPLOSION);

		
#ifdef MOE_DEBUG_PARTICLE			
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(DeleteSafe, 15000);
#else 
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(DeleteSafe, 500);
#endif
#endif		
	}	
	
	EntityAI GetSourceExplosive()
	{
		return m_ParentExplosive;
	}
	
	void PlayParticle(string ammoType)
	{
		int particleID = AmmoEffects.GetAmmoParticleID(ammoType);
		if(ParticleList.IsValidId(particleID)) 
		{
			vector local_orientation;
			MOE_AmmoData ammoData = GetMOE().GetAmmoData(ammoType);
			if(ammoData)
			{
				local_orientation = ammoData.ParticleOrientation;
			}

			m_ExplosionParticle = ParticleManager.GetInstance().PlayOnObject(particleID, this, vector.Zero, local_orientation);	
		}
	}
	
	void DestroyParticle(ParticleSource p)
	{
#ifndef SERVER
		if (p != null)
		{
			p.Stop();
		}
#endif
	}
	
	void SetAmmoType(string ammo)
	{
		m_AmmoType = ammo;
	}
	
	
	override void OnExplodeClient() {}
	
	
	override void OnExplosionEffects(Object source, Object directHit, int componentIndex, string surface, vector pos, vector surfNormal, float energyFactor, float explosionFactor, bool isWater, string ammoType) 
	{ 
		super.OnExplosionEffects(source, directHit, componentIndex, surface, pos, surfNormal, energyFactor, explosionFactor, isWater, ammoType);
#ifdef MOE_DEBUG_PARTICLE	
		Debug_DrawDirections();
#endif					
		PlayParticle(ammoType);	
	}
	
	override void EEDelete(EntityAI parent)
	{
		DestroyParticle(m_ExplosionParticle);
	}
	
	
#ifdef MOE_DEBUG_PARTICLE	
	protected Shape m_DirectionX, m_DirectionY, m_DirectionZ;
	
	void Debug_DrawDirections() 
	{	
		Debug_RemoveDirections();
		
		vector position = GetPosition();
		vector selfMatrix[4];
		GetTransform(selfMatrix);			
		
		m_DirectionX = Debug.DrawArrow( position, position + selfMatrix[0], 0.05, Colors.RED );
		m_DirectionY = Debug.DrawArrow( position, position + selfMatrix[1], 0.05, Colors.GREEN );
		m_DirectionZ = Debug.DrawArrow( position, position + selfMatrix[2], 0.05, Colors.BLUE );
	} 
	
	void Debug_RemoveDirections()
	{
		if(m_DirectionX) 
		{
			m_DirectionX.Destroy();
			m_DirectionX = null;
		}
		
		if(m_DirectionY) 
		{
			m_DirectionY.Destroy();
			m_DirectionY = null;
		}
		
		if(m_DirectionZ)
		{
			m_DirectionZ.Destroy();
			m_DirectionZ = null;
		}
	}
#endif
}