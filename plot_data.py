import sys
import numpy as np
from ROOT import *

# Import Corona module
gROOT.LoadMacro("Corona.C")
from ROOT import Corona

# Batch mode
gROOT.SetBatch(True)

# Analyzer
ana = Corona.Analyzer()
ana.read_dataset_from_csv("full_data_ita_reg.csv", "PC regioni")
ana.read_dataset_from_csv("full_data_ita_prov.csv", "PC province")
ana.read_dataset_from_csv("full_data_ecdc.csv", "ecdc")

# Corona data plot function
def plot_corona_data(name,
                     data_settings,
                     loc_settings,
                     log_scale = False):

    for data_set in data_settings:
        (data_name) = data_set
 
        canv = ana.get_canvas(data_name);
        canv.SetLogy(log_scale);

        pdf_name = name + "_" + data_name + ".pdf"
        canv.SaveAs(pdf_name + "[")

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
                canv.BuildLegend()
                canv.SaveAs(pdf_name)

        canv.SaveAs(pdf_name + "]")

# Plot province
loc_settings = [("ITA", "Bergamo", kRed      ), 
                ("ITA", "Brescia", kBlue     ), 
                ("ITA", "Pisa",    kGreen + 2), 
                ("ITA", "Milano",  kOrange   ), 
                ("ITA", "Torino",  kViolet   ), 
                ("ITA", "Padova",  kMagenta  ), 
                ("ITA", "Verona",  kGray     )]

data_settings = [("totale_casi")]

plot_corona_data("province", data_settings, loc_settings)

# Plot regioni
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
                 ("variazione_totale_positivi"),
                 ("nuovi_positivi"            ),
                 ("isolamento_domiciliare"    ),
                 ("tamponi_positivi"          ),
                 ("tamponi"                   )]

# Add custom data
for loc_set in loc_settings:
    (sta, reg, _) = loc_set

    pos  = np.array( ana.get_data("totale_positivi", sta, reg) )
    tamp = np.array( ana.get_data("tamponi", sta, reg) ) + 0.1
    tamp_pos = pos / tamp
    ana.add_to_dataset(sta, reg, "tamponi_positivi", tamp_pos)

plot_corona_data("regioni", data_settings, loc_settings, True)

# Plot stati
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
                ("ITA", "Sardegna")]        

data_settings = [("totale_casi",                kRed), 
                 ("deceduti",                   kBlue),
                 ("terapia_intensiva",          kGreen + 2), 
                 ("totale_positivi",            kOrange), 
                 ("totale_ospedalizzati",       kViolet), 
                 ("variazione_totale_positivi", kMagenta),
                 ("isolamento_domiciliare",     kCyan),
                 ("nuovi_positivi",             kGray)]

for loc_set in loc_settings:
    (sta, reg) = loc_set

    c = ana.get_canvas("summary_" + reg)
    c.SetLogy(True);
    pdf_name = "summary_" + reg + ".pdf"

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


# Cumulative plot (sum over ITA regions)    
ana = Corona.Analyzer()
ana.read_dataset_from_csv("full_data_ita_reg.csv", "PC regioni")
c = ana.get_canvas("summary_ITA")

data = ana.get_data("totale_casi", "ITA")
gr = ana.get_graph("totale_casi", data)
gr.Draw("APL")
c.SaveAs("totale_casi_ITA.pdf")

data = ana.get_data("terapia_intensiva", "ITA")
gr = ana.get_graph("terapia_intensiva", data)
gr.Draw("APL")
c.SaveAs("terapia_intensiva_ITA.pdf")

data = ana.get_data("totale_ospedalizzati", "ITA")
gr = ana.get_graph("totale_ospedalizzati", data)
gr.Draw("APL")
c.SaveAs("totale_ospedalizzati_ITA.pdf")

data = ana.get_data("deceduti", "ITA")
gr = ana.get_graph("deceduti", data)
gr.Draw("APL")
c.SaveAs("totale_deceduti_ITA.pdf")
