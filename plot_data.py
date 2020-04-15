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
                     csv_settings,
                     data_settings,
                     loc_settings,
                     log_scale = False,
                     days_to_pred = 3):

    (csv_file, csv_fmt) = csv_settings[0]
    ana = Corona.Analyzer(csv_file, csv_fmt)

    for data_set in data_settings:
        (data_name, fit_model) = data_set
 
        canv = ana.get_canvas(data_name);
        canv.SetLogy(log_scale);

        pdf_name = name + "_" + data_name + ".pdf"
        canv.SaveAs(pdf_name + "[")

        m_gr = TMultiGraph()
        m_gr.SetTitle("Corona trend;Days;" + data_name);

        for loc_set in loc_settings:
            (sta, reg, fit_range) = loc_set

            gr = ana.get_graph(data_name, sta, reg)
            gr.SetTitle(reg if reg != "" else sta)
            
            if fit_model != None: 
                ana.fit(gr,
                        fit_model,
                        fit_range[0], 
                        fit_range[1], 
                        days_to_pred)
                canv.SaveAs(pdf_name)

            else:
                m_gr.Add(gr, "PL")
                if loc_set == loc_settings[-1]: 
                    m_gr.Draw("A plc pmc")
                    canv.BuildLegend()
                    canv.SaveAs(pdf_name)

        canv.SaveAs(pdf_name + "]")

# Plot province
loc_settings = [("ITA", "Bergamo", [0, -1]), 
                ("ITA", "Brescia", [0, -1]), 
                ("ITA", "Pisa",    [0, -1]), 
                ("ITA", "Milano",  [0, -1]), 
                ("ITA", "Torino",  [0, -1]), 
                ("ITA", "Padova",  [0, -1]), 
                ("ITA", "Verona",  [0, -1])]

csv_settings  = [("full_data_ita_prov.csv", "PC province")]

data_settings = [("totale_casi", None)]

plot_corona_data("province",
                 csv_settings,
                 data_settings,
                 loc_settings)

# Plot regioni
loc_settings = [("ITA", "",               [7, -1]), 
                ("ITA", "Lombardia",      [7, -1]), 
                ("ITA", "Toscana",        [10, -1]), 
                ("ITA", "Piemonte",       [7, -1]), 
                ("ITA", "Veneto",         [7, -1]),
                ("ITA", "Marche",         [7, -1]), 
                ("ITA", "Umbria",         [10, -1]),
                ("ITA", "Campania",       [10, -1]),
                ("ITA", "Lazio",          [10, -1]),
                ("ITA", "Emilia-Romagna", [7, -1]),
                ("ITA", "Liguria",        [0, -1])]

csv_settings = [("full_data_ita_reg.csv", "PC regioni")]

data_settings = [("totale_casi",                None), 
                 ("deceduti",                   Corona.Functions.Type.test),
                 ("terapia_intensiva",          None), 
                 ("totale_positivi",            None), 
                 ("totale_ospedalizzati",       None), 
                 ("variazione_totale_positivi", None),
                 ("nuovi_positivi",             None)]

plot_corona_data("regioni",
                 csv_settings,
                 data_settings, 
                 loc_settings,
                 False, 
                 30)

# Plot stati
loc_settings = [("Italy",          "", [0, -1]),
                ("United States",  "", [10, -1]),
                ("France",         "", [20, -1]),
                ("Spain",          "", [0, -1]), 
                ("China",          "", [0, 30]),
                ("United Kingdom", "", [0, -1]),
                ("Germany",        "", [10, -1]),
                ("South Korea",    "", [0, -1]),
                ("Netherlands",    "", [0, -1])]

csv_settings = [("full_data_ecdc.csv", "ecdc")]

data_settings = [("total_cases",  None),
                 ("total_deaths", None)]

plot_corona_data("stati",
                 csv_settings,
                 data_settings, 
                 loc_settings)

