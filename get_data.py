import urllib
import csv

filename = "full_data.csv"
url = "https://covid.ourworldindata.org/data/" + filename
urllib.urlretrieve(url, filename)
