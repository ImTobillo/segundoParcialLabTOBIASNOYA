/// TOBIAS NOYA - SEGUNDO PARCIAL - LABORATORIO II - 12 / 11 / 2021 - COMISIÓN 6

#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include "conio.h"
#define pasajerosArch "pasajerosAviones.bin"

/// estructura

typedef struct
{
    char dni[9];
    char nombre[30];
    float pesoEquipaje;
    int edad;
    int idAvion;
    int idPasaje;
}registroPasajero;

typedef struct
{
    char nombre[30];
    char dni[9];
    int edad;
}Persona;

typedef struct nodoPasajero
{
    Persona p;
    int idPasaje;
    float pesoEquipaje;
    struct nodoPasajero* sig;
}nodoPasajero;

typedef struct nodoAvion
{
    int idAvion;
    nodoPasajero* pasajeros;
    struct nodoAvion* sig;
}nodoAvion;

/// funciones

// 1)

nodoPasajero* crearNodoP (Persona p, int idPasaje, float pesoEquip)
{
    nodoPasajero* nuevo = (nodoPasajero*)malloc(sizeof(nodoPasajero));
    nuevo->p = p;
    nuevo->idPasaje = idPasaje;
    nuevo->pesoEquipaje = pesoEquip;
    nuevo->sig = NULL;
    return nuevo;
}

nodoAvion* crearNodoA (int idAvion)
{
    nodoAvion* nuevo = (nodoAvion*)malloc(sizeof(nodoAvion));
    nuevo->idAvion = idAvion;
    nuevo->pasajeros = NULL;
    nuevo->sig = NULL;
    return nuevo;
}

void insertarPasajero (nodoPasajero** listaPasajeros, Persona p, int idPasaje, float pesoEquip)
{
    if (*listaPasajeros)
    {
        nodoPasajero* nuevo = crearNodoP(p, idPasaje, pesoEquip);
        nuevo->sig = *listaPasajeros;
        *listaPasajeros = nuevo;
    }
    else
        *listaPasajeros = crearNodoP(p, idPasaje, pesoEquip);
}

void insertarAvion (nodoAvion** listaAviones, int idAvion, Persona p, int idPasaje, float pesoEquip)
{
    if (*listaAviones)
    {
        if ((*listaAviones)->idAvion == idAvion)
            insertarPasajero(&(*listaAviones)->pasajeros, p, idPasaje, pesoEquip);
        else
            insertarAvion(&(*listaAviones)->sig, idAvion, p, idPasaje, pesoEquip);
    }
    else
    {
        *listaAviones = crearNodoA(idAvion);
        (*listaAviones)->pasajeros = crearNodoP(p, idPasaje, pesoEquip);
    }
}

Persona crearPersona (char* dni, int edad, char* nombre)
{
    Persona nueva;
    strcpy(nueva.dni, dni);
    nueva.edad = edad;
    strcpy(nueva.nombre, nombre);
    return nueva;
}

void despersistirArchivo (nodoAvion** listaAviones)
{
    FILE* fp = fopen(pasajerosArch, "rb");

    registroPasajero regisP_Buff;
    Persona persona_Buff;

    if (fp)
    {
        while (fread(&regisP_Buff, sizeof(registroPasajero), 1, fp) > 0)
        {
            persona_Buff = crearPersona(regisP_Buff.dni, regisP_Buff.edad, regisP_Buff.nombre);

            insertarAvion(listaAviones, regisP_Buff.idAvion, persona_Buff, regisP_Buff.idPasaje, regisP_Buff.pesoEquipaje);
        }

        fclose(fp);
    }
    else
        printf("ERROR DE DATOS.\n");

}


// 2)


int retornarCantMenores (nodoPasajero* listaPasajeros)
{
    if (listaPasajeros)
    {
        if (listaPasajeros->p.edad < 18)
            return 1 + retornarCantMenores(listaPasajeros->sig);
        else
            return retornarCantMenores(listaPasajeros->sig);
    }
    else
        return 0;
}

int retornarIdAvionConMasMenores(nodoAvion* listaAviones)
{
    if (listaAviones)
    {
        nodoAvion* avionConMas = listaAviones;

        while (listaAviones)
        {
            if (retornarCantMenores(avionConMas->pasajeros) < retornarCantMenores(listaAviones->pasajeros))
                avionConMas = listaAviones;
            listaAviones = listaAviones->sig;
        }

        return avionConMas->idAvion;
    }
    else
        return -1;
}




// 3)

float retornarPesoTotalAvion (nodoPasajero* listaPasajeros)
{
    if (listaPasajeros)
        return listaPasajeros->pesoEquipaje + retornarPesoTotalAvion(listaPasajeros->sig);
    else
        return 0;
}

void informarAvionesQueSuperan400kg (nodoAvion* listaAviones)
{
    printf("LOS ID DE LOS AVIONES CON PESO EN EQUIPAJE MAYOR A LOS 400KG SON:\n");

    while (listaAviones)
    {
        if (retornarPesoTotalAvion(listaAviones->pasajeros) > 400)
            printf("ID %i\n", listaAviones->idAvion);
        listaAviones = listaAviones->sig;
    }

    printf("============\n");
}

// 4)

void eliminarMayoresA60EnListaPasajeros(nodoPasajero** listaPasajeros)
{
    if (*listaPasajeros)
    {
        if ((*listaPasajeros)->p.edad > 60)
        {
            nodoPasajero* aux = *listaPasajeros;
            *listaPasajeros = (*listaPasajeros)->sig;
            free(aux);

            eliminarMayoresA60EnListaPasajeros(listaPasajeros);
        }
        else
            eliminarMayoresA60EnListaPasajeros(&(*listaPasajeros)->sig);
    }
}

void buscarIdAvionYEliminarMayoresA60 (nodoAvion** listaAviones, int idAvion)
{
    if (*listaAviones)
    {
        if ((*listaAviones)->idAvion == idAvion)
        {
            eliminarMayoresA60EnListaPasajeros(&(*listaAviones)->pasajeros);

            if ((*listaAviones)->pasajeros == NULL)
            {
                nodoAvion* aux = *listaAviones;
                *listaAviones = (*listaAviones)->sig;
                free(aux);
            }
        }
        else
            buscarIdAvionYEliminarMayoresA60(&(*listaAviones)->sig, idAvion);
    }
}

// 5)

void persistirPasajeros (nodoPasajero* listaPasajeros, int idAvion)
{
    char avion[15] = "avion";

    switch(idAvion)
    {
    case(1):
        strcat(avion, "1");
        break;
    case(2):
        strcat(avion, "2");
        break;
    case(3):
        strcat(avion, "3");
        break;
    case(4):
        strcat(avion, "4");
        break;
    case(5):
        strcat(avion, "5");
        break;
    default:
        break;
    }

    strcat(avion, ".bin");

    FILE* fp = fopen(avion, "wb");

    if (fp)
    {
        while(listaPasajeros)
        {
            fwrite(&listaPasajeros->p, sizeof(Persona), 1, fp);
            listaPasajeros = listaPasajeros->sig;
        }

        fclose(fp);
    }
}

void recorrerListaAvionesYPersistirPasajeros (nodoAvion* listaAviones)
{
    while (listaAviones)
    {
        persistirPasajeros(listaAviones->pasajeros, listaAviones->idAvion);

        listaAviones = listaAviones->sig;
    }
}

/// main

int main()
{
    /// 1)
    nodoAvion* aviones = NULL;
    despersistirArchivo(&aviones);

    /// 2)
    printf("ID DEL AVION CON MAS MENORES: %i\n", retornarIdAvionConMasMenores(aviones));

    /// 3)
    informarAvionesQueSuperan400kg(aviones);

    /// 4)

    buscarIdAvionYEliminarMayoresA60(&aviones, 0);

    /// 5)

    recorrerListaAvionesYPersistirPasajeros(aviones);

    return 0;
}
