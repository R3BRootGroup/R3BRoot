#include "FairFileSource.h"
#include "FairParRootFileIo.h"
#include "FairRootFileSink.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairTask.h"
#include "R3BNeulandPoint.h"
#include "TCAConnector.h"
#include "TCAViewer.h"
#include "TRandom3.h"
#include "TStopwatch.h"
#include <R3BNeulandHit.h>
#include <iostream>

class MyTask : public FairTask
{
  public:
    MyTask(const TString& input = "NeulandPoints", const TString& output = "NeulandTestPoints")
        : fPoints(input)
        , mNeulandInputPoints(input.Data())
        , mNeulandOutputPoints(output.Data())
    {
    }
    auto Init() -> InitStatus override
    {
        mNeulandInputPoints.Init();
        mNeulandOutputPoints.Init();
        fPoints.Init();

        return kSUCCESS;
    }

    void Exec(Option_t*) override
    {
        auto inputData = mNeulandInputPoints.Get();
        auto inputPoints_Connector = fPoints.Retrieve();
        // if (inputData.size() > 200) // this will cause error
        if (!inputData.empty())
        {
            auto outputPointers_Viewer = mNeulandOutputPoints.Get();
            for (const auto& point : inputData)
            {
                // auto output_Viewer = mNeulandOutputPoints.Get(); // this will cause error
                auto neulandPoint = *point;
                neulandPoint.SetEnergyLoss(neulandPoint.GetEnergyLoss() + 0.005);
                outputPointers_Viewer.Emplace_back(std::move(neulandPoint));
            }
        }

        // test output

        // check consistancy with TCAConnector
        for (int i = 1; i < inputPoints_Connector.size(); i++)
        {
            if (inputData[i]->GetLightYield() != inputPoints_Connector[i]->GetLightYield())
                LOG(error) << "light yield mismatch";
            // LOG(info) << "Viewer: " << inputPointers_Viewer[i]->GetLightYield()
            //           << "\t Con: " << inputPoints_Connector[i]->GetLightYield();
        }
    }

  private:
    TClonesArray* tracks = nullptr;
    TCAViewer::Data<R3BNeulandPoint, TCAViewer::in> mNeulandInputPoints;
    // TCAViewer::Data<R3BNeulandHit, TCAViewer::in> mNeulandInputPoints; // this will cause error
    TCAViewer::Data<R3BNeulandPoint, TCAViewer::out> mNeulandOutputPoints;
    TCAInputConnector<R3BNeulandPoint> fPoints;
};

auto main() -> int
{
    TStopwatch timer;
    timer.Start();

    FairLogger::GetLogger()->SetLogScreenLevel("info");

    auto run = std::make_unique<FairRunAna>();
    run->SetSource(new FairFileSource("test.simu.root"));
    run->SetSink(new FairRootFileSink("test.digi.root"));

    auto io = new FairParRootFileIo();
    io->open("test.para.root");
    run->GetRuntimeDb()->setFirstInput(io);

    run->AddTask(new MyTask());
    run->Init();
    run->Run(0, 100);

    timer.Stop();
    auto sink = run->GetSink();
    sink->Close();
    std::cout << "Macro finished successfully." << std::endl;
    std::cout << "Real time: " << timer.RealTime() << "s, CPU time: " << timer.CpuTime() << "s" << std::endl;
    return 0;
}
