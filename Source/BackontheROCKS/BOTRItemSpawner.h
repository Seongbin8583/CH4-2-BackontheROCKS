#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BOTRItemSpawner.generated.h"

class ABOTRPickup;
class USceneComponent;

UCLASS()
class BACKONTHEROCKS_API ABOTRItemSpawner : public AActor
{
    GENERATED_BODY()

public:
    ABOTRItemSpawner();

protected:
    virtual void BeginPlay() override;

    virtual void EndPlay(
        const EEndPlayReason::Type EndPlayReason
    ) override;

    UPROPERTY(VisibleAnywhere, Category = "Spawner")
    TObjectPtr<USceneComponent> SceneRoot;

    UPROPERTY(
        EditAnywhere,
        BlueprintReadOnly,
        Category = "Spawner"
    )
    TSubclassOf<ABOTRPickup> ItemClass;

    UPROPERTY(
        VisibleInstanceOnly,
        BlueprintReadOnly,
        Category = "Spawner"
    )
    TObjectPtr<ABOTRPickup> SpawnedItem;

    UPROPERTY(
        EditAnywhere,
        BlueprintReadOnly,
        Category = "Spawner",
        meta = (ClampMin = "0.1")
    )
    float RespawnDelay = 2.0f;

    UFUNCTION()
    void OnSpawnedItemDestroyed(AActor* DestroyedActor);

private:
    void SpawnItem();

    FTimerHandle RespawnTimerHandle;

    bool bEndingPlay = false;
};