// Fill out your copyright notice in the Description page of Project Settings.


#include "ZombieSiegePlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Doodad.h"
#include "GatherDoodadJob.h"
#include "Camera/CameraComponent.h"

DEFINE_LOG_CATEGORY(LogZombieSiegePlayerController);

void AZombieSiegePlayerController::OnCameraVertical(float value)
{
	APawn* posessedPawn = GetPawn();
	if (posessedPawn != nullptr)
	{
		FVector location = posessedPawn->GetActorLocation();
		location += FVector::LeftVector * cameraDefaultSpeed * value * UGameplayStatics::GetWorldDeltaSeconds(GetWorld());
		posessedPawn->SetActorLocation(location);
	}
}

void AZombieSiegePlayerController::OnCameraHorizontal(float value)
{
	APawn* posessedPawn = GetPawn();
	if (posessedPawn != nullptr)
	{
		FVector location = posessedPawn->GetActorLocation();
		location += FVector::ForwardVector * cameraDefaultSpeed * value * UGameplayStatics::GetWorldDeltaSeconds(GetWorld());
		posessedPawn->SetActorLocation(location);
	}
}

void AZombieSiegePlayerController::OnCameraZoom(float value)
{
	APawn* posessedPawn = GetPawn();
	if (posessedPawn == nullptr)
	{
		return;		
	}

	if (FMath::IsNearlyZero(value))
	{
		return;
	}

	TArray<AActor*> cameraActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), "StrategicCamera", cameraActors);

	if (cameraActors.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Strategic Camera not found!"));
		return;
	}

	AActor* cameraActor = cameraActors[0];
	UCameraComponent* cameraComponent = Cast< UCameraComponent>(cameraActor->GetComponentByClass(UCameraComponent::StaticClass()));
	if (cameraComponent == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Strategic Camera actor does not have a camera component!"));
		return;
	}

	float currentSize = cameraComponent->OrthoWidth;

	currentSize -= value * cameraDefaultZoomSpeed * UGameplayStatics::GetWorldDeltaSeconds(GetWorld());
	currentSize = FMath::Clamp(currentSize, cameraDefaultMinSize, cameraDefaultMaxSize);

	cameraComponent->SetOrthoWidth(currentSize);
}

void AZombieSiegePlayerController::OnSelectActionPressed()
{
}

void AZombieSiegePlayerController::OnSelectActionReleased()
{

}

void AZombieSiegePlayerController::OnSelectActionDoubleClick()
{
	float x, y;
	bool mousePresent = GetMousePosition(x, y);
	if (!mousePresent)
	{
		UE_LOG(LogZombieSiegePlayerController, Warning, TEXT("Mouse is invalid!"));
		return;
	}

	AActor* underCursorActor = GetActorUnderScreenPoint(x, y);
	if (underCursorActor)
	{
		FString actorName;
		underCursorActor->GetName(actorName);
		UE_LOG(LogZombieSiegePlayerController, Log, TEXT("Local player double-clicked on %s"), *actorName);
		ADoodad* doodad = Cast<ADoodad>(underCursorActor);
		if (doodad)
		{
			OnDoodadSelectDoubleClicked(doodad);
		}
	}
}

void AZombieSiegePlayerController::OnCommandActionReleased()
{
}

void AZombieSiegePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	this->InputComponent->BindAxis("CameraVertical", this, &AZombieSiegePlayerController::OnCameraVertical);
	this->InputComponent->BindAxis("CameraHorizontal", this, &AZombieSiegePlayerController::OnCameraHorizontal);
	this->InputComponent->BindAxis("CameraZoom", this, &AZombieSiegePlayerController::OnCameraZoom);

	this->InputComponent->BindAction("Select", EInputEvent::IE_Pressed, this, &AZombieSiegePlayerController::OnSelectActionPressed);
	this->InputComponent->BindAction("Select", EInputEvent::IE_Released, this, &AZombieSiegePlayerController::OnSelectActionReleased);
	this->InputComponent->BindAction("Select", EInputEvent::IE_DoubleClick, this, &AZombieSiegePlayerController::OnSelectActionDoubleClick);

	this->InputComponent->BindAction("Command", EInputEvent::IE_Released, this, &AZombieSiegePlayerController::OnSelectActionReleased);
}

void AZombieSiegePlayerController::OnDoodadSelectDoubleClicked(ADoodad* doodad)
{
	if (doodad->CanEverBeGathered())
	{
		FString name;
		doodad->GetName(name);
		
		AGatherDoodadJob* existingJob = nullptr;

		for (AJobBase* job : jobs)
		{
			AGatherDoodadJob* gatherJob = Cast<AGatherDoodadJob>(job);
			if (gatherJob->GetTargetDoodad() == doodad)
			{
				existingJob = gatherJob;
			}
		}

		if (existingJob)
		{
			UE_LOG(LogZombieSiegePlayerController, Log, TEXT("Player removes gather mark from %s"), *name);
			jobs.Remove(existingJob);
			existingJob->Destroy();
		}
		else
		{
			UE_LOG(LogZombieSiegePlayerController, Log, TEXT("Player marks the doodad %s as gather target"), *name);
			FVector location = doodad->GetActorLocation();
			FRotator rotation = doodad->GetActorRotation();
			FActorSpawnParameters params;
			AGatherDoodadJob* createdJob = GetWorld()->SpawnActor<AGatherDoodadJob>(
				AGatherDoodadJob::StaticClass(),
				location,
				rotation,
				params
				);

			check(createdJob);

			createdJob->SetOwningPlayerController(this);
			createdJob->SetTargetDoodad(doodad);

			jobs.Add(createdJob);
		}
	}
}

AActor* AZombieSiegePlayerController::GetActorUnderScreenPoint(float x, float y)
{
	FVector worldLocation;
	FVector worldDirection;
	bool deprojectOk = DeprojectScreenPositionToWorld(x, y, worldLocation, worldDirection);
	check(deprojectOk);

	FVector traceEnd = worldLocation + worldDirection * cameraDefaultHeight * 2;

	FHitResult hitResult;
	FCollisionObjectQueryParams queryParams;

	queryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_Destructible);
	queryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_Pawn);

	GetWorld()->LineTraceSingleByObjectType(hitResult, worldLocation, traceEnd, queryParams);

	AActor* actor = hitResult.GetActor();
	return actor;
}

void AZombieSiegePlayerController::ShowGameUiInternal_Implementation(bool bShow)
{
	UE_LOG(LogZombieSiegePlayerController, Warning, TEXT("Exec ShowGameUiInternal_Implementation(%d) called."), bShow);
}

void AZombieSiegePlayerController::AddToControlledUnits(AUnitBase* unit)
{
	check(unit);

	check(!controlledUnits.Contains(unit));

	controlledUnits.Add(unit);
}

void AZombieSiegePlayerController::RemoveFromControlledUnits(AUnitBase* unit)
{
	check(unit);

	controlledUnits.Remove(unit);
}

const TArray<AUnitBase*>& AZombieSiegePlayerController::GetControlledUnits()
{
	return controlledUnits;
}

void AZombieSiegePlayerController::RemoveJob(AJobBase* job)
{
	jobs.Remove(job);
}

const TArray<AJobBase*> AZombieSiegePlayerController::GetJobs()
{
	return jobs;
}

void AZombieSiegePlayerController::ShowGameUi(bool bShow)
{
	UE_LOG(LogZombieSiegePlayerController, Warning, TEXT("Exec ShowGameUi(%d) called"), bShow);

	ShowGameUiInternal(bShow);
}
