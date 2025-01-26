#include <TFile.h>
#include <TH1F.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TStyle.h>
#include <TDirectory.h>

void plotUncalibratedTimeSlices(const char* filename = "DQM_V0001_R000000001__Global__CMSSW_X_Y_Z__RECO.root", 
                                const char* output = "UncalibratedTimeSlices.pdf") {
    // Apri il file ROOT
    TFile* file = TFile::Open(filename, "READ");
    if (!file || file->IsZombie()) {
        std::cerr << "Errore: Impossibile aprire il file ROOT " << filename << std::endl;
        return;
    }

    // Naviga nella directory che contiene gli istogrammi
    TDirectory* dir = file->GetDirectory("DQMData/Run 1/MTD/Run summary/BTL/LocalReco");
    if (!dir) {
        std::cerr << "Errore: Directory DQMData/Run 1/MTD/Run summary/BTL/LocalReco non trovata!" << std::endl;
        file->Close();
        delete file;
        return;
    }
    dir->cd();

    // Parametri
    const unsigned int nHistos = 6; // Numero di istogrammi da plottare
    const int colors[] = {kRed, kBlue, kGreen, kMagenta, kOrange, kCyan}; // Array colori
    const int nColors = sizeof(colors) / sizeof(colors[0]);

    // Creazione del canvas
    TCanvas* c1 = new TCanvas("c1", "Uncalibrated Time Slices", 800, 600);
    TLegend* legend = new TLegend(0.7, 0.7, 0.9, 0.9);
    legend->SetTextSize(0.03);
    legend->SetBorderSize(0);

    // Stile generale
    gStyle->SetOptStat(0); // Disattiva le statistiche nei plot

    // Caricamento e disegno degli istogrammi
    bool firstPlot = true;
    for (unsigned int ihistoRU = 0; ihistoRU < nHistos; ++ihistoRU) {
        // Nome dell'istogramma
        std::string histoName = "BtlUncTimeRUSlice_" + std::to_string(ihistoRU + 1);

        // Recupera l'istogramma dal file ROOT
        TH1F* hist = dynamic_cast<TH1F*>(gDirectory->Get(histoName.c_str()));
        if (!hist) {
            std::cerr << "Attenzione: Istogramma " << histoName << " non trovato!" << std::endl;
            continue;
        }

        // Set stile e colore
        hist->SetLineColor(colors[ihistoRU % nColors]);
        hist->SetLineWidth(2);
        hist->SetMarkerStyle(20);
        hist->SetMarkerColor(colors[ihistoRU % nColors]);

        // Disegna con "SAME" tranne per il primo istogramma
        if (firstPlot) {
            hist->Draw("HIST");
            hist->SetTitle("Uncalibrated Time Slices;Time [ns];Entries");
            firstPlot = false;
        } else {
            hist->Draw("HIST SAME");
        }

        // Aggiungi la leggenda
        legend->AddEntry(hist, ("RU " + std::to_string(ihistoRU + 1)).c_str(), "l");
    }

    // Disegna la leggenda
    legend->Draw();

    // Salva il canvas in un file PDF
    c1->SaveAs(output);

    // Chiudi il file ROOT
    file->Close();
    delete file;
}

