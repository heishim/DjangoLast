
#from subprocess import Popen
from os import popen
from os import remove, rmdir, listdir, chdir, sep, walk

from shutil import unpack_archive
import zipfile 


#%% Version fonction 

def execution_fg(nomRSS):
    """
    Fonction permettant d'éxécuter la fonction de groupage
    
    Entrée : nom complet du fichier RSS (c'est-à-dire chemin inclus), avec un petit r' ' encadrant le chemin
                                         
    Sortie : pas de sortie
    """
    execut = "wine 'groupeur V2020'/BIN/fg1920.exe " + nomRSS +" 'groupeur V2020'/TABLES/" " 1" " 'groupeur V2020'/um.txt" " media/nouveau" 
    #execut = r'"C:\Users\Heishim\tuto_django_vuejs_axios\Django rest framework\groupeur V2020\BIN\fg1920.exe" ' + nomRSS + r' "C:\Users\Heishim\tuto_django_vuejs_axios\Django rest framework\groupeur V2020\TABLES/" 1 "C:\Users\Heishim\tuto_django_vuejs_axios\Django rest framework\groupeur V2020\um.txt" "C:\Users\Heishim\tuto_django_vuejs_axios\Django rest framework\media\nouveau" '
    
    #popen([r"cmd"])
    popen(execut)

#execution_fg("../media/media/test.txt")

def clean(dossier):
    for filename in listdir(dossier) :
        remove(dossier + "/" + filename)
        
def clean_action():
    clean("media/nouveau")
    clean("media/media")
    #clean(r'C:\Users\Heishim\tuto_django_vuejs_axios\Django rest framework\media\nouveau')
    #clean(r'C:\Users\Heishim\tuto_django_vuejs_axios\Django rest framework\media\media')


def dossier_zip(dossier):
    my_zip = zipfile.ZipFile("media/media/RSS_GROUPE.zip",'w', compression=zipfile.ZIP_DEFLATED)
    for filename in listdir(dossier) :
        my_zip.write(dossier+"/"+filename)
    my_zip.close()
#dossier_zip("../media/nouveau")

def conversion(a,b) : #a=dossier d'entree b=dossier de sortie
    chdir(a) #ouvre le dossier entree
    for subdir, dirs, files in walk(a):
        for file in files:
            filepath = subdir + sep + file
            if filepath.endswith(".rds"): #cherche les fichiers rds
                with open(file,"r") as rds : #ouvre le fichier en lecture
                    source= rds.read()
    chdir(b) #ouvre le dossier sortie pour créer le fichier
    txt= open("transfo_rum.txt","a")
    txt.write(source)
    txt.close()
    
    
#conversion("../media", "../media")







