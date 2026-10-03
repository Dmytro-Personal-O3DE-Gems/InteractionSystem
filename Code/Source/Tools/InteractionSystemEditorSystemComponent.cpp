
#include <AzCore/Serialization/SerializeContext.h>
#include "InteractionSystemEditorSystemComponent.h"

#include <InteractionSystem/InteractionSystemTypeIds.h>

namespace InteractionSystem
{
    AZ_COMPONENT_IMPL(InteractionSystemEditorSystemComponent, "InteractionSystemEditorSystemComponent",
        InteractionSystemEditorSystemComponentTypeId, BaseSystemComponent);

    void InteractionSystemEditorSystemComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<InteractionSystemEditorSystemComponent, InteractionSystemSystemComponent>()
                ->Version(0);
        }
    }

    InteractionSystemEditorSystemComponent::InteractionSystemEditorSystemComponent() = default;

    InteractionSystemEditorSystemComponent::~InteractionSystemEditorSystemComponent() = default;

    void InteractionSystemEditorSystemComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        BaseSystemComponent::GetProvidedServices(provided);
        provided.push_back(AZ_CRC_CE("InteractionSystemEditorService"));
    }

    void InteractionSystemEditorSystemComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        BaseSystemComponent::GetIncompatibleServices(incompatible);
        incompatible.push_back(AZ_CRC_CE("InteractionSystemEditorService"));
    }

    void InteractionSystemEditorSystemComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
        BaseSystemComponent::GetRequiredServices(required);
    }

    void InteractionSystemEditorSystemComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
        BaseSystemComponent::GetDependentServices(dependent);
    }

    void InteractionSystemEditorSystemComponent::Activate()
    {
        InteractionSystemSystemComponent::Activate();
        AzToolsFramework::EditorEvents::Bus::Handler::BusConnect();
    }

    void InteractionSystemEditorSystemComponent::Deactivate()
    {
        AzToolsFramework::EditorEvents::Bus::Handler::BusDisconnect();
        InteractionSystemSystemComponent::Deactivate();
    }

} // namespace InteractionSystem
