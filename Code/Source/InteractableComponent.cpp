
#include "InteractableComponent.h"

#include <InteractionSystem/InteractionInfo.h>

#include <AzCore/std/functional_basic.h>

#include <AzCore/Component/Entity.h>

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/RTTI/BehaviorContext.h>

namespace InteractionSystem
{
    //! Lets Script Canvas and Lua handle InteractableNotificationBus events.
    class InteractableNotificationBusBehaviorHandler
        : public InteractableNotificationBus::Handler
        , public AZ::BehaviorEBusHandler
    {
    public:
        // _WITH_DOC: after each event, one {name, tooltip} pair per parameter, so Script Canvas pins
        // show "Interactor" instead of the bare type name "EntityId". The count must match the parameters.
        AZ_EBUS_BEHAVIOR_BINDER_WITH_DOC(InteractableNotificationBusBehaviorHandler, "{B13063AF-8099-4C02-8028-CF44CE7FA742}",
            AZ::SystemAllocator,
            OnPressInteracted, ({ "Interactor", "The entity that performed the interaction (player or NPC)." }),
            OnHoldInteracted, ({ "Interactor", "The entity that performed the interaction (player or NPC)." }),
            OnInteractionRefused, ({ "Interactor", "The entity that tried to interact (player or NPC)." },
                                  { "Type", "What it tried to do. Compare with InteractionType_Press / InteractionType_Hold." }),
            OnCanInteractQuery, ({ "Interactor", "The entity asking whether it can interact." },
                                 { "Type", "What it wants to do. Compare with InteractionType_Press / InteractionType_Hold." }),
            ReturnInteractableLabel, ({ "Interactor", "The entity that is asking for the label." },
                                      { "Type", "Which action the label is for. Compare with InteractionType_Press / InteractionType_Hold." }));

        void OnPressInteracted(AZ::EntityId interactorId) override
        {
            Call(FN_OnPressInteracted, interactorId);
		}

        void OnHoldInteracted(AZ::EntityId interactorId) override
        {
			Call(FN_OnHoldInteracted, interactorId);
		}

        void OnInteractionRefused(AZ::EntityId interactorId, InteractionType type) override
        {
            Call(FN_OnInteractionRefused, interactorId, type);
        }

        bool OnCanInteractQuery(AZ::EntityId interactorId, InteractionType type) override
        {
            // true unless the graph answers: CallResult leaves it untouched when the graph
            // did not add this event, and a graph that only listens to OnPressInteracted must not block.
            bool canInteract = true;
            CallResult(canInteract, FN_OnCanInteractQuery, interactorId, type);
            return canInteract;
        }

        AZStd::string ReturnInteractableLabel(AZ::EntityId interactorId, InteractionType type) override
        {
            AZStd::string label;
            CallResult(label, FN_ReturnInteractableLabel, interactorId, type);
            return label;
		}
    };

    namespace
    {
        //! Collects the answers of all ReturnInteractableLabel listeners.
        //! EventResult does "result = <answer of a handler>" once per handler (same trick as EBusReduceResult),
        //! so operator= sees every answer. Empty answers mean "no opinion" and are skipped.
        struct LabelAnswers
        {
            AZStd::string m_firstLabel; //!< First non-empty answer.
            int m_count = 0;            //!< How many listeners gave a non-empty answer.

            void operator=(const AZStd::string& answer)
            {
                if (answer.empty())
                {
                    return;
                }
                if (m_count == 0)
                {
                    m_firstLabel = answer;
                }
                ++m_count;
            }
        };
    } // namespace

    AZ_COMPONENT_IMPL(InteractableComponent, "InteractableComponent", "{11B4071B-3140-40FA-8904-A2804A3B70FE}");

    void InteractableComponent::Activate()
    {
        InteractableRequestBus::Handler::BusConnect(GetEntityId());
    }

    void InteractableComponent::Deactivate()
    {
        InteractableRequestBus::Handler::BusDisconnect(GetEntityId());
    }

    InteractionInfo InteractableComponent::GetInteractionInfo() const
    {
        return m_info;
    }

    bool InteractableComponent::CanInteract(AZ::EntityId interactorId, InteractionType type) const
    {
        // Every listener votes; the votes are combined with AND, starting from true
        // (no listeners = allowed). One false blocks, regardless of listener order.
        AZ::EBusReduceResult<bool, AZStd::logical_and<bool>> canInteract(true);
        InteractableNotificationBus::EventResult(canInteract, GetEntityId(), &InteractableNotifications::OnCanInteractQuery, interactorId, type);
        return canInteract.value;
    }

    AZStd::string InteractableComponent::GetInteractableLabel(AZ::EntityId interactorId, InteractionType type) const
    {
        LabelAnswers answers;
        InteractableNotificationBus::EventResult(
            answers, GetEntityId(), &InteractableNotifications::ReturnInteractableLabel, interactorId, type);

        if (answers.m_count == 0)
        {
            // Nobody overrides it: the default label from the inspector.
            switch (type)
            {
            case InteractionType::Press:
                return m_info.m_OnPressLabel;
            case InteractionType::Hold:
                return m_info.m_OnHoldLabel;
            default:
                AZ_Assert(false, "Unknown InteractionType");
                return {};
            }
        }

        // Several owners is a setup error: handler order is not defined, so "first" is effectively random.
        AZ_Warning("Interactable", answers.m_count == 1,
            "Entity '%s': %d listeners returned a label for the same action, using one of them. "
            "A label must have a single owner.",
            GetEntity()->GetName().c_str(), answers.m_count);

        return answers.m_firstLabel;
    }

    void InteractableComponent::Interact(AZ::EntityId interactorId, InteractionType type)
    {
        switch(type)
        {
            case InteractionType::Press:
                InteractableNotificationBus::Event(GetEntityId(), &InteractableNotifications::OnPressInteracted, interactorId);
                break;
            case InteractionType::Hold:
                InteractableNotificationBus::Event(GetEntityId(), &InteractableNotifications::OnHoldInteracted, interactorId);
                break;
            default:
                AZ_Assert(false, "Unknown InteractionType");
                break;
		}
        
    }

    void InteractableComponent::Reflect(AZ::ReflectContext* context)
    {
        // Reflect the data types this component uses before the component itself.
        InteractionInfo::Reflect(context);

        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<InteractableComponent, AZ::Component>()
                ->Version(1)
				->Field("InteractionInfo", &InteractableComponent::m_info)
                ;

            if (AZ::EditContext* editContext = serializeContext->GetEditContext())
            {
                editContext->Class<InteractableComponent>(
                    "Interactable",
                    "Marks this entity as something an Interactor can interact with. "
                    "It does not define what happens: reaction components on the same entity listen to InteractableNotificationBus.")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::Category, "Interaction")
                    ->Attribute(AZ::Edit::Attributes::Icon, "Icons/Components/Component_Placeholder.svg")
                    ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC_CE("Game"))
                    ->Attribute(AZ::Edit::Attributes::AutoExpand, true)

                    ->DataElement(
                        AZ::Edit::UIHandlers::Default,
                        &InteractableComponent::m_info,
                        "Interaction Info",
						"Static description of this interactable entity.")
                    ;
            }
        }

        if (AZ::BehaviorContext* behaviorContext = azrtti_cast<AZ::BehaviorContext*>(context))
        {
            behaviorContext->Class<InteractableComponent>("InteractableComponent")
                ->Attribute(AZ::Script::Attributes::Category, "Interaction")
                ;

            behaviorContext->EBus<InteractableRequestBus>("InteractableRequestBus")
                ->Attribute(AZ::Script::Attributes::Scope, AZ::Script::Attributes::ScopeFlags::Common)
                ->Attribute(AZ::Script::Attributes::Category, "Interaction")
                ->Event("GetInteractionInfo", &InteractableRequests::GetInteractionInfo)
                ->Event("CanInteract", &InteractableRequests::CanInteract)
                ->Event("GetInteractableLabel", &InteractableRequests::GetInteractableLabel)
                ->Event("Interact", &InteractableRequests::Interact)
                ;

            behaviorContext->EBus<InteractableNotificationBus>("InteractableNotificationBus")
                ->Attribute(AZ::Script::Attributes::Scope, AZ::Script::Attributes::ScopeFlags::Common)
                ->Attribute(AZ::Script::Attributes::Category, "Interaction")
                ->Handler<InteractableNotificationBusBehaviorHandler>()
                ;
        }
    }

    void InteractableComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC_CE("InteractableComponentService"));
    }

    void InteractableComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        // Only one Interactable per entity: InteractableRequestBus allows a single handler per address.
        incompatible.push_back(AZ_CRC_CE("InteractableComponentService"));
    }

    void InteractableComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
    }

    void InteractableComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
    }
} // namespace InteractionSystem
