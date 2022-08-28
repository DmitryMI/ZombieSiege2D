// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ZombieSiegePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIESIEGE_API AZombieSiegePlayerController : public APlayerController
{
	GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly)
	float cameraDefaultSpeed = 1000;

	UPROPERTY(EditDefaultsOnly, Category = "DefaultPreferences|Camera")
	float cameraDefaultMaxSize = 4096;

	UPROPERTY(EditDefaultsOnly, Category = "DefaultPreferences|Camera")
	float cameraDefaultMinSize = 2048;

	UPROPERTY(EditDefaultsOnly, Category = "DefaultPreferences|Camera")
	float cameraDefaultZoomSpeed = 15000.0f;

	void OnCameraVertical(float value);
	void OnCameraHorizontal(float value);
	void OnCameraZoom(float value);
	
protected:
	virtual void SetupInputComponent() override;

	
};
