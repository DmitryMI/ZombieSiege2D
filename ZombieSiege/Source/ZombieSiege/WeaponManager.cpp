// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponManager.h"

TMap<UWorld*, AWeaponManager*> AWeaponManager::weaponManagerInstances;

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

	check(!weaponManagerInstances.Contains(GetWorld()));

	weaponManagerInstances.Add(GetWorld(), this);

	for (auto weaponClassPair : weaponClasses)
	{
		FName name = weaponClassPair.Key;		
		UWeaponInfoBase* weaponInstance = UWeaponInfoBase::CreateWeapon(this, name, weaponClassPair.Value);
		weaponInstances.Add(name, weaponInstance);
	}
}

TMap<FName, UWeaponInfoBase*>& AWeaponManager::GetWeaponInstancesMap()
{
	return weaponInstances;
}

