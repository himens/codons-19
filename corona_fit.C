/**********************/
/* Read data from csv */
/**********************/
// Parsed csv data are stored in map <dataset_name, data vector>
using Data_t = std::map<std::string, std::vector<float>>; 
Data_t get_data_from_csv(const std::string csv_file_name, 
                         const std::string format_name,
                         const std::string req_state,
                         const std::string req_region)
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

  // utility function: convert string to digit
  auto to_digit = [&] (const std::string str)
  {
    bool is_digit = str.empty() ? false : std::all_of(str.begin(), str.end(), ::isdigit);
    return is_digit ? std::stof(str) : 0.0;
  };
  
  std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
  std::cout << " Read CSV data from file: " << csv_file_name                          << std::endl;
  std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;

  // Sanity checks
  if (format_name != "ecdc" &&
      format_name != "PC regioni" &&
      format_name != "PC province")
  {
    std::cout << "No csv format " << format_name << " known!" << std::endl;
    return {};
  }

  // Open csv file
  std::ifstream file(csv_file_name, std::ios::in);
  if (!file.good()) 
  {
    std::cout << "Cannot find csv file: " << csv_file_name << std::endl;
    return {};
  }

  // Get csv fields
  std::string line;
  std::getline(file, line); // first line
  const auto &csv_fields = tokenize(line, ',');
  const auto num_csv_fields = csv_fields.size();

  // Parse csv file
  std::map<std::string, std::map<std::string, Data_t>> data_map;

  while (std::getline(file, line))
  {
    if (line.empty()) continue;  // skip empty line

    // Tokenize
    const auto &tokens = tokenize(line, ',');
    const auto num_tokens = tokens.size();
    if (num_tokens != num_csv_fields) 
    {
      if (num_tokens == num_csv_fields - 1)
      {
        std::cout << "Last token missing?" << std::endl;
      }
      else 
      {
        std::cout << "Found " << num_tokens << " tokens, " << num_csv_fields << " expected!" << std::endl;
        continue;
      }
    }

    // Data format
    // ECDC
    if (format_name == "ecdc")
    {
      std::string day    = tokens[0];
      std::string state  = tokens[1];
      std::string region = "N/A";
      for (size_t i = 2; i < tokens.size(); i++) 
      {
	 data_map[state][region][csv_fields[i]].push_back( to_digit(tokens[i]) );
      }
    }

    // Protezione Civile (regioni)
    if (format_name == "PC regioni")
    {
      std::string day    = tokens[0];
      std::string state  = tokens[1];
      std::string region = tokens[3];
      for (size_t i = 4; i < tokens.size(); i++) 
      {
	 data_map[state][region][csv_fields[i]].push_back( to_digit(tokens[i]) );
      }
    }

    // Protezione Civile (province)
    if (format_name == "PC province")
    {
      std::string day    = tokens[0];
      std::string state  = tokens[1];
      std::string region = tokens[5];
      for (size_t i = 6; i < tokens.size(); i++) 
      {
	 data_map[state][region][csv_fields[i]].push_back( to_digit(tokens[i]) );
      }
    }
  }

  file.close();

  // Fill output data
  Data_t &out_data = data_map[req_state][req_region];

  if (req_region.empty()) // no region requested... sum data over regions
  {
    std::cout << "No region requested. Sum data over regions..." << std::endl; 

    for (const auto &p : data_map[req_state])
    {
      for (const auto &p2 : p.second)
      {
	const auto &ds = p2.first;
	const auto &v = p2.second;
	auto &out_v = out_data[ds];

	out_v.resize( v.size() );
	std::transform(out_v.begin(), out_v.end(), v.begin(), out_v.begin(), std::plus<float>());
      }
    }
  }

  if (out_data.size() == 0) 
  { 
    std::cout << "Data for state " << req_state << ", region " << req_region << " not found!" << std::endl;
  }

  return out_data;
}


/********************/
/* Corona virus fit */
/********************/
TCanvas* corona_fit(std::string csv_file_name = "full_data_ita_prov.csv",
                    std::string format_name = "PC province",
                    std::string country = "ITA",
                    std::string region = "",
                    std::string dataset_name = "totale_casi",
		    std::string fit_model_name = "",
		    float fit_from_day = 0.0, 
		    float fit_to_day = -1.0,
		    int days_to_pred = 3,
                    bool y_in_log = false)
{ 
  // Sanity checks
  if (fit_from_day < 0.0 || 
      (fit_to_day != -1.0 && fit_from_day >= fit_to_day)) 
  {
    std::cout << "Wrong fit range [" << fit_from_day << ", " << fit_to_day << "]!" << std::endl;
    return nullptr;
  }

  if (days_to_pred < 0)
  {
    std::cout << "Number of days to predict are negative!" << std::endl;
    days_to_pred = 0;
  }

  if (!fit_model_name.empty() &&
      fit_model_name != "expo" &&
      fit_model_name != "test")
  {
    std::cout << "Fit model " << fit_model_name << " not known!" << std::endl;
    return nullptr;
  }

  // Set ROOT style
  gStyle->SetTitleFontSize(0.06);
  gStyle->SetTitleW(1);
  gStyle->SetOptFit(111);

  // Get data from csv 
  auto dataset = get_data_from_csv(csv_file_name, format_name, country, region); 
  if (!dataset.count(dataset_name))
  {
    std::cout << dataset_name << " dataset not found!" << std::endl;
    std::cout << "List of available datasets of format " << format_name << ":" << std::endl;
    for (const auto &p : dataset) std::cout << p.first << std::endl;
    return nullptr;
  }

  // Set data
  std::vector<float> &data = dataset[dataset_name]; 
  data.erase( std::remove(data.begin(), data.end(), 0.0), data.end() ); // strip days w/o counts

  std::vector<float> days( data.size() ); 
  std::iota(days.begin(), days.end(), 0); // from day 0

  if (data.size() == 0) 
  {
    std::cout << "Data size is zero!" << std::endl;
    return nullptr;
  }

  // Set errors
  std::vector<float> e_days(days.size(), 0.0); 
  std::vector<float> e_data(data.size(), 0.0);
  if (!fit_model_name.empty())
  {
    std::transform(data.begin(), data.end(), e_data.begin(), [] (float N) { return sqrt(N); });
  }

  std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
  std::cout <<  " " << dataset_name << " for: " << country << ", region: " << region  << std::endl;
  std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
  for (auto day : days)
  {
    std::cout << "Day = " << day << ", data = " << data[day] << " +- " << e_data[day] << std::endl;
  }

  // Make output file
  auto out_file = new TFile("corona_trend.root", "recreate");
  
  // Make graph
  auto gr_data = new TGraphErrors (days.size(), days.data(), data.data(), e_days.data(), e_data.data());
  gr_data->SetName("gr_data");
  gr_data->SetTitle( Form("Corona virus trend: %s %s;Days;%s", country.c_str(), region.c_str(), dataset_name.c_str()) );
  gr_data->GetXaxis()->SetTitleSize(0.045);
  gr_data->GetXaxis()->SetTitleOffset(0.8);
  gr_data->GetYaxis()->SetTitleSize(0.045);
  gr_data->GetYaxis()->SetTitleOffset(1.7);
  gr_data->SetMarkerStyle(21);
  gr_data->SetMarkerSize(2);
  gr_data->SetMarkerColor(kRed);
  gr_data->SetLineColor(kRed);
  gr_data->SetLineWidth(4);
  if (y_in_log) 
  { 
    gr_data->SetMinimum(1.0); 
    gr_data->SetMaximum(10 * data.back()); 
  }
  else gr_data->SetMinimum(0.0);

  // Draw data
  auto canv = new TCanvas("canv", "", 1000, 1000);
  canv->GetPad(0)->SetLeftMargin(0.16);
  canv->GetPad(0)->SetRightMargin(0.06);
  canv->SetLogy(y_in_log);
  canv->SetGridx();
  canv->SetGridy();
  gr_data->Draw("apl");

  // Exit if fit model not specified
  if (fit_model_name.empty()) return canv;

  // Define fit functions 
  // 1) exponential
  auto my_expo_fun = [] (double *x, double *p)
  {
    double norm    = p[0];
    double db_rate = p[1];
    double R0 = log(2) / db_rate;

    return exp(norm + R0 * x[0]);
  };

  auto expo_fun = new TF1("expo_fun", my_expo_fun, 0., 1e3, 2); 
  expo_fun->SetNpx(1e3);
  expo_fun->SetLineColor(kBlue);
  expo_fun->SetLineStyle(kDashed);
  expo_fun->SetLineWidth(4);
  expo_fun->SetParName(0, "Norm");
  expo_fun->SetParName(1, "Doubling rate");
  expo_fun->SetParameters(1.0, 1.0);

  // 2) test 
  auto my_test_fun = [] (double *x, double *p) 
  {
    double norm     = p[0];
    double db_rate  = p[1];
    double mu       = p[2];
    double sigma    = p[3];
    double R0 = log(2) / db_rate;

    //if (1. / (1. + exp(-mu / sigma)) < 0.98) return 0.0;
    //if (mu / sigma < 5.0) return 0.0; // max. population @x=0 (Fermi plateau)
    return exp(norm + R0 * (x[0] - sigma * log(exp(mu/sigma) + exp(x[0]/sigma))));
  };

  auto test_fun = new TF1("test_fun", my_test_fun, 0, 1e3, 4); // test
  test_fun->SetNpx(1e3);
  test_fun->SetLineColor(kBlue);
  test_fun->SetLineStyle(kDashed);
  test_fun->SetLineWidth(4);
  test_fun->SetParName(0, "Norm");
  test_fun->SetParName(1, "Doubling rate");
  test_fun->SetParName(2, "Half-max population");
  test_fun->SetParName(3, "#sigma population");
  test_fun->SetParLimits(0,  1.0, 1e3);
  test_fun->SetParLimits(1,  1.0, 10.0);
  test_fun->SetParLimits(2., 10.0, 1e3);
  test_fun->SetParLimits(3., 0.1, 50.0);
  test_fun->SetParameters(10.0, 2.0, 10.0, 5.0);
  //test_fun->FixParameter(1, 3.2); // from fit to Bergamo [6-14] (pure expo.)
  //test_fun->FixParameter(1, 1.9); // from fit to Brescia [6-14] (pure expo.)
  //test_fun->FixParameter(2, 37.);
  //test_fun->FixParameter(3, 5);

  // Select fit function
  TF1* fit_fun = nullptr;
  if (fit_model_name == "expo") fit_fun = expo_fun; 
  if (fit_model_name == "test") fit_fun = test_fun; 

  fit_fun->SetNpx(1e3);
  fit_fun->SetLineColor(kBlue);
  fit_fun->SetLineStyle(kSolid);
  fit_fun->SetLineWidth(4);

  // Estimate initial parameters
  if (fit_model_name == "test")  
  {
    std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
    std::cout << " Estimate R0 first via an expo fit...                               " << std::endl; 
    std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;

    gr_data->Fit(expo_fun, "0N", "", fit_from_day, fit_from_day + 10);
    auto R0_0 = expo_fun->GetParameter(1);
    test_fun->SetParLimits(1,  0.7 * R0_0, 1.3 * R0_0);
    test_fun->SetParameter(1,  R0_0);
  }

  // Set fit range
  if (fit_from_day == 0.0) fit_from_day = days.front();
  if (fit_to_day == -1.0)  fit_to_day = days.back();
  if (fit_from_day > days.back() || fit_to_day < days.front())
  {
    std::cout << "No data in fit range! Cannot fit!" << std::endl;
    return nullptr;
  }

  // Fit
  std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
  std::cout << " Fit with " << fit_model_name << " in range [" << fit_from_day << ", " << fit_to_day << "]" << std::endl;
  std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
  gr_data->Fit(fit_fun, "EMS", "", fit_from_day, fit_to_day);
  gPad->Modified();
  gPad->Update();

  // Change stats pos.
  auto st = (TPaveStats*)gr_data->GetListOfFunctions()->FindObject("stats");
  st->SetX1NDC(0.2);
  st->SetY1NDC(0.66);
  st->SetX2NDC(0.54);
  st->SetY2NDC(0.88);

  // Draw predicted data
  auto draw_fit_point = [&] (const float day, const bool print_text = true) 
  {
    float fit_val = fit_fun->Eval(day);

    auto point = new TMarker(day, fit_val, 21);
    point->SetMarkerSize(2);
    point->SetMarkerColor(kOrange);
    gr_data->GetXaxis()->SetLimits(0.0, 1.2*std::max(days.back(), day));
    if (y_in_log) gr_data->SetMaximum(10 * fit_val);
    else gr_data->SetMaximum(1.2 * fit_val);
    point->Draw("same");

    if (print_text)
    {
      auto txt_str = (fit_val < 1e5) ? Form("%1.0f", fit_val) : Form("%1.2e", fit_val);
      float txt_x_pos = day;
      if (fit_val < 1e3) txt_x_pos -= 1;
      else if (fit_val < 1e4) txt_x_pos -= 3;
      else if (fit_val < 1e5) txt_x_pos -= 4;
      else txt_x_pos -= 5; 

      auto text = new TText(txt_x_pos, fit_val, txt_str);
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

  // Draw test fun components
  if (fit_model_name == "test")
  {
    expo_fun->Draw("same"); // expo. from init. estimation

    if (!y_in_log)
    {
      auto my_fermi_fun = [] (double *x, double *p)
      {
	double norm  = p[0];
	double mu    = p[1];
	double sigma = p[2];

	return norm / (1. + exp((x[0] - mu) / sigma));
      };

      auto fermi_fun = new TF1("fermi_fun", my_fermi_fun, 0., 1e3, 3); 
      fermi_fun->SetNpx(1e3);
      fermi_fun->SetLineColor(kGreen + 3);
      fermi_fun->SetLineStyle(kDashed);
      fermi_fun->SetLineWidth(4);
      fermi_fun->FixParameter(0, 0.3 * data.back()); // scale
      fermi_fun->FixParameter(1, test_fun->GetParameter(2));
      fermi_fun->FixParameter(2, test_fun->GetParameter(3));
      fermi_fun->Draw("same");
    }
  }

  // Draw fit function derivative
  if (!y_in_log)
  {
    auto gr_deriv = (TGraph*)fit_fun->DrawDerivative("goff");
    for (int i = 0; i < gr_deriv->GetN(); i++) // scale
    {
      auto x = gr_deriv->GetX();
      auto y = gr_deriv->GetY();
      gr_deriv->SetPoint(i, x[i], 5 * y[i]);
    }
    gr_deriv->SetLineColor(kOrange);
    gr_deriv->SetLineStyle(kDashed);
    gr_deriv->SetLineWidth(4);
    gr_deriv->Draw("same");
  }

  // Draw error bands
  if (!y_in_log)
  {
    auto fun_low = new TF1(*fit_fun);
    fun_low->SetLineStyle(kDotted);
    fun_low->SetLineColor(kRed);

    auto fun_up = new TF1(*fit_fun);
    fun_up->SetLineStyle(kDotted);
    fun_up->SetLineColor(kRed);

    for (int i = 0; i < fit_fun->GetNpar(); i++)
    {
      fun_low->FixParameter(i, fit_fun->GetParameter(i)  + fit_fun->GetParError(i));
      fun_up->FixParameter (i, fit_fun->GetParameter(i)  - fit_fun->GetParError(i));
    }

    fun_low->DrawF1(gr_data->GetXaxis()->GetXmin(), gr_data->GetXaxis()->GetXmax(), "same");
    fun_up->DrawF1 (gr_data->GetXaxis()->GetXmin(), gr_data->GetXaxis()->GetXmax(), "same");
  }

  // Write output file
  canv->Write();
  gr_data->Write();
  out_file->Write();

  return canv;
};
  
