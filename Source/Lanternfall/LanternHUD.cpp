#include "LanternHUD.h"
#include "LanternPlayer.h"
#include "LanternUsable.h"
#include "JourneySubsystem.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "GameFramework/PlayerController.h"
void ALanternHUD::DrawHUD()
{
 Super::DrawHUD();
 if (!Canvas) return;
 auto* Player=Cast<ALanternPlayer>(GetOwningPawn());
 if (!Player) return;
 auto* Journey=GetGameInstance()->GetSubsystem<UJourneySubsystem>();
 const float X=32,Y=30,W=Canvas->ClipX,H=Canvas->ClipY;
 const FLinearColor Cream(.9f,.91f,.85f),Amber(.98f,.62f,.24f),Dark(.025f,.04f,.05f,.88f);
 DrawRect(Dark,0,0,W,130);DrawText(TEXT("LANTERNFALL"),Amber,X,Y,GEngine->GetLargeFont());
 DrawText(Journey->GetObjective().ToString(),Cream,X,Y+46,GEngine->GetSmallFont());
 DrawText(FString::Printf(TEXT("ENERGY CELLS  %d / 2"),Journey->GetCells()),Amber,W-220,Y+15,GEngine->GetSmallFont());
 DrawRect(Cream,W/2-2,H/2-2,4,4);
 if (auto* Target=Player->Focused()) DrawText(Target->GetPrompt().ToString(),Amber,W/2-130,H/2+30,GEngine->GetSmallFont());
 DrawRect(Dark,0,H-78,W,78);
 DrawText(TEXT("WASD move / Mouse look / E interact / 1-3 dialogue / Tab journal / F6 save / F7 load"),Cream,X,H-62,GEngine->GetSmallFont());
 DrawText(Journey->Status.ToString(),Amber,X,H-34,GEngine->GetSmallFont());
 if (Journey->IsDialogueOpen())
 {
  const float BoxX=FMath::Max(30.f,W*.12f),BoxY=H*.3f;
  DrawRect(Dark,BoxX-20,BoxY-20,W-2*BoxX+40,300);
  TArray<FString> Lines;Journey->GetDialogue().ToString().ParseIntoArrayLines(Lines);
  float TextY=BoxY;
  for (const FString& Line:Lines){DrawText(Line,Cream,BoxX,TextY,GEngine->GetSmallFont());TextY+=24;}
  TextY+=22;int32 Index=1;
  for (const FText& Choice:Journey->GetChoices())
  {DrawText(FString::Printf(TEXT("%d   %s"),Index++,*Choice.ToString()),Amber,BoxX,TextY,GEngine->GetSmallFont());TextY+=30;}
 }
 else if (Player->bJournal)
 {
  DrawRect(Dark,W*.16f,H*.27f,W*.68f,230);
  DrawText(TEXT("FIELD JOURNAL"),Amber,W*.19f,H*.3f,GEngine->GetLargeFont());
  DrawText(Journey->GetObjective().ToString(),Cream,W*.19f,H*.3f+50,GEngine->GetSmallFont());
  const auto& Snapshot=Journey->GetSnapshot();float LineY=H*.3f+95;
  for (const auto& Item:Snapshot.Items)
  {DrawText(FString::Printf(TEXT("%s  x%d"),UTF8_TO_TCHAR(Item.first.c_str()),Item.second),Cream,W*.19f,LineY,GEngine->GetSmallFont());LineY+=24;}
 }
}

