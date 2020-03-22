void PlotProvincia(TString Provincia = "PI", Int_t log = 0) {
  // vector to store cases
  Double_t casi[1000];
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
	  Int_t loc = datarow.Last(c);
	  TString chopped = datarow(loc+1, loc+10);
	  casi[day] = chopped.Atoi();
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
    filin.open((Form("../COVID-19/dati-province/dpc-covid19-ita-province-%d.csv",daynumber)));
    if(filin.good()) {
      while (filin >> datarow)
	{
	  //	std::cout << datarow << std::endl;
	  if(datarow.Contains(Provincia.Data())) {
	    // search for the last ',' and the value
	    char c = ',';
	    Int_t loc = datarow.Last(c);
	    TString chopped = datarow(loc+1, loc+10);
	    casi[day] = chopped.Atoi();
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

  TCanvas *canv = new TCanvas("canv");
  canv->SetLogy(log);
  canv->SetTickx(1);
  canv->SetTicky(1);
  canv->SetGridx(1);
  canv->SetGridy(1);
  TGraph  *plot = new TGraph(day,giorno,casi);
  plot->SetMarkerStyle(20);
  plot->GetXaxis()->SetTitle("days");
  plot->GetYaxis()->SetTitle("positive");
  plot->SetTitle("integrated increment");
  plot->Draw("ALP");
  TGraph *ploti = new TGraph(day,giorno,incremento);
  ploti->SetTitle("daily increment");
  ploti->SetMarkerStyle(20);
  ploti->SetMarkerColor(2);
  ploti->SetLineColor(2);
  ploti->Draw("samePL");
  canv->BuildLegend();
  TLatex *scritta = new TLatex(2,casi[day-1],Provincia.Data());
  scritta->Draw("same");
  canv->Update();
}
