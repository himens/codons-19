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
data_settings = [("totale_casi", None)]

loc_settings = [("ITA", "Bergamo", [0, -1]), 
                ("ITA", "Brescia", [0, -1]), 
                ("ITA", "Pisa",    [0, -1]), 
                ("ITA", "Milano",  [0, -1]), 
                ("ITA", "Torino",  [0, -1]), 
                ("ITA", "Padova",  [0, -1]), 
                ("ITA", "Verona",  [0, -1])]

plot_corona_data("province", data_settings, loc_settings)

# Plot regioni
data_settings = [("totale_casi",                None), 
                 ("deceduti",                   Corona.Functions.Type.test),
                 ("terapia_intensiva",          None), 
                 ("totale_positivi",            None), 
                 ("totale_ospedalizzati",       None), 
                 ("variazione_totale_positivi", None),
                 ("nuovi_positivi",             None)]

loc_settings = [("ITA", "Lombardia",      [0, 40]), 
                ("ITA", "Toscana",        [0, 45]), 
                ("ITA", "Piemonte",       [0, 45]), 
                ("ITA", "Veneto",         [0, 40]),
                ("ITA", "Marche",         [0, 45]), 
                ("ITA", "Umbria",         [0, 45]),
                ("ITA", "Campania",       [0, 45]),
                ("ITA", "Lazio",          [0, 45]),
                ("ITA", "Emilia-Romagna", [0, 45]),
                ("ITA", "Liguria",        [0, 45])]

plot_corona_data("regioni", data_settings, loc_settings, False, 30)

# Plot stati
data_settings = [("total_cases",  None),
                 ("total_deaths", None)]

loc_settings = [("Italy",          "", [0, -1]),
                ("United States",  "", [10, -1]),
                ("France",         "", [20, -1]),
                ("Spain",          "", [0, -1]), 
                ("China",          "", [0, 30]),
                ("United Kingdom", "", [0, -1]),
                ("Germany",        "", [10, -1]),
                ("South Korea",    "", [0, -1]),
                ("Netherlands",    "", [0, -1])]

plot_corona_data("stati", data_settings, loc_settings)

