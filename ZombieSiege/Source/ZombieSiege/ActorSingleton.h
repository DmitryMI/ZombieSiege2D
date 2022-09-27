#include "Kismet/GameplayStatics.h"

#define ACTOR_SINGLETON(Clazz)																	\
private:																						\
	static TMap<UWorld*, Clazz*> managerInstances;												\
																								\
	void InitActorSingleton()																	\
	{																							\
		check(!managerInstances.Contains(GetWorld()));											\
		managerInstances.Add(GetWorld(), this);													\
	}																							\
public:																							\
	static Clazz* GetInstance(UWorld* world)													\
	{																							\
		Clazz* loaded = managerInstances[world];												\
		if (loaded)																				\
		{																						\
			return loaded;																		\
		}																						\
																								\
		AActor* weaponManagerActor = UGameplayStatics::GetActorOfClass(world, StaticClass());	\
		check(weaponManagerActor);																\
																								\
		Clazz* manager = Cast<Clazz>(weaponManagerActor);										\
		check(manager);																			\
																								\
		return manager;																			\
	}
	
#define ACTOR_SINGLETON_IMPLEMENTATION(Clazz)													\
	TMap<UWorld*, Clazz*> Clazz::managerInstances;
