// Fill out your copyright notice in the Description page of Project Settings.


#include "ZombieSiegePlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Doodad.h"
#include "GatherDoodadJob.h"
#include "BuildingJob.h"
#include "BuildingPlacementMarker.h"
#include "ZombieSiegeUtils.h"
#include "Camera/CameraComponent.h"
#include "Survivor.h"
#include "SurvivorAiController.h"
#include "GameFramework/HUD.h"
#include "ZombieSiegePlayerState.h"
#include "SelectionIndicatorComponent.h"

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
	bool mouseOk = GetMousePosition(selectActionPressedPosition.X, selectActionPressedPosition.Y);
	if (mouseOk)
	{
		bSelectActionPressed = true;
	}
}

void AZombieSiegePlayerController::OnSelectActionReleased()
{
	bSelectActionPressed = false;

	if (controllerState == EControllerState::None)
	{
		ClearSelection();
	}
	else if (controllerState == EControllerState::BuildingPlacement)
	{
		if (!buildingPlacementMarker)
		{
			return;
		}
		if (buildingPlacementMarker->CanBeBuiltLater())
		{
			ABuildingJob* job = ABuildingJob::FromExistingMarker(GetWorld(), buildingPlacementMarker, buildingJobClass);
			job->SetOwningPlayerController(this);
			jobs.Add(job);
		}
		else
		{
			// TODO Print error for the player.			
			buildingPlacementMarker->Destroy();
		}

		buildingPlacementMarker = nullptr;
	}
	else if (controllerState == EControllerState::SelectingUnits)
	{
		SelectUnitsInsideSelectionBox();

		AZombieSiegeHUD* hud = GetHUD<AZombieSiegeHUD>();
		check(hud);
		hud->EndSelection();

		controllerState = EControllerState::None;
	}
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
	if (controllerState == EControllerState::None)
	{
		float mouseX = 0;
		float mouseY = 0;
		bool mouseOk = GetMousePosition(mouseX, mouseY);
		if (!mouseOk)
		{
			return;
		}
		AActor* commandTargetActor = GetActorUnderScreenPoint(mouseX, mouseY);

		if (commandTargetActor)
		{
			for (AUnitBase* unit : selectedUnits)
			{
				AUnitAiController* controller = unit->GetController<AUnitAiController>();
				if (!controller)
				{
					continue;
				}

				controller->HandleTargetActorCommandAction(commandTargetActor, bQueueOrderActionPressed);
			}
		}
		else
		{
			FVector location;
			bool deprojected = DeprojectMouseOnTerrain(location);
			if (deprojected)
			{
				for (AUnitBase* unit : selectedUnits)
				{
					AUnitAiController* controller = unit->GetController<AUnitAiController>();
					if (!controller)
					{
						continue;
					}

					controller->HandleTargetPointCommandAction(location, bQueueOrderActionPressed);
				}
			}
		}

		
	}
	else if (controllerState == EControllerState::BuildingPlacement)
	{
		check(buildingPlacementMarker);

		buildingPlacementMarker->Destroy();
		buildingPlacementMarker = nullptr;
	}
	else
	{

	}
}

void AZombieSiegePlayerController::OnQueueOrderActionPressed()
{
	bQueueOrderActionPressed = true;
}

void AZombieSiegePlayerController::OnQueueOrderActionReleased()
{
	bQueueOrderActionPressed = false;
}

void AZombieSiegePlayerController::OnMouseMoveX(float dx)
{
	mouseMoveAxisAccumulator.X = dx;
}

void AZombieSiegePlayerController::OnMouseMoveY(float dy)
{
	mouseMoveAxisAccumulator.X = dy;
}


void AZombieSiegePlayerController::BeginPlay()
{
	Super::BeginPlay();
	AZombieSiegePlayerState* playerState = GetPlayerState<AZombieSiegePlayerState>();

	playerState->OnControlledUnitDied().AddUObject(this, &AZombieSiegePlayerController::OnControlledUnitDiedHandler);
}

void AZombieSiegePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	this->InputComponent->BindAxis("CameraVertical", this, &AZombieSiegePlayerController::OnCameraVertical);
	this->InputComponent->BindAxis("CameraHorizontal", this, &AZombieSiegePlayerController::OnCameraHorizontal);
	this->InputComponent->BindAxis("CameraZoom", this, &AZombieSiegePlayerController::OnCameraZoom);
	this->InputComponent->BindAxis("MouseMoveX", this, &AZombieSiegePlayerController::OnMouseMoveX);
	this->InputComponent->BindAxis("MouseMoveY", this, &AZombieSiegePlayerController::OnMouseMoveY);

	this->InputComponent->BindAction("Select", EInputEvent::IE_Pressed, this, &AZombieSiegePlayerController::OnSelectActionPressed);
	this->InputComponent->BindAction("Select", EInputEvent::IE_Released, this, &AZombieSiegePlayerController::OnSelectActionReleased);
	this->InputComponent->BindAction("Select", EInputEvent::IE_DoubleClick, this, &AZombieSiegePlayerController::OnSelectActionDoubleClick);

	this->InputComponent->BindAction("Command", EInputEvent::IE_Released, this, &AZombieSiegePlayerController::OnCommandActionReleased);

	this->InputComponent->BindAction("QueueOrder", EInputEvent::IE_Pressed, this, &AZombieSiegePlayerController::OnQueueOrderActionPressed);
	this->InputComponent->BindAction("QueueOrder", EInputEvent::IE_Released, this, &AZombieSiegePlayerController::OnQueueOrderActionReleased);
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
			if (gatherJob && gatherJob->GetTargetDoodad() == doodad)
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
				gatherDoodadJobClass,
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
	AZombieSiegePlayerState* playerState = GetPlayerState<AZombieSiegePlayerState>();
	playerState->AddToControlledUnits(unit);
}

void AZombieSiegePlayerController::RemoveFromControlledUnits(AUnitBase* unit)
{
	AZombieSiegePlayerState* playerState = GetPlayerState<AZombieSiegePlayerState>();
	playerState->RemoveFromControlledUnits(unit);
}

const TArray<AUnitBase*>& AZombieSiegePlayerController::GetControlledUnits()
{
	AZombieSiegePlayerState* playerState = GetPlayerState<AZombieSiegePlayerState>();
	return playerState->GetControlledUnits();
}

void AZombieSiegePlayerController::RemoveJob(AJobBase* job)
{
	jobs.Remove(job);
}

const TArray<AJobBase*> AZombieSiegePlayerController::GetJobs()
{
	return jobs;
}

void AZombieSiegePlayerController::SelectUnitsInsideSelectionBox()
{
	AZombieSiegeHUD* hud = GetHUD<AZombieSiegeHUD>();
	check(hud);

	const TArray<AUnitBase*> controlledUnits = GetControlledUnits();
	TArray<AUnitBase*> unitInBox;
	unitInBox.Reserve(controlledUnits.Num());

	GetActorsInSelectionBox(controlledUnits, unitInBox);

	for (AUnitBase* unit : unitInBox)
	{
		AddUnitToSelection(unit);
	}
}

const TArray<TSubclassOf<AUnitBase>>& AZombieSiegePlayerController::GetBuildableUnits()
{
	return buildables;
}


void AZombieSiegePlayerController::BeginBuildingJobPlacement(TSubclassOf<ABuilding> buildingClass)
{
	if (controllerState != EControllerState::None)
	{
		return;
	}

	if (buildingPlacementMarker)
	{
		// Already busy with another building placement
		return;
	}

	FVector deprojected;

	if (DeprojectMouseOnTerrain(deprojected))
	{
		controllerState = EControllerState::BuildingPlacement;

		buildingPlacementMarker = GetWorld()->SpawnActor<ABuildingPlacementMarker>(
			buildingPlacementMarkerClass,
			deprojected,
			FRotator::ZeroRotator
			);

		check(buildingPlacementMarker);

		buildingPlacementMarker->SetOwningPlayerController(this);
		buildingPlacementMarker->SetBuildingClass(buildingClass);
	}
}

void AZombieSiegePlayerController::ShowGameUi(bool bShow)
{
	UE_LOG(LogZombieSiegePlayerController, Warning, TEXT("Exec ShowGameUi(%d) called"), bShow);

	ShowGameUiInternal(bShow);
}

void AZombieSiegePlayerController::DebugAlert(bool bEnabled)
{
	if (!bEnabled)
	{
		for (AUnitBase* unit : GetControlledUnits())
		{
			ABuilding* building = Cast<ABuilding>(unit);
			if (building)
			{
				building->MakeAllPassengersLeave();
			}
		}

		return;
	}

	TMap<ABuilding*, int> buildingSeatsMap;
	for (AUnitBase* unit : GetControlledUnits())
	{
		ABuilding* building = Cast<ABuilding>(unit);
		if (building)
		{
			buildingSeatsMap.Add(building, building->GetFreePassengerSeats());
		}
	}	

	if (buildingSeatsMap.Num() == 0)
	{
		UE_LOG(LogZombieSiegePlayerController, Warning, TEXT("[DebugAlert] Alert triggered, but player has no buildings!"));
		return;
	}

	for (AUnitBase* unit : GetControlledUnits())
	{
		ASurvivor* survivor = Cast<ASurvivor>(unit);
		if (!survivor)
		{
			continue;
		}
		ASurvivorAiController* controller = survivor->GetController<ASurvivorAiController>();
		if (!controller)
		{
			continue;
		}

		ABuilding* buildingToEnter = nullptr;

		for (auto& buildingSeatsPair : buildingSeatsMap)
		{
			if (buildingSeatsPair.Value > 0)
			{
				buildingToEnter = buildingSeatsPair.Key;
				break;
			}
		}

		if (buildingToEnter)
		{
			buildingSeatsMap[buildingToEnter]--;
			controller->IssueEnterPassengerCarrierOrder(buildingToEnter);
		}
		else
		{
			UE_LOG(LogZombieSiegePlayerController, Warning, TEXT("[DebugAlert] Not enough seats! %s will be left outside."), *survivor->GetName());
		}
		
	}
}

void AZombieSiegePlayerController::AddUnitToSelection(AUnitBase* unit)
{
	checkSlow(!selectedUnits.Contains(unit));

	USelectionIndicatorComponent* indicator = Cast<USelectionIndicatorComponent>(unit->GetComponentByClass(USelectionIndicatorComponent::StaticClass()));
	if (!indicator)
	{
		UE_LOG(LogTemp, Error, TEXT("Unit %s has no USelectionIndicatorComponent"), *unit->GetName());
		return;
	}
	
	selectedUnits.Add(unit);

	ETeamAttitude::Type attitude = FGenericTeamId::GetAttitude(this, unit);
	
	indicator->SetIsSelected(true, attitude);
}

void AZombieSiegePlayerController::RemoveUnitFromSelection(AUnitBase* unit)
{
	checkSlow(selectedUnits.Contains(unit));

	selectedUnits.Remove(unit);

	USelectionIndicatorComponent* indicator = Cast<USelectionIndicatorComponent>(unit->GetComponentByClass(USelectionIndicatorComponent::StaticClass()));
	if (!indicator)
	{
		UE_LOG(LogTemp, Error, TEXT("Unit %s has no USelectionIndicatorComponent"), *unit->GetName());
		return;
	}

	indicator->SetIsSelected(false, ETeamAttitude::Neutral);
}

void AZombieSiegePlayerController::ClearSelection()
{
	TArray<AUnitBase*> selectionCopy = selectedUnits;

	for (AUnitBase* unit : selectionCopy)
	{
		RemoveUnitFromSelection(unit);
	}
}

bool AZombieSiegePlayerController::DeprojectMouseOnTerrain(FVector& deprojectedLocation)
{
	FVector worldLocation;
	FVector worldDirection;
	bool mouseDeprojected = DeprojectMousePositionToWorld(worldLocation, worldDirection);
	if (!mouseDeprojected)
	{
		UE_LOG(LogZombieSiegePlayerController, Error, TEXT("Failed to deproject mouse during BeginBuildingJobPlacement"));
		return false;
	}

	deprojectedLocation = UZombieSiegeUtils::GetTerrainIntersection(worldLocation, worldDirection);

	return true;
}

void AZombieSiegePlayerController::OnControlledUnitDiedHandler(AZombieSiegePlayerState* sender, const FUnitDiedEventArgs& args)
{
	check(sender == GetPlayerState<AZombieSiegePlayerState>());


}

void AZombieSiegePlayerController::OnMouseMove(const FVector2D& delta)
{
	if (delta.IsNearlyZero())
	{
		return;
	}

	if (controllerState == EControllerState::None)
	{
		if (bSelectActionPressed)
		{
			FVector2D mousePosition;
			if (!GetMousePosition(mousePosition.X, mousePosition.Y))
			{
				return;
			}

			float mouseDeltaSqr = (mousePosition - selectActionPressedPosition).SizeSquared();
			if (mouseDeltaSqr >= selectionMouseDeltaThreshold)
			{
				AZombieSiegeHUD* hud = GetHUD<AZombieSiegeHUD>();
				check(hud);

				FVector2D boxStart;
				GetMousePosition(boxStart.X, boxStart.Y);
				hud->BeginSelection(boxStart);
				controllerState = EControllerState::SelectingUnits;
				return;
			}
		}
	}
	else if (controllerState == EControllerState::BuildingPlacement)
	{
		if (!buildingPlacementMarker)
		{
			controllerState = EControllerState::None;
			return;
		}
		FVector deprojected;
		if (DeprojectMouseOnTerrain(deprojected))
		{
			buildingPlacementMarker->SetActorLocation(deprojected);
		}
	}
	else if (controllerState == EControllerState::SelectingUnits)
	{
		AZombieSiegeHUD* hud = GetHUD<AZombieSiegeHUD>();
		check(hud);

		FVector2D boxEnd;
		GetMousePosition(boxEnd.X, boxEnd.Y);
		hud->UpdateSelection(boxEnd);
	}
	else
	{
		
	}
}

void AZombieSiegePlayerController::SetGenericTeamId(const FGenericTeamId& TeamID)
{
	AZombieSiegePlayerState* playerState = GetPlayerState<AZombieSiegePlayerState>();
	check(playerState);

	playerState->SetGenericTeamId(TeamID);
}

FGenericTeamId AZombieSiegePlayerController::GetGenericTeamId() const
{
	AZombieSiegePlayerState* playerState = GetPlayerState<AZombieSiegePlayerState>();
	check(playerState);

	return playerState->GetGenericTeamId();
}

void AZombieSiegePlayerController::Tick(float deltaSeconds)
{
	Super::Tick(deltaSeconds);

	OnMouseMove(mouseMoveAxisAccumulator);
}
