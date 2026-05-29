#include "ZombieAgentBrainComponent.h"

#include "AIController.h"
#include "GameFramework/Pawn.h"
#include "StudentPerceptor.h"

UZombieAgentBrainComponent::UZombieAgentBrainComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UZombieAgentBrainComponent::BeginPlay()
{
	Super::BeginPlay();

	Perceptor = GetOwner()->FindComponentByClass<UStudentPerceptor>();
	InventoryComponent = FindComponentByNamePart(TEXT("Inventory"));
	HealthComponent = FindComponentByNamePart(TEXT("Health"));
	StaminaComponent = FindComponentByNamePart(TEXT("Stamina"));
}

void UZombieAgentBrainComponent::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateState();
	ExecuteCurrentState(DeltaTime);

	GEngine->AddOnScreenDebugMessage(
		20,
		0.f,
		FColor::Cyan,
		FString::Printf(TEXT("AI State: %s"), *GetStateName())
	);
}

void UZombieAgentBrainComponent::UpdateState()
{
	AActor* ClosestZombie = GetClosestZombie();

	if (ClosestZombie)
	{
		const float ZombieDistance = FVector::Dist(
			GetOwner()->GetActorLocation(),
			ClosestZombie->GetActorLocation()
		);

		if (ZombieDistance <= ZombieFightRange)
		{
			CurrentState = EZombieAgentState::Fight;
			return;
		}

		if (CurrentState == EZombieAgentState::Flee)
		{
			if (ZombieDistance <= ZombieDangerExitRange)
			{
				CurrentState = EZombieAgentState::Flee;
				return;
			}
		}
		else if (ZombieDistance <= ZombieDangerEnterRange)
		{
			CurrentState = EZombieAgentState::Flee;
			return;
		}
	}

	if (ShouldUseItem())
	{
		CurrentState = EZombieAgentState::UseItem;
		return;
	}

	
	AActor* ClosestPurgeZone = GetClosestPurgeZone();

	if (ClosestPurgeZone)
	{
		const float PurgeDistance = FVector::Dist(
			GetOwner()->GetActorLocation(),
			ClosestPurgeZone->GetActorLocation()
		);

		if (PurgeDistance <= PurgeDangerRange)
		{
			CurrentState = EZombieAgentState::AvoidPurge;
			return;
		}
	}

	if (GetBestItem())
	{
		CurrentState = EZombieAgentState::SeekItem;
		return;
	}
	
	if (GetClosestHouse())
	{
		CurrentState = EZombieAgentState::SearchHouse;
		return;
	}

	CurrentState = EZombieAgentState::Explore;
}

void UZombieAgentBrainComponent::ExecuteCurrentState(float DeltaTime)
{
	switch (CurrentState)
	{
	case EZombieAgentState::Explore:
		ExecuteExplore(DeltaTime);
		break;

	case EZombieAgentState::SeekItem:
		ExecuteSeekItem();
		break;

	case EZombieAgentState::Flee:
		ExecuteFlee();
		break;

	case EZombieAgentState::UseItem:
		ExecuteUseItem();
		break;
		
	case EZombieAgentState::Fight:
		ExecuteFight(DeltaTime);
		break;
		
	case EZombieAgentState::SearchHouse:
		ExecuteSearchHouse();
		break;
		
	case EZombieAgentState::AvoidPurge:
		ExecuteAvoidPurge();
		break;

	default:
		break;
	}
}

void UZombieAgentBrainComponent::ExecuteExplore(float DeltaTime)
{
	TimeSinceLastExploreMove += DeltaTime;

	if (TimeSinceLastExploreMove < ExploreMoveInterval)
	{
		return;
	}

	TimeSinceLastExploreMove = FMath::FRandRange(-1.5f, 0.f);

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn) return;

	AAIController* AIController = Cast<AAIController>(OwnerPawn->GetController());
	if (!AIController) return;

	AIController->MoveToLocation(
		GetRandomExploreLocation(),
		100.f,
		true,
		true,
		true,
		false,
		nullptr,
		true
	);
}

void UZombieAgentBrainComponent::ExecuteSeekItem()
{
	AActor* ClosestItem = GetBestItem();
	if (!ClosestItem) return;
	
	if (IsInventoryFull())
	{
		if (TryReplaceInventoryItem(ClosestItem))
		{
			return;
		}
	}

	if (TryPickupItem(ClosestItem))
	{
		return;
	}

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn) return;

	AAIController* AIController = Cast<AAIController>(OwnerPawn->GetController());
	if (!AIController) return;

	AIController->MoveToActor(
		ClosestItem,
		25.f,
		true,
		true,
		true,
		nullptr,
		true
	);
}

void UZombieAgentBrainComponent::ExecuteFlee()
{
	AActor* ClosestZombie = GetClosestZombie();
	if (!ClosestZombie) return;

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn) return;

	AAIController* AIController = Cast<AAIController>(OwnerPawn->GetController());
	if (!AIController) return;

	AIController->MoveToLocation(
		GetFleeLocation(ClosestZombie),
		100.f,
		true,
		true,
		true,
		false,
		nullptr,
		true
	);

	GEngine->AddOnScreenDebugMessage(
		-1,
		0.f,
		FColor::Red,
		TEXT("FLEEING FROM ZOMBIE")
	);
}

void UZombieAgentBrainComponent::ExecuteUseItem()
{
	if (TryUseInventoryItem())
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			2.f,
			FColor::Green,
			TEXT("Used inventory item")
		);
	}
}

AActor* UZombieAgentBrainComponent::GetBestItem() const
{
	if (!Perceptor) return nullptr;

	AActor* BestItem = nullptr;
	int32 BestPriority = 0;
	float BestDistance = FLT_MAX;

	const FVector OwnerLocation = GetOwner()->GetActorLocation();

	for (AActor* Item : Perceptor->SeenItems)
	{
		if (!IsValid(Item)) continue;

		const int32 Priority = GetItemPriority(Item);

		// Ignore garbage / unknown useless items
		if (Priority <= 0)
		{
			continue;
		}

		const float Distance = FVector::Dist(
			OwnerLocation,
			Item->GetActorLocation()
		);

		if (
			Priority > BestPriority ||
			(Priority == BestPriority && Distance < BestDistance)
		)
		{
			BestPriority = Priority;
			BestDistance = Distance;
			BestItem = Item;
		}
	}

	return BestItem;
}

int32 UZombieAgentBrainComponent::GetItemPriority(AActor* ItemActor) const
{
	if (!ItemActor) return 0;

	const FString ItemName = ItemActor->GetName();
	const FString ClassName = ItemActor->GetClass()->GetName();

	if (ItemName.Contains(TEXT("Medkit")) || ClassName.Contains(TEXT("Medkit")))
	{
		return 4;
	}

	if (ItemName.Contains(TEXT("Food")) || ClassName.Contains(TEXT("Food")))
	{
		return 3;
	}

	if (
		ItemName.Contains(TEXT("Shotgun")) ||
		ClassName.Contains(TEXT("Shotgun")) ||
		ItemName.Contains(TEXT("Pistol")) ||
		ClassName.Contains(TEXT("Pistol"))
	)
	{
		return 2;
	}

	if (ItemName.Contains(TEXT("Garbage")) || ClassName.Contains(TEXT("Garbage")))
	{
		return 0;
	}

	return 1;
}

AActor* UZombieAgentBrainComponent::GetClosestZombie() const
{
	if (!Perceptor) return nullptr;

	AActor* ClosestZombie = nullptr;
	float ClosestDistance = FLT_MAX;
	const FVector OwnerLocation = GetOwner()->GetActorLocation();

	for (AActor* Zombie : Perceptor->SeenZombies)
	{
		if (!IsValid(Zombie)) continue;

		const float Distance = FVector::Dist(
			OwnerLocation,
			Zombie->GetActorLocation()
		);

		if (Distance < ClosestDistance)
		{
			ClosestDistance = Distance;
			ClosestZombie = Zombie;
		}
	}

	return ClosestZombie;
}

UActorComponent* UZombieAgentBrainComponent::FindComponentByNamePart(const FString& NamePart) const
{
	TArray<UActorComponent*> Components;
	GetOwner()->GetComponents(Components);

	for (UActorComponent* Component : Components)
	{
		if (!Component) continue;

		if (
			Component->GetName().Contains(NamePart) ||
			Component->GetClass()->GetName().Contains(NamePart)
		)
		{
			return Component;
		}
	}

	return nullptr;
}

bool UZombieAgentBrainComponent::TryPickupItem(AActor* ItemActor)
{
	if (!InventoryComponent || !ItemActor) return false;

	const float DistanceToItem = FVector::Dist(
		GetOwner()->GetActorLocation(),
		ItemActor->GetActorLocation()
	);

	const float AllowedPickupDistance = GetPickupRange() + 75.f;

	if (DistanceToItem > AllowedPickupDistance)
	{
		return false;
	}

	const int32 Capacity = GetInventoryCapacity();

	for (int32 SlotIndex = 0; SlotIndex < Capacity; ++SlotIndex)
	{
		if (TryGrabItemInSlot(SlotIndex, ItemActor))
		{
			if (Perceptor)
			{
				Perceptor->SeenItems.Remove(ItemActor);
			}

			GEngine->AddOnScreenDebugMessage(
				-1,
				2.f,
				FColor::Green,
				FString::Printf(TEXT("Picked up item in slot %d"), SlotIndex)
			);

			return true;
		}
	}

	return false;
}

bool UZombieAgentBrainComponent::TryGrabItemInSlot(int32 SlotIndex, AActor* ItemActor)
{
	if (!InventoryComponent) return false;

	UFunction* GrabFunction = InventoryComponent->FindFunction(TEXT("GrabItem"));
	if (!GrabFunction) return false;

	struct FGrabItemParams
	{
		int32 SlotIdx;
		AActor* Item;
		bool ReturnValue;
	};

	FGrabItemParams Params;
	Params.SlotIdx = SlotIndex;
	Params.Item = ItemActor;
	Params.ReturnValue = false;

	InventoryComponent->ProcessEvent(GrabFunction, &Params);

	return Params.ReturnValue;
}

bool UZombieAgentBrainComponent::ShouldUseItem() const
{
	const int CurrentHealth = GetCurrentHealth();
	const float CurrentStamina = GetCurrentStamina();

	const bool bCanHeal = CurrentHealth <= LowHealthThreshold && HasInventoryItemType(TEXT("Medkit"));
	const bool bCanRestoreStamina = CurrentStamina <= LowStaminaThreshold && HasInventoryItemType(TEXT("Food"));

	return bCanHeal || bCanRestoreStamina;
}

bool UZombieAgentBrainComponent::TryUseInventoryItem()
{
	const int32 Capacity = GetInventoryCapacity();

	const bool bNeedsHealth = GetCurrentHealth() <= LowHealthThreshold;
	const bool bNeedsStamina = GetCurrentStamina() <= LowStaminaThreshold;

	for (int32 SlotIndex = 0; SlotIndex < Capacity; ++SlotIndex)
	{
		if (bNeedsHealth && DoesInventorySlotContainItemType(SlotIndex, TEXT("Medkit")))
		{
			if (TryUseItemInSlot(SlotIndex))
			{
				TryRemoveItemInSlot(SlotIndex);
				return true;
			}
		}

		if (bNeedsStamina && DoesInventorySlotContainItemType(SlotIndex, TEXT("Food")))
		{
			if (TryUseItemInSlot(SlotIndex))
			{
				TryRemoveItemInSlot(SlotIndex);
				return true;
			}
		}
	}

	return false;
}

bool UZombieAgentBrainComponent::TryUseItemInSlot(int32 SlotIndex)
{
	if (!InventoryComponent) return false;

	UFunction* UseFunction = InventoryComponent->FindFunction(TEXT("UseItem"));
	if (!UseFunction) return false;

	struct FUseItemParams
	{
		int32 SlotIdx;
		bool ReturnValue;
	};

	FUseItemParams Params;
	Params.SlotIdx = SlotIndex;
	Params.ReturnValue = false;

	InventoryComponent->ProcessEvent(UseFunction, &Params);

	return Params.ReturnValue;
}

bool UZombieAgentBrainComponent::TryRemoveItemInSlot(int32 SlotIndex)
{
	if (!InventoryComponent) return false;

	UFunction* RemoveFunction = InventoryComponent->FindFunction(TEXT("RemoveItem"));
	if (!RemoveFunction) return false;

	struct FRemoveItemParams
	{
		int32 SlotIdx;
		bool ReturnValue;
	};

	FRemoveItemParams Params;
	Params.SlotIdx = SlotIndex;
	Params.ReturnValue = false;

	InventoryComponent->ProcessEvent(RemoveFunction, &Params);

	return Params.ReturnValue;
}

bool UZombieAgentBrainComponent::DoesInventorySlotContainItemType(int32 SlotIndex, const FString& ItemType) const
{
	if (!InventoryComponent) return false;

	UFunction* GetInventoryFunction = InventoryComponent->FindFunction(TEXT("GetInventory"));
	if (!GetInventoryFunction) return false;

	struct FGetInventoryParams
	{
		TArray<AActor*> ReturnValue;
	};

	FGetInventoryParams Params;
	InventoryComponent->ProcessEvent(GetInventoryFunction, &Params);

	if (!Params.ReturnValue.IsValidIndex(SlotIndex))
	{
		return false;
	}

	AActor* Item = Params.ReturnValue[SlotIndex];
	if (!Item)
	{
		return false;
	}

	return Item->GetName().Contains(ItemType) || Item->GetClass()->GetName().Contains(ItemType);
}

float UZombieAgentBrainComponent::GetPickupRange() const
{
	if (!InventoryComponent) return 100.f;

	UFunction* RangeFunction = InventoryComponent->FindFunction(TEXT("GetPickupRange"));
	if (!RangeFunction) return 100.f;

	struct FPickupRangeParams
	{
		float ReturnValue;
	};

	FPickupRangeParams Params;
	Params.ReturnValue = 100.f;

	InventoryComponent->ProcessEvent(RangeFunction, &Params);

	return Params.ReturnValue;
}

int32 UZombieAgentBrainComponent::GetInventoryCapacity() const
{
	if (!InventoryComponent) return 5;

	UFunction* CapacityFunction = InventoryComponent->FindFunction(TEXT("GetInventoryCapacity"));
	if (!CapacityFunction) return 5;

	struct FCapacityParams
	{
		int32 ReturnValue;
	};

	FCapacityParams Params;
	Params.ReturnValue = 5;

	InventoryComponent->ProcessEvent(CapacityFunction, &Params);

	return Params.ReturnValue;
}

bool UZombieAgentBrainComponent::IsInventoryFull() const
{
	if (!InventoryComponent) return true;

	UFunction* GetInventoryFunction = InventoryComponent->FindFunction(TEXT("GetInventory"));
	if (!GetInventoryFunction) return true;

	struct FGetInventoryParams
	{
		TArray<AActor*> ReturnValue;
	};

	FGetInventoryParams Params;
	InventoryComponent->ProcessEvent(GetInventoryFunction, &Params);

	for (AActor* Item : Params.ReturnValue)
	{
		if (!Item)
		{
			return false;
		}
	}

	return true;
}

int UZombieAgentBrainComponent::GetCurrentHealth() const
{
	if (!HealthComponent) return 10;

	UFunction* HealthFunction = HealthComponent->FindFunction(TEXT("GetHealth"));
	if (!HealthFunction) return 10;

	struct FHealthParams
	{
		int ReturnValue;
	};

	FHealthParams Params;
	Params.ReturnValue = 10;

	HealthComponent->ProcessEvent(HealthFunction, &Params);

	return Params.ReturnValue;
}

float UZombieAgentBrainComponent::GetCurrentStamina() const
{
	if (!StaminaComponent) return 10.f;

	UFunction* StaminaFunction = StaminaComponent->FindFunction(TEXT("GetCurrentStamina"));
	if (!StaminaFunction) return 10.f;

	struct FStaminaParams
	{
		float ReturnValue;
	};

	FStaminaParams Params;
	Params.ReturnValue = 10.f;

	StaminaComponent->ProcessEvent(StaminaFunction, &Params);

	return Params.ReturnValue;
}

FVector UZombieAgentBrainComponent::GetRandomExploreLocation() const
{
	const FVector CurrentLocation = GetOwner()->GetActorLocation();

	const FVector RandomDirection = FVector(
		FMath::FRandRange(-1.f, 1.f),
		FMath::FRandRange(-1.f, 1.f),
		0.f
	).GetSafeNormal();

	return CurrentLocation + RandomDirection * ExploreRadius;
}

FVector UZombieAgentBrainComponent::GetFleeLocation(AActor* ZombieActor) const
{
	const FVector OwnerLocation = GetOwner()->GetActorLocation();

	FVector BestLocation = OwnerLocation;
	float BestScore = -FLT_MAX;

	const int32 DirectionCount = 16;

	for (int32 DirectionIndex = 0; DirectionIndex < DirectionCount; ++DirectionIndex)
	{
		const float Angle = (2.f * PI / DirectionCount) * DirectionIndex;

		const FVector Direction = FVector(
			FMath::Cos(Angle),
			FMath::Sin(Angle),
			0.f
		);

		const FVector CandidateLocation = OwnerLocation + Direction * FleeDistance;
		const float CandidateScore = ScoreFleeLocation(CandidateLocation);

		if (CandidateScore > BestScore)
		{
			BestScore = CandidateScore;
			BestLocation = CandidateLocation;
		}
	}

	return BestLocation;
}

float UZombieAgentBrainComponent::ScoreFleeLocation(const FVector& Location) const
{
	if (!Perceptor) return 0.f;

	float Score = 0.f;

	for (AActor* Zombie : Perceptor->SeenZombies)
	{
		if (!IsValid(Zombie)) continue;

		const float DistanceToZombie = FVector::Dist(
			Location,
			Zombie->GetActorLocation()
		);

		Score += DistanceToZombie;
	}
	
	for (AActor* PurgeZone : Perceptor->SeenPurgeZones)
	{
		if (!IsValid(PurgeZone)) continue;

		const float DistanceToPurge = FVector::Dist(
			Location,
			PurgeZone->GetActorLocation()
		);

		Score += DistanceToPurge * 0.5f;
	}

	Score += FMath::FRandRange(0.f, 100.f);
	
	return Score;
}

FString UZombieAgentBrainComponent::GetStateName() const
{
	switch (CurrentState)
	{
	case EZombieAgentState::Explore:
		return "Explore";
	case EZombieAgentState::SeekItem:
		return "SeekItem";
	case EZombieAgentState::Flee:
		return "Flee";
	case EZombieAgentState::Fight:
		return "Fight";
	case EZombieAgentState::UseItem:
		return "UseItem";
	case EZombieAgentState::SearchHouse:
		return "SearchHouse";
	case EZombieAgentState::AvoidPurge:
		return "AvoidPurge";
	default:
		return "Unknown";
	}
}

void UZombieAgentBrainComponent::ExecuteFight(float DeltaTime)
{
	TimeSinceLastWeaponUse += DeltaTime;

	AActor* ClosestZombie = GetClosestZombie();
	if (!ClosestZombie)
	{
		return;
	}

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn)
	{
		return;
	}

	const FVector DirectionToZombie =
		(ClosestZombie->GetActorLocation() - OwnerPawn->GetActorLocation()).GetSafeNormal();

	OwnerPawn->SetActorRotation(DirectionToZombie.Rotation());

	if (TimeSinceLastWeaponUse >= WeaponUseInterval)
	{
		TimeSinceLastWeaponUse = 0.f;

		if (TryUseWeapon())
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				1.f,
				FColor::Purple,
				TEXT("FIGHT: used weapon")
			);

			return;
		}
	}
	ExecuteFlee();
}

bool UZombieAgentBrainComponent::TryUseWeapon()
{
	const int32 Capacity = GetInventoryCapacity();

	for (int32 SlotIndex = 0; SlotIndex < Capacity; ++SlotIndex)
	{
		if (
			DoesInventorySlotContainItemType(SlotIndex, TEXT("Shotgun")) ||
			DoesInventorySlotContainItemType(SlotIndex, TEXT("Pistol"))
		)
		{
			const bool bUsedWeapon = TryUseItemInSlot(SlotIndex);

			if (GetInventorySlotItemValue(SlotIndex) <= 0)
			{
				TryRemoveItemInSlot(SlotIndex);

				GEngine->AddOnScreenDebugMessage(
					-1,
					2.f,
					FColor::Red,
					TEXT("Removed empty weapon")
				);
			}

			return bUsedWeapon;
		}
	}

	return false;
}

int32 UZombieAgentBrainComponent::GetInventorySlotItemValue(int32 SlotIndex) const
{
	if (!InventoryComponent) return 0;

	UFunction* GetInventoryFunction = InventoryComponent->FindFunction(TEXT("GetInventory"));
	if (!GetInventoryFunction) return 0;

	struct FGetInventoryParams
	{
		TArray<AActor*> ReturnValue;
	};

	FGetInventoryParams Params;
	InventoryComponent->ProcessEvent(GetInventoryFunction, &Params);

	if (!Params.ReturnValue.IsValidIndex(SlotIndex))
	{
		return 0;
	}

	AActor* Item = Params.ReturnValue[SlotIndex];
	if (!Item)
	{
		return 0;
	}

	UFunction* GetValueFunction = Item->FindFunction(TEXT("GetValue"));
	if (!GetValueFunction) return 0;

	struct FGetValueParams
	{
		int32 ReturnValue;
	};

	FGetValueParams ValueParams;
	ValueParams.ReturnValue = 0;

	Item->ProcessEvent(GetValueFunction, &ValueParams);

	return ValueParams.ReturnValue;
}

void UZombieAgentBrainComponent::ExecuteSearchHouse()
{
	AActor* ClosestHouse = GetClosestHouse();
	if (!ClosestHouse) return;
	
	const float DistanceToHouse = FVector::Dist(GetOwner()->GetActorLocation(), ClosestHouse->GetActorLocation());

	if (DistanceToHouse <= HouseSearchAcceptanceRadius + 100.f)
	{
		if (!SearchedHouses.Contains(ClosestHouse))
		{
			SearchedHouses.Add(ClosestHouse);
		}
		
		if (Perceptor)
		{
			Perceptor->SeenHouses.Remove(ClosestHouse);
		}

		GEngine->AddOnScreenDebugMessage(
			-1,
			2.f,
			FColor::Blue,
			TEXT("Finished searching house")
		);

		return;
	}

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn) return;

	AAIController* AIController = Cast<AAIController>(OwnerPawn->GetController());
	if (!AIController) return;

	AIController->MoveToActor(
		ClosestHouse,
		HouseSearchAcceptanceRadius,
		true,
		true,
		true,
		nullptr,
		true
	);

	GEngine->AddOnScreenDebugMessage(
		-1,
		1.f,
		FColor::Blue,
		TEXT("Searching house")
	);
}

AActor* UZombieAgentBrainComponent::GetClosestHouse() const
{
	if (!Perceptor) return nullptr;

	AActor* ClosestHouse = nullptr;
	float ClosestDistance = FLT_MAX;

	const FVector OwnerLocation = GetOwner()->GetActorLocation();

	for (AActor* House : Perceptor->SeenHouses)
	{
		if (!IsValid(House)) continue;
		if (HasHouseBeenSearched(House)) continue;

		const float Distance = FVector::Dist(
			OwnerLocation,
			House->GetActorLocation()
		);

		if (Distance < ClosestDistance)
		{
			ClosestDistance = Distance;
			ClosestHouse = House;
		}
	}

	return ClosestHouse;
}

void UZombieAgentBrainComponent::ExecuteAvoidPurge()
{
	AActor* ClosestPurgeZone = GetClosestPurgeZone();
	if (!ClosestPurgeZone) return;

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn) return;

	AAIController* AIController = Cast<AAIController>(OwnerPawn->GetController());
	if (!AIController) return;

	AIController->MoveToLocation(
		GetPurgeAvoidanceLocation(ClosestPurgeZone),
		100.f,
		true,
		true,
		true,
		false,
		nullptr,
		true
	);

	GEngine->AddOnScreenDebugMessage(
		-1,
		1.f,
		FColor::Purple,
		TEXT("Avoiding purge zone")
	);
}

AActor* UZombieAgentBrainComponent::GetClosestPurgeZone() const
{
	if (!Perceptor) return nullptr;

	AActor* ClosestPurgeZone = nullptr;
	float ClosestDistance = FLT_MAX;

	const FVector OwnerLocation = GetOwner()->GetActorLocation();

	for (AActor* PurgeZone : Perceptor->SeenPurgeZones)
	{
		if (!IsValid(PurgeZone)) continue;

		const float Distance = FVector::Dist(
			OwnerLocation,
			PurgeZone->GetActorLocation()
		);

		if (Distance < ClosestDistance)
		{
			ClosestDistance = Distance;
			ClosestPurgeZone = PurgeZone;
		}
	}

	return ClosestPurgeZone;
}

FVector UZombieAgentBrainComponent::GetPurgeAvoidanceLocation(AActor* PurgeZone) const
{
	if (!PurgeZone)
	{
		return GetOwner()->GetActorLocation();
	}

	const FVector OwnerLocation = GetOwner()->GetActorLocation();

	const FVector AwayDirection =
		(OwnerLocation - PurgeZone->GetActorLocation()).GetSafeNormal();

	return OwnerLocation + AwayDirection * PurgeFleeDistance;
}

bool UZombieAgentBrainComponent::HasInventoryItemType(const FString& ItemType) const
{
	const int32 Capacity = GetInventoryCapacity();

	for (int32 SlotIndex = 0; SlotIndex < Capacity; ++SlotIndex)
	{
		if (DoesInventorySlotContainItemType(SlotIndex, ItemType))
		{
			return true;
		}
	}

	return false;
}

bool UZombieAgentBrainComponent::HasHouseBeenSearched(AActor* House) const
{
	return SearchedHouses.Contains(House);
}

int32 UZombieAgentBrainComponent::GetLowestInventoryPrioritySlot() const
{
	if (!InventoryComponent)
	{
		return INDEX_NONE;
	}

	UFunction* GetInventoryFunction = InventoryComponent->FindFunction(TEXT("GetInventory"));
	if (!GetInventoryFunction)
	{
		return INDEX_NONE;
	}

	struct FGetInventoryParams
	{
		TArray<AActor*> ReturnValue;
	};

	FGetInventoryParams Params;
	InventoryComponent->ProcessEvent(GetInventoryFunction, &Params);

	int32 LowestSlot = INDEX_NONE;
	int32 LowestPriority = 999;

	for (int32 SlotIndex = 0; SlotIndex < Params.ReturnValue.Num(); ++SlotIndex)
	{
		AActor* Item = Params.ReturnValue[SlotIndex];

		if (!Item)
		{
			continue;
		}

		const int32 Priority = GetItemPriority(Item);

		if (Priority < LowestPriority)
		{
			LowestPriority = Priority;
			LowestSlot = SlotIndex;
		}
	}

	return LowestSlot;
}

bool UZombieAgentBrainComponent::TryReplaceInventoryItem(AActor* NewItem)
{
	if (!NewItem)
	{
		return false;
	}

	const int32 NewPriority = GetItemPriority(NewItem);

	const int32 LowestSlot = GetLowestInventoryPrioritySlot();

	if (LowestSlot == INDEX_NONE)
	{
		return false;
	}

	UFunction* GetInventoryFunction = InventoryComponent->FindFunction(TEXT("GetInventory"));

	struct FGetInventoryParams
	{
		TArray<AActor*> ReturnValue;
	};

	FGetInventoryParams Params;
	InventoryComponent->ProcessEvent(GetInventoryFunction, &Params);

	if (!Params.ReturnValue.IsValidIndex(LowestSlot))
	{
		return false;
	}

	AActor* ExistingItem = Params.ReturnValue[LowestSlot];

	if (!ExistingItem)
	{
		return false;
	}

	const int32 ExistingPriority = GetItemPriority(ExistingItem);

	if (NewPriority <= ExistingPriority)
	{
		return false;
	}

	TryRemoveItemInSlot(LowestSlot);

	if (TryGrabItemInSlot(LowestSlot, NewItem))
	{
		if (Perceptor)
		{
			Perceptor->SeenItems.Remove(NewItem);
		}

		GEngine->AddOnScreenDebugMessage(
			-1,
			2.f,
			FColor::Orange,
			TEXT("Replaced low priority inventory item")
		);

		return true;
	}

	return false;
}