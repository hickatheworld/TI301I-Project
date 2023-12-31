#ifndef CONTACTS_H
#define CONTACTS_H

#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"

typedef struct s_appointment {
	Date* date;
	Time* time;
	Time* duration;
	char* reason;
} Appointment;

typedef struct s_appointment_list_cell {
	Appointment* value;
	struct s_appointment_list_cell* next;
} AppointmentListCell;

typedef struct s_appointment_list {
	AppointmentListCell* head;
} AppointmentList;

typedef struct s_contact {
	char* firstName;
	char* lastName;
	AppointmentList* appointments;
} Contact;


typedef struct s_contact_cell {
	Contact* contact;
	char* id;
	int nbLevels;
	struct s_contact_cell** levels;
} ContactCell;

typedef struct s_contact_store {
	ContactCell* heads[4];
} ContactStore;

Contact* createContact(char* firstname, char* lastname, ContactStore* calendar);
ContactCell* searchContactById(ContactStore* calendar, char* id);
ContactCell* searchContactLevel0(ContactStore* calendar, char* id);
ContactCell **searchContactsByQuery(ContactStore *calendar, char *query, int *resultLength);
Appointment* createAppointment(Date* date, Time* time, Time* duration, char* reason);
AppointmentListCell* createAppointmentListCell(Appointment* appointment);
void insertAppointmentInList(AppointmentList* list, Appointment* appointment);
int removeAppointmentByIndex(AppointmentList* list, int i);
void displayAppointments(AppointmentList* list);

#endif // CONTACTS_H
