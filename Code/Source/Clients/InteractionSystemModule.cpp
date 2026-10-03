
#include <InteractionSystem/InteractionSystemTypeIds.h>
#include <InteractionSystemModuleInterface.h>
#include "InteractionSystemSystemComponent.h"

namespace InteractionSystem
{
    class InteractionSystemModule
        : public InteractionSystemModuleInterface
    {
    public:
        AZ_RTTI(InteractionSystemModule, InteractionSystemModuleTypeId, InteractionSystemModuleInterface);
        AZ_CLASS_ALLOCATOR(InteractionSystemModule, AZ::SystemAllocator);
    };
}// namespace InteractionSystem

#if defined(O3DE_GEM_NAME)
AZ_DECLARE_MODULE_CLASS(AZ_JOIN(Gem_, O3DE_GEM_NAME), InteractionSystem::InteractionSystemModule)
#else
AZ_DECLARE_MODULE_CLASS(Gem_InteractionSystem, InteractionSystem::InteractionSystemModule)
#endif
