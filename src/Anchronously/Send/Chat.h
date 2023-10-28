// src\Anchronously\Send\Chat.h - 
#pragma once // Copyright 2023 Alex0vSky (https://github.com/Alex0vSky)
namespace syscross::HelloEOS::Anchronously::Send { class Chat : public Synchronously::Send::BaseSend {
	using BaseSend::BaseSend;

public:
	struct ExecutableCommand {
		std::function<bool(const std::string &text)> function;
		bool bRuned;
		bool operator()(const std::string &text) {
			bRuned = true;
			function( text );
		}
	};
	Networking::send_t message(const std::string &text) {
		auto future = std::async( &Chat::sendPacket_< std::decay_t< decltype( text ) > >, this, text );
		return future;
	}
	
	// @insp https://stackoverflow.com/questions/9779105/generic-member-function-pointer-as-a-template-parameter
	template<auto mf, typename T>
	auto make_proxy(T && obj) {
		return [&obj] (auto &&... args) { return (std::forward<T>(obj)->*mf)(std::forward<decltype(args)>(args)...); };
	}

	// @insp https://devblogs.microsoft.com/oldnewthing/20200713-00/?p=103978
	template<typename R, typename... Args>
	struct FunctionTraitsBase
	{
		using RetType = R;
		using ArgTypes = std::tuple<Args...>;
		static constexpr std::size_t ArgCount = sizeof...(Args);
	};
	template<typename F> struct FunctionTraits;
	template<typename R, typename... Args>
	struct FunctionTraits<R(*)(Args...)>
		: FunctionTraitsBase<R, Args...>
	{
		using base = FunctionTraitsBase<R, Args...>;
		using Pointer = R(*)(Args...);
		template<std::size_t N>
		using NthArg = std::tuple_element_t<N, ArgTypes>;
		using FirstArg = NthArg<0>;
		using LastArg = NthArg<base::ArgCount - 1>;
	};

	template<auto mf, typename T>
	struct proxy {
		std::function<bool(const std::string &text)> function;
	};

	template<auto mf>
	struct proxy2 {
		//std::function< decltype( mf ) > function;
	};

	template <class U> struct PToMTraits
	{
		enum { result = false };
	};
	template <class U, class V>
	struct PToMTraits<U V::*>
	{
		enum { result = true };
	};

class hello{
public:
    void f(){
        std::cout<<"f"<<std::endl;
    }
    virtual void ff(){
        std::cout<<"ff"<<std::endl;
    }
};
	template <typename T, T> struct proxy3; // primary
	template <typename T, typename R, typename ...Args, R (T::*mf)(Args...)>
	struct proxy3<R (T::*)(Args...), mf>
	{
		R call(T & obj, Args &&... args)
		{
			return (obj.*mf)(std::forward<Args>(args)...);
		}
	};

// @insp https://stackoverflow.com/questions/69692722/how-can-i-have-a-function-pointer-template-as-a-template-parameter
template <typename T, typename R, typename... Args>
struct EngineSystem{
    EngineSystem(R (T::*fun)(Args... args)) : 
		fun(fun)
    {}
    R (T::*fun)(Args... args);
};
struct A {
    int x = 3;
    bool fn(int a, int b){
        std::cout << a << b << x;
		return true;
    }
};

template <typename T, typename R, typename... Args>
auto makeEngine(R (T::*fun)(Args... args)){
    return EngineSystem<T, R, Args...>(fun);
}

	static void foo(int) {
	}
	auto message2(const std::string &text) {

		//proxy3< bool(Chat::*)(), &Chat::sendTextPacket_>::call( this );
		hello obj;
		proxy3<void(hello::*)(), &hello::f> qwe0;
		qwe0.call(obj);

//		A a;
//		EngineSystem<A, int, int> as(&A::fn);
//		(a.*(as.fun))( 0, 0 );
//
//		auto es = makeEngine(&A::fn);
//		(a.*(es.fun))( 0, 0 );

		auto need = makeEngine(&Chat::sendTextPacket_);
		(this ->*(need.fun))( text );


		//make_proxy< &Chat::sendTextPacket_ >( this )( text );
		auto x = &Chat::sendTextPacket_;
//		decltype( x ) = 0;
		PToMTraits< decltype( &Chat::sendTextPacket_ ) >::result;
		proxy< &Chat::sendTextPacket_, Chat *> asd0;

		proxy2< &Chat::sendTextPacket_> asd1;

		//FunctionTraits< decltype( x ) > asd2;
		//FunctionTraits< void(int) > asd2;

		ExecutableCommand executableCommand { 
				//std::bind( &Chat::sendPacket_<const std::string &>, this, std::placeholders::_1 )
				std::bind( &Chat::sendTextPacket_, this, std::placeholders::_1 )
			};
		return executableCommand;
	}
};
} // namespace syscross::HelloEOS::Anchronously::Send
