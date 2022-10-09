// Fill out your copyright notice in the Description page of Project Settings.


#include "SoundManager.h"

ACTOR_SINGLETON_IMPLEMENTATION(ASoundManager);

// Sets default values
ASoundManager::ASoundManager()
{
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ASoundManager::BeginPlay()
{
	Super::BeginPlay();
	
	InitActorSingleton();
}

USoundBase* ASoundManager::GetHitResponseSound(EArmorNature armorNature, EWeaponNature weaponNature)
{
	switch (armorNature)
	{
	case EArmorNature::Flesh:
		return fleshHitResponse;
	default:
		return nullptr;
	}
}

