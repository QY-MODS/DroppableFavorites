#pragma once
#include "Manager.h"

enum myEventTypes {
    kNone = 0,
    kSingle = 1,
    kLong = 2,
};

class myEventSink : public RE::BSTEventSink<RE::InputEvent*> {
    myEventSink() = default;
    myEventSink(const myEventSink&) = delete;
    myEventSink(myEventSink&&) = delete;
    myEventSink& operator=(const myEventSink&) = delete;
    myEventSink& operator=(myEventSink&&) = delete;


    const float heldThreshold = 0.24f;

    virtual RE::BSEventNotifyControl ProcessEvent(RE::InputEvent* const* evns,
                                                  RE::BSTEventSource<RE::InputEvent*>*) override;

    [[nodiscard]] inline const myEventTypes EventCheck(RE::InputEvent* event);
    [[nodiscard]] inline const std::int32_t GetSelectedIndex(const RE::FavoritesMenu::RUNTIME_DATA& runtime_data);
    [[nodiscard]] inline const RE::FormID GetSelectedItem(const RE::FavoritesMenu::RUNTIME_DATA& runtime_data, const std::int32_t selected_index);

public:
    static myEventSink* GetSingleton() {
        static myEventSink singleton;
        return &singleton;
    }

    inline void UpdateKeyCodes();
    std::uint32_t key_keyboard = 0;
    std::uint32_t key_gamepad = 0;
};