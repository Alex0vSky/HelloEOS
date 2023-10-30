// src\Anchronously\Send\Chat.h - 
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
#pragma region easy_bind
// @insp https://stackoverflow.com/questions/21271728/how-to-pass-member-function-pointer-to-stdfunction
namespace fx {
    template<int I> struct placeholder{};
} 
namespace std {
    template<int I>
    struct is_placeholder< ::fx::placeholder<I>> : std::integral_constant<int, I>{};
} // namespace std
namespace fx {
    template <size_t... Is>
    struct indices {};

    template <size_t N, std::size_t... Is>
    struct build_indices : build_indices<N-1, N-1, Is...> {};

    template <size_t... Is>
    struct build_indices<0, Is...> : indices<Is...> {};

    template<std::size_t... Is, class F, class... Args>
    auto easy_bind(indices<Is...>, F const& f, Args&&... args)
    -> decltype(std::bind(f, std::forward<Args>(args)..., placeholder<Is + 1>{}...))
    {
        return std::bind(f, std::forward<Args>(args)..., placeholder<Is + 1>{}...);
    }
    
    template<class R, class... FArgs, class... Args>
    auto easy_bind(std::function<R(FArgs...)> f, Args&&... args)
    -> decltype(fx::easy_bind(build_indices<sizeof...(FArgs) - sizeof...(Args)>{}, f, std::forward<Args>(args)...)) {
    return fx::easy_bind(build_indices<sizeof...(FArgs) - sizeof...(Args)>{}, f, std::forward<Args>(args)...);
    }
    
    template<class R, class... FArgs, class... Args>
    auto easy_bind(R (*f)(FArgs...), Args&&... args)
    -> decltype(fx::easy_bind(build_indices<sizeof...(FArgs) - sizeof...(Args)>{}, f, std::forward<Args>(args)...)) {
    	return fx::easy_bind(build_indices<sizeof...(FArgs) - sizeof...(Args)>{}, f, std::forward<Args>(args)...);
    }
    
    template <typename R, typename T, typename... FArgs, typename... Args>
	auto easy_bind(R (T::*mf)(FArgs...), Args&&... args)
	-> decltype(fx::easy_bind(std::function<R(T*,FArgs...)>(mf), args...))
	{
	    return fx::easy_bind(std::function<R(T*,FArgs...)>(mf), args...);
	}
}
#pragma endregion // easy_bind

namespace syscross::HelloEOS::Anchronously::Send {
class Chat : public Synchronously::Send::BaseSend {
	using BaseSend::BaseSend;

public:
	// @insp https://stackoverflow.com/questions/69692722/how-can-i-have-a-function-pointer-template-as-a-template-parameter
	template <typename T, typename R, typename... Args>
	struct Deduced {
		std::function< R(Args... args) > m_function;
	};
	template <typename T, typename R, typename... Args>
	auto makeDeduce(R (T::*fun)(Args... args)) {
		return Deduced< T, R, Args... >{ };
	}
	auto message(const std::string &text) {
		auto future = std::async( &Chat::sendTextPacket_, this, text );
		return future;
	}
};
} // namespace syscross::HelloEOS::Anchronously::Send
