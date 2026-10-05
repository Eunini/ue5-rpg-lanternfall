#include "LanternUsable.h"
#include "JourneySubsystem.h"
#include "PortfolioVisual.h"
#include "GameFramework/Pawn.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"

ALanternUsable::ALanternUsable()
{
 Body=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body"));SetRootComponent(Body);
 Body->SetStaticMesh(LoadObject<UStaticMesh>(nullptr,TEXT("/Engine/BasicShapes/Cube.Cube")));
 Body->SetCollisionProfileName(TEXT("BlockAll"));
 PrimaryActorTick.bCanEverTick=true;PrimaryActorTick.TickInterval=.2f;
}
void ALanternUsable::Configure(ELanternAction Value,FName Id){Action=Value;PickupId=Id;OnConstruction(GetActorTransform());}
void ALanternUsable::OnConstruction(const FTransform& Transform)
{
 Super::OnConstruction(Transform);
 PortfolioVisual::Clear(this);
 const bool Cell=Action==ELanternAction::Cell;
 Body->SetRelativeScale3D(Cell?FVector(.4f,.4f,.3f):FVector(.65f,.65f,1.8f));
 PortfolioVisual::Color(Body,Action==ELanternAction::Relay?FLinearColor(.06f,.5f,.7f):FLinearColor(.9f,.48f,.13f));
 PortfolioVisual::Label(this,GetPrompt().ToString(),FVector(0,0,Cell?65:160),Cell?23:30);
}
FText ALanternUsable::GetPrompt() const
{
 switch (Action)
 {
  case ELanternAction::Cell:return FText::FromString(TEXT("ENERGY CELL / E"));
  case ELanternAction::Clinic:return FText::FromString(TEXT("CLINIC CIRCUIT / E"));
  case ELanternAction::Relay:return FText::FromString(TEXT("RELAY CIRCUIT / E"));
  default:return FText::FromString(TEXT("NERA / E TO TALK"));
 }
}
void ALanternUsable::Interact_Implementation(APawn* Interactor){PerformAction(Interactor);}
void ALanternUsable::PerformAction(APawn* Interactor)
{
 if (!Interactor || !Interactor->IsLocallyControlled() || FVector::DistSquared(GetActorLocation(),Interactor->GetActorLocation())>FMath::Square(350.f)) return;
 auto* Journey=GetGameInstance()->GetSubsystem<UJourneySubsystem>();
 if (Action==ELanternAction::Nera) Journey->OpenDialogue();
 else if (Action==ELanternAction::Cell) Journey->CollectCell(PickupId);
 else Journey->DeliverPower(Action==ELanternAction::Clinic?1:2);
}
void ALanternUsable::Tick(float Delta)
{
 Super::Tick(Delta);
 if (Action!=ELanternAction::Cell) return;
 if (const auto* Journey=GetGameInstance()->GetSubsystem<UJourneySubsystem>())
 {
  const bool Claimed=Journey->IsClaimed(PickupId);
  SetActorHiddenInGame(Claimed);SetActorEnableCollision(!Claimed);
 }
}


void ALanternUsable::BeginPlay(){Super::BeginPlay();OnConstruction(GetActorTransform());}
