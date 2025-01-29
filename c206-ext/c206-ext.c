/*
 *  Předmět: Algoritmy (IAL) - FIT VUT v Brně
 *  Rozšíření pro příklad c206.c (Dvousměrně vázaný lineární seznam)
 *  Vytvořil: Daniel Dolejška, září 2024
 */

/**
 * Solved by
 * Name : 	Albert Tikaiev 
 * Login: 	xtikaia00
**/

#include "c206-ext.h"

bool error_flag;
bool solved;

/**
 * Function, that will decide if every second packet has to be deleted
 * 
 * @param qos Queue to be checked for MAX_PACKET_COUNT 
 */
void check_overfill(QosPacketListPtr qos);

/**
 * Tato metoda simuluje příjem síťových paketů s určenou úrovní priority.
 * Přijaté pakety jsou zařazeny do odpovídajících front dle jejich priorit.
 * "Fronty" jsou v tomto cvičení reprezentovány dvousměrně vázanými seznamy
 * - ty totiž umožňují snazší úpravy pro již zařazené položky.
 * 
 * Parametr `packetLists` obsahuje jednotlivé seznamy paketů (`QosPacketListPtr`).
 * Pokud fronta s odpovídající prioritou neexistuje, tato metoda ji alokuje
 * a inicializuje. Za jejich korektní uvolnení odpovídá volající.
 * 
 * V případě, že by po zařazení paketu do seznamu počet prvků v cílovém seznamu
 * překročil stanovený MAX_PACKET_COUNT, dojde nejdříve k promazání položek seznamu.
 * V takovémto případě bude každá druhá položka ze seznamu zahozena nehledě
 * na její vlastní prioritu ovšem v pořadí přijetí.
 * 
 * @param packetLists Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param packet Ukazatel na strukturu přijatého paketu
 */
void receive_packet( DLList *packetLists, PacketPtr packet ) { 

	/*IF THERE WAS NO QUEUE AT ALL*/
	if(packetLists->currentLength == 0){
		//Allocate new queue of packets
		QosPacketListPtr newList = malloc(sizeof(struct qos_packet_list_t));
		if(!newList){ //Check malloc
			return;
		}
		//Set priority of new queue
		newList->priority = packet->priority;
		//Allocate list of new queue
		newList->list = malloc(sizeof(DLList));
		if(!(newList->list)){ //Check malloc
			return;
		}
		//Initialize list for new queue and insert first queue and first packet 
		DLL_Init(newList->list);
		DLL_InsertLast(newList->list, (long)packet);
		DLL_InsertLast(packetLists, (long)newList);
		//Packet was added
		return;
	}

	/*TRY FIND A QUEUE WITH SAME PRIORITY*/
	//Make active element to be a first queue in list of queues
	DLL_First(packetLists);
	//Variable that will contain pointer to certain queue
	long currentQos;
	while(DLL_IsActive(packetLists)){
		//Get pointer of active queue in queues list 
		DLL_GetValue(packetLists, &currentQos);
		//Check if this queue has the same priority as packet, that has to be added
		if(((struct qos_packet_list_t*)currentQos)->priority == packet->priority){
			//If so, add this packet to the end of list(queue)
			DLL_InsertLast(((struct qos_packet_list_t*)currentQos)->list, (long)packet);
			//Check if this queue is overfilled (currentLength > MAX_PACKET_COUNT)
			check_overfill((struct qos_packet_list_t*)currentQos);
			//Packet was added
			return;
		}
		//Next queue
		DLL_Next(packetLists);
	}

	/*THERE WAS NO QUEUE WITH SAME PRIORITY*/
	//Allocate new queue of packets
	QosPacketListPtr newList = malloc(sizeof(struct qos_packet_list_t));
	if(!newList){ // Check malloc
		return;
	}
	//Set priority of new queue
	newList->priority = packet->priority;
	//Allocate list of new queue
	newList->list = malloc(sizeof(DLList));
	if(!(newList->list)){ // Check malloc
		return;
	}
	//Initialize list for new queue and insert first packet 
	DLL_Init(newList->list);
	DLL_InsertLast(newList->list, (long)packet);
	//Make active element to be a first queue in list of queues
	DLL_First(packetLists);
	//Want queues to be located in descending order 
	while(DLL_IsActive(packetLists)){
		//Get pointer of active queue in queues list 
		DLL_GetValue(packetLists, &currentQos);
		//If active queue from list of queues has less priority than new queue to be added
		//Then insert new queue before active queue
		if(((struct qos_packet_list_t*)currentQos)->priority < packet->priority){
			DLL_InsertBefore(packetLists, (long)newList);
			//Packet and new queue were added
			return;
		} 
		//If active queue from list of queues has bigger priority and it is last in list of queues
		//Then insert new queue to the end
		if(((struct qos_packet_list_t*)currentQos)->priority > packet->priority && packetLists->activeElement==packetLists->lastElement){
			DLL_InsertLast(packetLists, (long)newList);
			//Packet and new queue were added
			return;
		}
		DLL_Next(packetLists);
	}
}

/**
 * Tato metoda simuluje výběr síťových paketů k odeslání. Výběr respektuje
 * relativní priority paketů mezi sebou, kde pakety s nejvyšší prioritou
 * jsou vždy odeslány nejdříve. Odesílání dále respektuje pořadí, ve kterém
 * byly pakety přijaty metodou `receive_packet`.
 * 
 * Odeslané pakety jsou ze zdrojového seznamu při odeslání odstraněny.
 * 
 * Parametr `packetLists` obsahuje ukazatele na jednotlivé seznamy paketů (`QosPacketListPtr`).
 * Parametr `outputPacketList` obsahuje ukazatele na odeslané pakety (`PacketPtr`).
 * 
 * @param packetLists Ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param outputPacketList Ukazatel na seznam paketů k odeslání
 * @param maxPacketCount Maximální počet paketů k odeslání
 */
void send_packets( DLList *packetLists, DLList *outputPacketList, int maxPacketCount ) {
	//Variable that will have count of packets were already sent
	int packetsSent = 0;
	//Variable that will contain a pointer to the packet
	long packetP = 0;
	//Make active element to be a first queue in list of queues
	DLL_First(packetLists);
	//Variable that will contain pointer to certain queue
	long currentQos = 0;
	//Cycle through all queues
	while(DLL_IsActive(packetLists)){
		//Get pointer of active queue in queues list 
		DLL_GetValue(packetLists, &currentQos);
		//Get pointer to list of an active queue  
		DLList *currList = ((struct qos_packet_list_t*)currentQos)->list;
		//Make active element to be a first packet in list of packets
		DLL_First(currList);
		//Cycle through all packets
		while (DLL_IsActive(currList)) {
			//Get pointer of active packet
			DLL_GetValue(currList, &packetP);
			DLL_Next(currList);
			//Delete this packet from receiving queue
			DLL_DeleteFirst(currList);
			//Insert this packet to sending list
			DLL_InsertLast(outputPacketList, packetP);
			//Count packets
			packetsSent++;
			if(packetsSent==maxPacketCount) break;
		}
		DLL_Next(packetLists);
		if(packetsSent==maxPacketCount) break;
	}
}

void check_overfill(QosPacketListPtr qos){
	if(qos->list->currentLength > MAX_PACKET_COUNT){
		int i = 0;
		//Make active element to be a first packet in list of packets
		DLL_First(qos->list);
		//Cycle through all packets
		while(DLL_IsActive(qos->list)){
			i = (i+1)%2;
			if(i==1){
				//Delete every second packet
				DLL_DeleteAfter(qos->list);
				continue;
			}
			DLL_Next(qos->list);
		}
	}
}