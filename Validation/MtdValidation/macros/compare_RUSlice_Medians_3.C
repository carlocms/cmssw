#include <TFile.h>
#include <TGraphErrors.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <iostream>

void compare_median_graphs(const char* file1 = "file1.root", const char* file2 = "file2.root", const char* file3 = "file3.root") {
    // Open ROOT files
    TFile* f1 = TFile::Open(file1);
    TFile* f2 = TFile::Open(file2);
    TFile* f3 = TFile::Open(file3);

    if (!f1 || !f2 || !f3) {
        std::cerr << "Errore: impossibile aprire uno dei file." << std::endl;
        return;
    }

    // Get the graphs from each file
    TGraphErrors* g1 = (TGraphErrors*)f1->Get("MedianGraph");
    TGraphErrors* g2 = (TGraphErrors*)f2->Get("MedianGraph");
    TGraphErrors* g3 = (TGraphErrors*)f3->Get("MedianGraph");

    if (!g1 || !g2 || !g3) {
        std::cerr << "Errore: impossibile trovare MedianGraph in uno dei file." << std::endl;
        return;
    }

    // Customize style
    g1->SetMarkerStyle(22);
    g1->SetMarkerColor(kRed+1);
    g1->SetLineColor(kRed+1);

    g2->SetMarkerStyle(21);
    g2->SetMarkerColor(kBlue+1);
    g2->SetLineColor(kBlue+1);

    g3->SetMarkerStyle(20);
    g3->SetMarkerColor(kGreen+1);
    g3->SetLineColor(kGreen+1);

    g1->SetTitle("Comparison of MedianGraph;Tray Index;Median [ns]");

    // Create canvas
    TCanvas* c = new TCanvas("c", "Comparison", 800, 600);
    g1->Draw("AP");
    g2->Draw("P SAME");
    g3->Draw("P SAME");

    // Add legend
    TLegend* legend = new TLegend(0.55, 0.75, 0.88, 0.88);
    legend->AddEntry(g2, "w/o cut", "lp");
    legend->AddEntry(g1, "w/ cut - 88%", "lp");
    legend->AddEntry(g3, "w/ cut - 83%", "lp");
    legend->Draw();

    // Save outputs
    //c->SaveAs("comparison_median_graph.png");
    //c->SaveAs("comparison_median_graph.pdf");
}

