#include <fstream>
#include <iostream>
#include <vector>
#include "TCanvas.h"
#include "TGraphErrors.h"
#include "TLegend.h"
#include "TAxis.h"

void drawSliceGraphs() {
    const int nPoints = 12;
    double x[nPoints], ex[nPoints];
    double y1[nPoints], ey1[nPoints]; // Media delle mediane
    double y2[nPoints], ey2[nPoints]; // Media degli errori delle mediane

    std::ifstream infile("/gfsvol01/cms/users/giraldin/calib_new/CMSSW_15_0_0_pre2/src/Validation/MtdValidation/test/data_Medians_Cut_83/Medians_RU_Mean_Cut_83.txt"); // Sostituisci con il path corretto se serve
    if (!infile.is_open()) {
        std::cerr << "Errore: impossibile aprire il file." << std::endl;
        return;
    }

    for (int i = 0; i < nPoints; ++i) {
        infile >> x[i] >> y1[i] >> ey1[i] >> y2[i] >> ey2[i];
        ex[i] = 0.5; // Fisso
    }
    infile.close();

    // Primo grafico: media delle mediane
    TCanvas* c1 = new TCanvas("c1", "Media delle Median per Slice", 800, 600);
    TGraphErrors* g1 = new TGraphErrors(nPoints, x, y1, ex, ey1);
    g1->SetTitle("Media delle Median per RU Slice;RU Slice Index;Median Time [ns]");
    g1->SetMarkerStyle(20);
    g1->SetLineColor(kBlue + 1);
    g1->SetMarkerColor(kBlue + 1);
    g1->Draw("AP");

    TLegend* leg1 = new TLegend(0.6, 0.75, 0.88, 0.88);
    leg1->AddEntry(g1, "Mean of RU Median Times", "lp");
    leg1->SetBorderSize(0);
    leg1->SetFillStyle(0);
    leg1->Draw();
    //c1->SaveAs("mean_medians_vs_slice.png");

    // Secondo grafico: media degli errori delle mediane
    TCanvas* c2 = new TCanvas("c2", "Media Errori Median per Slice", 800, 600);
    TGraphErrors* g2 = new TGraphErrors(nPoints, x, y2, ex, ey2);
    g2->SetTitle("Media degli Errori delle Median per RU Slice;RU Slice Index;Error on Median Time [ns]");
    g2->SetMarkerStyle(21);
    g2->SetLineColor(kRed + 1);
    g2->SetMarkerColor(kRed + 1);
    g2->Draw("AP");

    TLegend* leg2 = new TLegend(0.6, 0.75, 0.88, 0.88);
    leg2->AddEntry(g2, "Mean Error on RU Median Times", "lp");
    leg2->SetBorderSize(0);
    leg2->SetFillStyle(0);
    leg2->Draw();
    //c2->SaveAs("mean_median_errors_vs_slice.png");
    
    // Salvataggio in file ROOT
    TFile* outFile = new TFile("Mean_MedianRUslice_CUT_83.root", "RECREATE");
    g1->Write("MedianGraph");
    g2->Write("MedianErrorGraph");
    c1->Write("CanvasMedian");
    c2->Write("CanvasMedianError");
    outFile->Close();

}

