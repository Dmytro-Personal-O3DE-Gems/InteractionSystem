
#include "InteractionSystemSystemComponent.h"

#include <InteractionSystem/InteractionSystemTypeIds.h>

#include <AzCore/Serialization/SerializeContext.h>

namespace InteractionSystem
{
    AZ_COMPONENT_IMPL(InteractionSystemSystemComponent, "InteractionSystemSystemComponent",
        InteractionSystemSystemComponentTypeId);

    void InteractionSystemSystemComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<InteractionSystemSystemComponent, AZ::Component>()
                ->Version(0)
                ;
        }
    }

    void InteractionSystemSystemComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC_CE("InteractionSystemService"));
    }

    void InteractionSystemSystemComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC_CE("InteractionSystemService"));
    }

    void InteractionSystemSystemComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
    }

    void InteractionSystemSystemComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
    }

    InteractionSystemSystemComponent::InteractionSystemSystemComponent()
    {
        if (InteractionSystemInterface::Get() == nullptr)
        {
            InteractionSystemInterface::Register(this);
        }
    }

    InteractionSystemSystemComponent::~InteractionSystemSystemComponent()
    {
        if (InteractionSystemInterface::Get() == this)
        {
            InteractionSystemInterface::Unregister(this);
        }
    }

    void InteractionSystemSystemComponent::Init()
    {
    }

    void InteractionSystemSystemComponent::Activate()
    {
        InteractionSystemRequestBus::Handler::BusConnect();
        AZ::TickBus::Handler::BusConnect();
    }

    void InteractionSystemSystemComponent::Deactivate()
    {
        AZ::TickBus::Handler::BusDisconnect();
        InteractionSystemRequestBus::Handler::BusDisconnect();
    }

    void InteractionSystemSystemComponent::OnTick([[maybe_unused]] float deltaTime, [[maybe_unused]] AZ::ScriptTimePoint time)
    {
    }

} // namespace InteractionSystem
