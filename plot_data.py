from ROOT import gROOT
from ROOT import TCanvas

# Import function
gROOT.LoadMacro("corona_fit.C")
from ROOT import corona_fit

# Batch mode
gROOT.SetBatch(True);

# Plot provnince
province = ["Bergamo", "Brescia", "Pisa", "Torino", "Padova", "Verona"]
pdf_name = "corona_prov.pdf"
for prov in province:
    canv = corona_fit("full_data_ita_prov.csv", "PC province", 
                      "ITA", prov,
                      "total_cases",
                      "test", 5, -1, 15)
    if prov == province[0]:  canv.SaveAs(pdf_name + "[")
    canv.SaveAs(pdf_name)
    if prov == province[-1]: canv.SaveAs(pdf_name + "]")

# Plot regioni
regioni = ["", "Lombardia", "Toscana", "Piemonte", "Veneto"]
pdf_name = "corona_reg.pdf"
for reg in regioni:
    canv = corona_fit("full_data_ita_reg.csv", "PC regioni", 
                      "ITA", reg,
                      "total_cases",
                      "test", 5, -1, 15)
    if reg == regioni[0]:  canv.SaveAs(pdf_name + "[")
    canv.SaveAs(pdf_name)
    if reg == regioni[-1]: canv.SaveAs(pdf_name + "]")
