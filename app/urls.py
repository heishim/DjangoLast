from django.urls import path
from . views import FichierView, FichierViewSet, donnee, sortie, delete

from rest_framework import routers

router = routers.DefaultRouter()
router.register('fichiers', FichierViewSet)
#urlpatterns = router.urls

urlpatterns = [
    path('donnee/', donnee),
    path('sortie/', sortie),
    path('clean/', delete)
]

urlpatterns += router.urls

