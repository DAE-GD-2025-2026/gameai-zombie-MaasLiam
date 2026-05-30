#include "ZombieSprintHelper.h"

#include "GameFramework/FloatingPawnMovement.h"
#include "UObject/UnrealType.h"

void FZombieSprintHelper::UpdateSprint(AActor* Owner, UActorComponent* StaminaComponent, bool bShouldSprint, float MinimumSprintStamina)
{
	if (!Owner)
	{
		return;
	}

	const float CurrentStamina = GetCurrentStamina(StaminaComponent);

	if (bShouldSprint && CurrentStamina >= MinimumSprintStamina)
	{
		StartRunning(Owner);
		return;
	}

	StopRunning(Owner);
}

void FZombieSprintHelper::StartRunning(AActor* Owner)
{
	if (!Owner)
	{
		return;
	}

	if (FBoolProperty* RunningProperty = FindFProperty<FBoolProperty>(Owner->GetClass(), TEXT("bIsRunning")))
	{
		RunningProperty->SetPropertyValue_InContainer(Owner, true);
	}

	UFloatingPawnMovement* MovementComponent = Owner->FindComponentByClass<UFloatingPawnMovement>();

	if (!MovementComponent)
	{
		return;
	}

	float RunningSpeed = 600.f;

	if (FFloatProperty* RunningSpeedProperty = FindFProperty<FFloatProperty>(Owner->GetClass(), TEXT("RunningSpeed")))
	{
		RunningSpeed = RunningSpeedProperty->GetPropertyValue_InContainer(Owner);
	}

	MovementComponent->MaxSpeed = RunningSpeed;
}

void FZombieSprintHelper::StopRunning(AActor* Owner)
{
	if (!Owner)
	{
		return;
	}

	if (FBoolProperty* RunningProperty = FindFProperty<FBoolProperty>(Owner->GetClass(), TEXT("bIsRunning")))
	{
		RunningProperty->SetPropertyValue_InContainer(Owner, false);
	}

	UFloatingPawnMovement* MovementComponent = Owner->FindComponentByClass<UFloatingPawnMovement>();

	if (!MovementComponent)
	{
		return;
	}

	float DefaultSpeed = 400.f;

	if (FFloatProperty* DefaultSpeedProperty = FindFProperty<FFloatProperty>(Owner->GetClass(), TEXT("DefaultSpeed")))
	{
		DefaultSpeed = DefaultSpeedProperty->GetPropertyValue_InContainer(Owner);
	}

	MovementComponent->MaxSpeed = DefaultSpeed;
}

bool FZombieSprintHelper::IsRunning(AActor* Owner)
{
	if (!Owner)
	{
		return false;
	}

	if (FBoolProperty* RunningProperty = FindFProperty<FBoolProperty>(Owner->GetClass(), TEXT("bIsRunning")))
	{
		return RunningProperty->GetPropertyValue_InContainer(Owner);
	}

	return false;
}

float FZombieSprintHelper::GetCurrentStamina(UActorComponent* StaminaComponent)
{
	if (!StaminaComponent)
	{
		return 0.f;
	}

	UFunction* StaminaFunction = StaminaComponent->FindFunction(TEXT("GetCurrentStamina"));

	if (!StaminaFunction)
	{
		return 0.f;
	}

	struct FStaminaParams
	{
		float ReturnValue;
	};

	FStaminaParams Params;
	Params.ReturnValue = 0.f;

	StaminaComponent->ProcessEvent(StaminaFunction, &Params);

	return Params.ReturnValue;
}