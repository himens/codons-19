  auto my_test_fun = [] (double *x, double *p) 
  {
    const double avg_incub_days = 5.0; // avg incubation time
    //const double thr = 0.9;
    double norm     = p[0];
    double db_rate  = p[1];
    double mu       = p[2];
    double sigma    = p[3];
    double R0 = log(2) / db_rate;
    //double mu = lock_day + 2*sigma + avg_incub_days;

    //if (1./(1. + exp((x[0]-mu)/sigma)) > thr) return norm * exp(R0 * x[0]);
    return norm * exp(R0 * (x[0] - sigma * log(exp(mu/sigma) + exp(x[0]/sigma))));
  };
Double_t PlotRegioni(TString Regione = "Toscana", Int_t tot_plot = 25) {
  // vector to store cases
  Double_t ricoverati[1000], terap_intens[1000], ospedalizzati[1000], isolamento_domiciliare[1000], totali_positivi[1000], nuovi_positivi[1000], dimessi[1000], deceduti[1000], totali[1000], tamponi[1000], perc_tamponi[1000],incremento[1000], incremento_positivi[1000], ti_incr[1000];
  // basenumber to index the file
  Int_t  daynumber, day = 0;
  Int_t loc;
  Int_t ti_molt = 10;

  TH1F *peakpos = new TH1F("peakpos","Peak Position",60,0,60);
  TH1F *plateau = new TH1F("plateau","Plateau",100,0,5e4);

  peakpos->SetLineWidth(2);
  peakpos->GetXaxis()->SetTitle("day of peak");
  plateau->SetLineWidth(2);
  plateau->GetXaxis()->SetTitle("foreseen plateau value");
 
  // loop on february
  for(Int_t iday = 24; iday<30; iday++) {
    TString datarow;
    std::ifstream filin;
    
    daynumber = 20200200 + iday;
    filin.open((Form("../COVID-19/dati-regioni/dpc-covid19-ita-regioni-%d.csv",daynumber)));
    //    Int_t uno = 0;
    while (filin >> datarow)
      //    while (uno<1)
      {
	//	filin>>datarow;
	//	uno++;
	char c = ',';
	// fix in case of white spaces
	while(datarow.Length()<60) {
	  TString toappend;
	  filin >> toappend;
	  datarow.Append(toappend);
	}
	//	cout<<datarow.Data()<<endl;
	if(datarow.Contains(Regione.Data())) {
	  // search for the last ',' and the value
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
	  incremento_positivi[day] = chopped.Atoi();
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
	  if(day == 0) {
	    incremento[day] = 0;
	    incremento_positivi[day] = 0;
	    ti_incr[0] = 0;
	  }else{
	    incremento[day] = totali[day]-totali[day-1];
	    incremento_positivi[day] = totali_positivi[day]-totali_positivi[day-1];
	    ti_incr[day] = (terap_intens[day] - terap_intens[day-1])*ti_molt;
	  }
	}
      }
    day++;
    filin.close();    	  
  }

  // loop on March
  for(Int_t iday = 1; iday<32; iday++) {
    TString datarow;
    std::ifstream filin;
    
    daynumber = 20200300 + iday;
    filin.open((Form("../COVID-19/dati-regioni/dpc-covid19-ita-regioni-%d.csv",daynumber)));
    if(filin.good()) {
      while (filin >> datarow)
	{
	  //	std::cout << datarow << std::endl;
	    // search for the last ',' and the value
	    char c = ',';
	    // fix in case of white spaces
	    while(datarow.Length()<60) {
	      TString toappend;
	      filin >> toappend;
	      datarow.Append(toappend);
	    }
	  if(datarow.Contains(Regione.Data())) {
	    //	cout<<datarow.Data()<<endl;
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
	    incremento_positivi[day] = chopped.Atoi();
	    chopped = chopped(loc+1, loc+100);
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
	    incremento[day] = totali[day]-totali[day-1];
	    incremento_positivi[day] = totali_positivi[day]-totali_positivi[day-1];
	    ti_incr[day] = (terap_intens[day] - terap_intens[day-1])*ti_molt;
	  }
	}
      day++;
      filin.close();    	  
    }
  }

    // loop on April
  for(Int_t iday = 1; iday<31; iday++) {
    TString datarow;
    std::ifstream filin;
    
    daynumber = 20200400 + iday;
    filin.open((Form("../COVID-19/dati-regioni/dpc-covid19-ita-regioni-%d.csv",daynumber)));
    if(filin.good()) {
      while (filin >> datarow)
	{
	  //	std::cout << datarow << std::endl;
	    // search for the last ',' and the value
	    char c = ',';
	    // fix in case of white spaces
	    while(datarow.Length()<60) {
	      TString toappend;
	      filin >> toappend;
	      datarow.Append(toappend);
	    }
	  if(datarow.Contains(Regione.Data())) {
	    //	cout<<datarow.Data()<<endl;
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
	    incremento_positivi[day] = chopped.Atoi();
	    chopped = chopped(loc+1, loc+100);
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
	    incremento[day] = totali[day]-totali[day-1];
	    incremento_positivi[day] = totali_positivi[day]-totali_positivi[day-1];
	    ti_incr[day] = (terap_intens[day] - terap_intens[day-1])*ti_molt;
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
  
  TCanvas *canv = new TCanvas("canv","canv",1600,1600);
  canv->Divide(2,4);
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
  TGraph *ptotali2 = new TGraph(day,giorno,totali);
  ptotali2->SetMarkerStyle(20);
  ptotali2->SetTitle("totali");
  TGraph *ptamponi = new TGraph(day,giorno,tamponi);
  ptamponi->SetMarkerStyle(20);
  ptamponi->SetTitle("tamponi");
  TGraph *pperc_tamponi = new TGraph(day,giorno,perc_tamponi);
  pperc_tamponi->SetMarkerStyle(20);
  pperc_tamponi->SetTitle("percentuali tamponi positivi");
  TGraph *pincremento = new TGraph(day,giorno,incremento);
  pincremento->SetMarkerStyle(20);
  pincremento->SetTitle("incremento");
  TGraph *pincremento_positivi = new TGraph(day,giorno,incremento_positivi);
  pincremento_positivi->SetMarkerStyle(20);
  pincremento_positivi->SetTitle("incremento positivi");
  TGraph *pti_incr = new TGraph(day,giorno,ti_incr);
  pti_incr->SetMarkerStyle(20);
  pti_incr->SetTitle(Form("incremento terapia intensiva (*%d)",ti_molt));

  // fit function
  auto test_fun = new TF1("test_fun", my_test_fun, 0, 1e3, 4); // test
  test_fun->SetParName(0, "Norm");
  test_fun->SetParName(1, "Doubling rate");
  test_fun->SetParName(2, "Half-max population");
  test_fun->SetParName(3, "#sigma population");
  test_fun->SetParLimits(0,  1e2, 4e8);
  test_fun->SetParLimits(1,  1.0, 10.0);
  test_fun->SetParLimits(2., 1.0, 1e3);
  //  test_fun->FixParameter(2., 210e3);
  test_fun->SetParLimits(3., 0.1, 50.0);
  //  test_fun->SetParameters(1e4, 1.0, 210e3, 1.0);
  test_fun->SetParameters(1e4, 1.0, 10., 1.0);
  
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
  pincremento->SetLineColor(7);
  pincremento->SetMarkerColor(7);
  pincremento->Draw("PLSame");
  pincremento_positivi->SetLineColor(8);
  pincremento_positivi->SetMarkerColor(8);
  pincremento_positivi->Draw("PLSame");
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
  pisolamento_domiciliare->GetXaxis()->SetTitle("days");
  //  canv->cd(2)->Update();
  if(isolamento_domiciliare[day-1]>ospedalizzati[day-1]) {
    pisolamento_domiciliare->Draw("ALP");
    pisolamento_domiciliare->GetYaxis()->SetRangeUser(-100,isolamento_domiciliare[day-1] * 1.1);
    pospedalizzati->Draw("PLSame");
  } else {
    pospedalizzati->Draw("APL");
    pospedalizzati->GetYaxis()->SetRangeUser(-100,ospedalizzati[day-1] * 1.1);
    pisolamento_domiciliare->Draw("LPSame");
  }
  pterap_intens->SetLineColor(4);
  pterap_intens->SetMarkerColor(4);
  pterap_intens->Draw("PLSame");
  pti_incr->SetLineColor(6);
  pti_incr->SetMarkerColor(6);
  pti_incr->Draw("PLSame");
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
  canv->cd(4);
  canv->cd(4)->SetLogy(0);
  canv->cd(4)->SetTickx(1);
  canv->cd(4)->SetTicky(1);
  canv->cd(4)->SetGridx(1);
  canv->cd(4)->SetGridy(1);
  ptotali2->GetXaxis()->SetTitle("days");
  for(Int_t iplot = 0; iplot<tot_plot; iplot++) {
    ptotali2->Fit(test_fun,"Q","Q",iplot+5,day);
    peakpos->Fill(test_fun->GetParameter(2));
    plateau->Fill(test_fun->GetParameter(0));
  }
  ptotali2->Fit(test_fun,"Q","Q",0,day);
  ptotali2->Draw("ALP");
  Int_t days_to_pred = 10;
  
  auto draw_fit_point = [&] (const float day, const bool print_text = true) 
    {
     float fit_val = test_fun->Eval(day);
     //   canv->cd(2);   
     auto point = new TMarker(day, fit_val, 21);
     point->SetMarkerSize(2);
     point->SetMarkerColor(kOrange);
     ptotali2->GetXaxis()->SetLimits(0.0, day+days_to_pred);
     ptotali2->SetMaximum(1.2*fit_val);
     point->Draw("same");
     
     auto txt_str = (fit_val < 1.e5) ? Form("Exp: %1.0f", fit_val) : Form("Exp: %1.2e", fit_val);
     auto text = new TText(day - 2, 1.05*fit_val, txt_str);
     text->SetTextSize(0.02);
     text->Draw("same");
    };

  bool print_text = days_to_pred < 5 ? true : ((day == days_to_pred) ? true : false); 
  for(Int_t iday = day; iday<day+days_to_pred; iday++)
    draw_fit_point(iday, print_text); 
  

  canv->Update();

  //  TCanvas *canc = new TCanvas("canc","canc",800,1600);
  //  canv->Divide(1,2);
  canv->cd(5);
  canv->cd(5)->SetTickx(1);
  canv->cd(5)->SetTicky(1);
  canv->cd(5)->SetGridx(1);
  canv->cd(5)->SetGridy(1);
  peakpos->SetFillColor(4);
  peakpos->Draw("BAR");
  canv->cd(6);
  canv->cd(6)->SetTickx(1);
  canv->cd(6)->SetTicky(1);
  canv->cd(6)->SetGridx(1);
  canv->cd(6)->SetGridy(1);
  plateau->SetFillColor(4);
  plateau->Draw("BAR");


  //babbo plot
  // rapporto terapie intensive su nuovi casi
  Double_t bindex[1000], bindexdiff[1000];
  for(Int_t iday = 0; iday<day; iday++) {
    if(totali[iday]!=0) 
      bindex[iday] = terap_intens[iday]/totali[iday];
    else
      bindex[iday] = 0;
    
    if(incremento[iday] != 0)
      bindexdiff[iday] = ti_incr[iday]/incremento[iday]/50;
    else
      bindexdiff[iday] = 0;

  }
    
  TGraph *pbplot = new TGraph(day,giorno,bindex);
  pbplot->SetMarkerStyle(20);
  pbplot->SetTitle("rapporto gravi su totali");
  
  TGraph *pbplotdiff = new TGraph(day,giorno,bindexdiff);
  pbplotdiff->SetMarkerStyle(20);
  pbplotdiff->SetTitle("rapporto gravi su totali giorno per giorno");

  //  TCanvas *cbab = new TCanvas("cbab","cbab",800,1600);
  //  cbab->Divide(1,2);
  canv->cd(7);
  canv->cd(7)->SetTickx(1);
  canv->cd(7)->SetTicky(1);
  canv->cd(7)->SetGridx(1);
  canv->cd(7)->SetGridy(1);
  pbplot->Draw("APL");
  canv->cd(8);
  canv->cd(8)->SetTickx(1);
  canv->cd(8)->SetTicky(1);
  canv->cd(8)->SetGridx(1);
  canv->cd(8)->SetGridy(1);
  pbplotdiff->Draw("APL");


  return incremento[day-1]/totali[day-2]*100;
}
