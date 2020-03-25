from ROOT import gROOT
from ROOT import TCanvas

# Import function
gROOT.LoadMacro("corona_trend.C")
from ROOT import corona_fit

# Batch mode
gROOT.SetBatch(True);

# Plot provnince
province = ["Bergamo", "Brescia", "Pisa", "Torino", "Padova", "Verona"]
canv_prov = TCanvas("canv_prov", "", 1000, 1000) 

for prov in province:
    canv_prov = corona_fit("full_data_ita_prov.csv", "PC province", 
                           "ITA", prov,
                           "total_cases",
                           "test", 5, -1, 15)
    if prov == province[0]:  canv_prov.SaveAs("corona_prov.pdf[")
    canv_prov.SaveAs("corona_prov.pdf")
    if prov == province[-1]: canv_prov.SaveAs("corona_prov.pdf]")

# Plot regioni
regioni = ["", "Lombardia", "Toscana", "Piemonte", "Veneto"]
canv_reg = TCanvas("canv_reg", "", 1000, 1000) 

for reg in regioni:
    canv_reg = corona_fit("full_data_ita_reg.csv", "PC regioni", 
                          "ITA", reg,
                          "total_cases",
                          "test", 5, -1, 15)
    if reg == regioni[0]:  canv_reg.SaveAs("corona_reg.pdf[")
    canv_reg.SaveAs("corona_reg.pdf")
    if reg == regioni[-1]: canv_reg.SaveAs("corona_reg.pdf]")
