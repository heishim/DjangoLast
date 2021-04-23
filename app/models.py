from django.db import models
from django.conf import settings



# Create your models here.

class Fichier(models.Model):

    title = models.CharField(max_length=250)
    contenu = models.FileField(upload_to = 'media')
    date = models.DateTimeField(auto_now_add=True)


    class Meta: 
        ordering = ['title']

    def __str__(self):
        return self.title






        