import sys
import numpy as np
from ROOT import *

# Import Corona module
gROOT.LoadMacro("Corona.C")
from ROOT import Corona

# Batch mode
gROOT.SetBatch(True)

# Corona data plot function
def plot_corona_data(name,
                     data_settings,
                     loc_settings,
                     log_scale = False):

    canv = ana.get_canvas("canvas_" + name)
    pdf_name = name + ".pdf"
    canv.SaveAs(pdf_name + "[")

    for data_set in data_settings:
        (data_name) = data_set
 
        c = ana.get_canvas(name + "_" + data_name);
        c.SetLogy(log_scale);

        m_gr = TMultiGraph()
        m_gr.SetTitle("Corona trend;Days;" + data_name);
        if log_scale: m_gr.SetMinimum(1);

        for loc_set in loc_settings:
            (sta, reg, color) = loc_set

            data = ana.get_data(data_name, sta, reg)
            e_data = []  

            gr = ana.get_graph(data_name, data, e_data)
            gr.SetTitle(reg if reg != "" else sta)
            gr.SetLineColor(color)
            gr.SetMarkerColor(color)        
            m_gr.Add(gr, "PL")

            if loc_set == loc_settings[-1]: 
                m_gr.Draw("A")
                c.BuildLegend()
                c.SaveAs(pdf_name)

    canv.SaveAs(pdf_name + "]")

# Plot province
ana = Corona.Analyzer("full_data_ita_prov.csv", "PC province")

loc_settings = [("ITA", "Bergamo", kRed      ), 
                ("ITA", "Brescia", kBlue     ), 
                ("ITA", "Pisa",    kGreen + 2), 
                ("ITA", "Milano",  kOrange   ), 
                ("ITA", "Torino",  kViolet   ), 
                ("ITA", "Padova",  kMagenta  ), 
                ("ITA", "Verona",  kGray     )]

data_settings = [("totale_casi")]

plot_corona_data("province_ita", data_settings, loc_settings)

# Plot regioni
ana = Corona.Analyzer("full_data_ita_reg.csv", "PC regioni")

loc_settings = [("ITA", "Lombardia",      kRed       ), 
                ("ITA", "Veneto",         kBlue      ),
                ("ITA", "Emilia-Romagna", kGreen + 2 ),
                ("ITA", "Marche",         kOrange    ),
                ("ITA", "Toscana",        kViolet    ), 
                ("ITA", "Piemonte",       kMagenta   ), 
                ("ITA", "Umbria",         kGray      ),
                ("ITA", "Campania",       kCyan + 3  ),
                ("ITA", "Lazio",          kBlack     ),
                ("ITA", "Liguria",        kYellow + 1),
                ("ITA", "Sardegna",       kBlue +   2)]

data_settings = [("totale_casi"               ), 
                 ("deceduti"                  ),
                 ("terapia_intensiva"         ), 
                 ("totale_positivi"           ), 
                 ("totale_ospedalizzati"      ), 
                 ("isolamento_domiciliare"    ),
                 ("tamponi"                   ),
                 ("ricoverati_con_sintomi"    )]

for loc_set in loc_settings: # add custom data
    (sta, reg, _) = loc_set
    pos  = np.array( ana.get_data("totale_positivi", sta, reg) )
    tamp = np.array( ana.get_data("tamponi", sta, reg) ) + 0.1
    tamp_pos = pos / tamp
    ana.add_to_dataset(sta, reg, "tamponi_positivi", tamp_pos)
data_settings += [("tamponi_positivi")]

plot_corona_data("regioni_ita", data_settings, loc_settings, False)

# Sum over ITA regions
loc_settings = [("ITA", "", kRed)]
data_settings = [("deceduti"            ),
                 ("terapia_intensiva"   ), 
                 ("totale_ospedalizzati"),
                 ("ricoverati_con_sintomi")]

plot_corona_data("totale_ita", data_settings, loc_settings, False)

# Summary plots per region
loc_settings = [("ITA", "Lombardia"),      
                ("ITA", "Veneto"),         
                ("ITA", "Emilia-Romagna"), 
                ("ITA", "Marche"),         
                ("ITA", "Toscana"),        
                ("ITA", "Piemonte"),       
                ("ITA", "Umbria"),         
                ("ITA", "Campania"),      
                ("ITA", "Lazio"),          
                ("ITA", "Liguria"),        
                ("ITA", "Sardegna"),        
                ("ITA", "Sicilia"),       
                ("ITA", "Puglia"),        
                ("ITA", "Calabria"),        
                ("ITA", "Basilicata")]        

data_settings = [("deceduti",                   kBlue),
                 ("terapia_intensiva",          kGreen + 2), 
                 ("totale_ospedalizzati",       kViolet),
                 ("ricoverati_con_sintomi",     kRed)]

c_sum = ana.get_canvas("summary_regioni")
pdf_name = "summary_regioni_ita.pdf"
c_sum.SaveAs(pdf_name + "[")

for loc_set in loc_settings:
    (sta, reg) = loc_set

    c = ana.get_canvas("summary_" + reg)
    c.SetLogy(False);

    m_gr = TMultiGraph()
    m_gr.SetTitle("Summary " + reg + ";Days;Counts");

    for data_set in data_settings:
        (data_name, color) = data_set

        data = ana.get_data(data_name, sta, reg)
        gr = ana.get_graph(data_name, data)
        gr.SetLineColor(color)
        gr.SetMarkerColor(color)        
        gr.SetTitle(data_name)
        m_gr.Add(gr, "PL")

    m_gr.SetMinimum(1)
    m_gr.Draw("A")
    c.BuildLegend()
    c.SaveAs(pdf_name)

c_sum.SaveAs(pdf_name + "]")

# Plot stati
ana = Corona.Analyzer("full_data_ecdc.csv", "ecdc")

loc_settings = [("Italy",          "", kRed       ),
                ("United States",  "", kBlue      ),
                ("France",         "", kGreen + 2 ),
                ("Spain",          "", kOrange    ), 
                ("China",          "", kViolet    ),
                ("United Kingdom", "", kMagenta   ),
                ("Germany",        "", kGray      ),
                ("South Korea",    "", kBlack     ),
                ("Netherlands",    "", kCyan + 3  ),
                ("Russia",         "", kYellow + 1),
                ("Japan",          "", kBlue - 2  ),
                ("Brazil",         "", kRed - 2   ),
                ("India",          "", kYellow - 2)]

data_settings = [("total_cases"),
                 ("total_deaths")]

plot_corona_data("stati", data_settings, loc_settings, True)

