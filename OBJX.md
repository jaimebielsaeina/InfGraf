OBJX es un formato de fichero que define escenas. Se inspira en el formato de fichero OBJ de definición de mallas de triángulos.

Cada comando ocupa una línea. A continuación, se definen los distintos comandos:
NOTA: int y float hacen referencia a valores de tipo natural y decimal, respectivamente.


CONFIGURACIÓN DE LA ESCENA

rpp int     Indica la cantidad de rayos que se lanzarán por píxel.
th int      Indica el número de hilos que se utilizarán para el procesado concurrente.
ph int      Marca el número de fotones que serán lanzados a las fuentes de luz (solo photon mapping).
rad float   Define el radio de búsqueda de fotones cercanos (solo photon mapping).
ngb int     Define la cantidad máxima de fotones cercanos que se va a buscar (solo photon mapping).


DEFINICIÓN DE PRIMITIVAS

p float float float         Define un punto en el espacio.
d float float float         Define una dirección.
c float float float         Define un color.

Una vez definidos, se identifican en el resto de comandos con un valor entero, comenzando desde 0 y de forma ascendente. Es decir, el primer punto que se defina será el 0, el siguiente el 1, el 2... Si se define la primera dirección será también la 0, 1, 2... Idem para colores. La identificación es independiente entre primitivas.


DEFINICIÓN DE COMPONENTES DE LA ESCENA

A partir de aquí, P, D, y C hacen referencia al identificados de un punto, dirección o color.

cam P D D D int int     Define la cámara de la escena, indicando el punto de origen, tres direcciones perpendiculares entre sí (up, left, forward), y el ancho y alto de la imagen en píxeles.
ls P C                  Define una fuente de luz, a partir del centro y la luz que emite.
a C C C C float         Define el aspecto de las figuras que se definan después. Utiliza colores para los coeficientes difuso, especular, transmisivo y luminoso, además del índice de refractancia.


DEFINICIÓN DE FIGURAS

Todas las figuras tienen como primer parámetro un valor entero, que representa su identificación. Para que todas las figuras sean consideradas distintas, se les ha de dar a todas identificadores distintos. Si varias figuras tienen el mismo identificador lerán consideradas la misma.

pl int D int                Plano con su normal (en sentido al origen) y distancia de esa normal.
sp int P float              Esfera con su centro y radio.
t int P P P                 Triángulo con tres puntos.
ds int P D float            Disco con su centro, normal y radio.
pd int P D float float      Disco perforado con centro, normal y dos radios.
cn int P D float float      Cono con el centro, dirección del eje, radio de la base y altura.
cl int P D float float      Cilindro con el centro, dirección del eje, radio de la base y altura.