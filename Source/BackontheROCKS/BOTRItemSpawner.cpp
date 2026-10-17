#include "BOTRItemSpawner.h"

#include "BOTRPickup.h"
#include "Components/SceneComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"

ABOTRItemSpawner::ABOTRItemSpawner()
{
    PrimaryActorTick.bCanEverTick = false;

    SceneRoot =
        CreateDefaultSubobject<USceneComponent>(
            TEXT("SceneRoot")
        );

    SetRootComponent(SceneRoot);
}

void ABOTRItemSpawner::BeginPlay()
{
    Super::BeginPlay();

    if (HasAuthority())
    {
        SpawnItem();
    }
}

void ABOTRItemSpawner::SpawnItem()
{
    if (!HasAuthority() ||
        bEndingPlay ||
        !ItemClass ||
        IsValid(SpawnedItem))
    {
        return;
    }

    FActorSpawnParameters Params;
    Params.Owner = this;
    Params.SpawnCollisionHandlingOverride =
        ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    SpawnedItem =
        GetWorld()->SpawnActor<ABOTRPickup>(
            ItemClass,
            GetActorLocation(),
            GetActorRotation(),
            Params
        );

    if (IsValid(SpawnedItem))
    {
        SpawnedItem->OnDestroyed.AddDynamic(
            this,
            &ABOTRItemSpawner::OnSpawnedItemDestroyed
        );
    }
    else
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("Item spawn failed: %s"),
            *GetName()
        );

        GetWorldTimerManager().SetTimer(
            RespawnTimerHandle,
            this,
            &ABOTRItemSpawner::SpawnItem,
            FMath::Max(RespawnDelay, 0.1f),
            false
        );
    }
}

void ABOTRItemSpawner::OnSpawnedItemDestroyed(
    AActor* DestroyedActor
)
{
    if (!HasAuthority() ||
        bEndingPlay ||
        DestroyedActor != SpawnedItem.Get())
    {
        return;
    }

    SpawnedItem = nullptr;

    GetWorldTimerManager().SetTimer(
        RespawnTimerHandle,
        this,
        &ABOTRItemSpawner::SpawnItem,
        FMath::Max(RespawnDelay, 0.1f),
        false
    );
}

void ABOTRItemSpawner::EndPlay(
    const EEndPlayReason::Type EndPlayReason
)
{
    bEndingPlay = true;

    GetWorldTimerManager().ClearTimer(
        RespawnTimerHandle
    );

    if (HasAuthority() && IsValid(SpawnedItem))
    {
        SpawnedItem->OnDestroyed.RemoveDynamic(
            this,
            &ABOTRItemSpawner::OnSpawnedItemDestroyed
        );

        SpawnedItem->Destroy();
        SpawnedItem = nullptr;
    }

    Super::EndPlay(EndPlayReason);
}