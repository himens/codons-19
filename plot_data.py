from ROOT import gROOT
from ROOT import TCanvas

# Import function
gROOT.LoadMacro("corona_fit.C")
from ROOT import corona_fit

# Batch mode
gROOT.SetBatch(True);

# Plot provnince
pdf_name = "corona_province.pdf"
province = {"Bergamo" : [6, -1], "Brescia" : [6, -1], "Pisa" : [6, -1], "Milano" : [6, -1], 
            "Torino" : [6, -1], "Padova" : [6, -1], "Verona" : [6, 25]}

for prov, day in province.items():
    canv = corona_fit("full_data_ita_prov.csv", "PC province", 
                      "ITA", prov,
                      "totale_casi",
                      "test", day[0], day[1], 20)
    if prov == province.items()[0][0]:  canv.SaveAs(pdf_name + "[")
    canv.SaveAs(pdf_name)
    if prov == province.items()[-1][0]: canv.SaveAs(pdf_name + "]")

# Plot regioni
pdf_name = "corona_regioni.pdf"
regioni = {"" : [0, -1], "Lombardia" : [0, -1], "Toscana" : [0, -1],
           "Piemonte" : [0, -1], "Veneto" : [0, -1], "Marche" : [0, -1]}

for reg, day in regioni.items():
    canv = corona_fit("full_data_ita_reg.csv", "PC regioni", 
                      "ITA", reg,
                      "terapia_intensiva",
                      "test", day[0], day[1], 20)
    if reg == regioni.items()[0][0]:  canv.SaveAs(pdf_name + "[")
    canv.SaveAs(pdf_name)
    if reg == regioni.items()[-1][0]: canv.SaveAs(pdf_name + "]")

# Plot stati
pdf_name = "corona_stati.pdf"
stati = {"Italy" : [30, -1], "United States" : [40, -1], "France" : [35, -1], 
         "Spain" : [20, -1], "China" : [0, 30], "United Kingdom" : [30, -1], 
         "Germany" : [30, -1], "South Korea" : [0, -1], "Netherlands" : [20, -1]}

for sta, day in stati.items():
    canv = corona_fit("full_data_ecdc.csv", "ecdc", 
                      sta, "",
                      "total_cases",
                      "test", day[0], day[1], 30)
    if sta == stati.items()[0][0]:  canv.SaveAs(pdf_name + "[")
    canv.SaveAs(pdf_name)
    if sta== stati.items()[-1][0]: canv.SaveAs(pdf_name + "]")
