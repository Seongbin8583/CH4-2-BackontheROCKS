#include "BOTRPickup.h"

#include "BOTRPlayerState.h"
#include "BOTRGameStateBase.h"
#include "BackontheROCKSPawn.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundBase.h"

ABOTRPickup::ABOTRPickup()
{
    PrimaryActorTick.bCanEverTick = false;

    bReplicates = true;
    bAlwaysRelevant = true;

    NetUpdateFrequency = 1.0f;
    MinNetUpdateFrequency = 1.0f;

    Trigger =
        CreateDefaultSubobject<USphereComponent>(
            TEXT("Trigger")
        );

    SetRootComponent(Trigger);

    Trigger->InitSphereRadius(100.0f);
    Trigger->SetCollisionEnabled(
        ECollisionEnabled::QueryOnly
    );
    Trigger->SetCollisionObjectType(ECC_WorldDynamic);
    Trigger->SetCollisionResponseToAllChannels(ECR_Ignore);
    Trigger->SetCollisionResponseToChannel(
        ECC_Vehicle,
        ECR_Overlap
    );
    Trigger->SetCollisionResponseToChannel(
        ECC_Pawn,
        ECR_Overlap
    );
    Trigger->SetGenerateOverlapEvents(true);

    ItemMesh =
        CreateDefaultSubobject<UStaticMeshComponent>(
            TEXT("ItemMesh")
        );

    ItemMesh->SetupAttachment(Trigger);
    ItemMesh->SetCollisionEnabled(
        ECollisionEnabled::NoCollision
    );
}

void ABOTRPickup::BeginPlay()
{
    Super::BeginPlay();

    if (HasAuthority())
    {
        Trigger->OnComponentBeginOverlap.AddDynamic(
            this,
            &ABOTRPickup::OnPickupOverlap
        );
    }
    else
    {
        // ク라이언트는 표시만 담당합니다.
        Trigger->SetCollisionEnabled(
            ECollisionEnabled::NoCollision
        );
    }
}

bool ABOTRPickup::GiveReward(ABOTRPlayerState* PS)
{
    return false;
}

void ABOTRPickup::OnPickupOverlap(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult
)
{
    if (!HasAuthority() || bCollected)
    {
        return;
    }

    const ABOTRGameStateBase* GS =
        GetWorld()->GetGameState<ABOTRGameStateBase>();

    if (!GS ||
        GS->CurrentState != EBOTRGameStateBase::Playing)
    {
        return;
    }

    ABackontheROCKSPawn* Vehicle =
        Cast<ABackontheROCKSPawn>(OtherActor);

    if (!Vehicle)
    {
        return;
    }

    ABOTRPlayerState* PS =
        Vehicle->GetPlayerState<ABOTRPlayerState>();

    if (!PS)
    {
        return;
    }

    // 같은 프레임에 여러 컴포넌트가 겹쳐도 한 번만 처리합니다.
    bCollected = true;

    if (!GiveReward(PS))
    {
        bCollected = false;
        return;
    }

    SetActorEnableCollision(false);

    MulticastCollected();

    // 연출 RPC를 보낸 뒤 서버에서 파괴합니다.
    // 스포너의 2초 타이머는 실제 파괴 시점부터 시작합니다.
    SetLifeSpan(0.25f);
}

void ABOTRPickup::MulticastCollected_Implementation()
{
    SetActorHiddenInGame(true);
    SetActorEnableCollision(false);

    if (GetNetMode() == NM_DedicatedServer)
    {
        return;
    }

    if (PickupParticle)
    {
        UGameplayStatics::SpawnEmitterAtLocation(
            GetWorld(),
            PickupParticle,
            GetActorLocation(),
            GetActorRotation()
        );
    }

    if (PickupSound)
    {
        UGameplayStatics::PlaySoundAtLocation(
            this,
            PickupSound,
            GetActorLocation()
        );
    }
}