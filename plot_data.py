import sys
from ROOT import *

# Import Corona module
gROOT.LoadMacro("Corona.C")
from ROOT import Corona

# Batch mode
gROOT.SetBatch(True)

# Corona data plot functions
def plot_per_data(name,
                  data_settings,
                  loc_settings,
                  log_scale = False):

    canv = ana.make_canvas("canvas_" + name)
    pdf_name = name + ".pdf"
    canv.SaveAs(pdf_name + "[")

    for data_set in data_settings:
        (data_name) = data_set
 
        c = ana.make_canvas(name + "_" + data_name);
        c.SetLogy(log_scale);

        m_gr = TMultiGraph()
        m_gr.SetTitle(data_name + ";Days;" + data_name);
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

def plot_per_loc(name,
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

loc_settings = [("ITA", "Bergamo", kRed      ), 
                ("ITA", "Brescia", kBlue     ), 
                ("ITA", "Pisa",    kGreen + 2), 
                ("ITA", "Milano",  kOrange   ), 
                ("ITA", "Torino",  kViolet   ), 
                ("ITA", "Padova",  kMagenta  ), 
                ("ITA", "Verona",  kGray     )]

data_settings = [("totale_casi")]

plot_per_data("province_ita", data_settings, loc_settings)

# Plot regioni
ana = Corona.Analyzer("full_data_ita_reg.csv", "PC regioni")

regions = [] # Add custom data
for p in ana.get_dataset()["ITA"]:
    regions.append(p.first)

for reg in regions: 
    pos  = ana.get_data("totale_positivi", "ITA", reg)
    tamp = ana.get_data("tamponi", "ITA", reg) 
    ter =  ana.get_data("terapia_intensiva", "ITA", reg) 
    osp =  ana.get_data("totale_ospedalizzati", "ITA", reg) 
    dec =  ana.get_data("deceduti", "ITA", reg) 
    ric =  ana.get_data("ricoverati_con_sintomi", "ITA", reg) 

    ana.add_data(100 * (pos / tamp), "tamponi_positivi (%)", "ITA", reg)
    ana.add_data(pos.derive().average(7), "variaz_positivi", "ITA", reg)
    ana.add_data(ter.derive().average(7), "variaz_terapie", "ITA", reg)
    ana.add_data(ric.derive().average(7), "variaz_ricoverati", "ITA", reg)
    ana.add_data(dec.derive().average(7), "variaz_deceduti", "ITA", reg)

loc_settings = [("ITA", "Lombardia",      kRed       ), 
                ("ITA", "Veneto",         kBlue      ),
                ("ITA", "Emilia-Romagna", kGreen + 2 ),
                ("ITA", "Marche",         kOrange    ),
                ("ITA", "Toscana",        kViolet    ), 
                ("ITA", "Piemonte",       kMagenta   ), 
                ("ITA", "Umbria",         kGray      ),
                ("ITA", "Campania",       kCyan + 3  ),
                ("ITA", "Lazio",          kBlack     ),
                ("ITA", "Liguria",        kYellow + 1)]

data_settings = [("totale_casi"               ), 
                 ("deceduti"                  ),
                 ("terapia_intensiva"         ), 
                 ("totale_positivi"           ), 
                 ("totale_ospedalizzati"      ), 
                 ("isolamento_domiciliare"    ),
                 ("tamponi"                   ),
                 ("ricoverati_con_sintomi"    ),
                 ("tamponi_positivi (%)"      )]

plot_per_data("regioni_ita", data_settings, loc_settings, False)

# Sum over ITA regions
loc_settings = [("ITA", "", kRed)]
data_settings = [("deceduti"            ),
                 ("terapia_intensiva"   ), 
                 ("totale_ospedalizzati"),
                 ("ricoverati_con_sintomi")]

plot_per_data("totale_ita", data_settings, loc_settings, False)

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

data_settings = [("deceduti",                                   kBlack),
                 ("terapia_intensiva",                          kGreen + 2), 
                 ("totale_ospedalizzati",                       kViolet),
                 ("ricoverati_con_sintomi",                     kRed)]

plot_per_loc("summary_regioni_ita", data_settings, loc_settings, False)

data_settings = [("variaz_ricoverati",                        kRed),
                 ("variaz_deceduti",                          kBlack),
                 ("variaz_terapie",                           kBlue)]

plot_per_loc("variaz_regioni_ita", data_settings, loc_settings, False)

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

plot_per_data("stati", data_settings, loc_settings, False)

