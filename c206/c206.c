/* ******************************* c206.c *********************************** */
/*  Předmět: Algoritmy (IAL) - FIT VUT v Brně                                 */
/*  Úkol: c206 - Dvousměrně vázaný lineární seznam                            */
/*  Návrh a referenční implementace: Bohuslav Křena, říjen 2001               */
/*  Vytvořil: Martin Tuček, říjen 2004                                        */
/*  Upravil: Kamil Jeřábek, září 2020                                         */
/*           Daniel Dolejška, září 2021                                       */
/*           Daniel Dolejška, září 2022                                       */
/* ************************************************************************** */
/*
** Implementujte abstraktní datový typ dvousměrně vázaný lineární seznam.
** Užitečným obsahem prvku seznamu je hodnota typu int. Seznam bude jako datová
** abstrakce reprezentován proměnnou typu DLList (DL znamená Doubly-Linked
** a slouží pro odlišení jmen konstant, typů a funkcí od jmen u jednosměrně
** vázaného lineárního seznamu). Definici konstant a typů naleznete
** v hlavičkovém souboru c206.h.
**
** Vaším úkolem je implementovat následující operace, které spolu s výše
** uvedenou datovou částí abstrakce tvoří abstraktní datový typ obousměrně
** vázaný lineární seznam:
**
**      DLL_Init ........... inicializace seznamu před prvním použitím,
**      DLL_Dispose ........ zrušení všech prvků seznamu,
**      DLL_InsertFirst .... vložení prvku na začátek seznamu,
**      DLL_InsertLast ..... vložení prvku na konec seznamu,
**      DLL_First .......... nastavení aktivity na první prvek,
**      DLL_Last ........... nastavení aktivity na poslední prvek,
**      DLL_GetFirst ....... vrací hodnotu prvního prvku,
**      DLL_GetLast ........ vrací hodnotu posledního prvku,
**      DLL_DeleteFirst .... zruší první prvek seznamu,
**      DLL_DeleteLast ..... zruší poslední prvek seznamu,
**      DLL_DeleteAfter .... ruší prvek za aktivním prvkem,
**      DLL_DeleteBefore ... ruší prvek před aktivním prvkem,
**      DLL_InsertAfter .... vloží nový prvek za aktivní prvek seznamu,
**      DLL_InsertBefore ... vloží nový prvek před aktivní prvek seznamu,
**      DLL_GetValue ....... vrací hodnotu aktivního prvku,
**      DLL_SetValue ....... přepíše obsah aktivního prvku novou hodnotou,
**      DLL_Previous ....... posune aktivitu na předchozí prvek seznamu,
**      DLL_Next ........... posune aktivitu na další prvek seznamu,
**      DLL_IsActive ....... zjišťuje aktivitu seznamu.
**
** Při implementaci jednotlivých funkcí nevolejte žádnou z funkcí
** implementovaných v rámci tohoto příkladu, není-li u funkce explicitně
** uvedeno něco jiného.
**
** Nemusíte ošetřovat situaci, kdy místo legálního ukazatele na seznam
** předá někdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodně komentujte!
**
** Terminologická poznámka: Jazyk C nepoužívá pojem procedura.
** Proto zde používáme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako procedury
** (v jazyce C procedurám odpovídají funkce vracející typ void).
**
**/

/**
 * Solved by
 * Name : 	Albert Tikaiev 
 * Login: 	xtikaia00
**/

#include "c206.h"

bool error_flag;
bool solved;

/**
 * Vytiskne upozornění na to, že došlo k chybě.
 * Tato funkce bude volána z některých dále implementovaných operací.
 */
void DLL_Error(void) {
	printf("*ERROR* The program has performed an illegal operation.\n");
	error_flag = true;
}

/**
 * Provede inicializaci seznamu list před jeho prvním použitím (tzn. žádná
 * z následujících funkcí nebude volána nad neinicializovaným seznamem).
 * Tato inicializace se nikdy nebude provádět nad již inicializovaným seznamem,
 * a proto tuto možnost neošetřujte.
 * Vždy předpokládejte, že neinicializované proměnné mají nedefinovanou hodnotu.
 *
 * @param list Ukazatel na strukturu dvousměrně vázaného seznamu
 */
void DLL_Init( DLList *list ) {
	list->firstElement = NULL;
	list->activeElement = NULL;
	list->lastElement = NULL;
	list->currentLength = 0;
}

/**
 * Zruší všechny prvky seznamu list a uvede seznam do stavu, v jakém se nacházel
 * po inicializaci.
 * Rušené prvky seznamu budou korektně uvolněny voláním operace free.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_Dispose( DLList *list ) {
	//Set temporary variable to last element
	DLLElementPtr temp = list->lastElement;
	//Until last element not equal first element
	while(temp != list->firstElement){
		//Make new last element to be previous element of previous last element
		list->lastElement = temp->previousElement;
		//Free previous last element
		free(temp);
		//Set temporary variable to new last element
		temp = list->lastElement;
	}
	//Free first element
	free(temp);
	//Reinit
	list->currentLength = 0;
	list->firstElement = NULL;
	list->activeElement = NULL;
	list->lastElement = NULL;
}

/**
 * Vloží nový prvek na začátek seznamu list.
 * V případě, že není dostatek paměti pro nový prvek při operaci malloc,
 * volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Hodnota k vložení na začátek seznamu
 */
void DLL_InsertFirst( DLList *list, long data ) {
	DLLElementPtr newElem = malloc(sizeof(struct DLLElement));
	if(!newElem){ //Check malloc
		DLL_Error();
		return;
	}
	//If there was first element, then make his previous element to be new element
	if(list->firstElement) list->firstElement->previousElement = newElem;
	//Otherwise make new element also last element
	else list->lastElement = newElem;
	//Set new element values
	newElem->nextElement = list->firstElement;
	newElem->previousElement = NULL;
	newElem->data = data;
	//Change list first element
	list->firstElement = newElem;
	list->currentLength+=1;
}

/**
 * Vloží nový prvek na konec seznamu list (symetrická operace k DLL_InsertFirst).
 * V případě, že není dostatek paměti pro nový prvek při operaci malloc,
 * volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Hodnota k vložení na konec seznamu
 */
void DLL_InsertLast( DLList *list, long data ) {
	DLLElementPtr newElem = malloc(sizeof(struct DLLElement));
	if(!newElem){ //Check malloc
		DLL_Error();
		return;
	}
	//Firstly set value previous element of new element to avoid cyclic situation
	newElem->previousElement = list->lastElement;
	//If there was not last element, then make new element also first
	if(!list->lastElement) list->firstElement = newElem;
	//Otherwise make new element to be next element of last element
	else list->lastElement->nextElement = newElem;
	//Set other values of new element
	newElem->nextElement = NULL;
	newElem->data = data;
	//Change list last element
	list->lastElement = newElem;
	list->currentLength+=1;
}

/**
 * Nastaví první prvek seznamu list jako aktivní.
 * Funkci implementujte jako jediný příkaz, aniž byste testovali,
 * zda je seznam list prázdný.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_First( DLList *list ) {
	list->activeElement = list->firstElement;
}

/**
 * Nastaví poslední prvek seznamu list jako aktivní.
 * Funkci implementujte jako jediný příkaz, aniž byste testovali,
 * zda je seznam list prázdný.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_Last( DLList *list ) {
	list->activeElement = list->lastElement;
}

/**
 * Prostřednictvím parametru dataPtr vrátí hodnotu prvního prvku seznamu list.
 * Pokud je seznam list prázdný, volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param dataPtr Ukazatel na cílovou proměnnou
 */
void DLL_GetFirst( DLList *list, long *dataPtr ) {
	if(!list->firstElement){
		DLL_Error();
		return;
	}
	*dataPtr = list->firstElement->data;
}

/**
 * Prostřednictvím parametru dataPtr vrátí hodnotu posledního prvku seznamu list.
 * Pokud je seznam list prázdný, volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param dataPtr Ukazatel na cílovou proměnnou
 */
void DLL_GetLast( DLList *list, long *dataPtr ) {
	if(!list->lastElement){
		DLL_Error();
		return;
	}
	*dataPtr = list->lastElement->data;
}

/**
 * Zruší první prvek seznamu list.
 * Pokud byl první prvek aktivní, aktivita se ztrácí.
 * Pokud byl seznam list prázdný, nic se neděje.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_DeleteFirst( DLList *list ) {
	if(list->firstElement){
		//Set temporary variable to first element
		DLLElementPtr temp = list->firstElement;
		//If active element was first element, then list will be unactive
		if(list->firstElement == list->activeElement) list->activeElement = NULL;
		//If there is next element for first element make it to be first now 
		if(list->firstElement->nextElement){ 
			list->firstElement = list->firstElement->nextElement;
			list->firstElement->previousElement = NULL;
		}
		//Otherwise make all to be NULL
		else {
			list->firstElement = NULL;
			list->lastElement = NULL;
		}
		//Free memory of first element
		free(temp);
		list->currentLength-=1;
	}
}

/**
 * Zruší poslední prvek seznamu list.
 * Pokud byl poslední prvek aktivní, aktivita seznamu se ztrácí.
 * Pokud byl seznam list prázdný, nic se neděje.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_DeleteLast( DLList *list ) {
	if(list->lastElement){
		//Set temporary variable to last element
		DLLElementPtr temp = list->lastElement;
		//If active element was last element, then list will be unactive
		if(list->lastElement == list->activeElement) list->activeElement = NULL;
		//If there is previous element for last element make it to be last now 
		if(list->lastElement->previousElement){
			list->lastElement = list->lastElement->previousElement;
			list->lastElement->nextElement = NULL;
		}
		//Otherwise make all to be NULL
		else {
			list->firstElement = NULL;
			list->lastElement = NULL;
		}
		//Free memory of last element
		free(temp);
		list->currentLength-=1;
	}
}

/**
 * Zruší prvek seznamu list za aktivním prvkem.
 * Pokud je seznam list neaktivní nebo pokud je aktivní prvek
 * posledním prvkem seznamu, nic se neděje.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_DeleteAfter( DLList *list ) {
	if(list->activeElement && list->activeElement!=list->lastElement){
		//Set temporary variable to next element of active element
		DLLElementPtr temp = list->activeElement->nextElement;
		//In case we are going to delete last element
		//Then active element must to be also last element
		if(temp == list->lastElement) list->lastElement = list->activeElement;
		//Otherwise if there is next element of deleting element
		//Then make its previous element to be active element
		if(temp->nextElement) temp->nextElement->previousElement = list->activeElement;
		//Make next element of active element to be next element of deleting one(Can be NULL)
		list->activeElement->nextElement = temp->nextElement;
		//Free memory of deleting one
		free(temp);
		list->currentLength-=1;
	}
}

/**
 * Zruší prvek před aktivním prvkem seznamu list .
 * Pokud je seznam list neaktivní nebo pokud je aktivní prvek
 * prvním prvkem seznamu, nic se neděje.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_DeleteBefore( DLList *list ) {
	if(list->activeElement && list->activeElement!=list->firstElement){
		//Set temporary variable to previous element of active element
		DLLElementPtr temp = list->activeElement->previousElement;
		//In case we are going to delete first element
		//Then active element must to be also first element
		if(temp == list->firstElement) list->firstElement = list->activeElement;
		//Otherwise if there is previous element of deleting element
		//Then make its next element to be active element
		if(temp->previousElement) temp->previousElement->nextElement = list->activeElement;
		//Make previous element of active element to be previous element of deleting one(Can be NULL)
		list->activeElement->previousElement = temp->previousElement; 
		//Free memory of deleting one
		free(temp);
		list->currentLength-=1;
	}
}

/**
 * Vloží prvek za aktivní prvek seznamu list.
 * Pokud nebyl seznam list aktivní, nic se neděje.
 * V případě, že není dostatek paměti pro nový prvek při operaci malloc,
 * volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Hodnota k vložení do seznamu za právě aktivní prvek
 */
void DLL_InsertAfter( DLList *list, long data ) {
	if(list->activeElement){
		DLLElementPtr newElem = malloc(sizeof(struct DLLElement));
		if(!newElem){ //Check malloc
			DLL_Error();
			return;
		}
		//In case there is next element for active element
		//Make its previous element to be new element
		if(list->activeElement->nextElement) list->activeElement->nextElement->previousElement = newElem;
		//Otherwise new element has to be last element
		else list->lastElement = newElem;
		//Set next element of new element to be next element of active element(Can be NULL)
		newElem->nextElement = list->activeElement->nextElement;
		//Set previous element of new element to be active element
		newElem->previousElement = list->activeElement;
		//Finally set next element of active element to be new element
		list->activeElement->nextElement = newElem;
		newElem->data = data;
		list->currentLength+=1;
	}
}

/**
 * Vloží prvek před aktivní prvek seznamu list.
 * Pokud nebyl seznam list aktivní, nic se neděje.
 * V případě, že není dostatek paměti pro nový prvek při operaci malloc,
 * volá funkci DLL_Error().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Hodnota k vložení do seznamu před právě aktivní prvek
 */
void DLL_InsertBefore( DLList *list, long data ) {
	if(list->activeElement){
		DLLElementPtr newElem = malloc(sizeof(struct DLLElement));
		if(!newElem){ //Check malloc
			DLL_Error();
			return;
		}
		//In case there is previous element for active element
		//Make its next element to be new element
		if(list->activeElement->previousElement) list->activeElement->previousElement->nextElement = newElem;
		//Otherwise new element has to be first element
		else list->firstElement = newElem;
		//Set next element of new element to be active element
		newElem->nextElement = list->activeElement;
		//Set previous element of new element to be previous element of active element(Can be NULL)
		newElem->previousElement = list->activeElement->previousElement;
		//Finally set previous element of active element to be new element
		list->activeElement->previousElement = newElem;
		newElem->data = data; 
		list->currentLength+=1;
	}
}

/**
 * Prostřednictvím parametru dataPtr vrátí hodnotu aktivního prvku seznamu list.
 * Pokud seznam list není aktivní, volá funkci DLL_Error ().
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param dataPtr Ukazatel na cílovou proměnnou
 */
void DLL_GetValue( DLList *list, long *dataPtr ) {
	if(!list->activeElement){
		DLL_Error();
		return;
	}
	*dataPtr = list->activeElement->data;
}

/**
 * Přepíše obsah aktivního prvku seznamu list.
 * Pokud seznam list není aktivní, nedělá nic.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param data Nová hodnota právě aktivního prvku
 */
void DLL_SetValue( DLList *list, long data ) {
	if(list->activeElement)	list->activeElement->data = data;
}

/**
 * Posune aktivitu na následující prvek seznamu list.
 * Není-li seznam aktivní, nedělá nic.
 * Všimněte si, že při aktivitě na posledním prvku se seznam stane neaktivním.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_Next( DLList *list ) {
	if(list->activeElement) list->activeElement = list->activeElement->nextElement;
}


/**
 * Posune aktivitu na předchozí prvek seznamu list.
 * Není-li seznam aktivní, nedělá nic.
 * Všimněte si, že při aktivitě na prvním prvku se seznam stane neaktivním.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 */
void DLL_Previous( DLList *list ) {
	if(list->activeElement) list->activeElement = list->activeElement->previousElement;
}

/**
 * Je-li seznam list aktivní, vrací nenulovou hodnotu, jinak vrací 0.
 * Funkci je vhodné implementovat jedním příkazem return.
 *
 * @param list Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 *
 * @returns Nenulovou hodnotu v případě aktivity prvku seznamu, jinak nulu
 */
bool DLL_IsActive( DLList *list ) {
	return list->activeElement != NULL;
}

/* Konec c206.c */
