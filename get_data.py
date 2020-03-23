import urllib
import csv

# URL
urls = { "https://covid.ourworldindata.org/data/ecdc/full_data.csv" : "full_data_ecdc.csv",
         "https://raw.githubusercontent.com/pcm-dpc/COVID-19/master/dati-regioni/dpc-covid19-ita-regioni.csv" : "full_data_ita_reg.csv",
         "https://raw.githubusercontent.com/pcm-dpc/COVID-19/master/dati-province/dpc-covid19-ita-province.csv" : "full_data_ita_prov.csv" }

# Download URL
for url, file_name in urls.items():
    print "Downloading", url, "to", file_name
    urllib.urlretrieve(url, file_name)
