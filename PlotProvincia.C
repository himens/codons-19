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

void PlotProvincia(TString Provincia = "PI", Int_t log = 0) {
  // vector to store cases
  Double_t casi[1000];
  Double_t casix10[1000];
  // basenumber to index the file
  Int_t  daynumber, day = 0;
  //  TString Provincia("PI");
  // loop on february
  for(Int_t iday = 24; iday<30; iday++) {
    TString datarow;
    std::ifstream filin;
    
    daynumber = 20200200 + iday;
    filin.open((Form("../COVID-19/dati-province/dpc-covid19-ita-province-%d.csv",daynumber)));
    while (filin >> datarow)
      {
	//	std::cout << datarow << std::endl;
	if(datarow.Contains(Provincia.Data())) {
	  // search for the last ',' and the value
	  char c = ',';
	  Int_t loc;
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
	  loc = chopped.First(c);
	  chopped = chopped(loc+1, loc+100);
	  loc = chopped.First(c);
	  chopped = chopped(loc+1, loc+100);
	  loc = chopped.First(c);
	  chopped = chopped(loc+1, loc+100);
	  //	  cout<<chopped.Data()<<endl;
	  casi[day] = chopped.Atoi();
	  casix10[day] = chopped.Atoi();
	  //	  cout<<casi[day]<<endl;
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
    filin.open((Form("../COVID-19/dati-province/dpc-covid19-ita-province-%d.csv",daynumber)));
    if(filin.good()) {
      while (filin >> datarow)
	{
	  if(datarow.Contains(Provincia.Data())) {
	  char c = ',';
	  Int_t loc;
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
	  loc = chopped.First(c);
	  chopped = chopped(loc+1, loc+100);
	  loc = chopped.First(c);
	  chopped = chopped(loc+1, loc+100);
	  loc = chopped.First(c);
	  chopped = chopped(loc+1, loc+100);
	  //	  cout<<chopped.Data()<<endl;
	  casi[day] = chopped.Atoi();
	  casix10[day] = chopped.Atoi();
	  //	  cout<<casi[day]<<endl;
       
	  }
	}
    day++;
    filin.close();    	  
    
    }
  }

  // loop on April
  for(Int_t iday = 1; iday<32; iday++) {
    TString datarow;
    std::ifstream filin;
    
    daynumber = 20200400 + iday;
    filin.open((Form("../COVID-19/dati-province/dpc-covid19-ita-province-%d.csv",daynumber)));
    if(filin.good()) {
      while (filin >> datarow)
	{
	  if(datarow.Contains(Provincia.Data())) {
	  char c = ',';
	  Int_t loc;
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
	  loc = chopped.First(c);
	  chopped = chopped(loc+1, loc+100);
	  loc = chopped.First(c);
	  chopped = chopped(loc+1, loc+100);
	  loc = chopped.First(c);
	  chopped = chopped(loc+1, loc+100);
	  //	  cout<<chopped.Data()<<endl;
	  casi[day] = chopped.Atoi();
	  casix10[day] = chopped.Atoi();
	  //	  cout<<casi[day]<<endl;
       
	  }
	}
    day++;
    filin.close();    	  
    
    }
  }

  Double_t giorno[1000];
  Double_t incremento[1000];
  for(Int_t iday = 0; iday<day; iday++) {
    //    cout<<casi[iday]<<endl;
    if(iday == 0)
      incremento[iday] = casi[iday];
    else
      incremento[iday] = casi[iday]-casi[iday-1];
    giorno[iday] = iday;
  }

  // fit function
  auto test_fun = new TF1("test_fun", my_test_fun, 0, 1e3, 4); // test
  test_fun->SetParName(0, "Norm");
  test_fun->SetParName(1, "Doubling rate");
  test_fun->SetParName(2, "Half-max population");
  test_fun->SetParName(3, "#sigma population");
  test_fun->SetParLimits(0,  1e2, 4.2e5);
  test_fun->SetParLimits(1,  1.0, 10.0);
  test_fun->SetParLimits(2., 1.0, 1e3);
  //  test_fun->FixParameter(2., 210e3);
  test_fun->SetParLimits(3., 0.1, 50.0);
  //  test_fun->SetParameters(1e4, 1.0, 210e3, 1.0);
    test_fun->SetParameters(1e4, 1.0, 10., 1.0);


  // plot
    TCanvas *canv = new TCanvas("canv","canv",800,1600);
  canv->Divide(1,2);
  canv->cd(1);
  canv->cd(1)->SetLogy(log);
  canv->cd(1)->SetTickx(1);
  canv->cd(1)->SetTicky(1);
  canv->cd(1)->SetGridx(1);
  canv->cd(1)->SetGridy(1);
  TGraph  *plot = new TGraph(day,giorno,casi);
  plot->SetMarkerStyle(20);
  plot->GetXaxis()->SetTitle("days");
  plot->GetYaxis()->SetTitle("positive");
  plot->SetTitle("cumulative increment");
  plot->Draw("ALP");
  TGraph  *plotbis = new TGraph(day,giorno,casix10);
  plotbis->SetMarkerStyle(20);
  plotbis->GetXaxis()->SetTitle("days");
  plotbis->GetYaxis()->SetTitle("positive");
  plotbis->SetTitle("cumulative increment");
  TGraph *ploti = new TGraph(day,giorno,incremento);
  ploti->SetTitle("daily increment");
  ploti->SetMarkerStyle(20);
  ploti->SetMarkerColor(2);
  ploti->SetLineColor(2);
  ploti->Draw("samePL");
  canv->cd(1)->BuildLegend();
  TLatex *scritta = new TLatex(2,casi[day-1],Provincia.Data());
  scritta->Draw("same");
  canv->cd(2);
  canv->cd(2)->SetLogy(log);
  canv->cd(2)->SetTickx(1);
  canv->cd(2)->SetTicky(1);
  canv->cd(2)->SetGridx(1);
  canv->cd(2)->SetGridy(1);
  plotbis->GetXaxis()->SetTitle("days");
  plotbis->Fit(test_fun,"","",20,day);
  plotbis->Draw("ALP");
  
  
  Int_t days_to_pred = 20;
  
  auto draw_fit_point = [&] (const float day, const bool print_text = true) 
    {
     float fit_val = test_fun->Eval(day);
     //   canv->cd(2);   
     auto point = new TMarker(day, fit_val, 21);
     point->SetMarkerSize(2);
     point->SetMarkerColor(kOrange);
     plotbis->GetXaxis()->SetLimits(0.0, day+days_to_pred);
     plotbis->SetMaximum(1.2*fit_val);
     point->Draw("same");
     
     auto txt_str = (fit_val < 1.e5) ? Form("Exp: %1.0f", fit_val) : Form("Exp: %1.2e", fit_val);
     auto text = new TText(day - 2, 1.05*fit_val, txt_str);
     if(print_text) {
       text->SetTextSize(0.02);
       text->Draw("same");
     }
    };

  bool print_text = days_to_pred < 5 ? true : ((day == days_to_pred) ? true : false); 
  for(Int_t iday = day; iday<day+days_to_pred; iday++)
    draw_fit_point(iday, print_text); 
  
  canv->Update();
}

