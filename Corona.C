namespace Corona
{
  using Data_t = std::vector<float>; 
  using Dataset_t = std::map<std::string, Data_t>; 

  /***********************/
  /* Functions namespace */
  /***********************/
  namespace Functions
  {
    enum class Type
    {
      null,
      expo,
      test, 
      fermi
    };

    std::ostream& operator<<(std::ostream& os, const Type t)
    {
      std::string str;
      if (t == Type::null) str = "null";
      if (t == Type::expo) str = "expo";
      if (t == Type::test) str = "test";
      if (t == Type::fermi) str = "fermi";
      os << str;
      return os;
    }

    TF1* null() { return nullptr; }
    TF1* expo (const std::string name = "");
    TF1* test (const std::string name = "");
    TF1* fermi(const std::string name = "");
    TF1* fun(const Type t, const std::string name = "") 
    {
      if      (t == Type::null)  return null();
      else if (t == Type::fermi) return fermi(name);
      else if (t == Type::expo)  return expo(name);
      else if (t == Type::test)  return test(name);
      else 
      {
	std::cout << "No function " << name << " known!" << std::endl;
	return nullptr;
      }
    }
  };

  /************************************/
  /* Corona virus data analyzer class */
  /************************************/
  class Analyzer
  {
    public:
      Analyzer() {};
      Analyzer(const std::string csv_file_name,
  	       const std::string format_name)
      {
	read_dataset_from_csv(csv_file_name, format_name);
      }

      void read_dataset_from_csv(const std::string csv_file_name, 
	                         const std::string format_name);

      Dataset_t get_dataset(const std::string state, 
	                    const std::string region = "");

      Data_t get_data(const std::string data_name,
	              const std::string state, 
	              const std::string region = "");

      TCanvas* get_canvas(const std::string name);

      TGraphErrors* get_graph(const std::string data_name,
	                      const Data_t &data, 
                              Data_t e_data = {});

      TGraphErrors* get_graph(const std::string data_name,
			      const std::string state, 
	                      const std::string region = "");

      TH1F* get_histo(const std::string data_name,
                      const Data_t &data,
		      const int num_bins,
		      const float x_min,  
	              const float x_max);

      void fit(TGraphErrors *gr,
	       Functions::Type type = Functions::Type::test,
	       float fit_from_day = 0.0, 
	       float fit_to_day = -1.0,
	       int days_to_pred = 3,
	       bool draw_extra_info = true);
 
      void fit(const std::string data_name,
               const std::string state,       	       
               const std::string region,
               Functions::Type type = Functions::Type::test,
               float fit_from_day = 0.0, 
	       float fit_to_day = -1.0,
	       int days_to_pred = 3,
	       bool draw_extra_info = true);
     
    private:
      std::map<std::string, std::map<std::string, Dataset_t>> m_dataset{};
  };

  /**********************/
  /* Read data from csv */
  /**********************/
  void Analyzer::read_dataset_from_csv(const std::string csv_file_name, 
                                       const std::string format_name)
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
    auto to_digit = [] (const std::string str)
    {
      bool is_digit = str.empty() ? false : std::all_of(str.begin(), str.end(), ::isdigit);
      return is_digit ? std::stof(str) : 0.0;
    };

    std::cout << std::endl;
    std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
    std::cout << " Read CSV data from file: " << csv_file_name                          << std::endl;
    std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;

    // Sanity checks
    if (format_name != "ecdc" &&
	format_name != "PC regioni" &&
	format_name != "PC province")
    {
      std::cout << "No csv format " << format_name << " known!" << std::endl;
      return;
    }

    // Open csv file
    std::ifstream file(csv_file_name, std::ios::in);
    if (!file.good()) 
    {
      std::cout << "Cannot find csv file: " << csv_file_name << std::endl;
      return; 
    }

    // Get csv fields
    std::string line;
    std::getline(file, line); // first line
    const auto &csv_fields = tokenize(line, ',');
    const auto num_csv_fields = csv_fields.size();

    // Parse csv file
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
	  m_dataset[state][region][csv_fields[i]].push_back( to_digit(tokens[i]) );
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
	  m_dataset[state][region][csv_fields[i]].push_back( to_digit(tokens[i]) );
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
	  m_dataset[state][region][csv_fields[i]].push_back( to_digit(tokens[i]) );
	}
      }
    }

    file.close();
  }

  /**********************************/
  /* Get dataset of (state, region) */
  /**********************************/
  Dataset_t Analyzer::get_dataset(const std::string state, 
                                  const std::string region)
  {
    std::cout << std::endl;
    std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
    std::cout << " Get dataset of " << state << ", " << region                          << std::endl; 
    std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;

    // Get dataset
    Dataset_t dataset;

    if (!m_dataset.count(state))
    {
      std::cout << state << " not in dataset!" << std::endl;
      return {};
    }

    if (region.empty()) // no region requested... sum data over regions
    {
      std::cout << "No region requested. Sum data over regions..." << std::endl; 

      for (const auto &p : m_dataset[state])
      {
	for (const auto &p2 : p.second)
	{
	  const auto &ds = p2.first;
	  const auto &v = p2.second;
	  auto &out_v = dataset[ds];

	  out_v.resize( v.size() );
	  std::transform(out_v.begin(), out_v.end(), v.begin(), out_v.begin(), std::plus<float>());
	}
      }
    }

    else if (!m_dataset[state].count(region))
    {
      std::cout << region << " not in dataset!" << std::endl;
    }

    else
    { 
      dataset = m_dataset[state][region];
    }

    std::cout << "List of datasets of " << state << ", " << region << ":" << std::endl;
    std::cout << "[";
    for (const auto &p : dataset) std::cout << p.first << ", ";
    std::cout << "]" << std::endl;

    return dataset;
  }

  /*******************************/
  /* Get data of (state, region) */
  /*******************************/
  Data_t Analyzer::get_data(const std::string data_name,
                            const std::string state, 
                            const std::string region)
  {
    std::cout << std::endl;
    std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
    std::cout << " Get data " << data_name << " of "  << state << ", " << region        << std::endl; 
    std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;

    auto ds = get_dataset(state, region);
    if (!ds.count(data_name))
    {
      std::cout << data_name << " dataset not found!" << std::endl;
      return {};
    }

    std::cout << std::endl;
    std::cout << data_name << " of " << state << ", " << region << ":" << std::endl;
    std::cout << "[";
    for (const auto &val : ds[data_name]) std::cout << val << ", ";
    std::cout << "]" << std::endl;

    return ds[data_name];
  }

  /**********************/
  /* Get default canvas */
  /**********************/
  TCanvas* Analyzer::get_canvas(const std::string name)
  {
    auto c = new TCanvas(Form("canv_%s", name.c_str()), "", 1000, 1000);
    c->GetPad(0)->SetLeftMargin(0.16);
    c->GetPad(0)->SetRightMargin(0.06);
    c->SetGridx();
    c->SetGridy();

    return c;
  }

  /******************/
  /* Get data graph */
  /******************/
  TGraphErrors* Analyzer::get_graph(const std::string data_name,
                                    const Data_t &data, 
                                    Data_t e_data)
  { 
    std::cout << std::endl;
    std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
    std::cout << " Get graph: " << data_name                                            << std::endl; 
    std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;

    // Sanity check
    if (data.size() == 0)
    {
      std::cout << "Data size is zero!" << std::endl;
      return nullptr;
    }
    
    if (e_data.size() > 0 && e_data.size() != data.size())
    {
      std::cout << "Error size differs from  data size!" << std::endl;
      return nullptr;
    }

    // Set ROOT style
    gStyle->SetTitleFontSize(0.06);
    gStyle->SetTitleW(1);
    gStyle->SetOptFit(111);

    // Set data
    Data_t days(data.size()); 
    Data_t e_days(days.size(), 0.0); 
    std::iota(days.begin(), days.end(), 0); // from day 0

    std::cout << "Data on graph:" << std::endl;
    std::cout << "[";
    for (const auto &val : data) std::cout << val << ", ";
    std::cout << "]" << std::endl;

    // Make graph
    auto gr = new TGraphErrors (days.size(), days.data(), data.data(), e_days.data(), e_data.data());
    gr->SetName(Form("gr_%s", data_name.c_str()));
    
    gr->GetXaxis()->SetTitle("Days");
    gr->GetXaxis()->SetTitleSize(0.045);
    gr->GetXaxis()->SetTitleOffset(0.8);
    
    gr->GetYaxis()->SetTitle(data_name.c_str());
    gr->GetYaxis()->SetTitleSize(0.045);
    gr->GetYaxis()->SetTitleOffset(1.7);
    
    gr->SetMarkerStyle(21);
    gr->SetMarkerSize(1);
    gr->SetMarkerColor(kRed);
    gr->SetLineColor(kRed);
    gr->SetLineWidth(4);
    gr->SetLineWidth(4);
    
    float max = *std::max_element(data.begin(), data.end());
    gr->SetMaximum(1.1 * max);
    gr->SetMinimum(0.0);

    return gr;
  };

  TGraphErrors* Analyzer::get_graph(const std::string data_name,
                                    const std::string state, 
                                    const std::string region)
  {
    auto data = get_data(data_name, state, region);
    Data_t e_data(data.size());
    std::transform(data.begin(), data.end(), e_data.begin(), [] (float N) { return sqrt(N); }); 

    return get_graph(data_name, data, e_data);
  }

  /*****************/
  /* Get histogram */
  /*****************/
  TH1F* Analyzer::get_histo(const std::string data_name,
                            const Data_t &data,
			    const int num_bins,
			    const float x_min,  
			    const float x_max)  
  {
    std::cout << std::endl;
    std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
    std::cout << " Get histogram: " << data_name                                        << std::endl; 
    std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;

    // Sanity checks
    if (data.size() == 0)
    {
      std::cout << "Data size is zero!" << std::endl;
      return nullptr;
    }

    if (num_bins <= 0 || x_max <= x_min)
    {
      std::cout << "Wrong histogram binning!" << std::endl;
      return nullptr;
    }
    
    // Set ROOT style
    gStyle->SetTitleFontSize(0.06);
    gStyle->SetTitleW(1);
    gStyle->SetOptFit(111);

    std::cout << "Data on histogram:" << std::endl;
    std::cout << "[";
    for (const auto &val : data) std::cout << val << ", ";
    std::cout << "]" << std::endl;

    // Make histogram
    auto h = new TH1F(Form("h_%s", data_name.c_str()), "", num_bins, x_min, x_max);
    h->SetTitle(Form(";%s;counts", data_name.c_str()));

    h->GetXaxis()->SetTitleSize(0.045);
    h->GetXaxis()->SetTitleOffset(0.8);

    h->GetYaxis()->SetTitleSize(0.045);
    h->GetYaxis()->SetTitleOffset(1.1);

    h->SetLineWidth(2);
    h->SetLineColor(kRed);
    h->SetMarkerColor(kRed);

    for (const auto &val : data) h->Fill(val);

    return h;
  }

  /*************/
  /* Fit graph */
  /*************/
  void Analyzer::fit(TGraphErrors *gr,
                     Functions::Type fun_type,
                     float fit_from_day, 
                     float fit_to_day,
                     int days_to_pred,
                     bool draw_extra_info)
  {
    std::cout << std::endl;
    std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
    std::cout << " Fit graph: " << gr->GetName()                                        << std::endl;
    std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;

    // Sanity checks
    if (gr == nullptr) 
    {
      std::cout << "Graph is nullptr!" << std::endl;
      return;
    }

    if (gr->GetN() == 0) 
    {
      std::cout << "Graph is empty!" << std::endl;
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

    // Draw data graph
    gr->Draw("apl");

    // Select fit function
    TF1* fit_fun = Functions::fun(fun_type, "fit");
    fit_fun->SetNpx(1e3);
    fit_fun->SetLineColor(kBlue);
    fit_fun->SetLineStyle(kSolid);
    fit_fun->SetLineWidth(4);

    // Estimate initial parameters
    auto expo_fun_0 = Functions::expo("init");

    if (fun_type == Functions::Type::test)  
    {
      std::cout << "Estimate R0 first via an expo fit..." << std::endl; 
      gr->Fit(expo_fun_0, "0N", "", fit_from_day, fit_from_day + 15);

      auto R0_0 = expo_fun_0->GetParameter(1);
      fit_fun->SetParLimits(1, 0.5 * R0_0, 1.5 * R0_0);
      fit_fun->SetParameter(1, R0_0);
    }
    std::cout << std::endl;

    // Set fit range
    const float last_day = gr->GetN();
    if (fit_from_day == 0.0) fit_from_day = 0;
    if (fit_to_day == -1.0)  fit_to_day = last_day;
    if (fit_from_day > last_day || fit_to_day < 0)
    {
      std::cout << "No data in fit range! Cannot fit!" << std::endl;
      return;
    }

    // Fit
    std::cout << "Fit with " << fun_type << " in [" << fit_from_day << ", " << fit_to_day << "]" << std::endl;
    gr->Fit(fit_fun, "EMS", "", fit_from_day, fit_to_day);
    gPad->Modified();
    gPad->Update();

    // Change stats pos.
    auto st = (TPaveStats*)gr->GetListOfFunctions()->FindObject("stats");
    st->SetX1( 2.0 );
    st->SetY1( 0.7 * gr->GetMaximum() );
    st->SetX2( 20.0 );
    st->SetY2( 0.98 * gr->GetMaximum() );

    // Draw predicted data
    auto draw_fit_point = [&] (const float day, const bool print_text = true) 
    {
      float fit_val = fit_fun->Eval(day);

      auto point = new TMarker(day, fit_val, 21);
      point->SetMarkerSize(2);
      point->SetMarkerColor(kOrange);
      gr->GetXaxis()->SetLimits(0.0, 1.2*std::max(last_day, day));
      gr->SetMaximum(1.2 * fit_val);
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

    int pred_from_day = std::min(fit_to_day, last_day) + 1;  
    int pred_to_day = pred_from_day + days_to_pred - 1;
    for (int day = pred_from_day; day <= pred_to_day; day++)
    {
      bool print_text = days_to_pred < 5 ? true : ((day == pred_to_day) ? true : false); 
      draw_fit_point(day, print_text); 
    }

    // Draw extra infos
    if (draw_extra_info)
    {
      // Draw components
      if (fun_type == Functions::Type::test)
      {
	expo_fun_0->SetParameter(1, fit_fun->GetParameter(1));
	expo_fun_0->Draw("same"); // expo. from init. estimation

	auto fermi_fun = Functions::fermi();
	fermi_fun->FixParameter(0, 0.3 * gr->GetMaximum()); // scale
	fermi_fun->FixParameter(1, fit_fun->GetParameter(2));
	fermi_fun->FixParameter(2, fit_fun->GetParameter(3));
	fermi_fun->Draw("same");
      }

      // Draw fit function derivative
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

      // Draw error bands
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

      fun_low->DrawF1(gr->GetXaxis()->GetXmin(), gr->GetXaxis()->GetXmax(), "same");
      fun_up->DrawF1 (gr->GetXaxis()->GetXmin(), gr->GetXaxis()->GetXmax(), "same");
    }
  }

  void Analyzer::fit(const std::string data_name,
                     const std::string state,
                     const std::string region,
                     Functions::Type fun_type,
                     float fit_from_day, 
                     float fit_to_day,
                     int days_to_pred,
                     bool draw_extra_info)
  {
    auto gr = get_graph(data_name, state, region);
    fit(gr, fun_type, fit_from_day, fit_to_day, days_to_pred, draw_extra_info);
  }

  /********************/
  /* Define functions */
  /********************/
  /* Exponential function */
  TF1* Functions::expo(const std::string name)
  {
    auto expo_fun = [] (double *x, double *p)
    {
      double norm    = p[0];
      double db_rate = p[1];
      double R0 = log(2) / db_rate;

      return exp(norm + R0 * x[0]);
    };

    auto fun = new TF1(Form("expo_%s", name.c_str()), expo_fun, 0., 1e3, 2); 
    fun->SetNpx(1e3);
    fun->SetLineColor(kBlue);
    fun->SetLineStyle(kDashed);
    fun->SetLineWidth(4);
    fun->SetParName(0, "Norm");
    fun->SetParName(1, "Doubling rate");
    fun->SetParameters(1.0, 1.0);

    return fun;
  }

  /* Test function */
  TF1* Functions::test(const std::string name)
  {
    auto test_fun = [] (double *x, double *p) 
    {
      double norm     = p[0];
      double db_rate  = p[1];
      double mu       = p[2];
      double sigma    = p[3];
      double R0 = log(2) / db_rate;

      //if (1. / (1. + exp(-mu / sigma)) < 0.8) return 0.0;
      //if (mu / sigma < 5.0) return 0.0; // max. population @x=0 (Fermi plateau)
      return exp(norm + R0 * (x[0] - sigma * log(exp(mu/sigma) + exp(x[0]/sigma))));
    };

    auto fun = new TF1(Form("test_%s", name.c_str()), test_fun, 0, 1e3, 4); // test
    fun->SetNpx(1e3);
    fun->SetLineColor(kBlue);
    fun->SetLineStyle(kDashed);
    fun->SetLineWidth(4);
    fun->SetParName(0, "Norm");
    fun->SetParName(1, "Doubling rate");
    fun->SetParName(2, "Half-max population");
    fun->SetParName(3, "#sigma population");
    fun->SetParLimits(0,  1.0, 1e3);
    fun->SetParLimits(1,  1.0, 10.0);
    fun->SetParLimits(2., 10.0, 1e3);
    fun->SetParLimits(3., 1.0, 10.0);
    fun->SetParameters(10.0, 2.0, 10.0, 5.0);
    //fun->FixParameter(1, 3.2); // from fit to Bergamo [6-14] (pure expo.)
    //fun->FixParameter(1, 1.9); // from fit to Brescia [6-14] (pure expo.)
    //fun->FixParameter(2, 37.);
    //fun->FixParameter(3, 5);

    return fun;
  }

  /* Fermi function */
  TF1* Functions::fermi(const std::string name)
  {
    auto fermi_fun = [] (double *x, double *p)
    {
      double norm  = p[0];
      double mu    = p[1];
      double sigma = p[2];

      return norm / (1. + exp((x[0] - mu) / sigma));
    };

    auto fun = new TF1(Form("fermi_%s", name.c_str()), fermi_fun, 0., 1e3, 3); 
    fun->SetNpx(1e3);
    fun->SetLineColor(kGreen + 3);
    fun->SetLineStyle(kDashed);
    fun->SetLineWidth(4);

    return fun;
  }

} // namespace Corona
