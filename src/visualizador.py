import matplotlib.pyplot as plt

#Lee un archivo de texto separado por comas y
#devuelve una lista de listas con los elementos de cada línea.
def leer_archivo(nombre_archivo):
    #Abrimos el archivo
    with open(nombre_archivo, "r") as archivo:
        lineas = archivo.readlines()

    elementos_totales = []
    for i, linea in enumerate(lineas):
        if i == 0:
            elementos_totales.append(linea.strip().split(",")) #El header no se transforma a numero
        else:
            elementos = linea.strip().split(",")
            elementos = [float(elem) for elem in elementos]  # Convierte cada elemento a un número decimal
            elementos_totales.append(elementos)

    return elementos_totales

#Me da una lista con los features de la matriz, segun el indice de su posicion
def getFeature(matriz,indice):
    res=[]
    for filas in matriz:
        res.append(filas[indice])
    return res

#Buscar si un servidor fue recibio algun emisor
#Retorna True si un emisor envio algo a un servidor
#Retorna False si un emisor nunca algo a un servidor
def buscarID(EmisoresID,ProcesoID):
    for IDE in EmisoresID:
        for IDP in ProcesoID:
            if(IDE==IDP):
                return True
    return False


#Me retorna una lista de las veces que un ID del emisor aparece en el server
def aparicionesID(EmisoresID,ProcesoID):
    apariciones=[]
    for IDE in EmisoresID:
        for IDP in ProcesoID:
            if(IDE==IDP):
                apariciones.append(IDP)
    return list(set(apariciones))

#Calculamos los promedios por feature
# listaID: La lista de los ID de emisores que aparecen en la matriz del server
# matrizFeature: La matriz con los insumos del server
# matrizEmisor:  La matriz con los insumos del emisor
# feature: 0 tiempo - 3 memoria
def promedioFeature(listaID,matrizFeature,matrizEmisor,feature):
    promedios=[]
    for ID in listaID:
        suma=0
        for elemento in matrizFeature:
            if(elemento[1]==ID):
                suma+=elemento[feature]

        for elemento in matrizEmisor:
            if(elemento[0] == ID):
                suma /= elemento[2]
                promedios.append(suma)
    return promedios

#Me da los emisores totales segun los id que hay en la matriz total de emisores
def getCiclosxID(listaID,EmisorTXT):
    apariciones=[]
    for id in listaID:
        for elemento in EmisorTXT:
            if(elemento[0] == id):
                apariciones.append(elemento[4])
    return apariciones

#Tenemos las matrices de los servidores y el emisor
EmisorTXT = leer_archivo("LogFiles/EmisorLog.txt")
SecuencialTXT = leer_archivo("LogFiles/SecuencialLog.txt")
HiloTXT = leer_archivo("LogFiles/HilosLog.txt")
HeavyTXT = leer_archivo("LogFiles/HeavyLog.txt")

########### Grafica 1: cantidad de solicitudes - Tiempo de ejecución

x_secuencial = getFeature(SecuencialTXT[1:],2)
y_secuencial = getFeature(SecuencialTXT[1:],0)

x_hilo = getFeature(HiloTXT[1:],2)
y_hilo = getFeature(HiloTXT[1:],0)

x_heavy = getFeature(HeavyTXT[1:],2)
y_heavy = getFeature(HeavyTXT[1:],0)

plt.plot(x_secuencial, y_secuencial, 'o', label='Secuencial')
plt.plot(x_hilo, y_hilo, 'o', label='Hilo')
plt.plot(x_heavy, y_heavy, 'o', label='Heavy')

plt.xlabel('Cantidad de solicitudes')
plt.ylabel('Tiempo de ejecución')
plt.legend()  # Mostrar la leyenda de las series

plt.show()

########### Grafica 2: cantidad de solicitudes - Tiempo promedio por solicitud

EmisorID = getFeature(EmisorTXT[1:],0)
EmisorID_secuencial = getFeature(SecuencialTXT[1:],1)
EmisorID_hilo = getFeature(HiloTXT[1:],1)
EmisorID_heavy = getFeature(HeavyTXT[1:],1)

secuencialID = aparicionesID(EmisorID,EmisorID_secuencial)
hiloID = aparicionesID(EmisorID,EmisorID_hilo)
heavyID = aparicionesID(EmisorID,EmisorID_heavy)

y_tiempo_secuencial = promedioFeature(secuencialID,SecuencialTXT[1:],EmisorTXT[1:],0)
x_total_secuencial=getCiclosxID(secuencialID,EmisorTXT[1:])

y_tiempo_hilo = promedioFeature(hiloID,HiloTXT[1:],EmisorTXT[1:],0)
x_total_hilo=getCiclosxID(hiloID,EmisorTXT[1:])

y_tiempo_heavy = promedioFeature(heavyID,HeavyTXT[1:],EmisorTXT[1:],0)
x_total_heavy=getCiclosxID(heavyID,EmisorTXT[1:])

plt.plot(x_total_secuencial, y_tiempo_secuencial, 'o', label='Secuencial')
plt.plot(x_total_hilo, y_tiempo_hilo, 'o', label='Hilo')
plt.plot(x_total_heavy, y_tiempo_heavy, 'o', label='Heavy')

plt.xlabel('Cantidad de solicitudes')
plt.ylabel('Tiempo promedio por solicitud')

plt.legend()  # Mostrar la leyenda de las series

plt.show()


########### Grafica 3: cantidad de solicitudes - consumo de memoria en el servidor


y_memoria_secuencial = promedioFeature(secuencialID,SecuencialTXT[1:],EmisorTXT[1:],3)
y_memoria_hilo = promedioFeature(hiloID,HiloTXT[1:],EmisorTXT[1:],3)
y_memoria_heavy = promedioFeature(heavyID,HeavyTXT[1:],EmisorTXT[1:],3)

plt.plot(x_total_secuencial, y_memoria_secuencial, 'o', label='Secuencial')
plt.plot(x_total_hilo, y_memoria_hilo, 'o', label='Hilo')
plt.plot(x_total_heavy, y_memoria_heavy, 'o', label='Heavy')

plt.xlabel('Cantidad de solicitudes')
plt.ylabel("Consumo de memoria en el servidor")

plt.legend()  # Mostrar la leyenda de las series

plt.show()
