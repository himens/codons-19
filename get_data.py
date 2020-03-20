import urllib
import csv

filename = "full_data.csv"
url = "https://covid.ourworldindata.org/data/ecdc/full_data.csv"
#url = "https://raw.githubusercontent.com/datasets/covid-19/master/time-series-19-covid-combined.csv"
urllib.urlretrieve(url, filename)
