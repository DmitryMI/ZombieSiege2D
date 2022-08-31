// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponInfoBase.h"
#include "WeaponManager.generated.h"

UCLASS()
class ZOMBIESIEGE_API AWeaponManager : public AActor
{
	GENERATED_BODY()

private:
	static TMap<UWorld*, AWeaponManager*> weaponManagerInstances;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TMap<FName, UWeaponInfoBase*> weaponInstances;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, TSubclassOf<UWeaponInfoBase>> weaponClasses;

	AWeaponManager();

protected:
	virtual void BeginPlay() override;

public:	

	static AWeaponManager* GetInstance(UWorld* world)
	{
		return weaponManagerInstances[world];
	}

	TMap<FName, UWeaponInfoBase*>& GetWeaponInstancesMap();

};
