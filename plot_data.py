from ROOT import gROOT
from ROOT import TCanvas

# Import function
gROOT.LoadMacro("corona_fit.C")
from ROOT import corona_fit

# Batch mode
gROOT.SetBatch(True);

# Plot provnince
province = {"Bergamo" : [6, -1], "Brescia" : [6, -1], "Pisa" : [6, -1], "Milano" : [6, -1], 
            "Torino" : [6, -1], "Padova" : [6, -1], "Verona" : [6, 25]}

for var in ["totale_casi"]:
    pdf_name = "corona_province_" + var + ".pdf"
    fit_model = ""

    for prov, day in province.items():
        canv = corona_fit("full_data_ita_prov.csv", "PC province", 
                          "ITA", prov,
                          "totale_casi",
                          fit_model, day[0], day[1], 20)

        if prov == province.items()[0][0]:  canv.SaveAs(pdf_name + "[")
        canv.SaveAs(pdf_name)
        if prov == province.items()[-1][0]: canv.SaveAs(pdf_name + "]")

# Plot regioni
regioni = {"" : [0, -1], "Lombardia" : [0, -1], "Toscana" : [5, -1],
           "Piemonte" : [0, -1], "Veneto" : [0, -1], "Marche" : [5, -1], 
           "Umbria" : [0, -1]}

for var in ["totale_casi", "deceduti", "terapia_intensiva", "totale_positivi"]:
    pdf_name = "corona_regioni_" + var + ".pdf"
    fit_model = "test" if var == "deceduti" else ""
    
    for reg, day in regioni.items():
        canv = corona_fit("full_data_ita_reg.csv", "PC regioni", 
                          "ITA", reg,
                           var,
                           fit_model, day[0], day[1], 40)

        if reg == regioni.items()[0][0]:  canv.SaveAs(pdf_name + "[")
        canv.SaveAs(pdf_name)
        if reg == regioni.items()[-1][0]: canv.SaveAs(pdf_name + "]")

# Plot stati
stati = {"Italy" : [0, -1], "United States" : [10, -1], "France" : [20, -1], 
         "Spain" : [0, -1], "China" : [0, 30], "United Kingdom" : [0, -1], 
         "Germany" : [10, -1], "South Korea" : [0, -1], "Netherlands" : [0, -1]}

for var in ["total_cases", "total_deaths"]:
    pdf_name = "corona_stati_" + var + ".pdf"
    fit_model = "test" if var == "total_deaths" else ""

    for sta, day in stati.items():
        canv = corona_fit("full_data_ecdc.csv", "ecdc", 
                          sta, "",
                          var,
                          fit_model,
                          day[0], day[1], 0, False)

        if sta == stati.items()[0][0]:  canv.SaveAs(pdf_name + "[")
        canv.SaveAs(pdf_name)
        if sta== stati.items()[-1][0]: canv.SaveAs(pdf_name + "]")


