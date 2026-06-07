#pragma once

#include "CoreMinimal.h"

class UStudentPerceptor;

struct FRememberedItem
{
	AActor* ItemActor = nullptr;
	FVector LastKnownLocation = FVector::ZeroVector;
	FString ItemType = "";
	float TimeSinceLastSeen = 0.f;
};

class FZombieItemMemoryHelper
{
public:
	static void UpdateItemMemory(TArray<FRememberedItem>& RememberedItems, UStudentPerceptor* Perceptor, float DeltaTime, float MaxMemoryAge);

	static FRememberedItem* GetBestRememberedItem(TArray<FRememberedItem>& RememberedItems, AActor* Owner, UActorComponent* InventoryComponent, UActorComponent* HealthComponent,
		UActorComponent* StaminaComponent, int LowHealthThreshold, float LowStaminaThreshold);

	static const FRememberedItem* GetBestRememberedItem(const TArray<FRememberedItem>& RememberedItems, AActor* Owner, UActorComponent* InventoryComponent,
		UActorComponent* HealthComponent, UActorComponent* StaminaComponent, int LowHealthThreshold, float LowStaminaThreshold);

	static void ForgetItem(TArray<FRememberedItem>& RememberedItems, AActor* ItemActor);

	static void ForgetItemNearLocation(TArray<FRememberedItem>& RememberedItems, const FVector& Location, float Radius);

private:
	static FString GetItemType(AActor* ItemActor);

	static bool IsUsefulItemType(const FString& ItemType);

	static bool DoesMemoryMatchNeed(const FRememberedItem& Memory, UActorComponent* InventoryComponent, UActorComponent* HealthComponent, UActorComponent* StaminaComponent, 
		int LowHealthThreshold, float LowStaminaThreshold);

	static void RememberVisibleItem(TArray<FRememberedItem>& RememberedItems, AActor* ItemActor);
};