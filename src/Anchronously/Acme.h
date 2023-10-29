// src\Anchronously\Ping.h - 
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

namespace syscross::HelloEOS::Async {

enum class Direction { Outgoing, Incoming };

namespace detail_ {
struct ICommand {
	virtual void act() const  = 0;
	virtual Direction getDirection() const = 0;
};
} // namespace detail_
typedef std::shared_ptr< detail_::ICommand > sptr_t;

class QueueCommands {
	std::queue< sptr_t > m_fifo;
public:
	void push(const sptr_t &p) {
		m_fifo.push( p );
	}
	sptr_t pop() {
		if ( m_fifo.empty( ) )
			return nullptr;
		auto x = m_fifo.front( );
		m_fifo.pop( );
		return x;
	}
};
struct Ctx {
	const std::string SocketName;
	const EOS_HPlatform m_PlatformHandle;
	const EOS_ProductUserId m_LocalUserId;
	const EOS_ProductUserId m_FriendLocalUserId;
};

// @insp https://stackoverflow.com/questions/16868129/how-to-store-variadic-template-arguments
template <typename F, typename... Ts>
class Action : public detail_::ICommand {
	static_assert( !(std::is_rvalue_reference_v<Ts> && ...) );
	const Direction m_direction;
	const F m_function;
	const std::tuple<Ts...> m_args;
public:
	template <typename FwdF, typename... FwdTs,
		typename = std::enable_if_t< (std::is_convertible_v< FwdTs&&, Ts > && ...) >>
	Action(Direction direction, FwdF&& func, FwdTs&&... args) : 
		m_direction( direction )
		, m_function( std::forward<FwdF>( func ) )
		, m_args{ std::forward<FwdTs>( args )... }
	{}
	void act() const {
		std::apply( m_function, m_args );
	}
	Direction getDirection() const {
		return m_direction;
	}
};

namespace detail_ {
template <typename F, typename... Args>
auto make_action(Direction direction, F&& f, Args&&... args) {
    return std::make_shared< 
				Action< std::decay_t< F >, std::remove_cv_t< std::remove_reference_t< Args > >...>
			>
			(direction, std::forward< F >( f ), std::forward< Args >( args )...)
		;
}

struct TextSender : public Synchronously::Send::BaseSend {
	using BaseSend::BaseSend;
	//static constexpr auto sendTextPacket = &BaseSend::sendTextPacket_;
	using BaseSend::sendTextPacket_;
	~TextSender() {
	}
};
} // namespace detail_

class Sending {
	Ctx m_ctx;
	QueueCommands *m_queueCommands;
public:
	Sending(Ctx ctx, Async::QueueCommands *queueCommands) : 
		m_ctx( ctx )
		, m_queueCommands( queueCommands )
	{}
	auto text(const std::string &text) {
		auto p = std::make_shared< detail_::TextSender >( 
			m_ctx.m_PlatformHandle
			, m_ctx.m_LocalUserId
			, m_ctx.m_FriendLocalUserId
		);
		auto command = make_action(
				Direction::Outgoing
				, [&] (const std::shared_ptr< detail_::TextSender > &p) { 
					p ->sendTextPacket_( text );
				}
				, p 
			);
		command ->act( );
		m_queueCommands ->push( command );
		return command;
	}
};
} // namespace syscross::HelloEOS::Anchronously
