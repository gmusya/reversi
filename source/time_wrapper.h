#include <chrono>
#include <functional>
#include <iomanip>
#include <type_traits>

namespace ReversiEngine {
    struct Time {
        double seconds;

        explicit Time(clock_t clock)
            : seconds(static_cast<double>(clock) / static_cast<double>(CLOCKS_PER_SEC)) {
        }

        Time& operator+=(const Time& other) {
            seconds += other.seconds;
            return *this;
        }

        bool operator<(const Time& time) const {
            return seconds < time.seconds;
        }

        friend std::ostream& operator<<(std::ostream& os, const Time& time) {
            return os << std::fixed << std::setprecision(6) << time.seconds << " sec";
        }
    };

    template<typename Function, typename... Args>
    auto MeasureFunction(Function&& function, Args&&... args) {
        const clock_t start_time = ::clock();
        if constexpr (std::is_same<typename std::result_of<Function(Args...)>::type, void>::value) {
            std::invoke(function, args...);
            return Time(::clock() - start_time);
        } else {
            auto result = std::invoke(function, args...);
            return std::make_tuple(result, Time(::clock() - start_time));
        }
    }

    template<typename Object, typename Method, typename... Args>
    auto MeasureMethod(Object&& object, Method&& method, Args&&... args) {
        return MeasureFunction(method, &object, args...);
    }
}// namespace ReversiEngine
