import urllib
import csv

filename = "full_data.csv"

# URL
#url = "https://covid.ourworldindata.org/data/ecdc/full_data.csv"
#url = "https://raw.githubusercontent.com/pcm-dpc/COVID-19/master/dati-regioni/dpc-covid19-ita-regioni.csv"
url = "https://raw.githubusercontent.com/pcm-dpc/COVID-19/master/dati-province/dpc-covid19-ita-province.csv"

# Download URL
urllib.urlretrieve(url, filename)
