
#pragma once

#include <AzCore/Component/Component.h>
#include <InteractionSystem/InteractorInterface.h>

namespace InteractionSystem
{
    class InteractorComponent
        : public AZ::Component
        , public InteractorRequestBus::Handler
    {
    public:
        AZ_COMPONENT_DECL(InteractorComponent);

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
