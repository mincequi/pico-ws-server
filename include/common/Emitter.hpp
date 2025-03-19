#pragma once

#include <cstdarg>
#include <functional>
#include <string>

struct Log {
    std::string_view message;
};

template<typename SelfT, typename... EventsT>
class Emitter {
public:
    template<typename EventT>
    using Handler = std::function<void(const SelfT&, const EventT&)>;

    virtual ~Emitter() noexcept {
        static_assert(std::is_base_of_v<Emitter<SelfT, EventsT...>, SelfT>);
    }

    template<typename EventT>
    void on(Handler<EventT> f) const {
        std::get<Handler<EventT>>(_handlers) = std::move(f);
    }

    template<typename EventT>
    void reset() noexcept {
        std::get<Handler<EventT>>(_handlers) = nullptr;
    }

    void reset() noexcept {
        (reset<EventsT>(), ...);
    }

    template<typename EventT>
    bool contains() const noexcept {
        return static_cast<bool>(std::get<Handler<EventT>>(_handlers));
    }

    void log(const char* fmt, ...) const {
        auto& h = std::get<Handler<Log>>(_handlers);
        if (!h) return;
    
        va_list args;
        va_start(args, fmt);
    
        // Format the message
        static char buffer[256];
        vsnprintf(buffer, sizeof(buffer), fmt, args);
        va_end(args);
    
        auto log = Log{buffer};
        h(*static_cast<const SelfT*>(this), log);
    }

protected:
    template<typename EventT>
    void emit(EventT event) {
        if (auto& h = std::get<Handler<EventT>>(_handlers); h) {
            h(*static_cast<SelfT*>(this), event);
        }
    }

private:
    // We do not always want to provide the Log event for each inheriting class
    mutable std::tuple<Handler<Log>, Handler<EventsT>...> _handlers {};
};
