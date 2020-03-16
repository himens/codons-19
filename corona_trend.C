void corona_trend(float fit_from_day = -1, float fit_to_day = -1,
                  bool y_in_log = false,
		  int days_to_pred = 3)
{
  // Set data
  std::vector<float> cases = {3, 16, 79, 157, 229, 322, 400, 650, 888, 1128, 1694, 2036, 2502, 3089, 3858, 4636, 5883, 7375, 9172, 10149, 
                              12462, 15113, 17660, 21157, 24747, 27980};
  std::vector<float> days(cases.size());
  for (size_t i = 0; i < cases.size(); i++) days[i] = i + 1; 
 
  // Set errors
  std::vector<float> e_days(days.size(), 0.0); 
  std::vector<float> e_cases(cases.size(), 0.0);
  std::transform(cases.begin(), cases.end(), e_cases.begin(), [] (const float N) { return std::sqrt(N); }); 

  // Sanity checks
  if (days.size() == 0) 
  {
    std::cout << "No data available!" << std::endl;
    return;
  }

  if (days.size() != cases.size()) 
  {
    std::cout << "Set " << days.size() << " days but " << cases.size() << " cases!" << std::endl;
    return;
  }

  if (fit_from_day == -1) fit_from_day = days.front();
  if (fit_to_day == -1) fit_to_day = days.back();

  if (fit_from_day >= fit_to_day) 
  {
    std::cout << "Wrong fit range!" << std::endl;
    return;
  }

  if (days_to_pred < 0)
  {
    std::cout << "Numer of days to predict are negative!" << std::endl;
    days_to_pred = 0;
  }

  // Make output file
  auto out_file = new TFile("corona_trend.root", "recreate");

  // Make graph
  auto gr_data = new TGraphErrors (days.size(), days.data(), cases.data(), e_days.data(), e_cases.data());
  gr_data->SetName("gr_data");
  gr_data->SetTitle("Corona virus trend;Days;Total cases");

  // Set style
  gStyle->SetTitleFontSize(0.07);
  gStyle->SetTitleW(1);
  gr_data->GetXaxis()->SetTitleSize(0.045);
  gr_data->GetXaxis()->SetTitleOffset(0.8);
  gr_data->GetYaxis()->SetTitleSize(0.045);
  gr_data->GetYaxis()->SetTitleOffset(1.7);
  gr_data->SetMarkerStyle(21);
  gr_data->SetMarkerSize(2);
  gr_data->SetMarkerColor(kRed);
  gr_data->SetLineColor(kRed);
  gr_data->SetLineWidth(4);

  // Draw data
  auto canv = new TCanvas("canv", "", 1000, 1000);
  canv->GetPad(0)->SetLeftMargin(0.16);
  canv->GetPad(0)->SetRightMargin(0.06);
  canv->SetLogy(y_in_log);
  canv->SetGridx();
  canv->SetGridy();

  gr_data->SetMinimum(0.0);
  gr_data->Draw("apl");

  // Fit functions
  auto expo_fun = new TF1("expo_fun", "[0] * exp([1] * x)", 0., 1e3); // exp
  expo_fun->SetParName(0, "Norm");
  expo_fun->SetParName(1, "Grow rate");
  expo_fun->SetParameters(1e1, 0.5);

  auto erf_fun = new TF1("erf_fun", "[0] * (1. + TMath::Erf(sqrt(2.) * (x - [1]) / [2]))", 0., 1e3); // erf
  erf_fun->SetParName(0, "Norm");
  erf_fun->SetParName(1, "Peak day");
  erf_fun->SetParName(2, "Critical days (2#sigma)");
  erf_fun->SetParameters(5e4, 10., 10.);

  auto test_fun = new TF1("test_fun", "[0] * exp( [1] * (x - [3] * log(exp([2]/[3]) + exp(x/[3]))) )", 0., 1e3); // test
  test_fun->SetParName(0, "Norm");
  test_fun->SetParName(1, "Grow rate");
  test_fun->SetParName(2, "#mu");
  test_fun->SetParName(3, "#sigma");
  test_fun->SetParLimits(0,  1e2, 1e7);
  test_fun->SetParLimits(1,  0.0,  1.0);
  test_fun->SetParLimits(2., 0.1, 50.0);
  test_fun->SetParLimits(3., 0.1, 50.0);
  test_fun->SetParameters(1e4, 0.25, 10.0, 1.0);
  test_fun->FixParameter(1, 0.21);
  //test_fun->FixParameter(2, 28.);
  //test_fun->FixParameter(3, 1.);

  // Pick-up your preferite fit function
  auto fit_fun = test_fun;

  // Fit
  gStyle->SetOptFit(111);
  fit_fun->SetNpx(1e3);
  fit_fun->SetLineColor(kBlue);
  fit_fun->SetLineWidth(4);
  gr_data->Fit(fit_fun, "VE", "", fit_from_day, fit_to_day);
  gPad->Modified();
  gPad->Update();

  // Change stats pos.
  auto st = (TPaveStats*)gr_data->GetListOfFunctions()->FindObject("stats");
  st->SetX1NDC(0.2);
  st->SetY1NDC(0.7);
  st->SetX2NDC(0.54);
  st->SetY2NDC(0.87);

  // Print days after which cases double
  if (std::string(fit_fun->GetName()) == "expo_fun" ||
      std::string(fit_fun->GetName()) == "test_fun")
  {
    float dt = log(2.0) / fit_fun->GetParameter(1);
    auto txt_str = Form("Cases double after %1.1f days", dt);
    float txt_x_pos = st->GetX1NDC();
    float txt_y_pos = 0.8 * st->GetY1NDC();
    if (y_in_log)
    {
      txt_x_pos = 0.3 + txt_x_pos;
      txt_y_pos = 1.0 - txt_y_pos;
    }
    auto text = new TText(txt_x_pos, txt_y_pos, txt_str);
    text->SetNDC();
    text->SetTextSize(0.03);
    text->Draw("same");
  }

  // Draw predicted cases
  auto draw_fit_point = [&] (const float day, const bool print_text = true) 
  {
    float cases = fit_fun->Eval(day);

    auto point = new TMarker(day, cases, 21);
    point->SetMarkerSize(2);
    point->SetMarkerColor(kOrange);
    gr_data->GetXaxis()->SetLimits(0.0, 1.2*std::max(days.back(), day));
    gr_data->SetMaximum(1.2*cases);
    point->Draw("same");

    if (print_text)
    {
      auto txt_str = (cases < 1.e5) ? Form("Exp: %1.0f", cases) : Form("Exp: %1.2e", cases);
      auto text = new TText(day - 2, 1.05*cases, txt_str);
      text->SetTextSize(0.02);
      text->Draw("same");
    }
  };

  int pred_from_day = std::min(fit_to_day, days.back()) + 1;  
  int pred_to_day = std::max(fit_to_day + days_to_pred, days.back());  
  for (int day = pred_from_day; day <= pred_to_day; day++)
  {
    bool print_text = (days_to_pred < 10) ? true : (day == pred_to_day) ? true : false; 
    draw_fit_point(day, print_text); 
  }

  // Draw fit function derivative
  auto gr_deriv = (TGraph*)fit_fun->DrawDerivative("same");
  gr_deriv->SetLineColor(kRed);
  gr_deriv->SetLineStyle(kDashed);
  gr_deriv->SetLineWidth(4);

  // Write output file
  canv->Write();
  gr_data->Write();
  gr_deriv->Write();
  out_file->Write();
};
  
