#include <TFile.h>
#include <TGraphErrors.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <iostream>

void compare_median_graphs(const char* file1 = "file1.root", const char* file2 = "file2.root") {
    // Open ROOT files
    TFile* f1 = TFile::Open(file1);
    TFile* f2 = TFile::Open(file2);

    if (!f1 || !f2) {
        std::cerr << "Errore: impossibile aprire uno dei file." << std::endl;
        return;
    }

    // Get the graphs from each file
    TGraphErrors* g1 = (TGraphErrors*)f1->Get("MedianErrorGraph");
    TGraphErrors* g2 = (TGraphErrors*)f2->Get("MedianErrorGraph");

    if (!g1 || !g2) {
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

    g1->SetTitle("Comparison of MedianGraph;Tray Index;Median [ns]");

    // Create canvas
    TCanvas* c = new TCanvas("c", "Comparison", 800, 600);
    g1->Draw("AP");
    g2->Draw("P SAME");

    // Add legend
    TLegend* legend = new TLegend(0.55, 0.75, 0.88, 0.88);
    legend->AddEntry(g1, "w/ cut", "lp");
    legend->AddEntry(g2, "w/o cut", "lp");
    legend->Draw();

    // Save outputs
    //c->SaveAs("comparison_median_graph.png");
    //c->SaveAs("comparison_median_graph.pdf");
}

