#include <stdio.h>
#include <stdlib.h>
#include "../midend/data.h"
#include "test.h"



// Eingabe von Umlaute als zwei normale char

void printer_test()
{
	struct Veranstaltung vst1;
	vst1.note = 2.7;
	vst1.lp = 8;
	vst1.semester.jahr = 2025;
	vst1.semester.jahreszeit = Sommer;
	vst1.state = Ausstehend;
	vst1.name = "Info 1";

	printf("Veranstaltung vst1: %s\n", vst1.name);
	printf("Note: %.1f\n", vst1.note);
	printf("LP: %i\n", vst1.lp);
	printf("jahr: %i\n", vst1.semester.jahr);
	printf("Jahreszeit: %i\n", vst1.semester.jahreszeit);
	printf("Status: %i\n", vst1.state);	
}



struct Veranstaltung *get_test_ver()
{
	int i = 0;
	struct Veranstaltung *p = malloc(SIZE_VER * sizeof(struct Veranstaltung));


	p[i].name = "Mathematik für Informatiker";
	p[i].note = 3;
	p[i].lp = 8;
	p[i].modulgruppenindex = 2;
	p[i].semester.jahr = 2025;
	p[i].semester.jahreszeit = Winter;
	p[i].state = Bestanden;
	++i;

	p[i].name = "Informatik 1";
	p[i].note = 2.7;
	p[i].lp = 8;
	p[i].modulgruppenindex = 1;
	p[i].semester.jahr = 2025;
	p[i].semester.jahreszeit = Winter;
	p[i].state = Bestanden;
	++i;

	p[i].name = "Diskrete Strukturen und Logik";
	p[i].note = 1.7;
	p[i].lp = 8;
	p[i].modulgruppenindex = 4;
	p[i].semester.jahr = 2025;
	p[i].semester.jahreszeit = Winter;
	p[i].state = Bestanden;
	++i;
	
	p[i].name = "Informatik 2";
	p[i].note = 2.0;
	p[i].lp = 8;
	p[i].modulgruppenindex = 1;
	p[i].semester.jahr = 2026;
	p[i].semester.jahreszeit = Sommer;
	p[i].state = Bestanden;
	++i;

	p[i].name = "Mathematik für Informatiker II";
	p[i].note = 0;
	p[i].lp = 8;
	p[i].modulgruppenindex = 3;
	p[i].semester.jahr = 2026;
	p[i].semester.jahreszeit = Sommer;
	p[i].state = NichtBestanden;
	++i;

	p[i].name = "Einführung in die theoretische Informatik";
	p[i].note = 0;
	p[i].lp = 8;
	p[i].modulgruppenindex = 1;
	p[i].semester.jahr = 2026;
	p[i].semester.jahreszeit = Sommer;
	p[i].state = Ausstehend;
	++i;

	p[i].name = "Informatik 3";
	p[i].note = 1.7;
	p[i].lp = 8;
	p[i].modulgruppenindex = 1;
	p[i].semester.jahr = 2026;
	p[i].semester.jahreszeit = Winter;
	p[i].state = Bestanden;	

	return p;
}




struct Modulgruppe *get_test_mod()
{
	int i = 0;
	struct Modulgruppe *p = malloc(SIZE_MOD * sizeof(struct Modulgruppe));
	

	p[i].name = "Informatik-Grundlagen";
	p[i].lp_todo = 95;
	p[i].modulgruppenindex = 1;
	++i;

	p[i].name = "Mathematische Grundlagen A";
	p[i].lp_todo = 8;
	p[i].modulgruppenindex = 2;
	++i;

	p[i].name = "Mathematische Grundlagen B";
	p[i].lp_todo = 8;
	p[i].modulgruppenindex = 3;
	++i;

	p[i].name = "Mathematische Grundlagen C";
	p[i].lp_todo = 13;
	p[i].modulgruppenindex = 4;	

	return p;
}



int get_test_data(struct Veranstaltung **ver, size_t *size_ver, struct Modulgruppe **mod, size_t *size_mod)
{
	*ver = get_test_ver();
	*size_ver = SIZE_VER;
	
	*mod = get_test_mod();
	*size_mod = SIZE_MOD;
	
	return SUCCESS;
}


