
#include "InteractionSystemModuleInterface.h"
#include <AzCore/Memory/Memory.h>

#include <InteractionSystem/InteractionSystemTypeIds.h>

#include <Clients/InteractionSystemSystemComponent.h>

#include "InteractorComponent.h"
#include "InteractableComponent.h"
#include "InteractionOverlapFinderComponent.h"

namespace InteractionSystem
{
    AZ_TYPE_INFO_WITH_NAME_IMPL(InteractionSystemModuleInterface,
        "InteractionSystemModuleInterface", InteractionSystemModuleInterfaceTypeId);
    AZ_RTTI_NO_TYPE_INFO_IMPL(InteractionSystemModuleInterface, AZ::Module);
    AZ_CLASS_ALLOCATOR_IMPL(InteractionSystemModuleInterface, AZ::SystemAllocator);

    InteractionSystemModuleInterface::InteractionSystemModuleInterface()
    {
        // Push results of [MyComponent]::CreateDescriptor() into m_descriptors here.
        // Add ALL components descriptors associated with this gem to m_descriptors.
        // This will associate the AzTypeInfo information for the components with the the SerializeContext, BehaviorContext and EditContext.
        // This happens through the [MyComponent]::Reflect() function.
        m_descriptors.insert(m_descriptors.end(), {
            InteractionSystemSystemComponent::CreateDescriptor(),
			InteractorComponent::CreateDescriptor(),
			InteractableComponent::CreateDescriptor(),
			InteractionOverlapFinderComponent::CreateDescriptor(),
            });
    }

    AZ::ComponentTypeList InteractionSystemModuleInterface::GetRequiredSystemComponents() const
    {
        return AZ::ComponentTypeList{
            azrtti_typeid<InteractionSystemSystemComponent>(),
        };
    }
} // namespace InteractionSystem
