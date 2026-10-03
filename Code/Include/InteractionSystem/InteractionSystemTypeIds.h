
#pragma once

namespace InteractionSystem
{
    // System Component TypeIds
    inline constexpr const char* InteractionSystemSystemComponentTypeId = "{5B690965-1A48-45AD-9FB7-44A29F405CFF}";
    inline constexpr const char* InteractionSystemEditorSystemComponentTypeId = "{EE48F0A1-08A5-4E22-8AF0-8F56D6D26BD9}";

    // Module derived classes TypeIds
    inline constexpr const char* InteractionSystemModuleInterfaceTypeId = "{60B0D656-3252-4D0E-965C-2913AEEB7831}";
    inline constexpr const char* InteractionSystemModuleTypeId = "{91444DC7-CA75-4049-9E7A-34E98FDD95E1}";
    // The Editor Module by default is mutually exclusive with the Client Module
    // so they use the Same TypeId
    inline constexpr const char* InteractionSystemEditorModuleTypeId = InteractionSystemModuleTypeId;

    // Interface TypeIds
    inline constexpr const char* InteractionSystemRequestsTypeId = "{79B7DA1A-736E-4A13-A7E7-E8A39E7D717E}";
} // namespace InteractionSystem
