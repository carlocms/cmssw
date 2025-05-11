#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "TCanvas.h"
#include "TGraphErrors.h"
#include "TAxis.h"
#include "TStyle.h"
#include "TLegend.h"


void drawMedianErrorGraph() {
    const int nPoints = 36;
    double x[nPoints], y[nPoints];
    double ex[nPoints], ey[nPoints];

    std::ifstream infile("/gfsvol01/cms/users/giraldin/calib_new/CMSSW_15_0_0_pre2/src/Validation/MtdValidation/test/MedianErrors_RU1_Trays.txt");
    if (!infile.is_open()) {
        std::cerr << "Errore: impossibile aprire il file File1.txt" << std::endl;
        return;
    }

    for (int i = 0; i < nPoints; ++i) {
        int trayIndex;
        double medianError, medianErrorUncertainty;

        infile >> trayIndex >> medianError >> medianErrorUncertainty;
        if (infile.fail()) {
            std::cerr << "Errore nella lettura del file alla riga " << i+1 << std::endl;
            break;
        }

        x[i] = trayIndex;
        y[i] = medianError;
        ex[i] = 0.5;  // larghezza della barra sull'asse X
        ey[i] = medianErrorUncertainty;
    }

    infile.close();

    TGraphErrors* graph = new TGraphErrors(nPoints, x, y, ex, ey);
    graph->SetTitle("Median Timing Error per Tray;Tray Index;Median Error [ns]");
    graph->SetMarkerStyle(20);
    graph->SetMarkerSize(1.0);
    graph->SetLineWidth(2);
    graph->SetLineColor(kBlue + 1);

    TCanvas* c = new TCanvas("c", "Median Error Graph", 800, 600);
    graph->Draw("AP");

    TLegend* legend = new TLegend(0.55, 0.75, 0.88, 0.88);
    legend->AddEntry(graph, "Median Error - RU Slice 1, Tray 1-36", "lp");
    legend->SetBorderSize(0);
    legend->SetFillStyle(0);
    legend->Draw();

    c->SaveAs("median_error_graph.png");
}

