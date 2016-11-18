#include "ReconstructionNeuralNetworkMultiplicity.h"

#include <iostream>
#include <memory>
#include "neuland.pb.h"
#include "FairMQMessageZMQ.h"

namespace Neuland
{
    ReconstructionNeuralNetworkMultiplicity::ReconstructionNeuralNetworkMultiplicity(const Double_t EToFMin,
                                                                                     const Double_t EToFMax,
                                                                                     const Double_t EDepMin,
                                                                                     const TString server)
        : fEToFMin(EToFMin)
        , fEToFMax(EToFMax)
        , fEDepMin(EDepMin)
        , fServer(server)
        , fSocket(new FairMQSocketZMQ("req", "neulandneuralnetworkclient", 1, "neuland"))
    {
    }

    ReconstructionNeuralNetworkMultiplicity::~ReconstructionNeuralNetworkMultiplicity()
    {
        if (fSocket)
        {
            fSocket->Close();
            delete fSocket;
        }
    }

    void ReconstructionNeuralNetworkMultiplicity::Init()
    {
        std::cout << "ReconstructionNeuralNetworkMultiplicity: Connecting to server " << fServer << std::endl;
        fSocket->Connect(fServer);
    }

    std::vector<R3BNeulandNeutron> ReconstructionNeuralNetworkMultiplicity::GetNeutrons(
        const std::vector<R3BNeulandCluster*>& clusters) const
    {
        Neuland::Event pbevent;
        for (const auto cluster : clusters)
        {
            for (const auto digi : cluster->GetDigis())
            {
                auto pbdigi = pbevent.add_digis();
                pbdigi->set_id(digi.GetPaddle());
                pbdigi->set_tl(digi.GetTdcL());
                pbdigi->set_tr(digi.GetTdcR());
                pbdigi->set_e(digi.GetE());
            }
        }

        UInt_t nNeutrons = 0;

        try
        {
            std::string* tmp = new std::string();
            pbevent.SerializeToString(tmp);
            // Really? Why can't I just give the string? Void*s? Is this 1980?
            std::unique_ptr<FairMQMessageZMQ> msg(new FairMQMessageZMQ(const_cast<char*>(tmp->c_str()),
                                                                       tmp->length(),
                                                                       [](void* /*data*/, void* object)
                                                                       {
                                                                           delete static_cast<std::string*>(object);
                                                                       },
                                                                       tmp));
            fSocket->Send(msg.get(), 0);

            // Why can't this just RETURN the message? /cry
            std::unique_ptr<FairMQMessageZMQ> answ(new FairMQMessageZMQ());
            fSocket->Receive(answ.get(), 0);
            nNeutrons = std::stoul(std::string(static_cast<char*>(answ->GetData()), answ->GetSize()));
            std::cout << "+" << std::flush;
        }
        catch (...)
        {
            std::cout << '-' << std::flush;
        }

        std::vector<R3BNeulandCluster*> filteredclusters;
        std::copy_if(clusters.begin(),
                     clusters.end(),
                     std::back_inserter(filteredclusters),
                     [&](const R3BNeulandCluster* a)
                     {
                         return true;
                         return a->GetFirstDigi().GetEToF() > fEToFMin && a->GetFirstDigi().GetEToF() < fEToFMax &&
                                a->GetE() > fEDepMin;
                     });

        std::sort(filteredclusters.begin(),
                  filteredclusters.end(),
                  [](const R3BNeulandCluster* a, const R3BNeulandCluster* b)
                  {
                      return a->GetFirstDigi().GetEToF() > b->GetFirstDigi().GetEToF();
                  });

        std::vector<R3BNeulandNeutron> neutrons;
        for (UInt_t n = 0; n < nNeutrons && n < filteredclusters.size(); n++)
        {
            const R3BNeulandDigi digi = filteredclusters.at(n)->GetFirstDigi();
            neutrons.push_back(R3BNeulandNeutron(digi.GetT(), digi.GetPosition()));
        }
        return neutrons;
    }
}
