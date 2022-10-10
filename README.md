# Proyecto de SO

### Desarrollado por:

#### Márquez Kevin, V-23.391.555, marquezberriosk@gmail.com
#### Ochoa Lewis, V-26.373.847, lewis8a@gmail.com

### Estructura del Proyecto

Los archivos que componen al proyecto se describen a continuación:

**makefile:** Archivo para compilación y ejecución del proyecto. Bastará con hacer make para que se compile y ejecute el proyecto. **Nota:** Por defecto el make usará el compilador **clang**, si desea usar cualquier otro compilador modifique la variable **“comp”** dentro del makefile.
 
**memorykey.h:** Encabezado de configuración de constantes globales para sincronización de la memoria virtual y algunas funciones auxiliares.

**washleaves.cpp:** Fuente que modela al proceso de lavado de hojas, este será instanciado por el proceso supervisor y abrirá una nueva terminal donde desplegará información sobre la actividad (washing, roasting o cutting up) que se encuentra realizando en el momento. Un ciclo completo de lavado de hojas dura 6 segundos.

**putdough.cpp:** Fuente que modela al proceso de engrasar y poner la masa, este será instanciado por el proceso supervisor y abrirá una nueva terminal donde desplegará información sobre la actividad (greasing o putting up dough) que se encuentra realizando en el momento. Un ciclo completo de engrasar y colocar la masa dura 3 segundos. Si no hay hojas limpias disponibles, el proceso entrará en espera de que haya hojas en la pila de hojas limpias.
 
**putstew.cpp:** Fuente que modela al proceso de poner guiso, este será instanciado por el proceso supervisor y abrirá una nueva terminal donde desplegará información sobre la actividad (putting up o closing) que se encuentra realizando en el momento. Su ciclo de ejecución es de 5 segundos. Si no hay hojas con masa, el proceso entrará en espera hasta que exista al menos una hoja con masa en la pila respectiva.

**tieuphallaca.cpp:** Fuente que modela al proceso de amarre de hallacas, este será instanciado por el proceso supervisor y abrirá una nueva terminal donde desplegará un menú con información para hacer las respectivas consultas, por debajo está ejecutando el proceso de amarre de hallacas (tying up) y una hebra será la encargada de manejar las consultas. El ciclo de ejecución del proceso de amarre es de 7 segundos. Si no hay hallacas por amarrar, el proceso entrará en espera hasta que exista al menos una hoja con masa en la pila respectiva, sin embargo, la hebra estará activa y podrá responder las consultas que se le soliciten mediante el menú mostrado al usuario por pantalla.

**supervisor.cpp:** Archivo que modela al proceso supervisor, este es quien inicia la ejecución del proyecto, al hacer **make** e iniciarse se le solicitará por terminal que introduzca la cantidad de guiso disponible (**en Kg**) y el límite de guiso (**en gr**) utilizado en el proceso **putstew** para indicarle al proceso **washleaves** que deje de lavar hojas, luego de eso hará lo necesario para iniciar los módulos explicados anteriormente. Después de ello,  limpiará la terminal cada segundo y desplegará la información de libre/ocupado de cada uno de los procesos, así como el estado actual de cada uno de las pilas (stacks). Este proceso está configurado para durar 3 minutos y luego de eso liberará los recursos solicitados al principio de la ejecución y cerrará las diferentes terminales abiertas al inicio, dando por concluido correctamente el programa.

### Ejecución del código: 

Solo debe hacer “**make**” en la terminal en la carpeta contenedora de los fuentes.

**Nota:** Todos los ficheros mencionados con anterioridad deben estar ubicados en el mismo directorio para su correcto funcionamiento.
 
**Información adicional:** El proyecto se desarrolló con el uso de los lenguajes C/C++. El programa hace uso de las bibliotecas de chrono, thread, csignal, unistd.h, sys/types.h, sys/ipc.h, sys/sem.h, sys/shm.h, sys/wait.h y sched.h.
