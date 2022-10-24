// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "ResourceType.h"
#include "Macros.h"
#include "UnitDiedEventArgs.h"
#include "GenericTeamAgentInterface.h"
#include "ZombieSiegePlayerState.generated.h"

class AUnitBase;

/**
 * 
 */
UCLASS()
class ZOMBIESIEGE_API AZombieSiegePlayerState : public APlayerState, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
	TMap<EResourceType, float> storedResources;

	UPROPERTY(EditDefaultsOnly)
	uint8 playerTeamId = 0;

	UPROPERTY(VisibleAnywhere)
	TArray<AUnitBase*> controlledUnits;

protected:
	virtual void OnControlledUnitDiedHandler(const FUnitDiedEventArgs& args);
	virtual void OnControlledUnitDestroyedHandler(AUnitBase* unit);
	
public:
	UFUNCTION(BlueprintCallable)
	/// <summary>
	/// Gets amount of resource this player has
	/// </summary>
	/// <param name="type">Resource Type</param>
	/// <returns>Stored Amount</returns>
	float GetStoredResourceAmount(EResourceType type);

	UFUNCTION(BlueprintCallable)
	/// <summary>
	/// Adds a resource to the storages.
	/// </summary>
	/// <param name="type">Resource Type</param>
	/// <param name="amount">Amount to add to storage</param>
	void AddResourceToStorage(EResourceType type, float amount);

	UFUNCTION(BlueprintCallable)
	/// <summary>
	/// Removes a resource from storage. If there is insuffient amount stored, than nothing will be changed and 
	/// this function will return False
	/// </summary>
	/// <param name="type">Resource Type</param>
	/// <param name="amount">Amount of resource to remove</param>
	/// <returns>Returns false, if the storage contained less resource than requested</returns>
	bool TakeResourceFromStorage(EResourceType type, float amount);

	UFUNCTION(BlueprintCallable)
	bool TakeResourcesFromStorage(TMap<EResourceType, float> resources);

	UFUNCTION(BlueprintCallable)
	bool HasEnoughResourcesInStorage(TMap<EResourceType, float> resources);

	UFUNCTION(BlueprintCallable)
	/// <summary>
	/// Directly sets the amount of stored resource
	/// </summary>
	/// <param name="type">Resource Type</param>
	/// <param name="amount">Amount</param>
	void SetStoredResourceAmount(EResourceType type, float amount);

	UFUNCTION(BlueprintCallable)
	virtual void SetGenericTeamId(const FGenericTeamId& TeamID) override { playerTeamId = TeamID.GetId(); };

	UFUNCTION(BlueprintCallable)
	virtual FGenericTeamId GetGenericTeamId() const override{ return FGenericTeamId(playerTeamId); }

	UFUNCTION(BlueprintCallable)
	void AddToControlledUnits(AUnitBase* unit);

	UFUNCTION(BlueprintCallable)
	void RemoveFromControlledUnits(AUnitBase* unit);

	UFUNCTION(BlueprintCallable)
	const TArray<AUnitBase*>& GetControlledUnits();

	DECLARE_EVENT_TwoParams(AZombieSiegePlayerState, FOnControlledUnitDiedProxyEvent, AZombieSiegePlayerState*, const FUnitDiedEventArgs& args)
	FOnControlledUnitDiedProxyEvent& OnControlledUnitDied() { return onControlledUnitDiedProxyEvent; }

private:
	FOnControlledUnitDiedProxyEvent onControlledUnitDiedProxyEvent;
};
