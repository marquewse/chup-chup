#include "ChupChupTargetBot.h"

#include "ChupChupHealthComponent.h"
#include "ChupChupVisuals.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"

AChupChupTargetBot::AChupChupTargetBot()
{
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));

	// Root sits at the feet.
	BotRoot = CreateDefaultSubobject<USceneComponent>(TEXT("BotRoot"));
	SetRootComponent(BotRoot);

	Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));
	Body->SetupAttachment(BotRoot);
	Body->SetStaticMesh(CylinderMesh.Object);
	Body->SetMobility(EComponentMobility::Movable);
	Body->SetRelativeLocation(FVector(0.f, 0.f, 75.f));
	Body->SetRelativeScale3D(FVector(0.6f, 0.6f, 1.5f));

	Head = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Head"));
	Head->SetupAttachment(BotRoot);
	Head->SetStaticMesh(SphereMesh.Object);
	Head->SetMobility(EComponentMobility::Movable);
	Head->SetRelativeLocation(FVector(0.f, 0.f, 168.f));
	Head->SetRelativeScale3D(FVector(0.32f));
	Head->ComponentTags.Add(TEXT("Head"));

	Health = CreateDefaultSubobject<UChupChupHealthComponent>(TEXT("Health"));
}

void AChupChupTargetBot::BeginPlay()
{
	Super::BeginPlay();

	HomeLocation = GetActorLocation();
	BodyMaterial = ChupChupVisuals::ApplyColor(Body, BodyColor);
	HeadMaterial = ChupChupVisuals::ApplyColor(Head, HeadColor);

	Health->OnDamaged.AddDynamic(this, &AChupChupTargetBot::HandleDamaged);
	Health->OnDeath.AddDynamic(this, &AChupChupTargetBot::HandleDeath);
}

void AChupChupTargetBot::SetStrafe(bool bEnable, float Amplitude, float Speed)
{
	bStrafe = bEnable;
	StrafeAmplitude = Amplitude;
	StrafeSpeed = Speed;
}

void AChupChupTargetBot::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bFlashing && GetWorld()->GetTimeSeconds() >= FlashEndTime)
	{
		SetFlash(false);
	}

	if (bStrafe && !Health->IsDead())
	{
		StrafeTime += DeltaSeconds;
		const float Offset = FMath::Sin(StrafeTime * StrafeSpeed) * StrafeAmplitude;
		SetActorLocation(HomeLocation + GetActorRightVector() * Offset);
	}
}

void AChupChupTargetBot::HandleDamaged(UChupChupHealthComponent* DamagedHealth, float Damage, AActor* DamageCauser)
{
	SetFlash(true);
	FlashEndTime = GetWorld()->GetTimeSeconds() + 0.08f;
}

void AChupChupTargetBot::HandleDeath(UChupChupHealthComponent* DeadHealth, AActor* Killer)
{
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	GetWorldTimerManager().SetTimer(RespawnTimerHandle, this, &AChupChupTargetBot::Respawn, RespawnDelay, false);
}

void AChupChupTargetBot::Respawn()
{
	Health->ResetHealth();
	SetFlash(false);
	StrafeTime = 0.f;
	SetActorLocation(HomeLocation);
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
}

void AChupChupTargetBot::SetFlash(bool bFlashOn)
{
	bFlashing = bFlashOn;

	if (BodyMaterial != nullptr)
	{
		BodyMaterial->SetVectorParameterValue(TEXT("Color"), bFlashOn ? FLinearColor::White : BodyColor);
	}
	if (HeadMaterial != nullptr)
	{
		HeadMaterial->SetVectorParameterValue(TEXT("Color"), bFlashOn ? FLinearColor::White : HeadColor);
	}
}
