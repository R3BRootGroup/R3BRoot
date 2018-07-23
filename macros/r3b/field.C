
void field()
{
    R3BGladFieldMap* magField = new R3BGladFieldMap("R3BGladMap");
    magField->SetScale(-0.8);

    magField->Init();

    Float_t y = 40.;

    Float_t x = 0.;
    Float_t z = 0.;

    Float_t by = 0.;

    TH2F* h2 = new TH2F("h2", "", 100, 0., 500., 100, -300., 200.);

    for (Int_t i = 0; i < 100; i++)
    {
        z = 0. + (i + 0.5) * 5.;

        for (Int_t j = 0; j < 100; j++)
        {
            x = -300. + (j + 0.5) * 5.;

            by = 0.1 * magField->GetBy(x, y, z);

            h2->Fill(z, x, by);
        }
    }

    TCanvas* c1 = new TCanvas("c2", "", 10, 10, 600, 600);
    h2->Draw("colz");

    TLine* l0 = new TLine(0., 0., 500., 0.);
    l0->Draw();

    TVector3 p0(0., 0., 106.2);
    TVector3 p1(-200., 0., 0.);
    TVector3 p2(200., 0., 0.);

    p1.RotateY(-14. * TMath::DegToRad());
    p2.RotateY(-14. * TMath::DegToRad());

    p1 += p0;
    p2 += p0;

    TLine* l2 = new TLine(p1.Z(), p1.X(), p2.Z(), p2.X());
    l2->Draw();

    TArc* a1 = new TArc(329.574, -50.137, 3.);
    a1->Draw();

    // 3D Plot ================================================================

    // Ranges for the histogram
    auto x_min = -300.;
    auto x_max = 200.;
    auto y_min = -100.;
    auto y_max = 0.;
    auto z_min = 0.;
    auto z_max = 700.;

    // Step-size
    auto step = 10.;

    // Lower cut-off
    auto cutb = 0.01;

    // Display negative values
    auto showNegative = kFALSE;

    // By offset (to avoid negative values)
    auto offset = 0.;

    // Calculate number of bins
    auto nbins_x = Int_t((x_max - x_min) / step);
    auto nbins_y = Int_t((y_max - y_min) / step);
    auto nbins_z = Int_t((z_max - z_min) / step);
    cout << nbins_x << " " << nbins_y << " " << nbins_z << endl;

    // Create histogram
    auto h3 = new TH3F("h3", "", nbins_z, z_min, z_max, nbins_x, x_min, x_max, nbins_y, y_min, y_max);

    // gStyle->SetCanvasPreferGL(true);

    for (auto i = 0; i < nbins_x; i++)
    {
        for (auto j = 0; j < nbins_y; j++)
        {
            for (auto k = 0; k < nbins_z; k++)
            {
                x = x_min + step * (i + 0.5);
                y = y_min + step * (j + 0.5);
                z = z_min + step * (k + 0.5);
                by = 0.1 * magField->GetBy(x, y, z);

                if (TMath::Abs(by) < cutb)
                {
                    continue;
                }

                if (!showNegative)
                {
                    if (by < 0.)
                    {
                        continue;
                    }
                }

                by += offset;
                h3->SetBinContent(k + 1, i + 1, j + 1, by);
            }
        }
    }

    auto c11 = new TCanvas("c11", "", 650, 10, 1000, 500);
    h3->Draw("BOX2Z");
}

