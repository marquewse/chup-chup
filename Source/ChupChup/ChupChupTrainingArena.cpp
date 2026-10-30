#include "ChupChupTrainingArena.h"

#include "ChupChupTargetBot.h"
#include "ChupChupVisuals.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/TargetPoint.h"
#include "Engine/World.h"
#include "UObject/ConstructorHelpers.h"

namespace
{
	// Floor top sits slightly above Z=0 so it never z-fights with a template floor.
	constexpr float FloorZ = 2.f;

	const FLinearColor FloorColor(0.36f, 0.38f, 0.42f);
	const FLinearColor GridColor(0.2f, 0.21f, 0.24f);
	const FLinearColor OuterWallColor(0.78f, 0.73f, 0.64f);
	const FLinearColor MidWallColor(0.5f, 0.56f, 0.66f);
	const FLinearColor CrateOrange(0.85f, 0.4f, 0.1f);
	const FLinearColor CrateTeal(0.06f, 0.5f, 0.52f);
	const FLinearColor PlatformColor(0.3f, 0.33f, 0.4f);
}

AChupChupTrainingArena::AChupChupTrainingArena()
{
	PrimaryActorTick.bCanEverTick = false;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> Cube(TEXT("/Engine/BasicShapes/Cube.Cube"));
	CubeMesh = Cube.Object;

	ArenaRoot = CreateDefaultSubobject<USceneComponent>(TEXT("ArenaRoot"));
	ArenaRoot->SetMobility(EComponentMobility::Static);
	SetRootComponent(ArenaRoot);
}

void AChupChupTrainingArena::BeginPlay()
{
	Super::BeginPlay();

	// Covers arenas placed by hand in a level; the game mode normally builds it first.
	BuildArena();
}

AActor* AChupChupTrainingArena::GetPlayerSpawnPoint() const
{
	return PlayerSpawnPoint;
}

UStaticMeshComponent* AChupChupTrainingArena::AddBlock(const FVector& Center, const FVector& Size, const FLinearColor& Color, const FRotator& Rotation)
{
	UStaticMeshComponent* Block = NewObject<UStaticMeshComponent>(this);
	Block->SetMobility(EComponentMobility::Static);
	Block->SetStaticMesh(CubeMesh);
	Block->SetupAttachment(ArenaRoot);
	Block->SetRelativeLocationAndRotation(Center, Rotation);
	Block->SetRelativeScale3D(Size / 100.f); // The engine cube is 100 units across.
	Block->RegisterComponent();
	AddInstanceComponent(Block);

	ChupChupVisuals::ApplyColor(Block, Color);
	return Block;
}

UStaticMeshComponent* AChupChupTrainingArena::AddBox(float X, float Y, const FVector& Size, const FLinearColor& Color, float BaseZ)
{
	return AddBlock(FVector(X, Y, FloorZ + BaseZ + Size.Z * 0.5f), Size, Color);
}

void AChupChupTrainingArena::SpawnBot(const FVector& FeetLocation, bool bStrafe)
{
	FActorSpawnParameters Params;
	Params.Owner = this;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// Bots face the spawn (-X).
	const FTransform& ArenaTransform = GetActorTransform();
	const FVector Location = ArenaTransform.TransformPosition(FeetLocation + FVector(0.f, 0.f, FloorZ));
	const FRotator Rotation = (ArenaTransform.GetRotation() * FRotator(0.f, 180.f, 0.f).Quaternion()).Rotator();

	AChupChupTargetBot* Bot = GetWorld()->SpawnActor<AChupChupTargetBot>(AChupChupTargetBot::StaticClass(), Location, Rotation, Params);
	if (Bot != nullptr)
	{
		Bot->SetStrafe(bStrafe);
		Bots.Add(Bot);
	}
}

void AChupChupTrainingArena::BuildArena()
{
	if (bBuilt || CubeMesh == nullptr)
	{
		return;
	}
	bBuilt = true;

	// Layout (cm): X forward from spawn, Y to the right. Playable area X -1000..5000, Y -1500..1500.
	constexpr float MinX = -1000.f;
	constexpr float MaxX = 5000.f;
	constexpr float HalfWidth = 1500.f;
	constexpr float WallHeight = 500.f;
	constexpr float WallThickness = 50.f;
	const float CenterX = (MinX + MaxX) * 0.5f;
	const float Length = MaxX - MinX;

	// Floor and grid lines every 5 m, for judging distance.
	AddBlock(FVector(CenterX, 0.f, FloorZ - 10.f), FVector(Length, HalfWidth * 2.f, 20.f), FloorColor);
	for (float X = MinX + 500.f; X < MaxX; X += 500.f)
	{
		AddBlock(FVector(X, 0.f, FloorZ + 0.5f), FVector(6.f, HalfWidth * 2.f, 1.f), GridColor);
	}
	for (float Y = -HalfWidth + 500.f; Y < HalfWidth; Y += 500.f)
	{
		AddBlock(FVector(CenterX, Y, FloorZ + 0.5f), FVector(Length, 6.f, 1.f), GridColor);
	}

	// Outer walls.
	AddBox(MaxX + WallThickness * 0.5f, 0.f, FVector(WallThickness, HalfWidth * 2.f + WallThickness * 2.f, WallHeight), OuterWallColor);
	AddBox(MinX - WallThickness * 0.5f, 0.f, FVector(WallThickness, HalfWidth * 2.f + WallThickness * 2.f, WallHeight), OuterWallColor);
	AddBox(CenterX, HalfWidth + WallThickness * 0.5f, FVector(Length, WallThickness, WallHeight), OuterWallColor);
	AddBox(CenterX, -HalfWidth - WallThickness * 0.5f, FVector(Length, WallThickness, WallHeight), OuterWallColor);

	// Spawn cover pillars.
	AddBox(-300.f, -700.f, FVector(120.f, 120.f, 300.f), MidWallColor);
	AddBox(-300.f, 700.f, FVector(120.f, 120.f, 300.f), MidWallColor);

	// Close range: crouch-height cover with three static bots behind it.
	AddBox(700.f, -600.f, FVector(60.f, 200.f, 100.f), CrateTeal);
	AddBox(700.f, 600.f, FVector(60.f, 200.f, 100.f), CrateTeal);
	SpawnBot(FVector(1100.f, -600.f, 0.f), false);
	SpawnBot(FVector(1100.f, 0.f, 0.f), false);
	SpawnBot(FVector(1100.f, 600.f, 0.f), false);

	// Mid wall with two doors (Y -900..-300 and 300..900).
	constexpr float MidX = 1800.f;
	constexpr float MidHeight = 400.f;
	AddBox(MidX, -1200.f, FVector(60.f, 600.f, MidHeight), MidWallColor);
	AddBox(MidX, 0.f, FVector(60.f, 600.f, MidHeight), MidWallColor);
	AddBox(MidX, 1200.f, FVector(60.f, 600.f, MidHeight), MidWallColor);
	AddBox(MidX, -600.f, FVector(60.f, 600.f, 100.f), MidWallColor, 300.f); // Door lintels.
	AddBox(MidX, 600.f, FVector(60.f, 600.f, 100.f), MidWallColor, 300.f);

	// Long range: crates for cover and peeking.
	AddBox(2600.f, -700.f, FVector(120.f, 120.f, 120.f), CrateOrange);
	AddBox(2600.f, -700.f, FVector(100.f, 100.f, 100.f), CrateOrange, 120.f);
	AddBox(2700.f, -560.f, FVector(120.f, 120.f, 120.f), CrateOrange);
	AddBox(3000.f, 500.f, FVector(150.f, 150.f, 150.f), CrateTeal);
	AddBox(3400.f, -150.f, FVector(100.f, 200.f, 100.f), CrateOrange);
	AddBox(3600.f, 1100.f, FVector(200.f, 120.f, 180.f), CrateTeal);

	// Ramp up to a raised platform in the back-left corner.
	constexpr float PlatformHeight = 250.f;
	AddBox(4750.f, -1200.f, FVector(500.f, 600.f, PlatformHeight), PlatformColor);
	const float RampRun = 500.f;
	const float RampPitch = FMath::RadiansToDegrees(FMath::Atan2(PlatformHeight, RampRun));
	const float RampLength = FMath::Sqrt(RampRun * RampRun + PlatformHeight * PlatformHeight);
	AddBlock(FVector(4500.f - RampRun * 0.5f, -1200.f, FloorZ + PlatformHeight * 0.5f), FVector(RampLength, 300.f, 20.f), PlatformColor, FRotator(RampPitch, 0.f, 0.f));

	// Far bots: strafing on the sides, static in the middle, one on the crate and one on the platform.
	SpawnBot(FVector(4200.f, -500.f, 0.f), true);
	SpawnBot(FVector(4200.f, 300.f, 0.f), false);
	SpawnBot(FVector(4200.f, 900.f, 0.f), true);
	SpawnBot(FVector(3000.f, 500.f, 150.f), false);
	SpawnBot(FVector(4850.f, -1200.f, PlatformHeight), false);

	// Player spawn at the back, looking down the range.
	FActorSpawnParameters Params;
	Params.Owner = this;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	const FVector SpawnLocation = GetActorTransform().TransformPosition(FVector(-700.f, 0.f, FloorZ + 100.f));
	PlayerSpawnPoint = GetWorld()->SpawnActor<ATargetPoint>(ATargetPoint::StaticClass(), SpawnLocation, GetActorRotation(), Params);
}
