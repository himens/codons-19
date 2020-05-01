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
                     log_scale = False,
                     days_to_pred = 3):

    for data_set in data_settings:
        (data_name, fit_model) = data_set
 
        canv = ana.get_canvas(data_name);
        canv.SetLogy(log_scale);

        pdf_name = name + "_" + data_name + ".pdf"
        canv.SaveAs(pdf_name + "[")

        m_gr = TMultiGraph()
        m_gr.SetTitle("Corona trend;Days;" + data_name);

        for loc_set in loc_settings:
            (sta, reg, color, fit_range) = loc_set

            data = ana.get_data(data_name, sta, reg)
            e_data = [] if fit_model == None else np.sqrt(data) 

            gr = ana.get_graph(data_name, data, e_data)
            gr.SetTitle(reg if reg != "" else sta)
            gr.SetLineColor(color)
            gr.SetMarkerColor(color)        

            if fit_model != None: 
               gr.SetLineColor(kRed)
               gr.SetMarkerColor(kRed)        
               ana.fit(gr,
                       fit_model,
                       fit_range[0], 
                       fit_range[1], 
                       days_to_pred)
               canv.SaveAs(pdf_name)

            else:
               m_gr.Add(gr, "PL")
               if loc_set == loc_settings[-1]: 
                   m_gr.Draw("A")
                   canv.BuildLegend()
                   canv.SaveAs(pdf_name)

        canv.SaveAs(pdf_name + "]")

# Plot province
loc_settings = [("ITA", "Bergamo", kRed,       [0, -1]), 
                ("ITA", "Brescia", kBlue,      [0, -1]), 
                ("ITA", "Pisa",    kGreen + 2, [0, -1]), 
                ("ITA", "Milano",  kOrange,    [0, -1]), 
                ("ITA", "Torino",  kViolet,    [0, -1]), 
                ("ITA", "Padova",  kMagenta,   [0, -1]), 
                ("ITA", "Verona",  kGray,      [0, -1])]

data_settings = [("totale_casi", None)]

plot_corona_data("province", data_settings, loc_settings)

# Plot regioni
loc_settings = [("ITA", "Lombardia",      kRed,        [0, -1]), 
                ("ITA", "Veneto",         kBlue,       [0, -1]),
                ("ITA", "Emilia-Romagna", kGreen + 2,  [0, -1]),
                ("ITA", "Marche",         kOrange,     [0, -1]),
                ("ITA", "Toscana",        kViolet,     [0, -1]), 
                ("ITA", "Piemonte",       kMagenta,    [0, -1]), 
                ("ITA", "Umbria",         kGray,       [0, -1]),
                ("ITA", "Campania",       kCyan + 3,   [0, -1]),
                ("ITA", "Lazio",          kBlack,      [0, -1]),
                ("ITA", "Liguria",        kYellow + 1, [0, -1])]

data_settings = [("totale_casi",                None), 
                 ("deceduti",                   None),
                 ("terapia_intensiva",          None), 
                 ("totale_positivi",            None), 
                 ("totale_ospedalizzati",       None), 
                 ("variazione_totale_positivi", None),
                 ("nuovi_positivi",             None),
                 ("isolamento_domiciliare",     None),
                 ("tamponi_positivi",           None),
                 ("tamponi",                    None)]

# Add custom data
for loc_set in loc_settings:
    (sta, reg, _, _) = loc_set

    pos  = np.array( ana.get_data("totale_positivi", sta, reg) )
    tamp = np.array( ana.get_data("tamponi", sta, reg) ) + 0.1
    tamp_pos = pos / tamp
    ana.add_to_dataset("tamponi_positivi", sta, reg, tamp_pos)

plot_corona_data("regioni", data_settings, loc_settings, False, 30)

# Plot stati
loc_settings = [("Italy",          "", kRed,        [0, -1]),
                ("United States",  "", kBlue,       [10, -1]),
                ("France",         "", kGreen + 2,  [20, -1]),
                ("Spain",          "", kOrange,     [0, -1]), 
                ("China",          "", kViolet,     [0, 30]),
                ("United Kingdom", "", kMagenta,    [0, -1]),
                ("Germany",        "", kGray,       [10, -1]),
                ("South Korea",    "", kBlack,      [0, -1]),
                ("Netherlands",    "", kCyan + 3,   [0, -1]),
                ("Russia",         "", kYellow + 1, [0, -1]),
                ("Japan",          "", kBlue - 2,   [0, -1])]

data_settings = [("total_cases",  None),
                 ("total_deaths", None)]

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
                ("ITA", "Liguria")]        

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

    m_gr.Draw("A")
    c.BuildLegend()
    c.SaveAs(pdf_name)

