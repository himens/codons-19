{

  Double_t incremento[21],indice[21];

  TString lista[21];

  lista[0] = "Abruzzo";
  lista[1] = "Basilicata";
  lista[2] = "Bolzano";
  lista[3] = "Calabria";
  lista[4] = "Campania";
  lista[5] = "Emilia-Romagna";
  lista[6] = "FriuliVeneziaGiulia";
  lista[7] = "Lazio";
  lista[8] = "Liguria";
  lista[9] = "Lombardia";
  lista[10] = "Marche";
  lista[11] = "Molise";
  lista[12] = "Piemonte";
  lista[13] = "Puglia";
  lista[14] = "Sardegna";
  lista[15] = "Sicilia";
  lista[16] = "Toscana";
  lista[17] = "Trento";
  lista[18] = "Umbria";
  lista[19] = "Valled'Aosta";
  lista[20] = "Veneto";
  
  gROOT->LoadMacro("PlotRegioni.C");
  for(Int_t iloop = 0; iloop<21; iloop++){
    incremento[iloop] = PlotRegioni(lista[iloop].Data());
    indice[iloop] = iloop;
  }

  TCanvas *cloop = new TCanvas("cloop","cloop");
  TH1F *demo = new TH1F("demo","Incremento giornaliero",21,0.5,21.5);
  demo->Draw();
  for(Int_t i = 1; i<22; i++){
    demo->GetXaxis()->SetBinLabel(i,lista[i-1]);
    demo->SetBinContent(i,incremento[i-1]);
  }
  gStyle->SetOptStat(0);
  cloop->SetGridx(1);
  cloop->SetGridy(1);
  cloop->SetTickx(1);
  cloop->SetTicky(1);
  demo->SetLineWidth(2);
  demo->Draw("");


}
