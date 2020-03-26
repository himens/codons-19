from ROOT import gROOT
from ROOT import TCanvas

# Import function
gROOT.LoadMacro("corona_fit.C")
from ROOT import corona_fit

# Batch mode
gROOT.SetBatch(True);

# Plot provnince
province = ["Bergamo", "Brescia", "Pisa", "Milano", "Torino", "Padova", "Verona"]
pdf_name = "corona_province.pdf"
for prov in province:
    canv = corona_fit("full_data_ita_prov.csv", "PC province", 
                      "ITA", prov,
                      "total_cases",
                      "test", 6, -1)
    if prov == province[0]:  canv.SaveAs(pdf_name + "[")
    canv.SaveAs(pdf_name)
    if prov == province[-1]: canv.SaveAs(pdf_name + "]")

# Plot regioni
regioni = ["", "Lombardia", "Toscana", "Piemonte", "Veneto"]
pdf_name = "corona_regioni.pdf"
for reg in regioni:
    canv = corona_fit("full_data_ita_reg.csv", "PC regioni", 
                      "ITA", reg,
                      "total_cases",
                      "test", 6, -1)
    if reg == regioni[0]:  canv.SaveAs(pdf_name + "[")
    canv.SaveAs(pdf_name)
    if reg == regioni[-1]: canv.SaveAs(pdf_name + "]")

# Plot  stati
stati = ["Italy", "United States", "France", "Spain", "China", "United Kingdom"]
pdf_name = "corona_stati.pdf"
for sta in stati:
    canv = corona_fit("full_data_ecdc.csv", "ecdc", 
                      sta, "",
                      "total_cases",
                      "test", 0, -1)
    if sta == stati[0]:  canv.SaveAs(pdf_name + "[")
    canv.SaveAs(pdf_name)
    if sta== stati[-1]: canv.SaveAs(pdf_name + "]")
