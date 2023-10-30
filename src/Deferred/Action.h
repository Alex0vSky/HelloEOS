// src\Deferred\Action.h - 
// @insp https://stackoverflow.com/questions/16868129/how-to-store-variadic-template-arguments
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::Deferred {
namespace detail_ {
template <typename F, typename... Ts>
class Action : 
	public QueueCommands::ICommand 
	, public std::enable_shared_from_this< Action< F, Ts... > >
{
	static_assert( !( std::is_rvalue_reference_v<Ts> &&... ) );
	const QueueCommands::Direction m_direction;
	const F m_function;
	const std::tuple<Ts...> m_args;

public:
	template <typename FwdF, typename... FwdTs,
		typename = std::enable_if_t< ( std::is_convertible_v< FwdTs&&, Ts > &&... ) >>
	Action(QueueCommands::Direction direction, FwdF&& func, FwdTs&&... args) : 
		m_direction( direction )
		, m_function( std::forward<FwdF>( func ) )
		, m_args{ std::forward<FwdTs>( args )... }
	{}
	virtual ~Action()
	{}

	Networking::messageData_t act(
		const QueueCommands::AvoidPush& avoidPush = QueueCommands::AvoidPush{ } 
	) override {
		Networking::messageData_t messageData = std::apply( m_function, m_args );
		if ( !avoidPush.Yes )
			Deferred::QueueCommands::instance( ).push( shared_from_this( ) );
		return messageData;
	}
	QueueCommands::Direction getDirection() const override {
		return m_direction;
	}
};
template <typename F, typename... Args>
auto make_action(QueueCommands::Direction direction, F&& f, Args&&... args) {
    return std::make_shared
			< Action< std::decay_t< F >, std::remove_cv_t< std::remove_reference_t< Args > >...> >
			( direction, std::forward< F >( f ), std::forward< Args >( args )... )
		;
}
} // namespace detail_
} // namespace syscross::HelloEOS::Deferred
