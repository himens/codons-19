#include "Data_t.h"

namespace Corona
{
  /***********************/
  /* Functions namespace */
  /***********************/
  namespace Functions
  {
    enum class Type
    {
      expo,
      test, 
      fermi
    };

    std::ostream& operator<<(std::ostream& os, const Type t)
    {
      std::string str;
      if (t == Type::expo) str = "expo";
      if (t == Type::test) str = "test";
      if (t == Type::fermi) str = "fermi";
      os << str;
      return os;
    }

    /* Exponential function */
    TF1* expo(const std::string name = "")
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
    TF1* test(const std::string name = "")
    {
      auto test_fun = [] (double *x, double *p) 
      {
	double norm     = p[0];
	double db_rate  = p[1];
	double mu       = p[2];
	double sigma    = p[3];
	double R0 = log(2) / db_rate;

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

      return fun;
    }

    /* Fermi function */
    TF1* fermi(const std::string name = "")
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

    /* Wrapper function */ 
    TF1* fun(const Type t, const std::string name = "") 
    {
           if (t == Type::fermi) return fermi(name);
      else if (t == Type::expo)  return expo(name);
      else if (t == Type::test)  return test(name);
      else 
      {
	std::cout << "No function " << name << " known!" << std::endl;
	return nullptr;
      }
    }
  }; // Functions namespace


  /************************************/
  /* Corona virus data analyzer class */
  /************************************/
  class Analyzer
  {
    public:
      using Dataset_t = std::map<std::string, std::map<std::string, float>>;
 
    public:
      /* Constructors */
      Analyzer() {};
      Analyzer(const std::string csv_file_name,
  	       const std::string format_name)
      {
	std::cout << std::endl;
	std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
	std::cout << " Corona analyzer created!"                                            << std::endl;
	std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
	read_dataset_from_csv(csv_file_name, format_name);
      }

      /* Set debug flag */ 
      void set_debug(const bool flag) { m_debug = flag; };

      /* Read data from csv */
      void read_dataset_from_csv(const std::string csv_file_name, 
	                         const std::string format_name);

      /* Return full dataset */
      const std::map<std::string, std::map<std::string, Dataset_t>>& get_dataset() { return m_dataset; };

      /* Get dataset of (state, region) */
      Dataset_t get_dataset(const std::string state, 
	                    const std::string region = "");

      /* Get data of (state, region) */
      Data_t get_data(const std::string data_name,
	              const std::string state, 
	              const std::string region = "");

      /* Get all states stored in dataset */
      std::vector<std::string> get_states();

      /* Get all regions of a state stored in dataset */
      std::vector<std::string> get_regions(const std::string sta);

      /* Add new data into dataset */
      void add_data(const Data_t &data,
           	    const std::string data_name,
                    const std::string state, 
		    const std::string region);

      /* Make default canvas */
      TCanvas* make_canvas(const std::string name);

      /* Fit graph */
      void fit(TGraphErrors *gr,
	       Functions::Type type = Functions::Type::test,
	       float fit_from_day = 0.0, 
	       float fit_to_day = -1.0,
	       int days_to_pred = 3,
	       bool draw_extra_info = true);

      /* ostream overload */
      friend std::ostream& operator<<(ostream& os, const Dataset_t &dataset);

    private:
      bool m_debug = false; // debug flag
      std::map<std::string, std::map<std::string, Dataset_t>> m_dataset{}; // dataset
  };
  

  /********************/
  /* ostream overload */
  /********************/
  std::ostream& operator<<(ostream& os, const Analyzer::Dataset_t &dataset)
  {
    os << "{";
    for (const auto &p : dataset) 
    {
      os << "\n  " << p.first << ": \n";

      for (const auto &p2 : p.second)
	os << "  " << p2.first << ": " << p2.second << "\n";
    }
    os << "}\n";
    return os;
  }

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

      // Data format
      // ECDC
      if (format_name == "ecdc")
      {
	std::string day    = tokens[0];
	std::string state  = tokens[1];
	std::string region = "N/A";
	for (size_t i = 2; i < csv_fields.size(); i++) 
	{
	  m_dataset[state][region][csv_fields[i]][day] = to_digit(tokens[i]);
	}
      }

      // Protezione Civile (regioni)
      if (format_name == "PC regioni")
      {
	std::string day    = tokens[0];
	std::string state  = tokens[1];
	std::string region = tokens[3];
	for (size_t i = 4; i < csv_fields.size(); i++) 
	{
	  if (csv_fields[i] == "note") continue;
	  m_dataset[state][region][csv_fields[i]][day] = to_digit(tokens[i]);
	}
      }

      // Protezione Civile (province)
      if (format_name == "PC province")
      {
	std::string day    = tokens[0];
	std::string state  = tokens[1];
	std::string region = tokens[5];
	for (size_t i = 6; i < csv_fields.size(); i++) 
	{
	  if (csv_fields[i] == "note") continue;
	  m_dataset[state][region][csv_fields[i]][day] = to_digit(tokens[i]);
	}
      }
    }

    file.close();
  }

  /**********************************/
  /* Get dataset of (state, region) */
  /**********************************/
  Analyzer::Dataset_t Analyzer::get_dataset(const std::string state, 
                                            const std::string region)
  {
    // Get dataset
    Dataset_t dataset;

    if (!m_dataset.count(state))
    {
      std::cout << state << " not in dataset!" << std::endl;
      return {};
    }

    if (region.empty()) // no region requested... sum data over regions
    {
      for (const auto &p : m_dataset[state])
      {
	for (const auto &p2 : p.second)
	{
	  for (const auto &p3 : p2.second)
	  {
	    dataset[p2.first][p3.first] += p3.second;
	  }
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

    if (m_debug) 
    {
      std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
      std::cout << " Get dataset of " << state << ", " << region                          << std::endl; 
      std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
      std::cout << "Datasets of " << state << ", " << region << ":" << std::endl;
      std::cout << dataset << std::endl;
    }

    return dataset;
  }

  /*******************************/
  /* Get data of (state, region) */
  /*******************************/
  Data_t Analyzer::get_data(const std::string data_name,
                            const std::string state, 
                            const std::string region)
  {
    auto ds = get_dataset(state, region);
    if (!ds.count(data_name))
    {
      std::cout << data_name << " dataset not found!" << std::endl;
      return {};
    }

    Data_t data;
    for (const auto &p : ds[data_name]) data.push_back(p.second); 

    if (m_debug)
    {
      std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
      std::cout << " Get data " << data_name << " of "  << state << ", " << region        << std::endl; 
      std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
      std::cout << data_name << " of " << state << ", " << region << ":" << std::endl;
      std::cout << data << std::endl;
    }

    return data;
  }

  /*****************************/
  /* Add new data into dataset */
  /*****************************/
  void Analyzer::add_data(const Data_t &data,
                          const std::string data_name,
                          const std::string state, 
			  const std::string region)
  {
    // sanity check
    if (state.empty() || region.empty()) 
    {
      std::cout << "State or region not specified! Cannot add " << data_name << std::endl;
      return;
    }

    int i = 0;
    for (const auto &p : m_dataset[state][region].begin()->second)
    {
      if (i >= data.size()) break;
      m_dataset[state][region][data_name][p.first] = data[i];
      i++;
    }

    if (m_debug)
    {
      std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
      std::cout << " Add data " << data_name << ", "  << state << ", " << region          << std::endl; 
      std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
      std::cout << data_name << " of " << state << ", " << region << ":" << std::endl;
      std::cout << data << std::endl;
    }
  }

  /************************************/
  /* Get all states stored in dataset */
  /************************************/
  std::vector<std::string> Analyzer::get_states()
  {
    std::vector<std::string> states;
    for (const auto &p : m_dataset) states.push_back(p.first);
    return states;
  }

  /************************************************/
  /* Get all regions of a state stored in dataset */
  /************************************************/
  std::vector<std::string> Analyzer::get_regions(const std::string sta)
  {
    // sanity check 
    if (m_dataset.count(sta) == 0) return {};

    std::vector<std::string> regions;
    for (const auto &p : m_dataset[sta]) regions.push_back(p.first);
    return regions;
  }

  /***********************/
  /* Make default canvas */
  /***********************/
  TCanvas* Analyzer::make_canvas(const std::string name)
  {
    auto c = new TCanvas(Form("canv_%s", name.c_str()), "", 1000, 1000);
    c->GetPad(0)->SetLeftMargin(0.16);
    c->GetPad(0)->SetRightMargin(0.06);
    c->SetGridx();
    c->SetGridy();

    return c;
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
} // namespace Corona
