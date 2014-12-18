TEMPLATE = aux

NAME=harbour-letoh-ambience

OTHER_FILES = \
        harbour-letoh.ambience \
        images/* \
        rpm/*

ambience.files = \
        harbour-letoh.ambience
        
ambience.path = /usr/share/ambience/$${NAME}

images.files = images/*
images.path = $${ambience.path}/images

INSTALLS += \
        ambience \
        images

