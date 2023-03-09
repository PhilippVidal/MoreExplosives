#ifndef SERVER
modded class Hologram 
{
	protected bool m_RaycastHit_MOE;
	
	override void UpdateHologram(float timeslice)
	{
		m_RaycastHit_MOE = false;		
		if(m_Projection.IsInherited(MOE_ExplosiveBase))
		{
			UpdateExplosiveHologram_MOE(timeslice);
			return;
		}
		
		super.UpdateHologram(timeslice);
	}
	
	
	bool GetRaycastHit_MOE()
	{
		return m_RaycastHit_MOE;
	}
	
	protected void UpdateExplosiveHologram_MOE(float timeslice)
	{
		if (!m_Parent)
		{
			m_Player.TogglePlacingLocal();		
			return;
		}
		
		if (m_Player.IsSwimming() || m_Player.IsClimbingLadder() || m_Player.IsRaised() || m_Player.IsClimbing() || m_Player.IsRestrained() || m_Player.IsUnconscious())
		{
			m_Player.TogglePlacingLocal();		
			return;
		}

		//if (!GetUpdatePosition())
		//{
		//	return;
		//} 

		vector position, normal;
		float hitFraction;
		Object hitObject;
		
		m_RaycastHit_MOE = DoPlacementRaycast_MOE(m_Player, position, normal, hitFraction, hitObject);
		
		MOE_ExplosiveBase explosive = MOE_ExplosiveBase.Cast(m_Projection); 
		
		if(m_RaycastHit_MOE && explosive)
		{
			SetProjectionOrientation( GetExplosivePlacementOrientation_MOE(explosive, position, normal) );
			SetProjectionPosition( GetExplosivePlacementPosition_MOE(explosive, position, normal) );
			
#ifdef MOE_DEBUG_PLACEMENT
			Debug_DrawExplosivePlacementVectors();
		}
		else 
		{
			Debug_RemoveExplosivePlacementVectors();
#endif
		}
	
		m_Projection.SetInvisible(!m_RaycastHit_MOE);

		//Don't think these are really needed but whatever
		EvaluateCollision();
		RefreshTrigger();
		CheckPowerSource();
		
			
		RefreshVisual();

		m_Projection.OnHologramBeingPlaced(m_Player);
	}
	
	protected bool DoPlacementRaycast_MOE(notnull PlayerBase player, out vector hitPosition, out vector hitNormal, out float hitFraction, out Object hitObject, Object ignore = null)
	{
		
		float maxProjectionDistance = MOE_Constants.DISTANCE_MOUNT_EXPLOSIVE;
		vector from, to; 
		MiscGameplayFunctions.GetHeadBonePos(player, from);
		vector camDirection = GetGame().GetCurrentCameraDirection();
		
		//A-S-S => law of sines
		if(DayZPlayerCamera3rdPerson.Cast(player.GetCurrentCamera()))
		{	
			float a, b, c; 					//Triangle sides
			float a_A, a_B, a_C, sin_a_B; 	//Triangle angles
			vector camPosition = GetGame().GetCurrentCameraPosition();	
			vector camToHead = from - camPosition;	
			
			a = maxProjectionDistance; 	
			b = camToHead.Length();
			a_A = HDSN_Math.AngleBetweenVectorsRAD(camToHead.Normalized(), camDirection);
			sin_a_B = (b * Math.Sin(a_A)) / a;
			a_B = Math.Asin(sin_a_B);		
			a_C = Math.PI - a_A - a_B;		
			c = (b / sin_a_B) * Math.Sin(a_C);
	
			to = from + ((camPosition + camDirection * c) - from);	
		}
		else 
		{
			to = from + camDirection * maxProjectionDistance;
		}

		return DayZPhysics.RayCastBullet(from, to, MOE_Constants.PlacementCollisionLayers, ignore, hitObject, hitPosition, hitNormal, hitFraction);
	}
	
	protected vector GetExplosivePlacementPosition_MOE(MOE_ExplosiveBase explosive, vector hitPosition, vector hitNormal)
	{
		vector modelOffset = explosive.GetPlacementPositionOffset();		
		if(modelOffset == vector.Zero)
		{
			return hitPosition;
		}

		return hitPosition + explosive.VectorToParent(modelOffset);
	}
	
	protected vector GetExplosivePlacementOrientation_MOE(MOE_ExplosiveBase explosive, vector hitPosition, vector hitNormal)
	{	
		//Calculate angle difference to horizontal plane
		float angleToVerticalAxis = HDSN_Math.AngleBetweenVectorsDEG(vector.Up, hitNormal);
		float angleToHorizontalPlane = 90 - angleToVerticalAxis;
		float absAngle = Math.AbsFloat(angleToHorizontalPlane);
		
		//Get horizontal direction vector of the camera
		vector cameraDirection = GetGame().GetCurrentCameraDirection();
		cameraDirection[1] = 0;
		
		vector localRotationOffset[3];
		explosive.GetPlacementOffsetRotationMatrix(localRotationOffset);
		
		vector localRotationOffset_Inv[3];
		HDSN_Math.TransposeMatrix3(localRotationOffset, localRotationOffset_Inv);
		
		vector placementRotation[3];
		vector placementRotationWithOffset[3];
		
		//Almost perfect vertical normal -> leads to issues with calculation of angles 
		//(explosives spin around while placing, are flipped horizontaly)
		if(absAngle > 89)
		{
			//Get target angle to rotate around x (-90 or 90 degrees, depending on whether normal is (0, 1, 0) or (0, -1, 0))
			vector placementAngles; 
			placementAngles[1] = HDSN_Math.SignedAngleBetweenVectorsDEG(hitNormal, vector.Forward, vector.Aside);
			
			//Convert angles to rotation matrix
			Math3D.YawPitchRollMatrix(placementAngles, placementRotation);	
			
			//Apply local rotation offset first, then apply placement rotation 
			//the resulting rotation leads to the charge being aligned with the targeted surface 
			Math3D.MatrixMultiply3(placementRotation, localRotationOffset_Inv, placementRotationWithOffset);	
			
			//Adjust the orientation for the view direction of the player 
			//(i.e., screen on model is always facing the player, making the digits easily readable)
			return AdjustOrientationForViewDirection_MOE(cameraDirection, hitNormal, placementRotation, placementRotationWithOffset);
		}
			
		//If the normal of the targeted surface is not almost perfectly vertical 
		//the hit normal can be used directly for the placement rotation 
		Math3D.YawPitchRollMatrix(hitNormal.VectorToAngles(), placementRotation);
		
		//Apply local rotation offset first, then apply placement rotation 
		Math3D.MatrixMultiply3(placementRotation, localRotationOffset_Inv, placementRotationWithOffset);
		
		//Targeted surface is horizontal (i.e., slope is under set angle threshold)
		//=> no further adjustment needed, orientation for "up" direction of explosive 
		//happens based on the global up-vector 
		if(absAngle < MOE_Constants.ANGLE_PLACEMENT_ADJUSTMENT_THRESHOLD)
		{
			return Math3D.MatrixToAngles(placementRotationWithOffset);
		}
		
		//Adjust the orientation for the view direction of the player 
		//(i.e., screen on model is always facing the player, making the digits easily readable)
		return AdjustOrientationForViewDirection_MOE(cameraDirection, hitNormal, placementRotation, placementRotationWithOffset);
	}
	
	protected vector AdjustOrientationForViewDirection_MOE(vector viewDirection, vector hitNormal, vector placementRotationMat[3], vector placementRotationWithOffset[3])
	{
		//Project on plane defined by normal
		vector projectionOnPlane = HDSN_Math.ProjectOnPlane(viewDirection, hitNormal).Normalized();
		
		//Flip projection vector if upside down -> makes it so that explosive faces the right way when placing on ceilings
		float angleToHorizontalPlane = 90 - HDSN_Math.AngleBetweenVectorsDEG(vector.Up, hitNormal);
		if(angleToHorizontalPlane < 0)
		{
			projectionOnPlane = projectionOnPlane * -1;
		}
		
		//Get up-vector that is rotated with the placement rotation (i.e., is perpenticular to hit normal) 
		//and calculate the angle to rotate the explosive around the hit normal
		vector rotatedUp = vector.Up.Multiply3(placementRotationMat);
		float signedAngle = HDSN_Math.SignedAngleBetweenVectorsDEG(projectionOnPlane, rotatedUp, hitNormal);
		
		//Get rotation matrix for rotation around hit normal
		vector adjustementRot[3];
		HDSN_Math.RotationAroundAxis(hitNormal, -signedAngle, adjustementRot);
		
		//Apply rotation around hit normal
		vector adjustmentResult[3];
		Math3D.MatrixMultiply3(adjustementRot, placementRotationWithOffset, adjustmentResult);
		
		return Math3D.MatrixToAngles(adjustmentResult);
	}

#ifdef MOE_DEBUG_PLACEMENT
	protected Shape m_ExplosiveASIDE, m_ExplosiveUP, m_ExplosiveFORWARD;
	protected void Debug_DrawExplosivePlacementVectors()
	{
		Debug_RemoveExplosivePlacementVectors();
		
		MOE_ExplosiveBase explosive = MOE_ExplosiveBase.Cast(m_Projection); 
		if(explosive)
		{
			vector position 	= explosive.GetPositionWithPlacementOffset();
			m_ExplosiveASIDE 	= Debug.DrawArrow( position, position + explosive.GetPlacementAsideDirection(), 0.5, Colors.RED );
			m_ExplosiveUP 		= Debug.DrawArrow( position, position + explosive.GetPlacementUpDirection(), 0.5, Colors.GREEN );
			m_ExplosiveFORWARD 	= Debug.DrawArrow( position, position + explosive.GetPlacementForwardDirection(), 0.5, Colors.BLUE );
		}
	}

	protected void Debug_RemoveExplosivePlacementVectors()
	{
		if(m_ExplosiveASIDE)
		{
			m_ExplosiveASIDE.Destroy();
			m_ExplosiveASIDE = null;
		}
		
		if(m_ExplosiveUP)
		{
			m_ExplosiveUP.Destroy();
			m_ExplosiveUP = null;
		}
		
		if(m_ExplosiveFORWARD)
		{
			m_ExplosiveFORWARD.Destroy();
			m_ExplosiveFORWARD = null;
		}
	}
#endif
	
	void ~Hologram()
	{
#ifdef MOE_DEBUG_PLACEMENT
		Debug_RemoveExplosivePlacementVectors();		
#endif
	}
}
#endif