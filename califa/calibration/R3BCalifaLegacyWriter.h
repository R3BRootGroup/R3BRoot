#ifndef R3BCALIFALEGACYWRITER_H
#define R3BCALIFALEGACYWRITER_H

#include <vector>
#include <functional>
#include <FairTask.h>

class R3BCalifaLegacyWriter: public FairTask
{
 public:
  R3BCalifaLegacyWriter():
    FairTask("R3BCalifaLegacyWriter")
  {}
  InitStatus Init() override;
  void Exec(const char* ) override;

 protected:
  std::vector<std::function<void(void)>> fOnExec;
  ClassDefOverride(R3BCalifaLegacyWriter, 1);
};

#endif
