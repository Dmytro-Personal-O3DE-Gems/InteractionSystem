
#pragma once

#include <AzCore/Component/Component.h>
#include <InteractionSystem/InteractionOverlapFinderInterface.h>

namespace InteractionSystem
{
    class InteractionOverlapFinderComponent
        : public AZ::Component
        , public InteractionOverlapFinderRequestBus::Handler
    {
    public:
        AZ_COMPONENT_DECL(InteractionOverlapFinderComponent);

        static void Reflect(AZ::ReflectContext* context);

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

    protected:
        void Activate() override;
        void Deactivate() override;
    };
} // namespace InteractionSystem
