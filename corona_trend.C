/**********************/
/* Read data from csv */
/**********************/
std::tuple<std::vector<float>, 
           std::vector<float>> 
get_data_from_csv(const std::string csv_file_name, 
                  const std::string req_state,
                  const std::string req_location)
{
  // utility function: tokenize a csv line
  auto tokenize = [] (const std::string line, const char delim = ' ') 
  {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream iss(line);

    while (std::getline(iss, token, delim)) tokens.push_back(token);
    return tokens;
  };

  // utility function: tell if string is a digit
  auto is_digit = [] (const std::string str)
  {
    return std::all_of(str.begin(), str.end(), ::isdigit);
  };

  // utility function: convert string to digit
  auto to_digit = [&] (const std::string str)
  {
    float digit = 0.0;
    if (is_digit(str)) digit = std::stof(str);
    
    return digit;
  };
  
  // utility function: sum map data
  auto sum_data = [] (const std::map<std::string, std::vector<float>> &map)
  {
    std::vector<float> sum_data;
    for (const auto &p : map)
    {
      const auto &data = p.second;
      sum_data.resize( data.size() );
      std::transform(sum_data.begin(), sum_data.end(), data.begin(), sum_data.begin(), std::plus<float>()); // add data
    }

    return sum_data;
  };

  std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
  std::cout << " Read CSV data from file: " << csv_file_name                          << std::endl;
  std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;

  // Open csv file
  std::ifstream file(csv_file_name, std::ios::in);
  if (!file.good()) 
  {
    std::cout << "Cannot find csv file: " << csv_file_name << std::endl;
    return {};
  }

  // Get csv header 
  std::string header;
  std::getline(file, header);
  const size_t num_tokens = tokenize(header, ',').size();

  // Parse csv file
  std::string line;
  std::map<std::string, std::map<std::string, std::vector<float>>> tot_cases_map, tot_deaths_map;
  std::vector<std::string> states, locations;

  while (std::getline(file, line))
  {
    if (line.empty()) continue;  // skip empty line

    // Tokenize
    const auto tokens = tokenize(line, ',');
    if (tokens.size() != num_tokens) 
    {
      std::cout << "Found " << tokens.size() << " tokens, " << num_tokens << " expected!" << std::endl;
      continue; 
    }

    // Data format
    // ECDC
    //std::string day    = tokens[0];
    //std::string state  = tokens[1];
    //std::string location = "";
    //float new_cases    = to_digit( tokens[2] );
    //float new_deaths   = to_digit( tokens[3] );
    //float cases        = to_digit( tokens[4] );
    //float deaths       = to_digit( tokens[5] );

    // Protezione Civile (regioni)
    //std::string day     = tokens[0];
    //std::string state   = tokens[1];
    //std::string region  = tokens[3];
    //float tot_act_cases = to_digit( tokens[10] );
    //float new_act_cases = to_digit( tokens[11] );
    //float deaths        = to_digit( tokens[13] );
    //float cases         = to_digit( tokens[14] );
    //float tampons       = to_digit( tokens[15] );

    // Protezione Civile (province)
    std::string day      = tokens[0];
    std::string state    = tokens[1];
    std::string location = tokens[5];
    float cases          = to_digit( tokens[9] );
    float deaths         = 0.0;

    tot_cases_map[state][location] .push_back( cases ); 
    tot_deaths_map[state][location].push_back( deaths );
    
    if (std::find(states.begin(), states.end(), state) == states.end()) states.push_back( state );
    if (std::find(locations.begin(), locations.end(), location) == locations.end()) locations.push_back( location );

    if (state == req_state && 
	(location == req_location || req_location.empty()))
    {
      std::cout << "Read day: " << day << ", state: " << state << ", location: " << location 
	<< ", cases: " << cases << ", deaths: " << deaths << std::endl;
    }
  }

  file.close();

  // Fill tuple
  std::vector<float> tot_cases, tot_deaths;

  if (std::find(states.begin(), states.end(), req_state) != states.end()) 
  { 
    if (req_location.empty()) // no location requested... sum data over locations
    {
      std::cout << "No location requested. Sum data over locations..." << std::endl; 

      tot_cases  = sum_data( tot_cases_map[req_state] );
      tot_deaths = sum_data( tot_deaths_map[req_state] );
    }

    else if (std::find(locations.begin(), locations.end(), req_location) != locations.end()) 
    {
      tot_cases  = tot_cases_map[req_state][req_location];
      tot_deaths = tot_deaths_map[req_state][req_location];
    }   

    else std::cout << "Data for location " << req_location << " not found!" << std::endl;
  }

  else std::cout << "Data for state " << req_state << " not found!" << std::endl;

  return std::make_tuple(tot_cases, tot_deaths);
}


/********************/
/* Corona virus fit */
/********************/
void corona_trend(std::string country = "Italy",
                  std::string location = "",
                  std::string dataset_name = "total_cases",
		  std::string fit_model_name = "test",
		  float fit_from_day = 0.0, 
		  float fit_to_day = -1.0,
		  int days_to_pred = 3,
                  bool y_in_log = false)
{ 
  // Sanity checks
  if (dataset_name != "total_cases" &&
      dataset_name != "total_deaths") 
  {
    std::cout << "No dataset " << dataset_name << " known!" << std::endl;
    return;
  }

  if (fit_from_day < 0.0 || 
      (fit_to_day != -1.0 && fit_from_day >= fit_to_day)) 
  {
    std::cout << "Wrong fit range [" << fit_from_day << ", " << fit_to_day << "]!" << std::endl;
    return;
  }

  if (days_to_pred < 0)
  {
    std::cout << "Number of days to predict are negative!" << std::endl;
    days_to_pred = 0;
  }

  if (fit_model_name != "expo" && 
      fit_model_name != "test")
  {
    std::cout << "Fit model " << fit_model_name << " not known!" << std::endl;
    return;
  }

  // Set ROOT style
  gStyle->SetTitleFontSize(0.06);
  gStyle->SetTitleW(1);
  gStyle->SetOptFit(111);

  // Get data from csv 
  const std::string csv_file_name{"full_data.csv"};
  auto dataset = get_data_from_csv(csv_file_name, country, location); 

  // Set data
  std::string y_title;
  std::vector<float> data; 

  if (dataset_name == "total_cases") 
  {
    data = std::get<0>(dataset);
    y_title = "Total cases";
  }
  if (dataset_name == "total_deaths") 
  {
    data = std::get<1>(dataset);
    y_title = "Total deaths";
  }

  if (data.size() == 0) 
  {
    std::cout << "Data size is zero!" << std::endl;
    return;
  }

  std::vector<float> days(data.size()); 
  std::iota(days.begin(), days.end(), 0); // from day 0

  std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
  std::cout <<  " " << y_title << " for: " << country << ", location: " << location   << std::endl;
  std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
  for (auto day : days) std::cout << "Day = " << day << ", data = " << data[day]      << std::endl;

  // Set errors
  std::vector<float> e_days(days.size(), 0.0); 
  std::vector<float> e_data(data.size(), 0.0);
  std::transform(data.begin(), data.end(), e_data.begin(), [] (const float N) { return sqrt(N); }); 

  // Set fit range
  if (fit_from_day == 0.0) fit_from_day = days.front();
  if (fit_to_day == -1.0)  fit_to_day = days.back();

  // Make output file
  auto out_file = new TFile("corona_trend.root", "recreate");
  
  // Make graph
  auto gr_data = new TGraphErrors (days.size(), days.data(), data.data(), e_days.data(), e_data.data());
  gr_data->SetName("gr_data");
  gr_data->SetTitle( Form("Corona virus trend: %s %s;Days;%s", country.c_str(), location.c_str(), y_title.c_str()) );
  gr_data->GetXaxis()->SetTitleSize(0.045);
  gr_data->GetXaxis()->SetTitleOffset(0.8);
  gr_data->GetYaxis()->SetTitleSize(0.045);
  gr_data->GetYaxis()->SetTitleOffset(1.7);
  gr_data->SetMarkerStyle(21);
  gr_data->SetMarkerSize(2);
  gr_data->SetMarkerColor(kRed);
  gr_data->SetLineColor(kRed);
  gr_data->SetLineWidth(4);
  gr_data->SetMinimum(0.0);

  // Draw data
  auto canv = new TCanvas("canv", "", 1000, 1000);
  canv->GetPad(0)->SetLeftMargin(0.16);
  canv->GetPad(0)->SetRightMargin(0.06);
  canv->SetLogy(y_in_log);
  canv->SetGridx();
  canv->SetGridy();
  gr_data->Draw("apl");

  // Define fit functions 
  // 1) exponential
  auto my_expo_fun = [] (double *x, double *p)
  {
    double norm    = p[0];
    double db_rate = p[1];
    double R0 = log(2) / db_rate;
    return norm * exp(R0 * x[0]);
  };

  auto expo_fun = new TF1("expo_fun", my_expo_fun, 0., 1e3, 2); 
  expo_fun->SetParName(0, "Norm");
  expo_fun->SetParName(1, "Doubling rate");
  expo_fun->SetParameters(1e-1, 1.0);

  // 2) test 
  auto my_test_fun = [] (double *x, double *p) 
  {
    double norm     = p[0];
    double db_rate  = p[1];
    double mu       = p[2];
    double sigma    = p[3];
    double R0 = log(2) / db_rate;

    return norm * exp(R0 * (x[0] - sigma * log(exp(mu/sigma) + exp(x[0]/sigma))));
  };

  auto test_fun = new TF1("test_fun", my_test_fun, 0, 1e3, 4); // test
  test_fun->SetParName(0, "Norm");
  test_fun->SetParName(1, "Doubling rate");
  test_fun->SetParName(2, "Half-max population");
  test_fun->SetParName(3, "#sigma population");
  test_fun->SetParLimits(0,  1e2, 1e7);
  test_fun->SetParLimits(1,  1.0, 10.0);
  test_fun->SetParLimits(2., 1.0, 1e3);
  test_fun->SetParLimits(3., 0.1, 50.0);
  test_fun->SetParameters(1e4, 1.0, 10.0, 1.0);
  //test_fun->FixParameter(1, 3.1); // from fit to Lombardia [6-14] (pure expo.)
  //test_fun->FixParameter(2, 37.);
  //test_fun->FixParameter(3, 5);

  // Select fit function
  TF1* fit_fun = nullptr;
  if (fit_model_name == "expo") fit_fun = expo_fun; 
  if (fit_model_name == "test") fit_fun = test_fun; 

  fit_fun->SetNpx(1e3);
  fit_fun->SetLineColor(kBlue);
  fit_fun->SetLineWidth(4);

  // Fit
  std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
  std::cout << " Fit with " << fit_model_name << " in range [" << fit_from_day << ", " << fit_to_day << "]" << std::endl;
  std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
  if (fit_from_day > days.back() || fit_to_day < days.front()) 
  {
    std::cout << "No data in fit range! Cannot fit!" << std::endl; 
    return;
  }
  gr_data->Fit(fit_fun, "VE", "", fit_from_day, fit_to_day);
  gPad->Modified();
  gPad->Update();

  // Change stats pos.
  auto st = (TPaveStats*)gr_data->GetListOfFunctions()->FindObject("stats");
  st->SetX1NDC(0.2);
  st->SetY1NDC(0.66);
  st->SetX2NDC(0.64);
  st->SetY2NDC(0.88);

  // Draw predicted data
  auto draw_fit_point = [&] (const float day, const bool print_text = true) 
  {
    float fit_val = fit_fun->Eval(day);

    auto point = new TMarker(day, fit_val, 21);
    point->SetMarkerSize(2);
    point->SetMarkerColor(kOrange);
    gr_data->GetXaxis()->SetLimits(0.0, 1.2*std::max(days.back(), day));
    gr_data->SetMaximum(1.2*fit_val);
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
  
