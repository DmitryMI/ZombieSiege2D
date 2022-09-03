// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "DamageInstance.h"
#include "DamageReceivedEventArgs.h"
#include "UnitIsDyingEventArgs.h"
#include "UnitDiedEventArgs.h"
#include "ZombieSiegePlayerState.h"
#include "ZombieSiegePlayerController.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "UnitBase.generated.h"

UCLASS()
class ZOMBIESIEGE_API AUnitBase : public APawn
{
	GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly)
	float collisionRadius = 50.0f;

	UPROPERTY(EditDefaultsOnly)
	float collisionHeight = 400.0f;

	UPROPERTY(EditAnywhere)
	float maxHealth = 100.0f;

	UPROPERTY(EditAnywhere)
	float health = 100.0f;	

	UPROPERTY(EditAnywhere)
	float speed = 0.0f;

	UPROPERTY(VisibleAnywhere)
	bool bIsAlive = true;
	
	UPROPERTY(VisibleAnywhere)
	/// <summary>
	/// Is True when health becomes 0, but before anything else happens. Can be reset to false to prevent the actual death.
	/// </summary>
	bool bIsDying = false;

	UPROPERTY(EditDefaultsOnly)
	bool assignToLocalPlayerOnSpawn;

	UPROPERTY(VisibleAnywhere)
	AZombieSiegePlayerController* owningPlayerController;

protected:

	UPROPERTY(Transient)
	UFloatingPawnMovement* movementComponent;

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	/// <summary>
	/// Used internally to start dying process for this Unit after it received damage.
	/// </summary>
	/// <param name="killingDamageInstance">DamageInstance that made health zero</param>
	virtual void BeginDying(const FDamageInstance& killingDamageInstance);

	UFUNCTION(BlueprintCallable)
	/// <summary>
	/// Used internally to kill this Unit after it received damage and nothing has prevented it's death. Must set bIsAlive to False.
	/// </summary>
	/// <param name="killingDamageInstance">DamageInstance that made health zero</param>
	virtual void FinishDying(const FDamageInstance& killingDamageInstance);

	UFUNCTION(BlueprintCallable)
	void SetMovementComponentSpeedCap(float speedCap);

public:
	AUnitBase();

	virtual void GetSimpleCollisionCylinder(float& CollisionRadius,	float& CollisionHalfHeight) const override;

	/// <summary>
	/// Gets controlling player state of this unit. Can be nullptr if unit is uncontrolled.
	/// </summary>
	/// <returns>PlayerState of a player, responsible for this unit</returns>
	AZombieSiegePlayerState* GetOwningPlayerState();

	/// <summary>
	/// Gets controlling player of this Unit. Can be nullptr if unit is uncontrolled.
	/// </summary>
	/// <returns>PlayerController responsible for this unit</returns>
	AZombieSiegePlayerController* GetOwningPlayerController();

	void SetOwningPlayer(AZombieSiegePlayerController* controller);

	UFUNCTION(BlueprintCallable)
	virtual void SetMaxSpeed(float speedArg);

	UFUNCTION(BlueprintCallable)
	virtual float GetMaxSpeed();

	UFUNCTION(BlueprintCallable)
	/// <summary>
	/// Determines if a Unit can move now. Can be used by the AI
	/// </summary>
	/// <returns>True if unit can move</returns>
	virtual bool CanMove();

	UFUNCTION(BlueprintCallable)
	/// <summary>
	/// Instantly sets unit's health to Zero and begins the death sequence.
	/// </summary>
	/// <param name="bForceDeath">If set to True, will skip BeginDying sequence and will not send OnUnitIsDying event, making death imminent</param>
	void KillUnit(bool bForceDeath);

	UFUNCTION(BlueprintCallable)
	/// <summary>
	/// Revives this Unit. Unit must have non-zero Health PRIOR to calling this function.
	/// </summary>
	void ReviveUnit();

	UFUNCTION(BlueprintCallable)
	/// <summary>
	/// Revives this Unit and sets its current health to setHealthTo
	/// </summary>
	/// <param name="setHealthTo">Unit's health will be set to this value</param>
	void ReviveUnitWithHealth(float setHealthTo);

	UFUNCTION(BlueprintCallable)
	/// <summary>
	/// Must be be called during OnUnitIsDying Event to check, if somebody has already prevented this Unit's death
	/// </summary>
	/// <returns>True if Unit is still marked as dying</returns>
	bool IsDying();

	UFUNCTION(BlueprintCallable)
	/// <summary>
	/// Check if Unit is alive 
	/// </summary>
	/// <returns>True if unit is alive</returns>
	bool IsAlive();

	UFUNCTION(BlueprintCallable)
	/// <summary>
	/// Can be called during OnUnitIsDying Event handling to prevent actual death
	/// </summary>
	/// <param name="healthValue">New health value. Must be non-zero</param>
	void PreventDeath(float healthValue);

	UFUNCTION(BlueprintCallable)
	/// <summary>
	/// Directly sets this unit's health. Will not revive dead units, but WILL kill alive ones, if healthValue is less or equal to Zero
	/// </summary>
	/// <param name="healthValue"></param>
	virtual void SetHealth(float healthValue);

	UFUNCTION(BlueprintCallable)
	/// <summary>
	/// Gets current health of this unit
	/// </summary>
	/// <returns>Current health</returns>
	float GetHealth();

	UFUNCTION(BlueprintCallable)
	/// <summary>
	/// Sets this unit's Max Health. Will kill a unit, if maxHealthValue is less of equal to Zero
	/// </summary>
	/// <param name="maxHealthValue">New max health</param>
	virtual void SetMaxHealth(float maxHealthValue);

	UFUNCTION(BlueprintCallable)
	/// <summary>
	/// Gets max health of this unit.
	/// </summary>
	/// <returns>Unit's max health</returns>
	float GetMaxHealth();

	UFUNCTION(BlueprintCallable)
	virtual bool CanAttackTarget(AUnitBase* target);
	
	UFUNCTION(BlueprintCallable)
	/// <summary>
	/// Sends a Damage Instance to a specified target. Must do all checks here: allowed targets, distance, reachability, etc.
	/// </summary>
	/// <param name="target">Target to be damaged by this Unit</param>
	/// <returns>True if attack was successful (regardless of actual damage amount)</returns>
	virtual bool AttackTarget(AUnitBase* target);

	UFUNCTION(BlueprintCallable)
	/// <summary>
	/// Receive a damage instance unconditionally.
	/// </summary>
	virtual void ReceiveDamage(const FDamageInstance& damage);

	DECLARE_EVENT_OneParam(AUnitBase, FOnDamagedReceivedEvent, FDamageReceivedEventArgs);

	/// <summary>
	/// This event triggers when this unit receives damage
	/// </summary>
	/// <returns>Event declaration</returns>
	FOnDamagedReceivedEvent& OnDamageReceived() { return onDamageReceivedEvent; }

	DECLARE_EVENT_OneParam(AUnitBase, FOnUnitIsDyingEvent, FUnitIsDyingEventArgs);

	/// <summary>
	/// This event triggers when this unit hits zero health, but is not yet considered death.
	/// Handlers of this event have a chance to prevent death.
	/// </summary>
	/// <returns>Event declaration</returns>
	FOnUnitIsDyingEvent& OnUnitIsDying() { return onUnitIsDyingEvent; }

	DECLARE_EVENT_OneParam(AUnitBase, FOnUnitDiedEvent, FUnitDiedEventArgs);

	/// <summary>
	/// This event triggers when this unit becomes dead.
	/// </summary>
	/// <returns>Event declaration</returns>
	FOnUnitDiedEvent& OnUnitDied() { return onUnitDiedEvent; }

private:
	FOnDamagedReceivedEvent onDamageReceivedEvent;
	FOnUnitIsDyingEvent onUnitIsDyingEvent;
	FOnUnitDiedEvent onUnitDiedEvent;
};
