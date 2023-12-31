#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "../headers/appointments.h"

Contact* createContact(char *firstname, char *lastname, ContactStore* calendar) {
	//region 1. Contact creation
	Contact* contact = (Contact*) malloc(sizeof(Contact));
	contact->firstName = firstname;
	contact->lastName = lastname;
	contact->appointments = (AppointmentList*) malloc(sizeof(AppointmentList));
	contact->appointments->head = NULL;
	//endregion
	//region 2. ContactCell creation
	ContactCell* newCell = (ContactCell*) malloc(sizeof(ContactCell));
	newCell->contact = contact;
	char* lowerName = (char*) malloc(sizeof(*firstname));
	char* lowerLastName = (char*) malloc(sizeof(*lastname));
	int i = 0;
	while (firstname[i] != '\0') {
		lowerName[i] = (char)tolower(firstname[i]);
		i++;
	}
	i = 0;
	while (lastname[i] != '\0') {
		lowerLastName[i] = (char)tolower(lastname[i]);
		i++;
	}
	size_t length = strlen(lowerLastName) + strlen(lowerName) + 2;
	newCell->id = (char*) malloc(length * sizeof(char));
	sprintf(newCell->id, "%s_%s", lowerLastName, lowerName);
	newCell->nbLevels = -1;
	//endregion
	//region 3. Insertion in calendar
	//region 3.a Edge case: Empty list
	if (calendar->heads[0] == NULL) {
		newCell->nbLevels = 4;
		newCell->levels = (ContactCell**) malloc(4 * sizeof(ContactCell*));
		for (i = 0; i < 4; i++) {
			newCell->levels[i] = NULL;
			calendar->heads[i] = newCell;
		}
		return contact;
	}
	//endregion
	//region 3.b Edge case of head insertion
	if (strcmp(newCell->id, calendar->heads[0]->id) < 0) {
		newCell->nbLevels = 4;
		newCell->levels = (ContactCell **) malloc(4 * sizeof(ContactCell *));
		for (i = 0; i < 4; i++)
			newCell->levels[i] = NULL;
		int levels = 1;
		if (newCell->id[0] != calendar->heads[3]->id[0])
			levels = 4;
		else if (calendar->heads[2]->id[1] != newCell->id[1])
			levels = 3;
		else if (calendar->heads[1]->id[2] != newCell->id[2])
			levels = 2;
		for (i = 0; i < 4; i++) {
			if (i < levels)
				 newCell->levels[i] = calendar->heads[i];
			calendar->heads[i] = newCell;
		}
		return contact;
	}
	//endregion
	//region 3.c General Case
	ContactCell* current = NULL;
	current = calendar->heads[0];
	while (current->levels[0] != NULL && strcmp(current->levels[0]->id, newCell->id) < 0)
		current = current->levels[0];
	int levels = 1;
	if (newCell->id[0] != current->id[0])
		levels = 4;
	else if (current->id[1] != newCell->id[1])
		levels = 3;
	else if (current->id[2] != newCell->id[2])
		levels = 2;
	newCell->nbLevels = levels;
	newCell->levels = (ContactCell**) malloc(levels * sizeof(ContactCell*));
	for (i = 0; i < levels; i++) {
		current = calendar->heads[i];
		while (current->levels[i] != NULL && strcmp(current->id, newCell->id) < 0 && strcmp(current->levels[i]->id, newCell->id) < 0)
			current = current->levels[i];
		newCell->levels[i] = current->levels[i];
		current->levels[i] = newCell;
	}
	// Now, we might need to remove the next cell from certain levels
	if (levels > 1 && newCell->levels[1]
		&& newCell->levels[1]->id[0] == newCell->id[0]
		&& newCell->levels[1]->id[1] == newCell->id[1]
		&& newCell->levels[1]->id[2] == newCell->id[2])
		newCell->levels[1] = newCell->levels[1]->levels[1];

	if (levels > 2 && newCell->levels[2]
		&& newCell->levels[2]->id[0] == newCell->id[0]
		&& newCell->levels[2]->id[1] == newCell->id[1])
		newCell->levels[2] = newCell->levels[2]->levels[2];

	if (levels > 3 && newCell->levels[3] && newCell->levels[3]->id[0] == newCell->id[0])
		newCell->levels[3] = newCell->levels[3]->levels[3];
	//endregion
	//endregion
	return contact;
}

ContactCell* searchContactLevel0(ContactStore* calendar, char* id) {
	ContactCell* current = calendar->heads[0];
	while (current != NULL) {
		if (strcmp(current->id, id) == 0)
			return current;
		if (current->levels[0] != NULL && strcmp(current->levels[0]->id, id) < 0)
			return 0;
		current = current->levels[0];
	}
	return 0;
}

ContactCell* searchContactById(ContactStore* calendar, char* id) {
	int i = 3;
	ContactCell* current = calendar->heads[i];
	if (current == NULL)
		return NULL;
	while (i >= 0) {
		char* cid = current->id;
		if (i == 0 && strcmp(cid, id) > 0)
			return NULL;
		if (strcmp(cid, id) == 0)
			return current;
		if (strcmp(cid, id) > 0)
			current = calendar->heads[--i];
		else {
			while (current->levels[i] == NULL && i >= 0)
				i--;
			while (current->levels[i] != NULL && current->levels[i]->id > id && i > 0)
				i--;
			current = current->levels[i];
		}
	}
	return NULL;
}

ContactCell **searchContactsByQuery(ContactStore *calendar, char *query, int *resultLength) {
	if (query == NULL || query[0] == '\0') {
		*resultLength = 0;
		return NULL;
	}
	int i = 3;
	int len = strlen(query); // The length of the query
	char* sub = NULL; // The substring of the current cell's id, as long as the query string. 1 more char for the null terminating character.
	ContactCell* current = calendar->heads[i];
	while (i >= 0 && current != NULL) {
		char* cid = current->id;
		sub = substr(cid, len);
		if (strcmp(sub, query) == 0)
			break;
		if (i > 0 && strcmp(sub, query) > 0)
			current = calendar->heads[--i];
		else {
			while (current->levels[i] == NULL && i > 0)
				i--;
			while (current->levels[i] != NULL && strcmp(substr(current->levels[i]->id, len), query) > 0 && i > 0)
				i--;
			current = current->levels[i];
		}
	}
	if (current == NULL)
		return NULL;
	i = 0;
	ContactCell** res =(ContactCell**) malloc(sizeof(ContactCell*));
	res[i] = current;
	current = current->levels[0];
	while (current != NULL) {
		sub = substr(current->id, len);
		if (strcmp(sub, query) == 0) {
			res = (ContactCell**)realloc(res, (++i + 1) * sizeof(ContactCell*));
			res[i] = current;
		} else
			break;
		current = current->levels[0];
	}
	*resultLength = i + 1;
	return res;
}


Appointment* createAppointment(Date* date, Time* time, Time* duration, char* reason) {
	Appointment* appointment = (Appointment*) malloc(sizeof(Appointment));
	appointment->date = date;
	appointment->time = time;
	appointment->duration = duration;
	appointment->reason = reason;
	return appointment;
}

AppointmentListCell* createAppointmentListCell(Appointment* appointment) {
	AppointmentListCell* cell = (AppointmentListCell*) malloc(sizeof(AppointmentListCell));
	cell->value = appointment;
	cell->next = NULL;
	return cell;
}

void insertAppointmentInList(AppointmentList* list, Appointment* appointment) {
	AppointmentListCell* cell = createAppointmentListCell(appointment);
	cell->next = list->head;
	list->head = cell;
}

int removeAppointmentByIndex(AppointmentList* list, int i) {
	if (list->head == NULL)
		return 0; // List is empty, nothing to delete.
	int j = 1;
	AppointmentListCell *current = list->head,
						*prev = NULL;
	if (j == i) {
		list->head = current->next;
		free(current->value);
		free(current);
		return 1;
	}

	while (current != NULL && j != i) {
		prev = current;
		current = current->next;
		j++;
	}

	if (current != NULL) {
		prev->next = current->next;
		free(current->value);
		free(current);
		return 1;
	}
	return 0;
}

void displayAppointments(AppointmentList* list) {
	AppointmentListCell* current = list->head;
	if (current == NULL) {
		printf("No appointment.\n");
		return;
	}
	int i = 1;
	while (current != NULL) {
		printf("---\n");
		printf("Index: %d\n", i);
		printf("Date: %d/%d/%d\n", current->value->date->day, current->value->date->month, current->value->date->year);
		printf("Time: %d:%d\n", current->value->time->hours, current->value->time->minutes);
		printf("Duration: %d:%d\n", current->value->duration->hours, current->value->time->minutes);
		printf("Reason: %s\n", current->value->reason);
		printf("---\n");
		current = current->next;
		i++;
	}
}