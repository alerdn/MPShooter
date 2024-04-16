#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "MultiplayerSessionsSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FServerCreateDelegate, bool, bWasSuccessful);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FServerJoinDelegate, bool, bWasSuccessful);

UCLASS()
class MPSHOOTER_API UMultiplayerSessionsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UMultiplayerSessionsSubsystem();

	virtual void Initialize(FSubsystemCollectionBase &Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable)
	void CreateServer(FString ServerName);
	UFUNCTION(BlueprintCallable)
	void FindServer(FString ServerName);

	UFUNCTION(BlueprintPure)
	FString GetMapPath() const { return MapPath; }
	UFUNCTION(BlueprintCallable)
	void SetMapPath(FString NewMapPath) { MapPath = NewMapPath; }

private:
	FString MapPath;

	TSharedPtr<FOnlineSessionSearch> SessionSearch;
	IOnlineSessionPtr SessionInterface;

	bool bCreateServerAfterDestroy;
	FString DestroyedServerName;
	FString ServerNameToFind;
	FName MySessionName;

	UPROPERTY(BlueprintAssignable)
	FServerCreateDelegate ServerCreateDelegate;
	UPROPERTY(BlueprintAssignable)
	FServerJoinDelegate ServerJoinDelegate;

	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionsComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionrName, EOnJoinSessionCompleteResult::Type Result);
};
