import sys
from ROOT import *

# Import Corona module
gROOT.LoadMacro("Corona.C")
from ROOT import Corona

# Batch mode
gROOT.SetBatch(True)

# Corona data plot functions
def plot_data_summary(name,
                      data_settings,
                      loc_settings,
                      log_scale = False):

    canv = ana.make_canvas("canvas_" + name)
    pdf_name = name + ".pdf"
    canv.SaveAs(pdf_name + "[")

    for data_set in data_settings:
        (data_name, y_range) = data_set
 
        c = ana.make_canvas(name + "_" + data_name);
        c.SetLogy(log_scale);

        m_gr = TMultiGraph()
        m_gr.SetTitle(data_name + ";Days;" + data_name);
        if (len(y_range) == 2): 
            m_gr.SetMinimum(y_range[0])
            m_gr.SetMaximum(y_range[1])
        if log_scale: m_gr.SetMinimum(1);

        for loc_set in loc_settings:
            (sta, reg, color) = loc_set

            data = ana.get_data(data_name, sta, reg)

            gr = data.make_graph(data_name)
            gr.SetTitle(reg if reg != "" else sta)
            gr.SetLineColor(color)
            gr.SetMarkerColor(color)        
            m_gr.Add(gr, "PL")

        m_gr.Draw("A")
        c.BuildLegend()
        c.SaveAs(pdf_name)

    canv.SaveAs(pdf_name + "]")

def plot_location_summary(name,
                          data_settings,
                          loc_settings,
                          log_scale = False):

    canv = ana.make_canvas("canvas_" + name)
    pdf_name = name + ".pdf"
    canv.SaveAs(pdf_name + "[")

    for loc_set in loc_settings:
        (sta, reg) = loc_set

        c = ana.make_canvas(name + "_" + reg)
        c.SetLogy(log_scale);

        m_gr = TMultiGraph()
        m_gr.SetTitle(reg + ";Days;Counts");
        if log_scale: m_gr.SetMinimum(1);

        for data_set in data_settings:
            (data_name, color) = data_set

            data = ana.get_data(data_name, sta, reg)

            gr = data.make_graph(data_name)
            gr.SetLineColor(color)
            gr.SetMarkerColor(color)        
            gr.SetTitle(data_name)
            m_gr.Add(gr, "PL")

        m_gr.Draw("A")
        c.BuildLegend()
        c.SaveAs(pdf_name)

    canv.SaveAs(pdf_name + "]")


# Plot province
ana = Corona.Analyzer("full_data_ita_prov.csv", "PC province")

loc_settings = [("ITA", "Bergamo", kRed), 
                ("ITA", "Brescia", kBlue), 
                ("ITA", "Pisa",    kGreen + 2), 
                ("ITA", "Milano",  kOrange), 
                ("ITA", "Torino",  kViolet), 
                ("ITA", "Padova",  kMagenta), 
                ("ITA", "Verona",  kGray)]

data_settings = [("totale_casi", [])]

plot_data_summary("province_ita", data_settings, loc_settings)

# Plot regioni
ana = Corona.Analyzer("full_data_ita_reg.csv", "PC regioni")

days = 7
for reg in ana.get_regions("ITA"): # Add custom data
    positivi   = ana.get_data("totale_casi", "ITA", reg)
    tamponi    = ana.get_data("tamponi", "ITA", reg) 
    terapie    = ana.get_data("terapia_intensiva", "ITA", reg) 
    ospedaliz  = ana.get_data("totale_ospedalizzati", "ITA", reg) 
    deceduti   = ana.get_data("deceduti", "ITA", reg) 
    ricoverati = ana.get_data("ricoverati_con_sintomi", "ITA", reg) 

    d_positivi    = positivi.derive().average(days)
    d_tamponi     = tamponi.derive().average(days)
    d_ricoverati  = ricoverati.derive().average(days)
    d_terapie     = terapie.derive().average(days)
    d_deceduti    = deceduti.derive().average(days)

    ana.add_data(100 * (d_positivi / d_tamponi), "positivi/tamponi (%)", "ITA", reg)
    ana.add_data(100 * (d_ricoverati / d_positivi), "ricoverati/positivi (%)", "ITA", reg)
    ana.add_data(100 * (d_terapie / d_positivi), "terapie/positivi (%)", "ITA", reg)
    ana.add_data(0.1 * d_positivi, "variaz_positivi (x0.1)", "ITA", reg)
    ana.add_data(d_ricoverati, "variaz_ricoverati", "ITA", reg)
    ana.add_data(d_terapie, "variaz_terapie", "ITA", reg)
    ana.add_data(d_deceduti, "variaz_deceduti", "ITA", reg)
    ana.add_data(100 * ((d_ricoverati + d_terapie) / d_positivi), "variaz_malati/positivi (%)", "ITA", reg);

loc_settings = [("ITA", "Lombardia",      kRed), 
                ("ITA", "Veneto",         kBlue),
                ("ITA", "Emilia-Romagna", kGreen + 2),
#               ("ITA", "Marche",         kOrange),
                ("ITA", "Toscana",        kViolet), 
                ("ITA", "Piemonte",       kMagenta), 
#                ("ITA", "Umbria",         kGray),
                ("ITA", "Campania",       kCyan + 3),
                ("ITA", "Lazio",          kBlack),
                ("ITA", "Liguria",        kYellow + 1)]

data_settings = [("totale_casi",             []), 
                 ("deceduti",                []),
                 ("terapia_intensiva",       []), 
                 ("totale_ospedalizzati",    []), 
                 ("isolamento_domiciliare",  []),
                 ("tamponi",                 []),
                 ("ricoverati_con_sintomi",  []),
                 ("positivi/tamponi (%)",    []),
                 ("ricoverati/positivi (%)", [-50, 100]),
                 ("terapie/positivi (%)",    [-20, 20])]

plot_data_summary("regioni_ita", data_settings, loc_settings, False)

# Sum over ITA regions
loc_settings = [("ITA", "", kRed)]
data_settings = [("deceduti",               []),
                 ("terapia_intensiva",      []), 
                 ("totale_ospedalizzati",   []),
                 ("ricoverati_con_sintomi", []),
                 ("variaz_positivi (x0.1)", []),
                 ("variaz_deceduti",        []),
                 ("variaz_terapie",         []),
                 ("variaz_ricoverati",      [])]

plot_data_summary("totale_ita", data_settings, loc_settings, False)

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
                ("ITA", "Puglia")]        

data_settings = [("deceduti",               kBlack),
                 ("terapia_intensiva",      kGreen + 2), 
                 ("totale_ospedalizzati",   kViolet),
                 ("ricoverati_con_sintomi", kRed)]

plot_location_summary("summary_regioni_ita", data_settings, loc_settings, False)

data_settings = [("variaz_positivi (x0.1)",  kRed),
                 ("variaz_ricoverati",       kOrange + 1),
                 ("variaz_deceduti",         kBlack),
                 ("variaz_terapie",          kBlue)]

plot_location_summary("variaz_regioni_ita", data_settings, loc_settings, False)

# Plot stati
ana = Corona.Analyzer("full_data_ecdc.csv", "ecdc")

days = 7
for sta in ana.get_states(): # Add custom data
    for reg in ana.get_regions(sta):
        cases    = ana.get_data("total_cases", sta, reg)
        deaths   = ana.get_data("total_deaths", sta, reg)
        d_cases  = cases.derive().average(days)
        d_deaths = deaths.derive().average(days)

        ana.add_data(d_cases, "variaz_cases", sta, reg)
        ana.add_data(d_deaths, "variaz_deaths", sta, reg)
        ana.add_data(100 * (d_deaths / d_cases), "variaz_deaths/variaz_cases (%)", sta, reg)

loc_settings = [("Italy",          "", kRed),
#                ("United States",  "", kBlue),
                ("France",         "", kGreen + 2),
                ("Spain",          "", kOrange), 
#                ("China",          "", kViolet),
                ("United Kingdom", "", kMagenta),
                ("Germany",        "", kGray)]
#                ("South Korea",    "", kBlack),
#                ("Netherlands",    "", kCyan + 3),
#                ("Russia",         "", kYellow + 1),
#                ("Japan",          "", kBlue - 2),
#                ("Brazil",         "", kRed - 2)]
#                ("India",          "", kYellow - 2)]

data_settings = [("total_cases",                    []),
                 ("total_deaths",                   []),
                 ("variaz_cases",                   []),
                 ("variaz_deaths",                  []),
                 ("variaz_deaths/variaz_cases (%)", [0., 5])]

plot_data_summary("stati", data_settings, loc_settings, False)
