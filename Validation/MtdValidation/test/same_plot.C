#include <TFile.h>
#include <TH1.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <iostream>

void plotHistograms(const char* file1 = "DQM_MuGun_0_7_3_8PtCut.root", const char* file2 = "DQM_MuGun_3_8_6_9PtCut.root") {
    TFile* f1 = TFile::Open(file1);
    TFile* f2 = TFile::Open(file2);
    
    if (!f1 || !f2 || f1->IsZombie() || f2->IsZombie()) {
        std::cerr << "Errore nell'apertura dei file ROOT." << std::endl;
        return;
    }

    TH1* h1 = nullptr;
    TH1* h2 = nullptr;
    
    f1->GetObject("DQMData/Run 1/MTD/Run summary/BTL/LocalReco/meUncTimePhiSlice_corr_4", h1);
    f2->GetObject("DQMData/Run 1/MTD/Run summary/BTL/LocalReco/meUncTimePhiSlice_corr_4", h2);
    
    if (!h1 || !h2) {
        std::cerr << "Errore nel recupero degli istogrammi." << std::endl;
        return;
    }

    TCanvas* c1 = new TCanvas("c1", "Histograms Comparison", 800, 600);
    h1->SetLineColor(kRed);
    h2->SetLineColor(kBlue);
    h1->SetLineWidth(2);
    h2->SetLineWidth(2);
    
    h1->Draw();
    h2->Draw("SAME");
    
    TLegend* legend = new TLegend(0.7, 0.7, 0.9, 0.9);
    legend->AddEntry(h1, file1, "l");
    legend->AddEntry(h2, file2, "l");
    legend->Draw();
    
    //c1->SaveAs("HistComparison.png");
    
    f1->Close();
    f2->Close();
}

