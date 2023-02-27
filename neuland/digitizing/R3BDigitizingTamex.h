/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2023 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#ifndef R3BROOT_DIGITIZINGTAMEX_H
#define R3BROOT_DIGITIZINGTAMEX_H
/**
 * NeuLAND Tamex digitizing module with multiple hits per event
 * @author Yanzhao Wang
 * @since July 15, 2022
 *
 *
 */

#include "R3BDigitizingChannel.h"
#include "R3BDigitizingPaddle.h"
#include "TRandom3.h"
#include "Validated.h"
#include <optional>

class R3BNeulandHitPar;
class R3BNeulandHitModulePar;

namespace R3B::Digitizing::Neuland::Tamex
{
    class Channel;
    struct Params
    {
        double fPMTThresh;             // [MeV]
        double fSaturationCoefficient; // Saturation coefficient of PMTs
        bool fExperimentalDataIsCorrectedForSaturation;
        double fTimeRes; // time + Gaus(0., fTimeRes) [ns]
        double fEResRel; // Gaus(e, fEResRel * e) []
        double fEnergyGain;
        double fPedestal;
        double fTimeMax;
        double fTimeMin;
        double fQdcMin;
        TRandom3& fRnd;

        explicit Params(TRandom3&);
    };

    class PMTPeak
    {
      public:
        PMTPeak() = default;
        PMTPeak(Digitizing::Channel::Hit pmtHit, const Channel&);
        auto operator<(const PMTPeak& rhs) const -> bool { return (fLETime < rhs.fLETime); }
        auto operator==(const PMTPeak& rhs) const -> bool { return std::abs(fLETime - rhs.fLETime) < peakWidth; }
        auto operator+=(const PMTPeak& rhs) -> PMTPeak&;
        [[nodiscard]] auto GetQDC() const -> double { return fQdc; }
        [[nodiscard]] auto GetLETime() const -> double { return fLETime; }
        static const double peakWidth; // ns

      private:
        double fQdc = 0.0;
        double fLETime = 0.0;
    };

    class Peak
    {
      public:
        Peak(const PMTPeak& pmtPeak, Channel* channel);
        Peak() = default;

        // Getters:
        [[nodiscard]] auto GetWidth() const -> double { return fWidth; }
        [[nodiscard]] auto GetQDC() const -> double { return fQdc; }
        [[nodiscard]] auto GetLETime() const -> double { return fLETime; }
        [[nodiscard]] auto GetTETime() const -> double { return fTETime; }

        auto operator==(const Peak&) const -> bool;
        void operator+=(const Peak&);

        template <typename Par>
        static auto WidthToQdc(double width, const Par& par) -> double
        {
            return std::max(1., width - par.fPedestal) / par.fEnergyGain;
        }

        template <typename Par>
        static auto QdcToWidth(double qdc, const Par& par) -> double
        {
            auto width = 0.0;
            if (qdc > par.fQdcMin)
            {
                width = qdc * par.fEnergyGain + par.fPedestal;
            }
            else
            {
                width = qdc * par.fEnergyGain * (par.fPedestal + 1);
            }
            return width;
        }
        explicit operator Digitizing::Channel::Signal() const;

      private:
        double fWidth = 0.0;  // the temperal width of the TmxPeak in [ns]
        double fQdc = 0.0;    // the qdc value in [MeV] (without threshold)
        double fLETime = 0.0; // leading edge of the TmxPeak in [ns]
        double fTETime = 0.0; // tailing edge of the TmxPeak
        Channel* fChannel = nullptr;
    };

    class Channel : public Digitizing::Channel
    {
      public:
        Channel(ChannelSide, TRandom3&);
        explicit Channel(ChannelSide side)
            : Channel(side, GetRandom3Ref())
        {
        }
        void AddHit(Hit /*hit*/) override;

        // Getters:
        auto GetPar() -> Tamex::Params& { return par; }
        auto GetParConstRef() const -> const Tamex::Params& { return par; }
        auto GetFQTPeaks() -> const std::vector<Peak>&;
        auto GetPMTPeaks() -> const std::vector<PMTPeak>&;

        auto CreateSignal(const Peak& peak) const -> Signal;
        static void GetHitPar(const std::string& hitParName);

      private:
        // mutable std::vector<FQTPeak> fFQTPeaks;
        auto ConstructSignals() -> Signals override;
        void AttachToPaddle(Digitizing::Paddle* paddle) override;
        std::vector<PMTPeak> fPMTPeaks;
        std::vector<Peak> fFQTPeaks;
        static R3BNeulandHitPar const* fNeulandHitPar; // NOLINT
        R3BNeulandHitModulePar* fNeulandHitModulePar = nullptr;
        Tamex::Params par;

        auto CheckPaddleIDInHitPar() const -> bool;
        auto CheckPaddleIDInHitModulePar() const -> bool;
        void SetHitModulePar(int PaddleId);
        auto ToQdc(double) const -> double;
        auto ToTdc(double) const -> double;
        auto ToUnSatQdc(double) const -> double;
        template <typename Peak>
        void ApplyThreshold(/* inout */ std::vector<Peak>&);
        auto ConstructFQTPeaks(std::vector<PMTPeak>& pmtPeaks) -> std::vector<Peak>;
        template <typename Peak>
        static void PeakPilingUp(/* inout */ std::vector<Peak>& peaks);
    };

} // namespace R3B::Digitizing::Neuland::Tamex
#endif // R3BROOT_DIGITIZINGTAMEX_H
