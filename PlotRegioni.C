void PlotRegioni(TString Regione = "PI") {
  // vector to store cases
  Double_t ricoverati[1000], terap_intens[1000], ospedalizzati[1000], isolamento_domiciliare[1000], totali_positivi[1000], nuovi_positivi[1000], dimessi[1000], deceduti[1000], totali[1000], tamponi[1000], perc_tamponi[1000];
  // basenumber to index the file
  Int_t  daynumber, day = 0;
  Int_t loc;
  //  TString Regione("PI");
  // loop on february
  for(Int_t iday = 24; iday<30; iday++) {
    TString datarow;
    std::ifstream filin;
    
    daynumber = 20200200 + iday;
    filin.open((Form("../COVID-19/dati-regioni/dpc-covid19-ita-regioni-%d.csv",daynumber)));
    while (filin >> datarow)
      {
	//	std::cout << datarow << std::endl;
	if(datarow.Contains(Regione.Data())) {
	  // search for the last ',' and the value
	  char c = ',';
	  // the strategy is the following, searches for the first comma, then chop the string
	  // and go on until the right comma is found...
	  loc = datarow.First(c);
	  TString chopped = datarow(loc+1, loc+100);
	  loc = chopped.First(c);
	  chopped = chopped(loc+1, loc+100);
	  loc = chopped.First(c);
	  chopped = chopped(loc+1, loc+100);
	  loc = chopped.First(c);
	  chopped = chopped(loc+1, loc+100);
	  loc = chopped.First(c);
	  chopped = chopped(loc+1, loc+100);
	  loc = chopped.First(c);
	  chopped = chopped(loc+1, loc+100);
	  ricoverati[day] = chopped.Atoi();
	  loc = chopped.First(c);
	  chopped = chopped(loc+1, loc+100);
	  terap_intens[day] = chopped.Atoi();
	  loc = chopped.First(c);
	  chopped = chopped(loc+1, loc+100);
	  ospedalizzati[day] = chopped.Atoi();
	  loc = chopped.First(c);
	  chopped = chopped(loc+1, loc+100);
	  isolamento_domiciliare[day] = chopped.Atoi();
	  loc = chopped.First(c);
	  chopped = chopped(loc+1, loc+100);
	  totali_positivi[day] = chopped.Atoi();	
	  loc = chopped.First(c);
	  chopped = chopped(loc+1, loc+100);
	  nuovi_positivi[day] = chopped.Atoi();
	  loc = chopped.First(c);
	  chopped = chopped(loc+1, loc+100);
	  dimessi[day] = chopped.Atoi();
	  loc = chopped.First(c);
	  chopped = chopped(loc+1, loc+100);
	  deceduti[day] = chopped.Atoi();
	  loc = chopped.First(c);
	  chopped = chopped(loc+1, loc+100);
	  totali[day] = chopped.Atoi();
	  loc = chopped.First(c);
	  chopped = chopped(loc+1, loc+100);
	  tamponi[day] = chopped.Atoi();
	  if(tamponi[day]!=0)
	    perc_tamponi[day] = totali[day]/tamponi[day];
	  else
	    perc_tamponi[day] = 0;
	}
      }
    day++;
    filin.close();    	  
  }

  // loop on March
  for(Int_t iday = 1; iday<31; iday++) {
    TString datarow;
    std::ifstream filin;
    
    daynumber = 20200300 + iday;
    filin.open((Form("../COVID-19/dati-regioni/dpc-covid19-ita-regioni-%d.csv",daynumber)));
    if(filin.good()) {
      while (filin >> datarow)
	{
	  //	std::cout << datarow << std::endl;
	  if(datarow.Contains(Regione.Data())) {
	    // search for the last ',' and the value
	    char c = ',';
	    // the strategy is the following, searches for the first comma, then chop the string
	    // and go on until the right comma is found...
	    loc = datarow.First(c);
	    TString chopped = datarow(loc+1, loc+100);
	    loc = chopped.First(c);
	    chopped = chopped(loc+1, loc+100);
	    loc = chopped.First(c);
	    chopped = chopped(loc+1, loc+100);
	    loc = chopped.First(c);
	    chopped = chopped(loc+1, loc+100);
	    loc = chopped.First(c);
	    chopped = chopped(loc+1, loc+100);
	    loc = chopped.First(c);
	    chopped = chopped(loc+1, loc+100);
	    ricoverati[day] = chopped.Atoi();
	    loc = chopped.First(c);
	    chopped = chopped(loc+1, loc+100);
	    terap_intens[day] = chopped.Atoi();
	    loc = chopped.First(c);
	    chopped = chopped(loc+1, loc+100);
	    ospedalizzati[day] = chopped.Atoi();
	    loc = chopped.First(c);
	    chopped = chopped(loc+1, loc+100);
	    isolamento_domiciliare[day] = chopped.Atoi();
	    loc = chopped.First(c);
	    chopped = chopped(loc+1, loc+100);
	    totali_positivi[day] = chopped.Atoi();
	    loc = chopped.First(c);
	    chopped = chopped(loc+1, loc+100);
	    nuovi_positivi[day] = chopped.Atoi();
	    loc = chopped.First(c);
	    chopped = chopped(loc+1, loc+100);
	    dimessi[day] = chopped.Atoi();
	    loc = chopped.First(c);
	    chopped = chopped(loc+1, loc+100);
	    deceduti[day] = chopped.Atoi();
	    loc = chopped.First(c);
	    chopped = chopped(loc+1, loc+100);
	    totali[day] = chopped.Atoi();
	    loc = chopped.First(c);
	    chopped = chopped(loc+1, loc+100);
	    tamponi[day] = chopped.Atoi();
	    perc_tamponi[day] = totali[day]/tamponi[day];
	  }
	}
      day++;
      filin.close();    	  
    }
  }

  Double_t giorno[1000];
  for(Int_t iday = 0; iday<day; iday++) {
    giorno[iday] = iday;
  }
  
  TCanvas *canv = new TCanvas("canv","canv",800,1600);
  canv->Divide(1,3);
  TGraph *pricoverati = new TGraph(day,giorno,ricoverati);
  pricoverati->SetMarkerStyle(20);
  pricoverati->SetTitle("ricoverati");
  TGraph *pterap_intens = new TGraph(day,giorno,terap_intens);
  pterap_intens->SetMarkerStyle(20);
  pterap_intens->SetTitle("terapia intensiva");
  TGraph *pospedalizzati = new TGraph(day,giorno,ospedalizzati);
  pospedalizzati->SetMarkerStyle(20);
  pospedalizzati->SetTitle("ospedalizzati");
  TGraph *pisolamento_domiciliare = new TGraph(day,giorno,isolamento_domiciliare);
  pisolamento_domiciliare->SetMarkerStyle(20);
  pisolamento_domiciliare->SetTitle("isolamento domiciliare");
  TGraph *ptotali_positivi = new TGraph(day,giorno,totali_positivi);
  ptotali_positivi->SetMarkerStyle(20);
  ptotali_positivi->SetTitle("totali positivi");
  TGraph *pdimessi = new TGraph(day,giorno,dimessi);
  pdimessi->SetMarkerStyle(20);
  pdimessi->SetTitle("dimessi");
  TGraph *pdeceduti = new TGraph(day,giorno,deceduti);
  pdeceduti->SetMarkerStyle(20);
  pdeceduti->SetTitle("deceduti");
  TGraph *ptotali = new TGraph(day,giorno,totali);
  ptotali->SetMarkerStyle(20);
  ptotali->SetTitle("totali");
  TGraph *ptamponi = new TGraph(day,giorno,tamponi);
  ptamponi->SetMarkerStyle(20);
  ptamponi->SetTitle("tamponi");
  TGraph *pperc_tamponi = new TGraph(day,giorno,perc_tamponi);
  pperc_tamponi->SetMarkerStyle(20);
  pperc_tamponi->SetTitle("percentuali tamponi positivi");

  // totali, positivi, guariti, morti
  canv->cd(1);
  canv->cd(1)->SetTickx(1);
  canv->cd(1)->SetTicky(1);
  canv->cd(1)->SetGridx(1);
  canv->cd(1)->SetGridy(1);
  canv->cd(1)->SetLogy(0);
  ptotali->GetXaxis()->SetTitle("days");
  ptotali->Draw("ALP");
  ptotali_positivi->SetLineColor(2);
  ptotali_positivi->SetMarkerColor(2);
  ptotali_positivi->Draw("PLSame");
  pdimessi->SetLineColor(4);
  pdimessi->SetMarkerColor(4);
  pdimessi->Draw("PLSame");
  pdeceduti->SetLineColor(6);
  pdeceduti->SetMarkerColor(6);
  pdeceduti->Draw("PLSame");
  canv->cd(1)->BuildLegend();
  TLatex *scritta = new TLatex(2,totali[day-1],Regione.Data());
  scritta->Draw("same");
  // domiciliari, ospedalizzati, terapia intensiva
  canv->cd(2);
  canv->cd(2)->SetTickx(1);
  canv->cd(2)->SetTicky(1);
  canv->cd(2)->SetGridx(1);
  canv->cd(2)->SetGridy(1);
  pospedalizzati->SetLineColor(2);
  pospedalizzati->SetMarkerColor(2);
  pospedalizzati->GetXaxis()->SetTitle("days");
  pospedalizzati->Draw("APL");
  pisolamento_domiciliare->Draw("LPSame");
  pterap_intens->SetLineColor(4);
  pterap_intens->SetMarkerColor(4);
  pterap_intens->Draw("PLSame");
  canv->cd(2)->BuildLegend();
  // percentuali tamponi/positivi
  canv->cd(3);
  canv->cd(3)->SetTickx(1);
  canv->cd(3)->SetTicky(1);
  canv->cd(3)->SetGridx(1);
  canv->cd(3)->SetGridy(1);
  pperc_tamponi->GetXaxis()->SetTitle("days");
  pperc_tamponi->Draw("ALP");
  canv->cd(3)->BuildLegend();

  canv->Update();

}
