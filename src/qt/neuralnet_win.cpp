#include "neuralnet_win.h"
#include "version.h"
#include "sync.h"
#include "util.h"

#include <QString>
#include <QAxObject>
#include <ActiveQt/qaxbase.h>
#include <ActiveQt/qaxobject.h>

#include <functional>
#include <cstdio>
#include <string>

extern bool fTestNet;

using namespace NN;

namespace
{
    // Static factory registration.
    bool registered = []()
    {
        NN::Factory factory([]() { return std::make_shared<NeuralNetWin32>(); });
        NN::RegisterFactory(factory);
        return true;
    };
}

// While transitioning to dotnet the NeuralNet implementation has been split
// into 3 implementations; Win32 with Qt, Win32 without Qt and the rest.
// After the transition both Win32 implementations can be removed.

// Win32 with Qt enabled.
NeuralNetWin32::NeuralNetWin32()
    : globalcom(new QAxObject("BoincStake.Utilization"))
{
    LogPrintf("Initializing Neuralnet");
    globalcom->dynamicCall("SetTestNetFlag(QString)", QString(fTestNet ? "TESTNET" : "MAINNET"));
}

bool NeuralNetWin32::IsEnabled()
{
    return GetArgument("disableneuralnetwork", "false") == "false";
}

std::string NeuralNetWin32::GetNeuralVersion()
{
    int neural_id = static_cast<int>(IsNeuralNet());
    return std::to_string(CLIENT_VERSION_MINOR) + "." + std::to_string(neural_id);
}

std::string NeuralNetWin32::GetNeuralHash()
{
    QString res = globalcom->dynamicCall("GetNeuralHash()").toString();
    return res.toStdString();
}

std::string NeuralNetWin32::GetNeuralContract()
{
    return globalcom->dynamicCall("GetNeuralContract()").toString().toStdString();
}

bool NeuralNetWin32::SynchronizeDPOR(const std::string& data)
{
    int result = globalcom->dynamicCall(
                     "SyncCPIDsWithDPORNodes(Qstring)",
                     data.c_str()).toInt();

    LogPrintf("Done syncing. %d", result);
    return true;
}

std::string NeuralNetWin32::ExecuteDotNetStringFunction(std::string function, std::string data)
{
    std::string request = function+"(Qstring)";
    QString result = globalcom->dynamicCall(
                               request.c_str(),
                               data.c_str()).toString();
    return result.toStdString();
}

int64_t NeuralNetWin32::IsNeuralNet()
{
    return globalcom->dynamicCall("NeuralNetwork()").toInt();
}

void NeuralNetWin32::Show()
{
    globalcom->dynamicCall("ShowMiningConsole()");
}
