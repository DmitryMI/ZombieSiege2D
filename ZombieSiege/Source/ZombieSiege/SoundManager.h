// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ActorSingleton.h"
#include "Sound/SoundCue.h"
#include "WeaponNature.h"
#include "ArmorNature.h"
#include "SoundManager.generated.h"

UCLASS()
class ZOMBIESIEGE_API ASoundManager : public AActor
{
	GENERATED_BODY()

	ACTOR_SINGLETON(ASoundManager);

private:
	UPROPERTY(EditAnywhere)
	USoundBase* fleshHitResponse;
	
public:	
	// Sets default values for this actor's properties
	ASoundManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	UFUNCTION(BlueprintCallable)
	USoundBase* GetHitResponseSound(EArmorNature armorNature, EWeaponNature weaponNature);
};
