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
      bool is_digit = !str.empty() && (str.find_first_not_of("0123456789.") == std::string::npos);
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

    // Parse csv file
    while (std::getline(file, line))
    {
      if (line.empty()) continue;  // skip empty line
      if (line.find(",") == std::string::npos) continue; // skip line with no delimiter
      if (line.front() == '#' || line.front() == '"') continue; // skip line with comment

      // Tokenize
      auto tokens = tokenize(line, ',');
      if (tokens.size() > csv_fields.size()) tokens.resize(csv_fields.size()); // too much tokens, resize

      // Data format
      // ECDC
      if (format_name == "ecdc")
      {
	std::string state  = tokens[2];
	std::string day    = tokens[3];
	std::string region = "N/A";
	for (size_t i = 4; i < tokens.size(); i++) 
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
	for (size_t i = 4; i < tokens.size(); i++) 
	{
	  m_dataset[state][region][csv_fields[i]][day] = to_digit(tokens[i]);
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
} // namespace Corona
