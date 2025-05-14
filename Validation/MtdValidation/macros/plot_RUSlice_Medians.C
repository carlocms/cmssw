#include <iostream>
#include <fstream>
#include <TCanvas.h>
#include <TGraphErrors.h>
#include <TAxis.h>
#include <TStyle.h>
#include <TLegend.h>
#include <TFile.h>

void plot_RUSlice_Medians() {
    const int nPoints = 36;
    double x[nPoints], ex[nPoints];
    double y1[nPoints], ey1[nPoints];  // Per il grafico della mediana
    double y2[nPoints], ey2[nPoints];  // Per il grafico dell'errore della mediana

    std::ifstream infile("/gfsvol01/cms/users/giraldin/calib_new/CMSSW_15_0_0_pre2/src/Validation/MtdValidation/test/data_Medians_Cut/Medians_RUSlice_12.txt");
    if (!infile.is_open()) {
        std::cerr << "Errore: impossibile aprire il file." << std::endl;
        return;
    }

    for (int i = 0; i < nPoints; ++i) {
        double trayIndex, median, medianError, medianErrorUncertainty;

        infile >> trayIndex >> median >> medianError >> medianErrorUncertainty;
        if (infile.fail()) {
            std::cerr << "Errore nella lettura del file alla riga " << i + 1 << std::endl;
            break;
        }

        x[i] = trayIndex;
        ex[i] = 0.5;
        y1[i] = median;
        ey1[i] = medianError;
        y2[i] = medianError;
        ey2[i] = medianErrorUncertainty;
    }

    infile.close();

    // Primo grafico: mediana
    TGraphErrors* graph1 = new TGraphErrors(nPoints, x, y1, ex, ey1);
    graph1->SetTitle("Median Value per Tray;Tray Index;Median [ns]");
    graph1->SetMarkerStyle(21);
    graph1->SetLineColor(kBlue + 1);

    // Secondo grafico: errore della mediana
    TGraphErrors* graph2 = new TGraphErrors(nPoints, x, y2, ex, ey2);
    graph2->SetTitle("Median Error per Tray;Tray Index;Error [ns]");
    graph2->SetMarkerStyle(22);
    graph2->SetLineColor(kRed + 1);

    TCanvas* c1 = new TCanvas("c1", "Median Graph", 800, 600);
    graph1->Draw("AP");
    TLegend* leg1 = new TLegend(0.55, 0.75, 0.88, 0.88);
    leg1->AddEntry(graph1, "Median per Tray", "lp");
    leg1->Draw();
    //c1->SaveAs("median_graph.png");

    TCanvas* c2 = new TCanvas("c2", "Median Error Graph", 800, 600);
    graph2->Draw("AP");
    TLegend* leg2 = new TLegend(0.55, 0.75, 0.88, 0.88);
    leg2->AddEntry(graph2, "Median Error per Tray", "lp");
    leg2->Draw();
    //c2->SaveAs("median_error_graph.png");

    // Salvataggio in file ROOT
    TFile* outFile = new TFile("median_RuSlice12_CUT.root", "RECREATE");
    graph1->Write("MedianGraph");
    graph2->Write("MedianErrorGraph");
    c1->Write("CanvasMedian");
    c2->Write("CanvasMedianError");
    outFile->Close();
}

