#from django.shortcuts import render
#from django.views.generic import TemplateView
#from django.core.files.storage import FileSystemStorage

from rest_framework.response import Response
from rest_framework import generics
from . models import Fichier
from . serializers import FichierSerializer
from django.http import JsonResponse
from . ouverture_fg import execution_fg, clean, clean_action, dossier_zip



from rest_framework.viewsets import ModelViewSet
from django.http import JsonResponse

class FichierView(generics.RetrieveAPIView):
    queryset = Fichier.objects.all()

    def get(self, request, *args, **kwargs):
        queryset = self.get_queryset()
        serializer = FichierSerializer(queryset, many=True)
        return Response(serializer.data)

class FichierViewSet(ModelViewSet):
    queryset = Fichier.objects.all()
    serializer_class = FichierSerializer

def donnee(request):
    fichiers = []

    for fichier in Fichier.objects.all():
        fichiers.append({
            'title' : fichier.title,
            'contenu': 'media/'+ str(fichier.contenu),
            'date' : str(fichier.date),
        })
    chemin_execution = "media/" + str(fichier.contenu)
    execution_fg(chemin_execution)
    return JsonResponse(fichiers, safe=False)

def sortie(request):
    fichiers = []
    chemin = "/media/nouveau/"
    for fichier in Fichier.objects.all():
        fichiers.append({
            'id' : fichier.id,
            'contenu': 'media/'+ str(fichier.contenu),
            'chemin' : chemin,
        })
    dossier_zip("media/nouveau")


    return JsonResponse(fichiers[0], safe=False)

def delete(request):

    fichiers = []
    for fichier in Fichier.objects.all():
        fichiers.append({
            'resultat' : "le fichier id :" + str(fichier.id) + "a bien été supprimé",
        })

    Fichier.objects.all().delete()

    clean_action()
    
    #clean(r'C:\Users\Heishim\tuto_django_vuejs_axios\Django rest framework\media\media')
    #clean("http://127.0.0.1:8000/media/media")
    #clean(r'C:\Users\Heishim\tuto_django_vuejs_axios\Django rest framework\media\nouveau')

    return JsonResponse(fichiers, safe=False)



    


    










