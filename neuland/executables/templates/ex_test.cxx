#include <FairRtdbRun.h>
#include <R3BEventHeader.h>
#include <R3BPaddleTamexMappedData.h>
#include <R3BPaddleTamexMappedData2.h>
#include <R3BShared.h>
#include <TFile.h>
#include <TTree.h>
#include <iostream>
#include <map>
#include <range/v3/view.hpp>
#include <vector>

// void Read()
// {

//     auto vec = std::make_unique<std::vector<R3BPaddleTamexMappedData2>>();
//     auto rootFile = R3B::make_rootfile("test.root", "READ");
//     // auto dataTree = std::make_unique<TTree>("evt", "data tree");

//     auto* dataTree = rootFile->Get<TTree>("evt");
//     if (dataTree == nullptr)
//     {
//         return;
//     }

//     dataTree->Print();
//     auto* ptr = vec.get();
//     auto res = dataTree->SetBranchAddress("mappedData", &ptr);

//     std::cout << "res: " << res << std::endl;
//     if (res != 0)
//     {
//         return;
//     }

//     for (const auto entry : ranges::iota_view(0, dataTree->GetEntries()))
//     {
//         dataTree->GetEntry(entry);
//         std::cout << "-----------------" << std::endl;
//         for (const auto& item : *vec)
//         {
//             std::cout << "info:" << item.barID << std::endl;
//         }
//     }
// }
// void ReadMap()
// {

//     auto map = std::make_unique<std::map<int, R3BPaddleTamexMappedData2>>();
//     auto rootFile = R3B::make_rootfile("test.root", "READ");
//     // auto dataTree = std::make_unique<TTree>("evt", "data tree");

//     auto* dataTree = rootFile->Get<TTree>("evt");
//     if (dataTree == nullptr)
//     {
//         return;
//     }

//     dataTree->Print();
//     auto* ptr = map.get();
//     auto res = dataTree->SetBranchAddress("mappedData", &ptr);

//     std::cout << "res: " << res << std::endl;
//     if (res != 0)
//     {
//         return;
//     }

//     for (const auto entry : ranges::iota_view(0, dataTree->GetEntries()))
//     {
//         dataTree->GetEntry(entry);
//         std::cout << "-----------------" << std::endl;
//         for (const auto& [key, item] : *map)
//         {
//             std::cout << "key:" << key << std::endl;
//             std::cout << "info:" << item.barID << std::endl;
//         }
//     }
// }

// void Write()
// {
//     auto rootFile = R3B::make_rootfile("test.root", "RECREATE");
//     auto dataTree = std::make_unique<TTree>("evt", "data tree");

//     auto myMap = std::map<int, R3BPaddleTamexMappedData2>{};

//     const auto mapSize = 5;
//     const auto entrySize = 10;

//     dataTree->Branch("mappedData", &myMap);

//     for (const auto _ : ranges::iota_view(0, entrySize))
//     {
//         for (const auto key : ranges::iota_view(0, mapSize))
//         {
//             auto mapData = R3BPaddleTamexMappedData2{};
//             mapData.barID = key + 10;
//             myMap.insert({ key, mapData });
//         }
//         dataTree->Fill();
//         myMap.clear();
//     }
//     dataTree->Print();
//     dataTree->Show(3);
//     dataTree->Write();
// }

auto main() -> int
{
    // Write();
    // ReadMap();
    // auto rootFile = R3B::make_rootfile("test.root", "RECREATE");

    // auto lists = std::make_unique<TList>();
    // for (auto count : ranges::iota_view(0, 10))
    // {
    //     auto run = std::make_unique<FairRtdbRun>(100 + count, 2);
    //     lists->Add(run.release());
    // }
    // rootFile->WriteObject(lists.get(), "a list");

    return 0;
}
