
#pragma once

#include <AzCore/Component/ComponentBus.h>

namespace InteractionSystem
{
    class InteractorInputRequests
        : public AZ::ComponentBus
    {
    public:
        AZ_RTTI(InteractionSystem::InteractorInputRequests, "{78597809-2D3B-4902-ACDF-FBD1378D3301}");

        // Put your public request methods here.
        
        // Put notification events here. Examples:
        // void RegisterEvent(AZ::EventHandler<...> notifyHandler);
        // AZ::Event<...> m_notifyEvent1;
        
    };

    using InteractorInputRequestBus = AZ::EBus<InteractorInputRequests>;

} // namespace InteractionSystem
