
#pragma once

#include <InteractionSystem/InteractionSystemTypeIds.h>

#include <AzCore/EBus/EBus.h>
#include <AzCore/Interface/Interface.h>

namespace InteractionSystem
{
    class InteractionSystemRequests
    {
    public:
        AZ_RTTI(InteractionSystemRequests, InteractionSystemRequestsTypeId);
        virtual ~InteractionSystemRequests() = default;
        // Put your public methods here
    };

    class InteractionSystemBusTraits
        : public AZ::EBusTraits
    {
    public:
        //////////////////////////////////////////////////////////////////////////
        // EBusTraits overrides
        static constexpr AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;
        static constexpr AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::Single;
        //////////////////////////////////////////////////////////////////////////
    };

    using InteractionSystemRequestBus = AZ::EBus<InteractionSystemRequests, InteractionSystemBusTraits>;
    using InteractionSystemInterface = AZ::Interface<InteractionSystemRequests>;

} // namespace InteractionSystem
