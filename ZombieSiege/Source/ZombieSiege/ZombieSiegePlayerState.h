// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "ResourceType.h"
#include "ZombieSiegePlayerState.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIESIEGE_API AZombieSiegePlayerState : public APlayerState
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
	TMap<EResourceType, int> storedResources;
	
public:
	UFUNCTION(BlueprintCallable)
	/// <summary>
	/// Gets amount of resource this player has
	/// </summary>
	/// <param name="type">Resource Type</param>
	/// <returns>Stored Amount</returns>
	int GetStoredResourceAmount(EResourceType type);

	UFUNCTION(BlueprintCallable)
	/// <summary>
	/// Adds a resource to the storages.
	/// </summary>
	/// <param name="type">Resource Type</param>
	/// <param name="amount">Amount to add to storage</param>
	void AddResourceToStorage(EResourceType type, int amount);

	UFUNCTION(BlueprintCallable)
	/// <summary>
	/// Removes a resource from storage. If there is insuffient amount stored, than nothing will be changed and 
	/// this function will return False
	/// </summary>
	/// <param name="type">Resource Type</param>
	/// <param name="amount">Amount of resource to remove</param>
	/// <returns>Returns false, if the storage contained less resource than requested</returns>
	bool TakeLumberFromStorage(EResourceType type, int amount);

	UFUNCTION(BlueprintCallable)
	/// <summary>
	/// Directly sets the amount of stored resource
	/// </summary>
	/// <param name="type">Resource Type</param>
	/// <param name="amount">Amount</param>
	void SetStoredResourceAmount(EResourceType type, int amount);
};
