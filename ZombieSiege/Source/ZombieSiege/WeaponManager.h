// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponInfo.h"
#include "Kismet/GameplayStatics.h"
#include "WeaponManager.generated.h"

UCLASS()
class ZOMBIESIEGE_API AWeaponManager : public AActor
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
	TMap<FName, TSubclassOf<UWeaponInfo>> weaponClasses;

	static TMap<UWorld*, AWeaponManager*> weaponManagerInstances;

protected:

	UPROPERTY(VisibleAnywhere)
	TMap<FName, UWeaponInfo*> weaponInstances;

	AWeaponManager();

protected:
	virtual void BeginPlay() override;

public:	

	static AWeaponManager* GetInstance(UWorld* world)
	{
		AWeaponManager* loaded = weaponManagerInstances[world];
		if (loaded)
		{
			return loaded;
		}

		AActor* weaponManagerActor = UGameplayStatics::GetActorOfClass(world, StaticClass());
		check(weaponManagerActor);

		AWeaponManager* weaponManager = Cast<AWeaponManager>(weaponManagerActor);
		check(weaponManager);
		
		return weaponManager;
	}

	UFUNCTION(BlueprintCallable, meta = (DeprecatedFunction, DeprecationMessage = "This method exposes the internal map for read-write."))
	/// <summary>
	/// Get the Weapon instances map. Map is mutable.
	/// </summary>
	/// <returns>Map: Weapon Name, WeaponInfo</returns>
	TMap<FName, UWeaponInfo*>& GetWeaponInstancesMapMutable();

	UFUNCTION(BlueprintCallable)
	/// <summary>
	/// Get the Weapon instances map
	/// </summary>
	/// <returns>Map<Weapon Name, WeaponInfo></returns>
	const TMap<FName, UWeaponInfo*>& GetWeaponInstancesMap() const;

	UFUNCTION(BlueprintCallable)
	/// <summary>
	/// Get weapon info by name
	/// </summary>
	/// <param name="name">Weapon name</param>
	/// <returns>WeaponInfo instance. nullptr if no weapon exists for this name</returns>
	UWeaponInfo* GetWeaponInfo(const FName& name) const;

	UFUNCTION(BlueprintCallable)
	/// <summary>
	/// Adds new weapon info instance to the collection
	/// </summary>
	/// <param name="name">Name of a new weapon</param>
	/// <param name="weapon">Weapon info instance</param>
	/// <returns>Returns false if a weapon with this name already exists</returns>
	bool AddWeaponInfo(const FName& name, UWeaponInfo* weapon);

	UFUNCTION(BlueprintCallable)
	/// <summary>
	/// Checks if weapon with this name exists
	/// </summary>
	/// <param name="name">Weapon name</param>
	/// <returns>True if weapon exists</returns>
	bool HasWeaponInfo(const FName& name) const;
};
