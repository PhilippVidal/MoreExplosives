//Base class for all explosives 
//Explosives can have different types of triggers 
//Assigned Explosion Behaviour determines 'how' it explodes
class MOE_ExplosiveBase : ItemBase
{	
	protected const string SLOT_TRIGGER = "Trigger_MOE";
	
	protected ref Timer m_DeleteTimer;	
	protected ref MOE_ExplosionBehaviourBase m_ExplosionBehaviour;
	protected ref MOE_ExplosiveData m_ConfigData;
	protected MOE_ComponentTriggerBase m_Trigger;
	protected MOE_TriggerLight m_TriggerLight;
	
	protected int m_State, m_PrevState; 
	
	//used for keeping track of who interacted with the trigger/explosive 
	protected string m_PlayerName, m_PlayerSteam64;

	protected bool m_Stored_IsArmed, m_Stored_IsMounted;
	
	protected Object m_PlacementTarget;
	protected int m_PlacementTargetComponent;
	
	void MOE_ExplosiveBase() 
	{ 
		Init_MOE(); 
	}
	
	void ~MOE_ExplosiveBase()
	{
		Fini_MOE();
#ifdef MOE_DEBUG_EXPLOSIVE
		Debug_RemoveDirections();
#endif
	}	
	
	void Init_MOE()
	{
		RegisterNetSyncVariableInt("m_State", 0, 7);
		m_DeleteTimer = new Timer();	
		UpdateAttachmentVisuals();			
	}

	void Fini_MOE()
	{
		if(m_ExplosionBehaviour)
		{
			delete m_ExplosionBehaviour;
		}
		
		DestroyTriggerLight();
	}
	
	override void DeferredInit()
	{
		super.DeferredInit();
		InitializeState();
	}
	
	MOE_TriggerLight CreateTriggerLight(notnull MOE_ComponentTriggerBase trigger)
	{
		if(m_TriggerLight)
		{
			DestroyTriggerLight();
		}
		
		m_TriggerLight = MOE_TriggerLight.Cast(ScriptedLightBase.CreateLight(MOE_TriggerLight, GetPosition()));	
		m_TriggerLight.AttachOnObject(this, GetLightPosition_Local());
		m_TriggerLight.InitLight(this, trigger);	
		return m_TriggerLight;
	}
	
	string GetLedColorTexture()
	{
		vector color = "1 1 1";
		if(ValidateTrigger())
		{
			if(m_TriggerLight)
			{
				color = m_TriggerLight.GetColorToDisplay();
			}
		}
		
		return MiscGameplayFunctions.GetColorString(color[0], color[1], color[2], 1);
	}
	
	void SetLedState(bool state)
	{				
		
	}
	
	MOE_TriggerLight GetTriggerLight()
	{
		return m_TriggerLight;
	}
	
	bool IsTriggerLightOn()
	{
		return m_TriggerLight && m_TriggerLight.IsOn();
	}
	
	void DestroyTriggerLight()
	{
		if(m_TriggerLight)
		{
			m_TriggerLight.TurnOff();
			m_TriggerLight.Destroy();
			m_TriggerLight = null;
		}
	}
	
	void InitializeState()
	{	
		m_State |= MOE_EExplosiveStateFlags.INITIALIZED;
		SetSynchDirty();
	}
	
	
	void SetPlacementTarget(Object target)
	{
		m_PlacementTarget = target;
	}
	
	Object GetPlacementTarget()
	{
		return m_PlacementTarget;
	}

	void SetPlacementTargetComponent(int component)
	{
		m_PlacementTargetComponent = component;
	}
	
	int GetPlacementTargetComponent()
	{
		return m_PlacementTargetComponent;
	}
	
	void SetIsArmed(bool isArmed)
	{
		m_State = HDSN_MiscFunctions.SetBits(m_State, MOE_EExplosiveStateFlags.ARMED, isArmed);	
		SetSynchDirty();	
		
		SetTriggerSlotLock(isArmed);	
	}
	
	void SetIsMounted(bool isMounted) 
	{
		m_State = HDSN_MiscFunctions.SetBits(m_State, MOE_EExplosiveStateFlags.MOUNTED, isMounted);	
		SetSynchDirty();
	}

	int GetState()
	{
		bool isArmed = IsArmed();
		if(!isArmed)
		{
			return MOE_EExplosiveStates.DISARMED;
		}
		
		if(isArmed && IsTriggerActive())
		{
			return MOE_EExplosiveStates.ACTIVE;	
		}
		
		return MOE_EExplosiveStates.ARMED;
	}

	
	void Arm(EntityAI usedTool = null)
	{
		SetIsArmed(true);
		OnArmed(usedTool);
	}
	
	void Disarm(EntityAI usedTool = null)
	{
		SetIsArmed(false);
		OnDisarmed(usedTool);
	}
	
	void Defuse(EntityAI usedTool = null, PlayerBase player = null)
	{		
		if(ValidateTrigger())
		{
			m_Trigger.Deactivate(usedTool);
			OnDefused(usedTool, player);
		}
	}
	
	void Mount(Object placementTarget, int hitComponent, vector position, vector orientation)
	{
		SetPlacementTarget(placementTarget);
		SetPlacementTargetComponent(hitComponent);
		SetIsMounted(true);
		OnMounted();
			
		if(!GetMOESettings().IsParentExplosivesToTargetEnabled)
		{
			return;
		}
		
		BaseBuildingBase baseBuildingBase;
		if(CastTo(baseBuildingBase, placementTarget) && !baseBuildingBase.AllowsExplosiveAttaching_MOE(hitComponent))
		{
			return;
		}
		
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(AttachExplosive, 500, false, placementTarget, hitComponent, position, orientation);		
	}

	void Dismount()
	{
		SetPlacementTarget(null);
		SetIsMounted(false);
		OnDismounted();
		
		IEntity parent = GetParent();
		if(parent)
		{
			parent.RemoveChild(this, true);
		}
		
		if(m_TriggerLight)
		{
			m_TriggerLight.AttachOnObject(this, GetLightPosition_Local());
		}
	}
	
	bool AttachExplosive(Object target, int component, vector position, vector orientation)
	{
		if(!target)
		{
			return false;
		}
		
		int pivot = HDSN_MiscFunctions.GetPivotFromComponent(target, component);
	
		vector targetTransformMat[4];	
		if (pivot == -1)
		{
			target.GetTransform(targetTransformMat);
		}
		else
		{
			target.GetBoneTransformWS(pivot, targetTransformMat);
		}
		
		
		vector transformMat[4];		
		Math3D.YawPitchRollMatrix(orientation, transformMat);
		transformMat[3] = position;
		
		Math3D.MatrixInvMultiply4(targetTransformMat, transformMat, transformMat);
		Math3D.MatrixOrthogonalize4(transformMat);
		SetTransform(transformMat);		
		
		return target.AddChild(this, pivot);
	}
	
	protected void SetTriggerSlotLock(bool value)
	{
		GetInventory().SetSlotLock(InventorySlots.GetSlotIdFromString(SLOT_TRIGGER), value);
	}
	
	

	protected bool CreateExplosionBehaviour()
	{
		string behaviourName = GetExplosionBehaviourType();
		typename behaviour = behaviourName.ToType();
		
		if(behaviour && behaviour.IsInherited(MOE_ExplosionBehaviourBase))
		{
			m_ExplosionBehaviour = MOE_ExplosionBehaviourBase.Cast(behaviour.Spawn());
			return true;
		}
		
		Log_MOE(string.Format("%1::CreateExplosionBehaviour -> Failed to create explosion behaviour! Behaviour Type = %2", this, behaviourName), MOE_ELogTypes.ERROR);
		return false;
	}
	
	void InitiateExplosion(EntityAI source = null)
	{			
		if(!CreateExplosionBehaviour())
		{
			return;
		}
		
		OnExplosionInitiated(source);
		m_ExplosionBehaviour.Detonate(this);				
	}

	void DeleteExplosive(float delay = 0.25)
	{
#ifdef SERVER
		m_DeleteTimer.Run(delay, this, "DeleteSafe");	
#endif
	}
		
	bool ValidateTrigger()
	{
		return m_Trigger || ( GetInventory() && MOE_ComponentTriggerBase.Cast( FindAttachmentBySlotName(SLOT_TRIGGER) ) );
	}
	
	MOE_ComponentTriggerBase GetTrigger()
	{	
		return m_Trigger;
	}
	
	bool GetReceiver(out MOE_ComponentReceiverBase receiver)
	{				
		return ValidateTrigger() && CastTo(receiver, GetTrigger());
	}
	
	bool GetTimer(out MOE_ComponentTimerBase timer)
	{
		return ValidateTrigger() && CastTo(timer, GetTrigger());
	}
	
	//Called from OnStoreSave 
	//Can be overridden without causing potential issues with super call hierarchy OnStoreSave
	void StoreSaveData(ParamsWriteContext ctx)
	{		
		ctx.Write(IsArmed());
		ctx.Write(IsMounted());
	} 
	
	//Called from OnStoreLoad 
	//Can be overridden without causing potential issues with super call hierarchy in OnStoreLoad
	bool LoadSaveData(ParamsReadContext ctx, int version)
	{
		if (!ctx.Read(m_Stored_IsArmed))
		{
			return false;
		}

		if (!ctx.Read(m_Stored_IsMounted))
		{
			return false;
		}
		return true;
	}
	
	void EvaluateStateAfterLoad()
	{	
		bool shouldBeArmed = m_Stored_IsArmed && CanBeArmed();
		SetIsArmed(shouldBeArmed);
		
		bool shouldBeMounted = m_Stored_IsMounted && CanBeMounted() && (GetMountingMode() == MOE_EExplosiveMountingModes.EVERYWHERE);
		SetIsMounted(shouldBeMounted);
		
		UpdateAttachmentVisuals();
		InitializeState();
	}
	
	void EvaluateComponents()
	{
		if(ValidateTrigger())
		{
			m_Trigger.EvaluateState();
		}	
	}
	
	void SetInteractingPlayer(PlayerBase player)
	{
		if(player)
		{
			m_PlayerName = player.GetIdentity().GetName(); 
			m_PlayerSteam64 = player.GetIdentity().GetPlainId();
			return;
		}
		
		m_PlayerName = ""; 
		m_PlayerSteam64 = "";
	}
		
	bool GetInteractingPlayer(out string playerName, out string playerSteam64)
	{
		if(m_PlayerName != "" && m_PlayerSteam64 != "")
		{
			playerName = m_PlayerName;
			playerSteam64 = m_PlayerSteam64;
			
			return true;
		}
		
		return false;
	}
	
	//========================================
	//============= Visuals ==================
	//========================================

	//Override if explosive uses custom digit textures 
	string GetDigitPath(int digit)
	{
		return string.Format("MoreExplosives\\Data\\Models\\Textures\\Displays\\HomemadeExplosive\\%1_ca.paa", Math.Clamp(digit, 0, 9));
	}
	
	string GetDigitOffPath()
	{
		return "MoreExplosives\\Data\\Models\\Textures\\Displays\\HomemadeExplosive\\digit_off_ca.paa";
	}

	//automatic max calculation if max < 0
	protected void SetDigitsOnSelections(array<string> selections, int value, int max = 99)
	{
		if(!selections)
		{
			return;
		}
		
		int selectionCount = selections.Count();
		if(selectionCount == 0)
		{
			return;
		}
		
		if(max < 0)
		{
			max = Math.Pow(10, selectionCount) - 1;
		}
		
		array<int> digits = HDSN_MiscFunctions.GetDigitsArray(Math.Clamp(value, 0, max));
		
		//Fill with 0s
		int difference = selectionCount - digits.Count();
		for(int i = 0; i < difference; i++)
		{
			digits.Insert(0);
		}
		
		for(int idx = 0; idx < selectionCount; idx++)
		{
			int selectionIdx = GetHiddenSelectionIndex(selections[idx]);
			SetObjectTexture(selectionIdx,  GetDigitPath(digits[idx]));
		}
	}
	
	protected void SetDigitsToOff(array<string> selections)
	{
		if(!selections)
		{
			return;
		}
		
		int selectionCount = selections.Count();
		if(selectionCount == 0)
		{
			return;
		}

		for(int idx = 0; idx < selectionCount; idx++)
		{
			int selectionIdx = GetHiddenSelectionIndex(selections[idx]);
			SetObjectTexture(selectionIdx,  GetDigitOffPath());
		}
	}

	//Updates which proxies and parts are displayed on the model
	//Updated on Init, when attachment is attached/detached and when armed/disarmed
	protected void UpdateAttachmentVisuals() {}	
	
	//Initiates update for visuals of the specific triggers used on the model
	//e.g., digits on screen
	void UpdateTriggerVisuals(MOE_ComponentTriggerBase trigger)
	{
		MOE_ComponentTimerBase timer;
		if(CastTo(timer, trigger))
		{
			UpdateTimerVisuals(timer);
			return;
		}
		
		MOE_ComponentReceiverBase receiver;
		if(CastTo(receiver, trigger))
		{
			UpdateReceiverVisuals(receiver);
			return;
		}
	}
	
	void UpdateTimerVisuals(notnull MOE_ComponentTimerBase timer, int timerValueOverride = -1) {}
	
	void UpdateReceiverVisuals(notnull MOE_ComponentReceiverBase receiver) {}
		
	
	//========================================
	//============= "Events" =================
	//========================================
	
	//Called from explosion behaviour, once all explosives have been spawned
	void OnExplosionBehaviourFinished()
	{
		DeleteExplosive();
	}
	
	protected void OnArmed(EntityAI usedTool)
	{		
		if(ValidateTrigger())
		{
			m_Trigger.OnConnectedExplosiveArmed(this);
		}

		UpdateAttachmentVisuals();	
	}
	
	protected void OnDisarmed(EntityAI usedTool)
	{
		if(ValidateTrigger())
		{
			m_Trigger.OnConnectedExplosiveDisarmed(this);
		}
		
		UpdateAttachmentVisuals();
	}
	
	
	protected void OnDefused(EntityAI usedTool, PlayerBase player)
	{
		string logStr = string.Format("%1 [%2] got defused", this, GetPosition().ToString());	
		if(player)
		{
			logStr += string.Format(" by Player: %1 [%2]", player.GetIdentity().GetName(), player.GetIdentity().GetPlainId());
		}
		
		Log_MOE(logStr, MOE_ELogTypes.EXPLOSIVE);
	}
	
	protected void OnMounted() {}
	
	protected void OnDismounted() {}
	
	protected void OnExplosionInitiated(EntityAI source = null)
	{	
		string logStr = string.Format("%1 [%2] exploded", this, GetPosition().ToString());	

		Object target = GetPlacementTarget();
		if(target)
		{
			logStr += string.Format(", PlacementTarget: %1 [%2], PlacementTargetComponent: %3", target, target.GetPosition().ToString(), GetPlacementTargetComponent());
		}

		if(source)
		{		
			logStr += string.Format(", Source: %1 [%2]", source, source.GetPosition().ToString());
		}
	
		string playerName, playerSteam64;
		if(GetInteractingPlayer(playerName, playerSteam64))
		{
			logStr += string.Format(", Player: %1 [%2]", playerName, playerSteam64);	
		}
		
		Log_MOE(logStr, MOE_ELogTypes.EXPLOSIVE);
	}

	// ignoredHitDetectionFlags are used for determining what the explosion object can be used for 
	// e.g., if MOE_EHitDetectionFlags.DIRECT_TARGET is not set, the explosion can damage the target directly
	// if there is an explosion behaviour that creates multiple explosions this can be used to turn off hit detection 
	// e.g., there is a main explosion that should damage the target and there are a few other `cosmetic` explosions that shouldn't damage the placement target
	void OnExplosionObjectDetonated(MOE_ExplosionObject explosionObject, string ammo, int ignoredHitDetectionFlags)
	{
		if(!HDSN_MiscFunctions.IsBitSet(ignoredHitDetectionFlags, MOE_EHitDetectionFlags.DIRECT_TARGET) && GetMOESettings().IsDamagePlacementTargetDirectlyEnabled)
		{
			GetMOE().GetDestructionSystem().HandleDirectDamage(
				this, 
				explosionObject,
				explosionObject.GetPosition(),
				GetPlacementTarget(), 
				GetPlacementTargetComponent(), 
				"",
				ammo);
		}
		
		if(!HDSN_MiscFunctions.IsBitSet(ignoredHitDetectionFlags, MOE_EHitDetectionFlags.MANUAL_SPHERE) && GetMOESettings().AreaDamageMode == MOE_EAreaDamageModes.MANUAL_SPHERE)
		{
			GetMOE().GetDestructionSystem().DealAreaDamage(this, explosionObject, explosionObject.GetPosition(), ammo);
		}	
	}
	
	

	protected void OnTriggerAttached(EntityAI entity)
	{
		m_Trigger = MOE_ComponentTriggerBase.Cast(entity);
	}
	
	protected void OnTriggerDetached(EntityAI entity)
	{		
		m_Trigger = null;
		DestroyTriggerLight();
	}

	void OnTriggerLightTurnedOn()
	{
		if(ValidateTrigger())
		{
			m_Trigger.OnTriggerLightTurnedOn();
		}
	}
	

	void OnTriggerLightTurnedOff()
	{
		if(ValidateTrigger())
		{
			m_Trigger.OnTriggerLightTurnedOff();
		}
	}
	
	//========================================
	//========== Conditional Functions =======
	//========================================
	
	bool HasTimer()
	{
		if(m_Trigger && m_Trigger.IsInherited(MOE_ComponentTimerBase))
		{
			return true;
		}
		
		MOE_ComponentTimerBase timer;
		return GetTimer(timer);
	}
	
	bool HasReceiver()
	{
		if(m_Trigger && m_Trigger.IsInherited(MOE_ComponentReceiverBase))
		{
			return true;
		}
		
		MOE_ComponentReceiverBase receiver;
		return GetReceiver(receiver);
	}
	
	bool HasTrigger()
	{
		return m_Trigger || FindAttachmentBySlotName(SLOT_TRIGGER);		
	}
	
	bool IsTriggerSlotLocked()
	{
		return GetInventory().GetSlotLock(InventorySlots.GetSlotIdFromString(SLOT_TRIGGER));
	}
	
	bool CanExplosiveBeDamaged()
	{
		if(IsArmed() && !CanTakeDamageWhileArmed())
		{
			return false;
		}
		
		if(IsMounted() && !CanTakeDamageWhileMounted())
		{
			return false;
		}
		
		return true;
	}
	
	bool CanExplode()
	{
		if(IsRuined())
		{
			return false;
		}
		
		if(!IsArmed())
		{
			return false;
		}
		
		if(HasToBeMounted() && !IsMounted())
		{
			return false;
		}
		
		return true;
	}
	
	bool CanBeMoved()
	{
		return !IsMounted();
	}
	
	bool CanBeMounted()
	{
		return IsArmed();
	}
	
	bool CanBeMountedOn(Object placementTarget, int component, vector position)
	{		
		MOE_DestructionSystem destructionSystem = GetMOE().GetDestructionSystem();
		return destructionSystem.IsPlacementAllowed(this, placementTarget, component, position) && destructionSystem.IsExplosiveCompatible(this, placementTarget, component, position);	
	}
	
	bool CanBeDismounted()
	{
		if(ValidateTrigger())
		{
			return IsMounted() && !m_Trigger.IsActive(); 
		}
		
		return IsMounted();
	}

	bool IsStateInitialized() 
	{ 
		return HDSN_MiscFunctions.IsBitSet(m_State, MOE_EExplosiveStateFlags.INITIALIZED); 
	}
	
	bool IsArmed() 
	{ 
		return HDSN_MiscFunctions.IsBitSet(m_State, MOE_EExplosiveStateFlags.ARMED); 
	}
	
	bool IsMounted() 
	{ 
		return HDSN_MiscFunctions.IsBitSet(m_State, MOE_EExplosiveStateFlags.MOUNTED); 
	}
	
	bool IsTriggerActive()
	{	
		return ValidateTrigger() && m_Trigger.IsActive();
	}
	
	bool CanBeArmed()
	{
		//Can't be armed if it's broken
		if(IsRuined())
		{
			return false;
		}
		
		//Can't be armed if the explosive is placed
		if(IsMounted())
		{
			return false;
		}	

		if(!ValidateTrigger() || !m_Trigger.AllowsArming())
		{
			return false;
		}	
		
		if(IsArmed())
		{
			return false;
		}

		return true;
	}
	
	//Defusing = deactivating the trigger component
	//(e.g., stopping the countdown on a timer if it is running (is active))
	bool CanBeDefused(EntityAI usedTool = null)
	{
		if(!IsArmed())
		{
			return false;
		}
		
		return ValidateTrigger() && m_Trigger.IsActive() && m_Trigger.CanBeDeactivated(usedTool);
	}
	
	//Disarming = removing the trigger component
	bool CanBeDisarmed()
	{
		//DrawPlacementInfo();
		if(IsMounted())
		{
			return false;
		}	
		
		if(!IsArmed())
		{
			return false;
		}
			
		return ValidateTrigger() && !m_Trigger.IsActive();
	}
	
	bool CanBeArmedWith(EntityAI usedTool)
	{		
		return IsToolAllowed(usedTool, GetArmTools());
	}
	
	bool CanBeDisarmedWith(EntityAI usedTool)
	{
		return IsToolAllowed(usedTool, GetDisarmTools());
	}
	
	bool CanBeDefusedWith(EntityAI usedTool)
	{
		return IsToolAllowed(usedTool, GetDefuseTools());
	}
	
	bool IsToolAllowed(EntityAI usedTool, array<string> allowedTools)
	{
		if(!allowedTools || allowedTools.Count() == 0)
		{
			return true;
		}
	
		string usedToolType = "Unarmed";	
		if(usedTool)
		{
			usedToolType = usedTool.GetType();
		}
		
		foreach(string tool : allowedTools)
		{
			if(tool == usedToolType)
			{
				return true;
			}
		}

		return false;
	}
	
	bool IsTriggerAllowed(EntityAI attachment)
	{		
		//Only MOE_ComponentTriggerBase can be attached as a trigger	
		if(!attachment.IsInherited(MOE_ComponentTriggerBase))
		{
			return false;
		}
		
		array<string> allowedTriggers = GetAttachableTriggers();	
		foreach(string trigger : allowedTriggers)
		{
			if(attachment.GetType() == trigger)
			{
				return true;
			}
		}
		
		return false;
	}
	
	
	//========================================
	//===== Placement & Offset Functions =====
	//========================================
	
	//Where should the explosion originate from?
	vector GetExplosionPosition()
	{
		return GetPositionWithPlacementOffset();
	}
	
	//Where should the explosion point? -> used for particle effect
	vector GetExplosionOrientation()
	{	
		Man player = GetHierarchyRootPlayer();	
		if(player)
		{
			return (-player.GetDirectionUp()).VectorToAngles();
		}

		vector mat[4];
		Math3D.DirectionAndUpMatrix(GetPlacementDirectionForward(), GetPlacementDirectionUp(), mat);
		return Math3D.MatrixToAngles(mat);
	}
	
	//Translation offset in model space
	vector GetPlacementPositionOffset()
	{
		return "0 0 0";
	}
	
	//Yaw, Pitch, Roll to apply before placement rotation
	vector GetPlacementRotationOffset()
	{
		return "0 0 0";
	}
	
	
	//Position that has been adjusted with the placement offset
	vector GetPositionWithPlacementOffset()
	{
		return GetPosition() - VectorToParent(GetPlacementPositionOffset());
	}
	
	//Rotation Matrix to apply the rotation offset for placement
	void GetPlacementOffsetRotationMatrix(out vector rotMat[3])
	{
		Math3D.YawPitchRollMatrix(GetPlacementRotationOffset(), rotMat);
	}
	
	
	//Aside direction of the charge when placed 
	vector GetPlacementDirectionAside()
	{
		vector offsetRotMat[3];
		GetPlacementOffsetRotationMatrix(offsetRotMat);
		return VectorToParent(vector.Aside.Multiply3(offsetRotMat));
	}
	
	//Up direction of the charge when placed 
	vector GetPlacementDirectionUp()
	{
		vector offsetRotMat[3];
		GetPlacementOffsetRotationMatrix(offsetRotMat);
		return VectorToParent(vector.Up.Multiply3(offsetRotMat));
	}
	
	//Forward direction of the charge when placed 
	vector GetPlacementDirectionForward()
	{
		vector offsetRotMat[3];
		GetPlacementOffsetRotationMatrix(offsetRotMat);
		return VectorToParent(vector.Forward.Multiply3(offsetRotMat));
	}
	
	
	string GetLightMemoryPoint()
	{
		return "";
	}
	
	string GetTriggerMemoryPoint()
	{
		return "";
	}
	
	vector GetLightPosition_Local()
	{
		string lightMemoryPoint = GetLightMemoryPoint();
		if(lightMemoryPoint != "" && MemoryPointExists(lightMemoryPoint))
		{
			return GetMemoryPointPos(lightMemoryPoint);
		}
		
		return "0 0 0.2";
	}
	
	vector GetTriggerPosition_Local()
	{
		string triggerMemoryPoint = GetTriggerMemoryPoint();
		if(triggerMemoryPoint != "" && MemoryPointExists(triggerMemoryPoint))
		{
			return GetMemoryPointPos(triggerMemoryPoint);
		}
		
		return vector.Zero;
	}
	
	vector GetLightPosition_World()
	{
		return ModelToWorld(GetLightPosition_Local());
	}

	vector GetTriggerPosition_World()
	{
		return ModelToWorld(GetTriggerPosition_Local());
	}
	
	//========================================
	//========== Modded Functions ============
	//========================================
	
	override bool EEOnDamageCalculated(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{	 
		return super.EEOnDamageCalculated(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef) && CanExplosiveBeDamaged();
	}
	
	override void OnActivatedByItem(notnull ItemBase item)
	{
		super.OnActivatedByItem(item);
		
#ifndef SERVER
			return;
#endif
		
		if(CanExplode())
		{		
			InitiateExplosion(item);
		}
	}	
	
	override void EEItemAttached(EntityAI item, string slot_name)
	{
		super.EEItemAttached(item, slot_name);
				
		if(slot_name == SLOT_TRIGGER)
		{
			OnTriggerAttached(item);
		}
		
		UpdateAttachmentVisuals();
	}
	
	override void EEItemDetached(EntityAI item, string slot_name)
	{
		super.EEItemDetached(item, slot_name);
		
		if(slot_name == SLOT_TRIGGER)
		{
			OnTriggerDetached(item);
		}
		
		UpdateAttachmentVisuals();
	}
	
	override void EEHealthLevelChanged(int oldLevel, int newLevel, string zone)
	{
		super.EEHealthLevelChanged(oldLevel, newLevel, zone);

#ifndef SERVER
			return;
#endif
		
		if(newLevel != GameConstants.STATE_RUINED)
		{
			return;
		}
		
		if(Math.RandomFloat01() < GetExplodeOnRuinedChance())
		{							
			InitiateExplosion();
		}
		
		//Explosives and their attachments get deleted after the explosion, 
		//so this only really applies to when an explosive is damaged otherwise (e.g., shot directly)
		if(Math.RandomFloat01() < GetRuinAttachmentsOnRuinedChance())
		{
			for (int attachmentIdx = 0; attachmentIdx < GetInventory().AttachmentCount(); attachmentIdx++)
			{
				ItemBase attachment = ItemBase.Cast(GetInventory().GetAttachmentFromIndex(attachmentIdx));
				if (attachment)
				{
					attachment.UnlockFromParent();
					attachment.SetHealth("", "", 0.0);
				}
			}
		}		
	}
	
	override bool CanReceiveAttachment(EntityAI attachment, int slotId)
	{
		if(slotId == InventorySlots.GetSlotIdFromString(SLOT_TRIGGER))
		{
			if(!IsTriggerAllowed(attachment))
			{
				return false;
			}		
		}
		
		return super.CanReceiveAttachment(attachment, slotId);
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		RemoveAction(ActionTakeItemToHands);
		
		//Arming, Disarming & Defusing
		AddAction(MOE_ActionManipulateExplosive);					//In world
		AddAction(MOE_ActionManipulateExplosiveInHands);			//In hands - unarmed 
		AddAction(MOE_ActionManipulateExplosiveInHandsWithTool);	//In hands - with tool		
		
		//Placement 
		AddAction(MOE_ActionMountExplosive);
		AddAction(MOE_ActionMountExplosiveInvalid);
		
		AddAction(MOE_ActionDismountExplosive);
		AddAction(MOE_ActionToggleExplosivePlacement);
	
		//Timer
		AddAction(MOE_ActionManipulateTimer);
		AddAction(MOE_ActionManipulateTimerInHands);
		AddAction(MOE_ActionOpenTriggerComponentUI);
		AddAction(MOE_ActionOpenTriggerComponentUIInHands);
		
		
	}
	
	override bool NameOverride(out string output)
	{	
		GetGame().ObjectGetDisplayName(this, output);	
		
		if(IsMounted())
		{
			output += " [#STR_MOE_Mounted]";
			return true;
		}
		
		if(IsArmed())
		{
			output += " [#STR_MOE_Armed]";
			return true;
		}
		
		return false;
	}
	
	
	protected void GenerateDescriptionString(out string output)
	{
		output = ConfigGetString("descriptionShort") + "\n#STR_MOE_Explosive_Usage_Description\n";

		if(GetMOESettings().IsRaidSchedulingEnabled)
		{
			output += string.Format("\n%1: %2", "#STR_MOE_Raiding_Allowed_Schedule", MoreExplosives.GetYesNo(GetMOE().GetRaidScheduling().IsInSchedule()));
		}

		MOE_ExplosiveData explosiveData = GetConfigData();	
		if(explosiveData.CanOnlyRaidDoors || GetMOESettings().IsDoorRaidOnlyEnabled)
		{
			output += string.Format("\n%1.", "#STR_MOE_Explosive_CanOnlyRaidDoors");
		}

		if(explosiveData.HasToBeMounted)
		{
			output += string.Format("\n%1.", "#STR_MOE_Explosive_HasToBeMounted");
		}

		if(explosiveData.CanOnlyDamagePlacementTarget)
		{
			output += string.Format("\n%1.", "#STR_MOE_Explosive_CanOnlyDamagePlacementTarget");
		}

			
		output += string.Format("\n%1:\t%2s", "#STR_MOE_Explosive_TimeToArm", explosiveData.TimeToArm);
		output += string.Format("\n%1:\t%2s", "#STR_MOE_Explosive_TimeToDisarm", explosiveData.TimeToDisarm);
		output += string.Format("\n%1:\t%2s", "#STR_MOE_Explosive_TimeToDefuse", explosiveData.TimeToDefuse);
		output += string.Format("\n%1:\t%2s", "#STR_MOE_Explosive_TimeToMount", explosiveData.TimeToMount);
		output += string.Format("\n%1:\t%2s", "#STR_MOE_Explosive_TimeToDismount", explosiveData.TimeToDismount);
	

		int maxDisplayed = 3;
		array<string> tools = explosiveData.ArmTools;
		if(tools && tools.Count() != 0)
		{
			output += string.Format(
						"\n%1:\n%2", 
						"#STR_MOE_Explosive_ArmTools", 
						MoreExplosives.GetToolDisplayNamesFormatted(tools, maxDisplayed));
		}

		tools = explosiveData.DisarmTools;
		if(tools && tools.Count() != 0)
		{
			output += string.Format(
						"\n%1:\n%2", 
						"#STR_MOE_Explosive_DisarmTools", 
						MoreExplosives.GetToolDisplayNamesFormatted(tools, maxDisplayed));
		}
		
		
		tools = explosiveData.DefuseTools;
		if(tools && tools.Count() != 0)
		{
			output += string.Format(
						"\n%1:\n%2", 
						"#STR_MOE_Explosive_DefuseTools", 
						MoreExplosives.GetToolDisplayNamesFormatted(tools, maxDisplayed));
		}	
	}

	override bool DescriptionOverride(out string output)
	{		
		GenerateDescriptionString(output);
		return true;
	}
	
	override bool IsExplosive()
	{
		return true;
	}	
	
	override bool CanPutInCargo(EntityAI parent)
	{	
		return CanBeMoved();
	}		
	override bool CanPutIntoHands(EntityAI parent)
	{
		return CanBeMoved();
	}
	
	override bool IsDeployable()
	{
		return CanBeMoved();
	}
	
	override bool IsTakeable()
	{
		return CanBeMoved() && super.IsTakeable();
	}
	
	override void OnStoreSave(ParamsWriteContext ctx)
	{
		super.OnStoreSave(ctx);		
		StoreSaveData(ctx);
	}
	
	override bool OnStoreLoad(ParamsReadContext ctx, int version)
	{		
		return super.OnStoreLoad(ctx, version) && LoadSaveData(ctx, version);
	}
	
	//EEOnAfterLoad() -> only called by server on boot-up
	//AfterStoreLoad() -> called on server on boot-up + when player inventory is loaded 
	// - on boot-up order: EEItemAttached -> OnStoreLoad -> AfterStoreLoad -> EEOnAfterLoad => Item (i.e., trigger) attached before anything else!
	// - on player inv. load order: OnStoreLoad -> EEItemAttached -> AfterStoreLoad called (EEOnAfterLoad does not get called at all)
	override void AfterStoreLoad()
	{
		super.AfterStoreLoad();
		EvaluateStateAfterLoad();
		EvaluateComponents();	
	}
	
	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		
		if(m_PrevState == m_State) 
		{
			return;
		}
		
		bool wasInitializedBefore = HDSN_MiscFunctions.IsBitSet(m_PrevState, MOE_EExplosiveStateFlags.INITIALIZED);	
		if(wasInitializedBefore)
		{
			bool wasArmedBefore = HDSN_MiscFunctions.IsBitSet(m_PrevState, MOE_EExplosiveStateFlags.ARMED);
			if(IsArmed() && !wasArmedBefore)	
			{
				OnArmed(null);
			}	
			else if(!IsArmed() && wasArmedBefore)	
			{
				OnDisarmed(null);
			}
			
			bool wasMountedBefore = HDSN_MiscFunctions.IsBitSet(m_PrevState, MOE_EExplosiveStateFlags.MOUNTED);
			if(IsMounted() && !wasMountedBefore)
			{
				OnMounted();
			}	
			else if(!IsMounted() && wasMountedBefore)
			{
				OnDismounted();
			} 
		}

		if(IsStateInitialized() && !wasInitializedBefore) 
		{
			EvaluateComponents();
		}

		m_PrevState = m_State;
	}
	

	//========================================
	//======= Access Cached Config Data ======
	//========================================
	
	void LoadConfigData()
	{
		m_ConfigData = GetMOEConfig().GetExplosiveData(GetType());
	}
	
	MOE_ExplosiveData GetConfigData()
	{
		if(!m_ConfigData)
		{
			LoadConfigData();
		}
		
		return m_ConfigData;
	}
	
	float GetTimeToArm()
	{
		return GetConfigData().TimeToArm;
	}
	
	float GetTimeToDisarm()
	{
		return GetConfigData().TimeToDisarm;
	}
	
	float GetTimeToDefuse()
	{
		return GetConfigData().TimeToDefuse;
	}
	
	float GetTimeToMount()
	{
		return GetConfigData().TimeToMount;
	}
	
	float GetTimeToDismount()
	{
		return GetConfigData().TimeToDismount;
	}
	
	float GetExplodeOnRuinedChance()
	{
		return GetConfigData().ExplodeOnRuinedChance;
	}
	
	float GetRuinAttachmentsOnRuinedChance()
	{
		return GetConfigData().RuinAttachmentsOnRuinedChance;
	}
	
	bool HasToBeMounted()
	{		
		return GetConfigData().HasToBeMounted;
	}
	
	bool CanTakeDamageWhileMounted()
	{		
		return GetConfigData().CanTakeDamageWhileMounted;
	}
	
	bool CanTakeDamageWhileArmed()
	{		
		return GetConfigData().CanTakeDamageWhileArmed;
	}
	
	bool CanOnlyRaidDoors()
	{		
		return GetConfigData().CanOnlyRaidDoors;
	}
	
	bool CanOnlyDamagePlacementTarget()
	{
		return GetConfigData().CanOnlyDamagePlacementTarget;
	}

	bool CanOnlyMountDuringSchedule()
	{
		return GetConfigData().CanOnlyMountDuringSchedule;
	}

	int GetMountingMode()
	{
		return GetConfigData().MountingMode;
	}
	
	string GetExplosionBehaviourType()
	{
		return GetConfigData().ExplosionBehaviourType;
	}
	
	string GetAmmoType()
	{
		return GetConfigData().AmmoType;
	}
	
	array<string> GetAttachableTriggers()
	{
		return GetConfigData().AttachableTriggers;
	}
	
	array<string> GetArmTools()
	{
		return GetConfigData().ArmTools;
	}
	
	array<string> GetDisarmTools()
	{
		return GetConfigData().DisarmTools;
	}
	
	array<string> GetDefuseTools()
	{
		return GetConfigData().DefuseTools;
	}

	
	//========================================
	//============ Debug Functions ===========
	//========================================
	
	//GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(DrawPlacementInfo, 500);
	
#ifdef MOE_DEBUG_EXPLOSIVE	
	protected Shape m_DirectionX, m_DirectionY, m_DirectionZ;
		
	protected Shape m_AdjustedASIDE, m_AdjustedUP, m_AdjustedFORWARD;

	void Debug_DrawDirections()
	{	
		Debug_RemoveDirections();
		
		vector position = GetPositionWithPlacementOffset();
		
		vector selfMatrix[4];
		GetTransform(selfMatrix);			
		
		m_PlacementDirectionX = Debug.DrawSphere( position + selfMatrix[0], 0.05, Colors.RED );
		m_PlacementDirectionY = Debug.DrawSphere( position + selfMatrix[1], 0.05, Colors.GREEN );
		m_PlacementDirectionZ = Debug.DrawSphere( position + selfMatrix[2], 0.05, Colors.BLUE );
		
		m_AdjustedASIDE 	= Debug.DrawArrow( position, position + GetPlacementDirectionAside(), 	0.1, Colors.RED );
		m_AdjustedUP	 	= Debug.DrawArrow( position, position + GetPlacementDirectionUp(), 		0.1, Colors.GREEN );
		m_AdjustedFORWARD 	= Debug.DrawArrow( position, position + GetPlacementDirectionForward(), 	0.1, Colors.BLUE );		
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
		
		
		if(m_AdjustedASIDE)
		{
			m_AdjustedASIDE.Destroy();
			m_AdjustedASIDE = null;
		}
		
		if(m_AdjustedUP)
		{
			m_AdjustedUP.Destroy();
			m_AdjustedUP = null;
		}
		
		if(m_AdjustedFORWARD)
		{
			m_AdjustedFORWARD.Destroy();
			m_AdjustedFORWARD = null;
		}
	}
#endif


}