
#pragma once

#include <AzCore/Component/ComponentBus.h>

namespace InteractionSystem
{
    class InteractionOverlapFinderRequests
        : public AZ::ComponentBus
    {
    public:
        AZ_RTTI(InteractionSystem::InteractionOverlapFinderRequests, "{CF8C12A5-FB33-4D18-916D-778E08C340D6}");

        // Put your public request methods here.
        
        // Put notification events here. Examples:
        // void RegisterEvent(AZ::EventHandler<...> notifyHandler);
        // AZ::Event<...> m_notifyEvent1;
        
    };

    using InteractionOverlapFinderRequestBus = AZ::EBus<InteractionOverlapFinderRequests>;

} // namespace InteractionSystem
