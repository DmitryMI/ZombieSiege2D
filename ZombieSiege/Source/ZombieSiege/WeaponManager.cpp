// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponManager.h"

ACTOR_SINGLETON_IMPLEMENTATION(AWeaponManager)

// Sets default values
AWeaponManager::AWeaponManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AWeaponManager::BeginPlay()
{
	Super::BeginPlay();

	InitActorSingleton();

	for (auto weaponClassPair : weaponClasses)
	{
		FName name = weaponClassPair.Key;		
		UWeaponInfo* weaponInstance = UWeaponInfo::CreateWeapon(this, name, weaponClassPair.Value);
		weaponInstances.Add(name, weaponInstance);
	}
}

TMap<FName, UWeaponInfo*>& AWeaponManager::GetWeaponInstancesMapMutable()
{
	return weaponInstances;
}

const TMap<FName, UWeaponInfo*>& AWeaponManager::GetWeaponInstancesMap() const
{
	return weaponInstances;
}

UWeaponInfo* AWeaponManager::GetWeaponInfo(const FName& name) const
{
	if (!HasWeaponInfo(name))
	{
		FString nameStr = name.ToString();
		UE_LOG(LogTemp, Error, TEXT("Weapon with name %s does not exist!"), *nameStr);
		return nullptr;
	}

	return weaponInstances[name];
}

bool AWeaponManager::AddWeaponInfo(const FName& name, UWeaponInfo* weapon)
{
	if (HasWeaponInfo(name))
	{
		UE_LOG(LogTemp, Warning, TEXT("Attempting to add an existing weapon (%s)!"), *name.ToString());
		return false;
	}

	check(weapon);
	weaponInstances.Add(name, weapon);
	return true;
}

bool AWeaponManager::HasWeaponInfo(const FName& name) const
{
	return weaponInstances.Contains(name);
}
