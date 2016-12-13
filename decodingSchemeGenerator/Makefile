all: main.exe

main.exe:
	g++ -o main.exe -g src/piterCode-final.cpp -I json -I log -D_LINUX -std=c++11 -I json/src -I log/src json/src/jsoncpp.cpp src/CS_CodingScheme.cpp src/CS_RecoveryAlgorithm.cpp src/CS_RecoveryRate.cpp src/CS_Time.cpp log/src/LogControl.cpp log/src/LogChannel.cpp log/src/LogClient.cpp log/src/LogMessage.cpp log/src/rtcsbaseobject.cpp log/src/rtcsprofile.cpp -lpthread
