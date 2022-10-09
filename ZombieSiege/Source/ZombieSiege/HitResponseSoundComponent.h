// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "SoundManager.h"
#include "DamageReceivedEventArgs.h"
#include "HitResponseSoundComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ZOMBIESIEGE_API UHitResponseSoundComponent : public UAudioComponent
{
	GENERATED_BODY()

private:
	UPROPERTY(Transient)
	ASoundManager* soundManager;
	
protected:
	virtual void BeginPlay() override;

	virtual void OnOwnerDamageReceived(const FDamageReceivedEventArgs& args);
};
