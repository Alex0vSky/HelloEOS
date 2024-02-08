# HelloEOS
Checking the Epic Online Services (EOS) p2p relays in different ways in C++.
Useful for indie multiplayer games.

@todo
- [x] chat
- [x] lite bandwidth checking, in single queue
- [x] make sure that the EOS SDK is not considered thread safe, crash
- [x] 'deferred' approach instead of async
- [x] multi-channeling
- [x] PING-PONG
- [x] measure ping in milli
- [x] gRPC, 'poc' unary calls over UDP
- [-] ([declined](https://eoshelp.epicgames.com/s/question/0D5QP000002TQJ40AO/p2p-relay-bandwidth-meter-limits-and-permissions?language=en_US))more responsible bandwidth measurement
- [x] async
- [x] [Bench/Stand](https://github.com/Alex0vSky/BenchEosP2p)
- [x] blogpost
- [ ] Clean up the code
- [ ] Use the library in a simple multiplayer game
- [ ] UnrealEngine UObject-s Serialize/Deserialize, replication
- [ ] Python version

## Build
Building the project and tests from the source code requires the presence of the __Microsoft Visual Studio 2019 Community__, and using *.sln and *.vcxproj project files.

## Contributing
Can ask questions. PRs are accepted. No requirements for contributing.

## Thanks
Mr. [G. Sliepen](https://codereview.stackexchange.com/users/129343/g-sliepen) for participating in code review.

## License
See the [LICENSE](https://github.com/Alex0vSky/HelloEOS/blob/main/LICENSE) file for license rights and limitations (MIT).
