// file: database.c
#include <string.h>
#include "database.h"
#include "defs.h"

/********************************************************
 ********************************************************
          NOTES TABLE AND LOOKUP
 ********************************************************
 *******************************************************/
int Patient_notes_table_create(sqlite3 *db)
{
  int rc;
  char *error = "ERROR";

  char *sql = "CREATE TABLE NOTES(" \
    "MRN INTEGER NOT NULL," \
    "TITLE CHAR(50) NOT NULL, " \
    "AUTHOR CHAR(80) NOT NULL, " \
    "TIME INTEGER NOT NULL, " \
    "REPLACED INTEGER NOT NULL, " \
    "TEXT CHAR(8000)" \
    ");";

  rc = sqlite3_exec(db, sql, NULL, 0, &error);

  if(rc != SQLITE_OK) {
    fprintf(stderr, "SQL Error: %s\n", error);
  } else {
    fprintf(stdout, "Table created.\n");
  }

  sqlite3_free(error);

  return rc;
}

char *Create_add_note_query(Note *n)
{
  char *query = malloc(sizeof(char) * MAX_QUERY);
  

  return query;
}


/********************************************************
 ********************************************************
            PATIENT DEMOGRAPHICS AND LOOKUP
 ********************************************************
 *******************************************************/
int Patient_demographics_table_create(sqlite3 *db)
{
  int rc;
  char *error = "ERROR";
  
  char *sql = "CREATE TABLE PATIENTS(" \
    "MRN INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, " \
    "FIRST CHAR(40) NOT NULL, " \
    "MIDDLE CHAR(40), " \
    "LAST CHAR(40) NOT NULL, " \
    "MONTH INT NOT NULL, " \
    "DAY INT NOT NULL, " \
    "YEAR INT NOT NULL, " \
    "ADDR_F1 CHAR(50) NOT NULL, " \
    "ADDR_F2 CHAR(50) NOT NULL, " \
    "ADDR_F3 CHAR(50) NOT NULL, " \
    "ADDR_F4 CHAR(50) NOT NULL, " \
    "PHONE_H CHAR(40), " \
    "PHONE_C CHAR(40), " \
    "PHONE_W CHAR(40), " \
    "EMAIL CHAR(50), " \
    "EC1_FULLNAME CHAR(80) NOT NULL, " \
    "EC1_RELATIONSHIP CHAR(100) NOT NULL, " \
    "EC1_PHONE_H CHAR(40), " \
    "EC1_PHONE_W CHAR(40), " \
    "EC1_PHONE_C CHAR(40), " \
    "EC1_EMAIL CHAR(50), " \
    "EC2_FULLNAME CHAR(80) NOT NULL, " \
    "EC2_RELATIONSHIP CHAR(100) NOT NULL, " \
    "EC2_PHONE_H CHAR(40), " \
    "EC2_PHONE_W CHAR(40), " \
    "EC2_PHONE_C CHAR(40), " \
    "EC2_EMAIL CHAR(50), " \
    "PID CHAR(40)" \
    ");";

  rc = sqlite3_exec(db, sql, NULL, 0, &error);

  if (rc != SQLITE_OK) {
    fprintf(stderr, "SQL Error: %s\n", error);
  } else {
    fprintf(stdout, "Table created.\n");
  }

  sqlite3_free(error);
  
  return rc;
}

char *Create_add_user_query(Patient *p)
{
  char *query = malloc(sizeof(char) * MAX_QUERY);
  char day[2];
  sprintf(day, "%d", p->dob->day);
  char month[2];
  sprintf(month, "%d", p->dob->month);
  char year[4];
  sprintf(year, "%d", p->dob->year);

  query[0] = '\0';
  strcat(query, "INSERT INTO PATIENTS VALUES(");
  strcat(query, "NULL, '");
  strcat(query, p->name->first);
  strcat(query, "', '");
  strcat(query, p->name->middle);
  strcat(query, "', '");
  strcat(query, p->name->last);
  strcat(query, "', '");
  strcat(query, month);
  strcat(query, "', '");
  strcat(query, day);
  strcat(query, "', '");
  strcat(query, year);
  strcat(query, "', '");
  strcat(query, p->addr->field1);
  strcat(query, "', '");
  strcat(query, p->addr->field2);
  strcat(query, "', '");
  strcat(query, p->addr->field3);
  strcat(query, "', '");
  strcat(query, p->addr->field4);
  strcat(query, "', '");
  strcat(query, p->contact->phone_h);
  strcat(query, "', '");
  strcat(query, p->contact->phone_w);
  strcat(query, "', '");
  strcat(query, p->contact->phone_c);
  strcat(query, "', '");
  strcat(query, p->contact->email);
  strcat(query, "', '");
  strcat(query, p->emerg1->full_name);
  strcat(query, "', '");
  strcat(query, p->emerg1->relationship);
  strcat(query, "', '");
  strcat(query, p->emerg1->contact->phone_h);
  strcat(query, "', '");
  strcat(query, p->emerg1->contact->phone_w);
  strcat(query, "', '");
  strcat(query, p->emerg1->contact->phone_c);
  strcat(query, "', '");
  strcat(query, p->emerg1->contact->email);
  strcat(query, "', '");
  strcat(query, p->emerg2->full_name);
  strcat(query, "', '");
  strcat(query, p->emerg2->relationship);
  strcat(query, "', '");
  strcat(query, p->emerg2->contact->phone_h);
  strcat(query, "', '");
  strcat(query, p->emerg2->contact->phone_w);
  strcat(query, "', '");
  strcat(query, p->emerg2->contact->phone_c);
  strcat(query, "', '");
  strcat(query, p->emerg2->contact->email);
  strcat(query, "', '");
  strcat(query, p->pid);
  strcat(query, "');");

  return query;
}

Patient *Patient_lookup_last(char  *last, sqlite3 *db)
{
  PQR pqr;
  Patient *pt;
  char sql[100];
  char *error = "Sqlite3 ERROR.";
  int rc, i, selection;

  for(i = 0; i < MAX_RESULTS; i++) {
    pqr.resultList[i] = Patient_create();
  }
  pqr.count = 0;
  
  sql[0] = '\0';
  strcat(sql, "SELECT * FROM PATIENTS WHERE LOWER(LAST) = LOWER('");
  strcat(sql, last);
  strcat(sql, "');\0");

  // the 4th argument (pt) is provided to callback as the first
  // argument 'void *upd'. This is how we pass patient information
  // to a patient from the SQL query return.
  rc = sqlite3_exec(db, sql, Patient_find_callback, &pqr, &error);
  selection = Patient_select(&pqr, last);
  pt = pqr.resultList[selection];
  
  // If the resulting query is still a blank patient, destroy it
  // and set it to null as the query was unsuccessful.  
  if (strcmp(pt->mrn, "") == 0 || rc != SQLITE_OK) {
    Patient_destroy(pt);
    pt = NULL;
  }
  
  // this causes memory leak; but it's a test
  for(i = 0; i < MAX_RESULTS; i++) {
    if(pqr.resultList[i] && i != selection) free(pqr.resultList[i]);
  }

  sqlite3_free(error);

  return pt;
}

Patient *Patient_lookup_mrn(char  *mrn, sqlite3 *db)
{
  PQR pqr;
  Patient *pt;
  char sql[100];
  char *error = "Sqlite3 ERROR.";
  int rc, i;

  for(i = 0; i < MAX_RESULTS; i++) {
    pqr.resultList[i] = Patient_create();
  }
  pqr.count = 0;
  
  sql[0] = '\0';
  strcat(sql, "SELECT * FROM PATIENTS WHERE mrn = '");
  strcat(sql, mrn);
  strcat(sql, "';\0");

  // the 4th argument (pt) is provided to callback as the first
  // argument 'void *upd'. This is how we pass patient information
  // to a patient from the SQL query return.
  // NOTE: when pqr is exchaged with pqr.resultList[n] this works
  rc = sqlite3_exec(db, sql, Patient_find_callback, &pqr, &error);

  pt = pqr.resultList[0];
  
  // If the resulting query is still a blank patient, destroy it
  // and set it to null as the query was unsuccessful.  
  if (strcmp(pt->mrn, "") == 0 || rc != SQLITE_OK) {
    Patient_destroy(pt);
    pt = NULL;
  }
  
  // this causes memory leak; but it's a test
  for(i = 1; i < MAX_RESULTS; i++) {
    if(pqr.resultList[i]) free(pqr.resultList[i]);
  }

  sqlite3_free(error);
    
  return pt;
}

Patient *Patient_lookup_first(char  *first, sqlite3 *db)
{
  PQR pqr;
  Patient *pt;
  char sql[100];
  char *error = "Sqlite3 ERROR.";
  int rc, i, selection;

  for(i = 0; i < MAX_RESULTS; i++) {
    pqr.resultList[i] = Patient_create();
  }
  pqr.count = 0;
  
  sql[0] = '\0';
  strcat(sql, "SELECT * FROM PATIENTS WHERE LOWER(FIRST) = LOWER('");
  strcat(sql, first);
  strcat(sql, "');\0");

  // the 4th argument (pt) is provided to callback as the first
  // argument 'void *upd'. This is how we pass patient information
  // to a patient from the SQL query return.
  rc = sqlite3_exec(db, sql, Patient_find_callback, &pqr, &error);
  selection = Patient_select(&pqr, first);
  pt = pqr.resultList[selection];

  // If the resulting query is still a blank patient, destroy it
  // and set it to null as the query was unsuccessful.  
  if (strcmp(pt->mrn, "") == 0 || rc != SQLITE_OK) {
    Patient_destroy(pt);
    pt = NULL;
  }

  sqlite3_free(error);
  
  return pt;
}

int Patient_find_callback(void *udp, int c_num, char *c_vals[], char *c_names[])
{
  int i = ((PQR*)udp)->count;
  
  strcpy(((PQR*)udp)->resultList[i]->mrn, c_vals[0]);
  strcpy(((PQR*)udp)->resultList[i]->name->first, c_vals[1]);
  strcpy(((PQR*)udp)->resultList[i]->name->middle, c_vals[2]);
  strcpy(((PQR*)udp)->resultList[i]->name->last, c_vals[3]);
  ((PQR*)udp)->resultList[i]->dob->month = atoi(c_vals[4]);
  ((PQR*)udp)->resultList[i]->dob->day = atoi(c_vals[5]);
  ((PQR*)udp)->resultList[i]->dob->year = atoi(c_vals[6]);
  strcpy(((PQR*)udp)->resultList[i]->addr->field1, c_vals[7]);
  strcpy(((PQR*)udp)->resultList[i]->addr->field2, c_vals[8]);
  strcpy(((PQR*)udp)->resultList[i]->addr->field3, c_vals[9]);
  strcpy(((PQR*)udp)->resultList[i]->addr->field4, c_vals[10]);
  strcpy(((PQR*)udp)->resultList[i]->contact->phone_h, c_vals[11]);
  strcpy(((PQR*)udp)->resultList[i]->contact->phone_w, c_vals[12]);
  strcpy(((PQR*)udp)->resultList[i]->contact->phone_c, c_vals[13]);
  strcpy(((PQR*)udp)->resultList[i]->contact->email, c_vals[14]);
  strcpy(((PQR*)udp)->resultList[i]->emerg1->full_name, c_vals[15]);
  strcpy(((PQR*)udp)->resultList[i]->emerg1->relationship, c_vals[16]);
  strcpy(((PQR*)udp)->resultList[i]->emerg1->contact->phone_h, c_vals[17]);
  strcpy(((PQR*)udp)->resultList[i]->emerg1->contact->phone_w, c_vals[18]);
  strcpy(((PQR*)udp)->resultList[i]->emerg1->contact->phone_c, c_vals[19]);
  strcpy(((PQR*)udp)->resultList[i]->emerg1->contact->email, c_vals[20]);
  strcpy(((PQR*)udp)->resultList[i]->emerg2->full_name, c_vals[21]);
  strcpy(((PQR*)udp)->resultList[i]->emerg2->relationship, c_vals[22]);
  strcpy(((PQR*)udp)->resultList[i]->emerg2->contact->phone_h, c_vals[23]);
  strcpy(((PQR*)udp)->resultList[i]->emerg2->contact->phone_w, c_vals[24]);
  strcpy(((PQR*)udp)->resultList[i]->emerg2->contact->phone_c, c_vals[25]);
  strcpy(((PQR*)udp)->resultList[i]->emerg2->contact->email, c_vals[26]);
  strcpy(((PQR*)udp)->resultList[i]->pid, c_vals[27]);

  ((PQR*)udp)->count++;

  return 0;
}

int Patient_select(const PQR *pqr, char *last)
{
  int selection, i;
  size_t nbytes = 4;

  if(pqr->count == 1) {
    selection = 0;
  } else {
    printf("\n\n"
	   "Multiple results found for query \"%s\".\n"
	   THIN_LINE
	   , last);
    for (i = 0; i < pqr->count; i++) {
      printf("[# %d] ", i);
      Patient_print_search_result(pqr->resultList[i]);
    }
    printf("\n"
	   "Please enter the selection number for the correct patient below.\n"
	   "::> ");
    modgetlatoi(&selection, &nbytes);
  }
  
  return selection;
}
// eof: database.c
