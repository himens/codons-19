from ROOT import gROOT
from ROOT import TCanvas

# Import function
gROOT.LoadMacro("corona_fit.C")
from ROOT import corona_fit

# Batch mode
gROOT.SetBatch(True);

# Plot provnince
province = {"Bergamo" : 6, "Brescia" : 6, "Pisa" : 6, "Milano" : 6, "Torino" : 6, "Padova" : 6, "Verona" : 0}
pdf_name = "corona_province.pdf"
for prov, day_min in province.items():
    canv = corona_fit("full_data_ita_prov.csv", "PC province", 
                      "ITA", prov,
                      "total_cases",
                      "test", day_min, -1, 20)
    if prov == province.items()[0][0]:  canv.SaveAs(pdf_name + "[")
    canv.SaveAs(pdf_name)
    if prov == province.items()[-1][0]: canv.SaveAs(pdf_name + "]")

# Plot regioni
regioni = {"" : 7, "Lombardia" : 7, "Toscana" : 11, "Piemonte" : 11, "Veneto" : 7, "Marche" : 7}
pdf_name = "corona_regioni.pdf"
for reg, day_min in regioni.items():
    canv = corona_fit("full_data_ita_reg.csv", "PC regioni", 
                      "ITA", reg,
                      "total_cases",
                      "test", day_min, -1, 20)
    if reg == regioni.items()[0][0]:  canv.SaveAs(pdf_name + "[")
    canv.SaveAs(pdf_name)
    if reg == regioni.items()[-1][0]: canv.SaveAs(pdf_name + "]")

# Plot  stati
stati = {"Italy" : 10, "United States" : 50, "France" : 45, "Spain" : 32, "China" : 10, "United Kingdom" : 40}
pdf_name = "corona_stati.pdf"
for sta, day_min in stati.items():
    canv = corona_fit("full_data_ecdc.csv", "ecdc", 
                      sta, "",
                      "total_cases",
                      "test", day_min, -1, 10, True)
    if sta == stati.items()[0][0]:  canv.SaveAs(pdf_name + "[")
    canv.SaveAs(pdf_name)
    if sta== stati.items()[-1][0]: canv.SaveAs(pdf_name + "]")
