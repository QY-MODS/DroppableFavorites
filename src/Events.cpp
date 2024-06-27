#include "Events.h"

RE::BSEventNotifyControl myEventSink::ProcessEvent(RE::InputEvent* const* evns, RE::BSTEventSource<RE::InputEvent*>*) {
    if (!*evns) return RE::BSEventNotifyControl::kContinue;

    for (RE::InputEvent* e = *evns; e; e = e->next) {
        const auto eventType = static_cast<int>(EventCheck(e));
        if (!eventType) continue;
        const auto menu = RE::UI::GetSingleton()->GetMenu<RE::FavoritesMenu>();
        if (!menu) continue;
        const auto& rtd = menu->GetRuntimeData();
        std::int32_t idx = GetSelectedIndex(rtd);
        if (idx < 0) continue;
        const auto formid = GetSelectedItem(rtd, idx);
        if (!formid) continue;
        SKSE::GetTaskInterface()->AddTask([formid, eventType]() {
            auto* bound = RE::TESForm::LookupByID<RE::TESBoundObject>(formid);
            if (!bound) return;
            auto* player = RE::PlayerCharacter::GetSingleton();
            const auto count = eventType == 2 ? player->GetItemCount(bound) : 1;
            player->RemoveItem(bound, count, RE::ITEM_REMOVE_REASON::kDropping, nullptr, nullptr);
            RE::SendUIMessage::SendInventoryUpdateMessage(player, nullptr);
		});
    }
    return RE::BSEventNotifyControl::kContinue;
}

inline const myEventTypes myEventSink::EventCheck(RE::InputEvent* event) {
    if (!event) return myEventTypes::kNone;
    if (event->eventType.get() != RE::INPUT_EVENT_TYPE::kButton) return myEventTypes::kNone;
    RE::ButtonEvent* a_event = event->AsButtonEvent();
    RE::IDEvent* id_event = event->AsIDEvent();
    if (!a_event || !id_event) return myEventTypes::kNone;
    if (!a_event->IsUp()) return myEventTypes::kNone;
    if (auto* ui = RE::UI::GetSingleton(); !ui || !ui->IsMenuOpen(RE::FavoritesMenu::MENU_NAME)) return myEventTypes::kNone;
    logger::trace("{}", id_event->userEvent.c_str());
    if (id_event->userEvent != "Ready Weapon") return myEventTypes::kNone;
    const auto heldDuration = a_event->HeldDuration();
    logger::trace("Ready Weapon");
    logger::trace("heldDuration: {}", heldDuration);
    return heldDuration > heldThreshold ? myEventTypes::kLong : myEventTypes::kSingle;
}

inline const std::int32_t myEventSink::GetSelectedIndex(const RE::FavoritesMenu::RUNTIME_DATA& runtime_data) { 
    RE::GFxValue selectedIndex;
    if (!runtime_data.root.GetMember("selectedIndex", &selectedIndex)) return -1;
    const std::int32_t selected_index = static_cast<std::int32_t>(selectedIndex.GetNumber());
    logger::trace("selectedIndex: {}", selected_index);
    return selected_index;
}

inline const RE::FormID myEventSink::GetSelectedItem(const RE::FavoritesMenu::RUNTIME_DATA& runtime_data,
                                                 const std::int32_t selected_index) {
    const auto& items = runtime_data.favorites;
    if (selected_index < 0 || static_cast<std::size_t>(selected_index) >= items.size()) return 0;
    const auto item = items[selected_index].item;
    if (!item) return 0;
    if constexpr (item->FORMTYPE == RE::FormType::Spell) return 0;
    logger::trace("item: {}", item->GetName());
    return item->GetFormID();
}