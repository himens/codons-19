#ifndef DATA_T_H
#define DATA_T_H

/**************/
/* Data class */
/**************/
// Enhance std::vector<float> with other methods
class Data_t : public std::vector<float>
{
  public:
    /* Constructors */
    using std::vector<float>::vector; // use same constr of std::vector

    /* ostream overload */
    friend std::ostream& operator<< (std::ostream& os, const Data_t &data);

    /* / overload */
    Data_t operator/ (const Data_t &data);
 
    /* * overload (right mult. by scalar) */
    Data_t operator* (const float scale); 

    /* * overload (left mult. by scalar) */
    friend Data_t operator* (const float scale, Data_t &data);

    /* Print data */
    void show();

    /* Get data derivative */ 
    Data_t derive();

    /* Put data on ROOT graph */ 
    TGraph* make_graph(const std::string data_name);

    /* Put data on ROOT histogram */
    TH1F* make_histo(const std::string data_name,
	             const int num_bins,
                     const float x_min,  
	             const float x_max);  
};

/********************/
/* ostream overload */
/********************/
std::ostream& operator<< (std::ostream& os, const Data_t &data)
{
  os << "[";
  for (auto it = data.begin(); it != data.end() -1; it++) os << *it << ", ";
  os << data.back() << "]";
  return os;
} 

/**************/
/* / overload */
/**************/
Data_t Data_t::operator/ (const Data_t &data)
{
  if (this->size() != data.size())
  {
    std::cout << "Undefined division (size differs)!" << std::endl; 
    return {};
  }

  Data_t div_data(this->size());
  for (size_t i = 0; i < this->size(); i++)
  {
    div_data[i] = data[i] > 0.0 ? this->at(i) / data[i] : 0.0;
  }

  return div_data;
} 

/**************************************/
/* * overload (right mult. by scalar) */
/**************************************/
Data_t Data_t::operator* (const float scale) 
{
  Data_t scaled_data(this->size());
  for (size_t i = 0; i < this->size(); i++) 
  {
    scaled_data[i] = scale * this->at(i);
  }

  return scaled_data;
};

/*************************************/
/* * overload (left mult. by scalar) */
/*************************************/
Data_t operator* (const float scale, Data_t &data) 
{
  return data * scale;
}

/**************/
/* Print data */
/**************/
void Data_t::show()
{ 
  std::cout << std::endl;
  std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
  std::cout << " Print data"                                                          << std::endl;
  std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
  std::cout << "Data size is: " << this->size() << std::endl;
  std::cout << "Data content:" << std::endl;
  std::cout << *this << std::endl; 
};

/***********************/
/* Get data derivative */ 
/***********************/
Data_t Data_t::derive()
{
  Data_t der(this->size() - 1);
  for (size_t i = 1; i < this->size(); i++)
  {
    der[i-1] = this->at(i) - this->at(i-1);
  }

  return der;
}    

/**************************/
/* Put data on ROOT graph */ 
/**************************/
TGraph* Data_t::make_graph(const std::string data_name)
{ 
  // Sanity check
  if (this->size() == 0)
  {
    std::cout << "Data size is zero!" << std::endl;
    return nullptr;
  }

  // Set ROOT style
  gStyle->SetTitleFontSize(0.06);
  gStyle->SetTitleW(1);
  gStyle->SetOptFit(111);
  gStyle->SetStatX( 0.55 );
  gStyle->SetStatY( 0.87 );

  // Set data on x-axis
  Data_t x_data( this->size() ); 
  std::iota(x_data.begin(), x_data.end(), 0); // from 0

  // Make graph
  auto gr = new TGraph (x_data.size(), x_data.data(), this->data());
  gr->SetName(Form("gr_%s", data_name.c_str()));

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

  float max = *std::max_element(this->begin(), this->end());
  gr->SetMaximum(1.1 * max);
  gr->SetMinimum(0.0);

  return gr;
};

/******************************/
/* Put data on ROOT histogram */
/******************************/
TH1F* Data_t::make_histo(const std::string data_name,
                         const int num_bins,
                         const float x_min,  
                         const float x_max)  
{ 
  // Sanity checks
  if (this->size() == 0)
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
  gStyle->SetStatX( 0.55 );
  gStyle->SetStatY( 0.87 );

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

  for (const auto &val : *this) h->Fill(val);

  return h;
}
#endif
