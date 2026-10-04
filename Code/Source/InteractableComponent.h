
#pragma once

#include <AzCore/Component/Component.h>
#include <InteractionSystem/InteractableInterface.h>

namespace InteractionSystem
{
    class InteractableComponent
        : public AZ::Component
        , public InteractableRequestBus::Handler
    {
    public:
        AZ_COMPONENT_DECL(InteractableComponent);

        static void Reflect(AZ::ReflectContext* context);

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

    protected:

        void Activate() override;
        void Deactivate() override;

        // InteractableRequestBus
        InteractionInfo GetInteractionInfo() const override;
        bool CanInteract(AZ::EntityId interactorId, InteractionType type) const override;
        void Interact(AZ::EntityId interactorId, InteractionType type) override;

    private:
        InteractionInfo m_info;
    };
} // namespace InteractionSystem
