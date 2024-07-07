#include "CPortal.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

ACPortal::ACPortal()
{
	PrimaryActorTick.bCanEverTick = true;

	SetActorEnableCollision(true);

	Collider = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));

}

void ACPortal::BeginPlay()
{
	Super::BeginPlay();
	
	Collider->OnComponentBeginOverlap.AddDynamic(this, &ACPortal::OnOverlapBegin);
}

void ACPortal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACPortal::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Log, TEXT("Overlap Event"));

	//ACharacter* tempCharacter = Cast<ACharacter>(OtherActor);
	//if (tempCharacter == nullptr) return;
	//FVector CurrLocation = tempCharacter->GetActorLocation();
	//CurrLocation.Z += 200.f;
	//tempCharacter->SetActorLocation(CurrLocation);

	//UE_LOG(LogTemp, Log, TEXT("Jump Event"));

	//GetWorld()->ServerTravel(LevelName);
	UGameplayStatics::OpenLevel(GetWorld(), LevelName);
}

