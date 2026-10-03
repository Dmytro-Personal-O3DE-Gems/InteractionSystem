
#pragma once

#include <AzCore/Component/ComponentBus.h>

namespace InteractionSystem
{
    class InteractorRequests
        : public AZ::ComponentBus
    {
    public:
        AZ_RTTI(InteractionSystem::InteractorRequests, "{25EB8BBC-FEB4-4995-B8D3-49D5C832EEE4}");

        // Put your public request methods here.
        
        // Put notification events here. Examples:
        // void RegisterEvent(AZ::EventHandler<...> notifyHandler);
        // AZ::Event<...> m_notifyEvent1;
        
    };

    using InteractorRequestBus = AZ::EBus<InteractorRequests>;

} // namespace InteractionSystem
