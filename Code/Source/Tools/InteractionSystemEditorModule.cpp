
#include <InteractionSystem/InteractionSystemTypeIds.h>
#include <InteractionSystemModuleInterface.h>
#include "InteractionSystemEditorSystemComponent.h"

namespace InteractionSystem
{
    class InteractionSystemEditorModule
        : public InteractionSystemModuleInterface
    {
    public:
        AZ_RTTI(InteractionSystemEditorModule, InteractionSystemEditorModuleTypeId, InteractionSystemModuleInterface);
        AZ_CLASS_ALLOCATOR(InteractionSystemEditorModule, AZ::SystemAllocator);

        InteractionSystemEditorModule()
        {
            // Push results of [MyComponent]::CreateDescriptor() into m_descriptors here.
            // Add ALL components descriptors associated with this gem to m_descriptors.
            // This will associate the AzTypeInfo information for the components with the the SerializeContext, BehaviorContext and EditContext.
            // This happens through the [MyComponent]::Reflect() function.
            m_descriptors.insert(m_descriptors.end(), {
                InteractionSystemEditorSystemComponent::CreateDescriptor(),
            });
        }

        /**
         * Add required SystemComponents to the SystemEntity.
         * Non-SystemComponents should not be added here
         */
        AZ::ComponentTypeList GetRequiredSystemComponents() const override
        {
            return AZ::ComponentTypeList {
                azrtti_typeid<InteractionSystemEditorSystemComponent>(),
            };
        }
    };
}// namespace InteractionSystem

#if defined(O3DE_GEM_NAME)
AZ_DECLARE_MODULE_CLASS(AZ_JOIN(Gem_, O3DE_GEM_NAME, _Editor), InteractionSystem::InteractionSystemEditorModule)
#else
AZ_DECLARE_MODULE_CLASS(Gem_InteractionSystem_Editor, InteractionSystem::InteractionSystemEditorModule)
#endif
