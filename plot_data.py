import sys
from ROOT import *

# Import Corona module
gROOT.LoadMacro("Corona.C")
from ROOT import Corona

# Batch mode
gROOT.SetBatch(True)

# Corona data plot functions
# Plot the summary of a data for all locations
def plot_data_summary(name,
                      data_settings,
                      loc_settings,
                      log_scale = False):

    canv = ana.make_canvas("canvas_" + name)
    pdf_name = name + ".pdf"
    canv.SaveAs(pdf_name + "[")

    for data_set in data_settings:
        (data_name, x_range, y_range, x_title) = data_set
 
        c = ana.make_canvas(name + "_" + data_name);
        c.SetLogy(log_scale);

        m_gr = TMultiGraph()
        m_gr.SetTitle(data_name + ";" + x_title + ";Counts");

        for loc_set in loc_settings:
            (sta, reg, color) = loc_set

            data = ana.get_data(data_name, sta, reg)
            gr = data.make_graph(data_name) 
            gr.SetTitle(reg if reg != "" else sta + ";" + x_title + ";Counts")
            gr.SetLineColor(color)
            gr.SetMarkerColor(color)        
            m_gr.Add(gr, "PL")

        h = m_gr.GetHistogram();
        x_min = x_range[0] if (len(x_range) and x_range[0] != None) else h.GetXaxis().GetXmin() 
        x_max = x_range[1] if (len(x_range) == 2 and x_range[1] != None) else h.GetXaxis().GetXmax() 
        y_min = y_range[0] if (len(y_range) and y_range[0] != None) else h.GetYaxis().GetXmin()
        y_max = y_range[1] if (len(y_range) == 2 and y_range[1] != None) else h.GetYaxis().GetXmax()

        h.GetXaxis().SetLimits(0.95*x_min, 1.05*x_max)
        h.GetYaxis().SetRangeUser(0.95*y_min, 1.05*y_max)
        if log_scale: h.SetMinimum(1);

        m_gr.Draw("A")
        c.BuildLegend()
        c.SaveAs(pdf_name)

    canv.SaveAs(pdf_name + "]")

# Plot the summary of all data for a location
def plot_location_summary(name,
                          data_settings,
                          loc_settings,
                          log_scale = False):

    canv = ana.make_canvas("canvas_" + name)
    pdf_name = name + ".pdf"
    canv.SaveAs(pdf_name + "[")

    for loc_set in loc_settings:
        (sta, reg) = loc_set
        (_, _, _, _, x_title) = data_settings[0] # assume all data have same units

        c = ana.make_canvas(name + "_" + reg)
        c.SetLogy(log_scale);

        m_gr = TMultiGraph()
        m_gr.SetTitle(reg + ";" + x_title + ";Counts");

        for data_set in data_settings:
            (data_name, color, x_range, y_range, x_title) = data_set

            data = ana.get_data(data_name, sta, reg)
            gr = data.make_graph(data_name)
            gr.SetLineColor(color)
            gr.SetMarkerColor(color)        
            gr.SetTitle(data_name + ";" + x_title + ";Counts")
        
            m_gr.Add(gr, "PL")

        h = m_gr.GetHistogram();
        x_min = x_range[0] if (len(x_range) and x_range[0] != None) else h.GetXaxis().GetXmin() 
        x_max = x_range[1] if (len(x_range) == 2 and x_range[1] != None) else h.GetXaxis().GetXmax() 
        y_min = y_range[0] if (len(y_range) and y_range[0] != None) else h.GetYaxis().GetXmin()
        y_max = y_range[1] if (len(y_range) == 2 and y_range[1] != None) else h.GetYaxis().GetXmax()

        h.GetXaxis().SetLimits(0.95*x_min, 1.05*x_max)
        h.GetYaxis().SetRangeUser(0.95*y_min, 1.05*y_max)
        if log_scale: h.SetMinimum(1);

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

data_settings = [("totale_casi", [], [], "Days")]

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

loc_settings = [("ITA", "Lombardia",      kRed), 
                ("ITA", "Veneto",         kBlue),
                ("ITA", "Emilia-Romagna", kGreen + 2),
                ("ITA", "Marche",         kOrange),
                ("ITA", "Toscana",        kViolet), 
                ("ITA", "Piemonte",       kMagenta), 
                ("ITA", "Umbria",         kGray),
                ("ITA", "Campania",       kCyan + 3),
                ("ITA", "Lazio",          kBlack),
                ("ITA", "Liguria",        kYellow + 1)]

data_settings = [("totale_casi",             [], [],  "Days"), 
                 ("deceduti",                [], [],  "Days"),
                 ("terapia_intensiva",       [], [],  "Days"), 
                 ("totale_ospedalizzati",    [], [],  "Days"), 
                 ("isolamento_domiciliare",  [], [],  "Days"),
                 ("tamponi",                 [], [],  "Days"),
                 ("ricoverati_con_sintomi",  [], [],  "Days"),
                 ("positivi/tamponi (%)",    [20, None], [0, 30],   "Avg of %s days" %days),
                 ("ricoverati/positivi (%)", [20, None], [-10, 30], "Avg of %s days" %days),
                 ("terapie/positivi (%)",    [20, None], [-5, 10], "Avg of %s days" %days)]

plot_data_summary("regioni_ita", data_settings, loc_settings, False)

# Sum over ITA regions
loc_settings = [("ITA", "", kRed)]
data_settings = [("deceduti",               [], [], "Days"),
                 ("terapia_intensiva",      [], [], "Days"), 
                 ("totale_ospedalizzati",   [], [], "Days"),
                 ("ricoverati_con_sintomi", [], [], "Days"),
                 ("variaz_positivi (x0.1)", [], [], "Avg of %s days" %days),
                 ("variaz_deceduti",        [], [], "Avg of %s days" %days),
                 ("variaz_terapie",         [], [], "Avg of %s days" %days),
                 ("variaz_ricoverati",      [], [], "Avg of %s days" %days)]

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

data_settings = [("deceduti",               kBlack,     [], [], "Days"),
                 ("terapia_intensiva",      kGreen + 2, [], [], "Days"), 
                 ("totale_ospedalizzati",   kViolet,    [], [], "Days"),
                 ("ricoverati_con_sintomi", kRed,       [], [], "Days")]

plot_location_summary("summary_regioni_ita", data_settings, loc_settings, False)

data_settings = [("variaz_positivi (x0.1)",  kRed,        [], [], "Avg of %s days" %days),
                 ("variaz_ricoverati",       kOrange + 1, [], [], "Avg of %s days" %days),
                 ("variaz_deceduti",         kBlack,      [], [], "Avg of %s days" %days),
                 ("variaz_terapie",          kBlue,       [], [], "Avg of %s days" %days)]

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

data_settings = [("total_cases",              [], [],  "Days"),
                 ("total_deaths",             [], [],  "Days"),
                 ("new_vaccinations",         [330, None], [],  "Days"),
                 ("total_vaccinations",       [330, None], [],  "Days"),
                 ("people_fully_vaccinated",  [330, None], [],  "Days"),
                 ("variaz_cases",             [], [],  "Avg of %s days" %days),
                 ("variaz_deaths",            [], [],  "Avg of %s days" %days)]

# Europe
loc_settings = [("Italy",          "", kRed),
                ("France",         "", kGreen + 2),
                ("Spain",          "", kOrange), 
                ("United Kingdom", "", kMagenta),
                ("Germany",        "", kGray),
                ("Netherlands",    "", kCyan + 3)]

plot_data_summary("europe", data_settings, loc_settings, False)

# Extra Europe
loc_settings = [("United States",  "", kBlue),
                ("China",          "", kViolet),
                ("South Korea",    "", kBlack),
                ("Russia",         "", kOrange),
                ("Japan",          "", kGreen + 2),
                ("Brazil",         "", kRed),
                ("India",          "", kMagenta)]

plot_data_summary("extra_europe", data_settings, loc_settings, False)
