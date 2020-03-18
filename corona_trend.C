/**********************/
/* Read data from csv */
/**********************/
std::tuple<std::vector<float>, 
           std::vector<float>, 
           std::vector<float>, 
           std::vector<float>> 
get_data_from_csv(const std::string csv_file, 
                  const std::string country)
{
  // utility function: tokenize a csv line
  auto tokenize = [] (const std::string line, const char delim = ' ') 
  {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream iss(line);

    while (std::getline(iss, token, delim)) 
    {
      tokens.push_back(token);
    }

    return tokens;
  };

  // utility function: tell if string is a digit
  auto is_digit = [](const std::string str) 
  {
    if (str.empty()) return false;
    else return std::all_of(str.begin(), str.end(), ::isdigit); 
  };

  const size_t min_tokens = 5; // date,location,new_cases,new_deaths,total_cases,(total_deaths)
  std::vector<float> new_cases, new_deaths, tot_cases, tot_deaths;
  std::string line;

  // Open csv file
  std::ifstream file(csv_file, std::ios::in);
  if (!file.good()) 
  {
    std::cout << "Cannot find csv file: " << csv_file << std::endl;
    return {};
  }

  // Parse csv file
  while (std::getline(file, line))
  {
    if (line.empty()) continue;  // skip empty line
    if (line.find("date") != std::string::npos) continue; // skip header

    const auto tokens = tokenize(line, ',');
    if (tokens.size() < min_tokens) 
    {
      std::cout << "Num. csv tokens is " << tokens.size() << " less than " 
	<< min_tokens << ". Skip this line!" << std::endl;
      continue; 
    }

    std::string read_date    = tokens[0];
    std::string read_country = tokens[1];
    float read_new_cases     = is_digit(tokens[2]) ? std::stof(tokens[2]) : 0.0;
    float read_new_deaths    = is_digit(tokens[3]) ? std::stof(tokens[3]) : 0.0;
    float read_tot_cases     = is_digit(tokens[4]) ? std::stof(tokens[4]) : 0.0;
    float read_tot_deaths    = tokens.size() > min_tokens ? (is_digit(tokens[5]) ? std::stof(tokens[5]) : 0.0) : 0.0;

    // Fill data vector
    if (read_country == country) 
    {
      std::cout << read_country << ", " << read_date << ": " 
	<< "new cases = " << read_new_cases << ", new deaths = " << read_new_deaths 
	<< ", total_cases = " << read_tot_cases << ", total_deaths = " << read_tot_deaths << std::endl;

      new_cases.push_back( read_new_cases );
      new_deaths.push_back( read_new_deaths );
      tot_cases.push_back( read_tot_cases );
      tot_deaths.push_back( read_tot_deaths );
    }
  }

  return std::make_tuple(new_cases, new_deaths, tot_cases, tot_deaths);
}


/********************/
/* Corona virus fit */
/********************/
void corona_trend(std::string country = "Italy",
                  std::string dataset_name = "total_deaths",
		  std::string fit_model_name = "test",
		  float fit_from_day = -1, 
		  float fit_to_day = -1,
		  int days_to_pred = 3,
                  bool y_in_log = false)
{ 
  // Sanity checks
  if (dataset_name != "total_cases" &&
      dataset_name != "total_deaths") 
  {
    std::cout << "No dataset " << dataset_name << " known! Exit!" << std::endl;
    return;
  }

  if (fit_from_day != -1 && 
      fit_to_day != -1 &&
      fit_from_day >= fit_to_day) 
  {
    std::cout << "Wrong fit range!" << std::endl;
    return;
  }

  if (days_to_pred < 0)
  {
    std::cout << "Numer of days to predict are negative!" << std::endl;
    days_to_pred = 0;
  }

  if (fit_model_name != "expo" && 
      fit_model_name != "erf" && 
      fit_model_name != "test")
  {
    std::cout << "Fit model " << fit_model_name << " not known! Exit!" << std::endl;
    return;
  }

  // Get data from csv 
  const std::string csv_file{"full_data.csv"};
  auto dataset = get_data_from_csv(csv_file, country);
  const size_t data_size = std::get<0>(dataset).size();

  if (data_size == 0) 
  {
    std::cout << "No data available!" << std::endl;
    return;
  }

  // Set data
  std::string y_title;
  std::vector<float> data(data_size); 
  std::vector<float> days(data_size);
  std::iota(days.begin(), days.end(), 1);

  if (dataset_name == "total_cases") 
  {
    data = std::get<2>(dataset);
    y_title = "Total cases";
  }
  if (dataset_name == "total_deaths") 
  {
    data = std::get<3>(dataset);
    y_title = "Total deaths";
  }

  // Set errors
  std::vector<float> e_days(days.size(), 0.0); 
  std::vector<float> e_data(data.size(), 0.0);
  std::transform(data.begin(), data.end(), e_data.begin(), [] (const float N) { return std::sqrt(N); }); 

  // Make output file
  auto out_file = new TFile("corona_trend.root", "recreate");

  // Make graph
  auto gr_data = new TGraphErrors (days.size(), days.data(), data.data(), e_days.data(), e_data.data());
  gr_data->SetName("gr_data");
  gr_data->SetTitle( Form("Corona virus trend;Days;%s", y_title.c_str()) );

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
  // exponential
  auto expo_fun = new TF1("expo_fun", "[0] * exp([1] * x)", 0., 1e3); 
  expo_fun->SetParName(0, "Norm");
  expo_fun->SetParName(1, "Grow rate");
  expo_fun->SetParameters(1e1, 0.5);

  // ERF
  auto erf_fun = new TF1("erf_fun", "[0] * (1. + TMath::Erf(sqrt(2.) * (x - [1]) / [2]))", 0., 1e3); 
  erf_fun->SetParName(0, "Norm");
  erf_fun->SetParName(1, "Peak day");
  erf_fun->SetParName(2, "Critical days (2#sigma)");
  erf_fun->SetParameters(5e4, 10., 10.);

  // test 
  auto my_fun = [](double *x, double *p) 
  {
    const double avg_incub_days = 5.0; // avg incubation time
    const double epsi = 0.1;
    double norm     = p[0];
    double R0       = p[1];
    double lock_day = p[2];
    double sigma    = p[3];
    double mu = lock_day + 2*sigma + avg_incub_days;

    if (1./(1. + exp(-mu/sigma)) < 1.0 - epsi) return 0.0; // full popolation at day = 0
    return norm * exp(R0 * (x[0] - sigma * log(exp(mu/sigma) + exp(x[0]/sigma))));
  };

  auto test_fun = new TF1("test_fun", my_fun, 0, 1e3, 4); // test
  test_fun->SetParName(0, "Norm");
  test_fun->SetParName(1, "Grow rate");
  test_fun->SetParName(2, "Lockdown day");
  test_fun->SetParName(3, "#sigma popolation");
  test_fun->SetParLimits(0,  1e2, 1e7);
  test_fun->SetParLimits(1,  0.0, 1.0);
  test_fun->SetParLimits(2., 0.1, 1e3);
  test_fun->SetParLimits(3., 0.1, 50.0);
  test_fun->SetParameters(1e4, 0.25, 10.0, 1.0);
  //test_fun->FixParameter(1, 0.21);
  //test_fun->FixParameter(2, 37.);
  //test_fun->FixParameter(3, 3.4);

  // Select fit function
  TF1* fit_fun = nullptr;
  if (fit_model_name == "expo") fit_fun = expo_fun; 
  if (fit_model_name == "erf")  fit_fun = erf_fun; 
  if (fit_model_name == "test") fit_fun = test_fun; 

  fit_fun->SetNpx(1e3);
  fit_fun->SetLineColor(kBlue);
  fit_fun->SetLineWidth(4);

  // Fit
  if (fit_from_day == -1) fit_from_day = days.front();
  if (fit_to_day == -1) fit_to_day = days.back();

  gStyle->SetOptFit(111);
  gr_data->Fit(fit_fun, "E", "", fit_from_day, fit_to_day);
  gPad->Modified();
  gPad->Update();

  // Change stats pos.
  auto st = (TPaveStats*)gr_data->GetListOfFunctions()->FindObject("stats");
  st->SetX1NDC(0.2);
  st->SetY1NDC(0.7);
  st->SetX2NDC(0.54);
  st->SetY2NDC(0.87);

  // Print days after which data double
  if (fit_model_name == "expo" ||
      fit_model_name == "test")
  {
    float dt = log(2.0) / fit_fun->GetParameter(1);
    auto txt_str = Form("Data double after %1.1f days", dt);
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

  // Draw predicted data
  auto draw_fit_point = [&] (const float day, const bool print_text = true) 
  {
    float fit_val = fit_fun->Eval(day);

    auto point = new TMarker(day, fit_val, 21);
    point->SetMarkerSize(2);
    point->SetMarkerColor(kOrange);
    gr_data->GetXaxis()->SetLimits(0.0, 1.2*std::max(days.back(), day));
    gr_data->SetMaximum(1.2*std::max(fit_val, data.back()));
    point->Draw("same");

    if (print_text)
    {
      auto txt_str = (fit_val < 1.e5) ? Form("Exp: %1.0f", fit_val) : Form("Exp: %1.2e", fit_val);
      auto text = new TText(day - 2, 1.05*fit_val, txt_str);
      text->SetTextSize(0.02);
      text->Draw("same");
    }
  };

  int pred_from_day = std::min(fit_to_day, days.back()) + 1;  
  int pred_to_day = pred_from_day + days_to_pred - 1;
  for (int day = pred_from_day; day <= pred_to_day; day++)
  {
    bool print_text = days_to_pred < 5 ? true : ((day == pred_to_day) ? true : false); 
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
  
