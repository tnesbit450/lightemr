// file: database.c

#include "database.h"

/********************************************************
 ********************************************************
          NOTES TABLE AND LOOKUP
 ********************************************************
 *******************************************************/
// Generate an SQL command to create a table with
// fields mrn, title, author, time, replaced, and text.
// NOTE: the 'replaced' field is meant to effectively
// rended a field deactivated. There will be no
// updates to notes. They will only be replaced with
// new notes.
int Patient_notes_table_create(sqlite3 *db)
{
  int rc;
  char *error = "ERROR";

  char *sql = "CREATE TABLE NOTES(" \
    "MRN INTEGER NOT NULL," \
    "TITLE CHAR(50) NOT NULL, " \
    "AUTHOR CHAR(80) NOT NULL, " \
    "TIME CHAR(40) NOT NULL, " \
    "TIME_SEC INTEGER NOT NULL, " \
    "REPLACED INTEGER NOT NULL, " \
    "TEXT CHAR(8000)" \
    ");";

  rc = sqlite3_exec(db, sql, NULL, NULL, &error);

  // The input and output here would likely benefit
  // from development of a 'log' rather than sending this
  // to stdout/stderr.
  if(rc != SQLITE_OK) {
    fprintf(stderr, "SQL Error: %s\n", error);
  } else {
    fprintf(stdout, "Table created.\n");
  }

  // Clean up the sqlite3 query/command
  sqlite3_free(error);

  // Return the sqlite3_exec return value
  return rc;
}

char *Create_add_note_query(Note *n)
{
  // Allocate memory on heap for the query we're
  // returning as a string.
  char *query = malloc(sizeof(char) * MAX_QUERY);
  char timeStr[MAX_DATA];
  int i;
  
  CLEAR_STRING(query, i, MAX_QUERY);
  strcat(query, "INSERT INTO NOTES VALUES('");
  strcat(query, n->mrn);
  strcat(query, "', '");
  strcat(query, n->title);
  strcat(query, "', '");
  strcat(query, n->author);
  strcat(query, "', '");
  strcat(query, n->time);
  strcat(query, "', '");
  CLEAR_STRING(timeStr, i, MAX_DATA);
  sprintf(timeStr, "%lld",(long long)n->time_sec);
  strcat(query, timeStr);
  strcat(query, "', '");
  strcat(query, n->replaced);
  strcat(query, "', '");
  strcat(query, n->text);
  strcat(query, "');");

  return query;
}


/********************************************************
 ********************************************************
       PATIENT DEMOGRAPHICS TABLE AND LOOKUP
 ********************************************************
 *******************************************************/
// Create a sqlite statement which creates a table
// containing all fields from the "Patient" struct.
int Patient_demographics_table_create(sqlite3 *db)
{
  int rc;
  char *error = "ERROR";

  // The SQL statement to add patient 'p' to database
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

  // Execute the query
  rc = sqlite3_exec(db, sql, NULL, NULL, &error);

  // To do: send to a log instead of stderr/stdout
  if (rc != SQLITE_OK) {
    fprintf(stderr, "SQL Error: %s\n", error);
  } else {
    fprintf(stdout, "Table created.\n");
  }

  // Clean up the sqlite query/statement
  sqlite3_free(error);
  
  return rc;
}

// Create a statement for adding patient 'p' to
// the database.
char *Create_add_user_query(Patient *p)
{
  // Allocate heap memory
  char *query = malloc(sizeof(char) * MAX_QUERY);
  
  // Turn the day / mo / year values into strings
  // so they can be placed into the statement
  char day[2]; sprintf(day, "%d", p->dob->day);
  char month[2]; sprintf(month, "%d", p->dob->month);
  char year[4]; sprintf(year, "%d", p->dob->year);

  // Before we concatenate, make sure there is a null
  // terminating character which can be recognized by the
  // strcat function
  query[0] = '\0';
  // Generate the statement
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
///////////////////////////////////////////////////////////////////
// Patient_lookup()                                             ///
///////////////////////////////////////////////////////////////////
// Generate a query that will allow us to lookup a
// patient by the query provided.
// RETURN: return's a pointer to a Patient object on the heap
///////////////////////////////////////////////////////////////////
Patient *Patient_lookup(sqlite3 *db, char *identifier, char *field)
{
  PQ_node *p_head = PQ_node_alloc();
  PQ_node *p_tmp = NULL;
  Patient *p_pt = NULL;
  char *sqlQuery = NULL;
  char *error = "Sqlite3 ERROR.";
  int rc, selection;

  // Create query, run query, free sqlQuery from mem when done
  sqlQuery = Create_patient_lookup_query(identifier, field);
  rc = sqlite3_exec(db, sqlQuery, Patient_find_callback, p_head, &error);
  free(sqlQuery);
  sqlite3_free(error); // cleanup the sqlite3 error message

  // Remove the dummy patient that is returned with each patient_find_callback
  // If there is no 'next', then no dummy was returned, and likely there is
  // an error so list is purged and null returned.
  if(p_head->next && rc == 0) {
    PQ_list_pop(p_head);
  } else {
    PQ_list_purge(p_head);
    p_pt = NULL;
    // Would prefer to move this print statement outside of this
    // file/function. #TODO
    fprintf(stdout, "Identifier \"%s\" was not found.\n", identifier);
    return p_pt;
  }

  // Get the users selection of correct patient
  selection = Process_lookup_results(p_head, identifier);

  // if patient is found by list, as identified by selection #, then
  // copy it to new pt before destroying returned list.
  for(p_tmp = p_head; p_tmp->next; p_tmp = p_tmp->next) {
    if(p_tmp->count == selection && p_tmp->pt) p_pt = Patient_copy(p_tmp->pt);
  }
  if(p_tmp->count == selection && p_tmp->pt) p_pt = Patient_copy(p_tmp->pt);

  // Destroy the linked list of patients (a PQ_node)
  PQ_list_purge(p_head);
    
  return p_pt;
}

// Returns the query for a patient lookup
char *Create_patient_lookup_query(char *identifier, char *field)
{
  char *sql = malloc(sizeof(char) * MAX_QUERY);
  
  // Create the query
  sql[0] = '\0';
  strcat(sql, "SELECT * FROM PATIENTS WHERE LOWER(");
  strcat(sql, field);
  strcat(sql, ") = LOWER('");
  strcat(sql, identifier);
  strcat(sql, "');\0");

  return sql;
}

/*******************************************************************
                 PATIENT FIND CALLBACK FUNCTION
 ******************************************************************
 This function is executed inside of sql3_exec for EACH row in the
 database. c_num is the number of columns, *c_vals[] is a pointer
 to a list of 'c_num' values as extracted from columns in that row
 and *c_names[] is a similar list but which represents the names
 for the column headers.

 RETURN: 0 for success, -1 for failure to get patient from database

 ******************************************************************/
int Patient_find_callback(void *udp, int c_num, char *c_vals[], char *c_names[])
{
  PQ_node *tail = PQ_list_find_tail((PQ_node*)udp);

  // Copy the values from each column into their respective
  // fields in the PQ_node *list nodes.
  strcpy(tail->pt->mrn, c_vals[0]);
  strcpy(tail->pt->name->first, c_vals[1]);
  tail->pt->name->first[0] = toupper(tail->pt->name->first[0]);
  strcpy(tail->pt->name->middle, c_vals[2]);
  tail->pt->name->middle[0] = toupper(tail->pt->name->middle[0]);
  strcpy(tail->pt->name->last, c_vals[3]);
  tail->pt->name->last[0] = toupper(tail->pt->name->last[0]);
  tail->pt->dob->month = atoi(c_vals[4]); // Recall: mo/day/yr are ints
  tail->pt->dob->day = atoi(c_vals[5]);   // Recall: mo/day/yr are ints
  tail->pt->dob->year = atoi(c_vals[6]);  // Recall: mo/day/yr are ints
  strcpy(tail->pt->addr->field1, c_vals[7]);
  strcpy(tail->pt->addr->field2, c_vals[8]);
  strcpy(tail->pt->addr->field3, c_vals[9]);
  strcpy(tail->pt->addr->field4, c_vals[10]);
  strcpy(tail->pt->contact->phone_h, c_vals[11]);
  strcpy(tail->pt->contact->phone_w, c_vals[12]);
  strcpy(tail->pt->contact->phone_c, c_vals[13]);
  strcpy(tail->pt->contact->email, c_vals[14]);
  strcpy(tail->pt->emerg1->full_name, c_vals[15]);
  strcpy(tail->pt->emerg1->relationship, c_vals[16]);
  strcpy(tail->pt->emerg1->contact->phone_h, c_vals[17]);
  strcpy(tail->pt->emerg1->contact->phone_w, c_vals[18]);
  strcpy(tail->pt->emerg1->contact->phone_c, c_vals[19]);
  strcpy(tail->pt->emerg1->contact->email, c_vals[20]);
  strcpy(tail->pt->emerg2->full_name, c_vals[21]);
  strcpy(tail->pt->emerg2->relationship, c_vals[22]);
  strcpy(tail->pt->emerg2->contact->phone_h, c_vals[23]);
  strcpy(tail->pt->emerg2->contact->phone_w, c_vals[24]);
  strcpy(tail->pt->emerg2->contact->phone_c, c_vals[25]);
  strcpy(tail->pt->emerg2->contact->email, c_vals[26]);
  strcpy(tail->pt->pid, c_vals[27]);

  PQ_node_add(tail);

  // If the mrn isn't blank (initialized but not set),
  // return 0 for success otherwise -1 
  return (strcmp(((PQ_node*)udp)->pt->mrn, "") == 0 ? -1 : 0);
}

/************************************************************************
              PROCESS LOOKUP RESULTS ()
************************************************************************

 PURPOSE: When multiple results are passed back to the PQ_node list through
 the patient callback, this function is used to allow the user to discriminate
 between them and select their intended patient from that list of candidates.

 RETURN: returns an integer which is associated with the position of the
 pointer to the Patient of interest in the list of the Patient object
 pointers.

***********************************************************************/
int Process_lookup_results(PQ_node *head, char *identifier)
{
  int selection;
  PQ_node *tail = PQ_list_find_tail(head);

  if (tail == head) {
    selection = head->count;
    head->selected = TRUE;
  } else {
    selection = Lookup_result_selection(head, identifier);
  }

  return selection;
}

int Process_note_lookup_results(NQ_node *head, char *identifier)
{
  int selection;
  NQ_node *tail = NQ_list_find_tail(head);

  if (tail == head) {
    selection = head->count;
    head->selected = TRUE;
  } else {
    selection = Lookup_notes_result_selection(head, identifier);
  }

  return selection;
}

/**************************************************
 Patient lookup query linked list implementation
 **************************************************/
// Return a newly allocated (P)atient(Q)uery node.
PQ_node *PQ_node_alloc(void) {
  
  PQ_node *n = malloc(sizeof(PQ_node));
  n->next = NULL;
  n->prev = NULL;
  n->count = 1;
  n->selected = FALSE;

  n->pt = Patient_create();
  
  return n;
}

// Add a node to list; return success as TRUE
BOOL PQ_node_add(PQ_node *n) {
  PQ_node *new = PQ_node_alloc();
  PQ_node *tail = PQ_list_find_tail(n);

  tail->next = new;
  new->prev = tail;
  new->count = tail->count + 1;

  return ((new) ? TRUE : FALSE);
}

// Pop a node from tail of list; return success at TRUE
BOOL PQ_list_pop(PQ_node *n)
{
  PQ_node *tail = PQ_list_find_tail(n);
  PQ_node *newTail = tail->prev;

  if(tail->pt) {
    Patient_destroy(tail->pt);
    tail->pt = NULL;
  } else {
    return FALSE;
  }
  if (tail) {
    free(tail);
    tail = NULL;
  } else {
    return FALSE;
  }
  if (newTail) {
    newTail->next = NULL;
  }
  
  return TRUE;
}

// Find tail of list, return pointer
PQ_node *PQ_list_find_tail(PQ_node *n)
{
  PQ_node *tail = NULL;

  for(tail = n; tail->next; tail = tail->next);

  return tail;
}

// Completely destroy the PQ_node list
BOOL PQ_list_purge(PQ_node *n)
{
  PQ_node *p_head = n;
  PQ_node *p_next = NULL;
  PQ_node *p_curr = NULL;

  p_curr = p_head;
  while(p_curr->next){
    if(p_curr->next) p_next = p_curr->next; p_curr->next = NULL;
    if(p_curr->pt) Patient_destroy(p_curr->pt); p_curr->pt = NULL;
    if(p_curr) free(p_curr); p_curr = NULL;
    p_curr = p_next;
  }
  if(p_curr->pt) Patient_destroy(p_curr->pt); p_curr->pt = NULL;
  if(p_curr) free(p_curr); p_curr = NULL;

  return (!p_curr) ? TRUE : FALSE;
}

// Extract a single node from the list, return it,
// and close up the list so there are no dangling pointers
PQ_node *PQ_list_node_extract(PQ_node *n)
{
  PQ_node *p_extracted, *p_left, *p_right;
  p_extracted = p_left = p_right = NULL;

  p_extracted = n;
  if(n->prev) p_left = n->prev;
  if(n->next) p_right = n->next;

  p_left->next = p_right;
  p_right->prev = p_left;

  if(p_extracted->next) p_extracted->next = NULL;
  if(p_extracted->prev) p_extracted->prev = NULL;

  return p_extracted;  
}

// Destroy a single node from list
BOOL PQ_node_destroy(PQ_node *n)
{
  if(n->pt) Patient_destroy(n->pt);
  if(n->pt) n->pt = NULL;
  if(n) free(n);
  n = NULL;

  return (!n) ? TRUE : FALSE;
}

// Need to sort and explain these


Note *Note_lookup(sqlite3 *db, char *identifier, char *field)
{
  NQ_node *p_head = NQ_node_alloc();
  NQ_node *p_tmp = NULL;
  Note *pn = NULL;
  char *sqlQuery = Create_note_lookup_query(identifier, field);
  char *error = NULL;
  int rc = 0, selection = 0;

  rc = sqlite3_exec(db, sqlQuery, Note_find_callback,  p_head, &error);
  if(sqlQuery) free(sqlQuery); sqlQuery = NULL;
  if(error) sqlite3_free(error); error = NULL;

  if(p_head->next && rc == 0) {
    NQ_list_pop(p_head);
  } else {
    NQ_list_purge(p_head);
    pn = NULL;
    // Would prefer to move this print statement outside of this
    // file/function. #TODO
    fprintf(stdout, "Identifier \"%s\" was not found.\n", identifier);
    return pn;
  }

  selection = Process_note_lookup_results(p_head, identifier);

  // if note is found by list, as identified by selection #, then
  // copy it to new note before destroying returned list.
  for(p_tmp = p_head; p_tmp->next; p_tmp = p_tmp->next) {
    if(p_tmp->count == selection && p_tmp->note) {
      pn = Note_copy(p_tmp->note);
    }
  }
  if(p_tmp->count == selection && p_tmp->note) {
    pn = Note_copy(p_tmp->note);
  }

  // Destroy the linked list of notes (a NQ_node)
  NQ_list_purge(p_head);

  return pn;
}



int Note_find_callback(void *udp, int c_num, char *c_vals[], char *c_names[])
{
  NQ_node *tail = NQ_list_find_tail(((NQ_node*)udp));
  
  strcpy(tail->note->mrn, c_vals[0]);
  strcpy(tail->note->title, c_vals[1]);
  strcpy(tail->note->author, c_vals[2]);
  strcpy(tail->note->time, c_vals[3]);
  tail->note->time_sec = atoi(c_vals[4]);
  strcpy(tail->note->replaced, c_vals[5]);
  strcpy(tail->note->text, c_vals[6]);

  NQ_node_add(tail);
  
  return (strcmp(((NQ_node*)udp)->note->mrn, "NULL") == 0 ? -1 : 0);
}

char *Create_note_lookup_query(char *identifier, char *field)
{
  char *query = malloc(sizeof(char) * MAX_QUERY);
  int i;
  // Empty out the query string with null terms
  for (i = 0; i < MAX_QUERY; ++i) query[i] = '\0';

  // SELECT * FROM NOTES WHERE LOWER("MRN") = LOWER("1"); 
  strcat(query, "SELECT * FROM NOTES WHERE LOWER(\"");
  strcat(query, field);
  strcat(query, "\") = LOWER(\"");
  strcat(query, identifier);
  strcat(query, "\");");	 
  
  return query;
}

NQ_node *NQ_node_alloc()
{
  NQ_node *n = malloc(sizeof(NQ_node));
  n->next = NULL;
  n->prev = NULL;
  n->count = 1;
  n->selected = FALSE;

  n->note = Note_create("NULL");  

  return n;
}

NQ_node *NQ_list_find_tail(NQ_node *n)
{
  NQ_node *tail;

  for (tail = n; tail->next; tail = tail->next);

  return tail;
}

BOOL NQ_node_add(NQ_node *n)
{
  NQ_node *tail = NQ_list_find_tail(n);
  NQ_node *new_node = NQ_node_alloc();
  tail->next = new_node;
  new_node->prev = tail;
  new_node->count = tail->count + 1;
  new_node->next = NULL;
    
  return ((new_node) ? TRUE : FALSE);
}

BOOL NQ_list_pop(NQ_node *n)
{
  NQ_node *tail = NQ_list_find_tail(n);
  NQ_node *newTail = (tail->prev) ? tail->prev : NULL;;

  if(tail->note) {
    Note_destroy(tail->note);
    tail->note = NULL;
  } else {
    return FALSE;
  }
  if(tail) {
    free(tail);
    tail = NULL;
  } else {
    return FALSE;
  }
  if(newTail) {
    newTail->next = NULL;
  }
  
  return TRUE;
}

BOOL NQ_list_purge(NQ_node *n)
{
  NQ_node *p_head = n;
  NQ_node *p_next = NULL;
  NQ_node *p_curr = NULL;

  p_curr = p_head;
  while(p_curr->next){
    if(p_curr->next) p_next = p_curr->next; p_curr->next = NULL;
    if(p_curr->note) Note_destroy(p_curr->note); p_curr->note = NULL;
    if(p_curr) free(p_curr); p_curr = NULL;
    p_curr = p_next;
  }
  if(p_curr->note) Note_destroy(p_curr->note); p_curr->note = NULL;
  if(p_curr) free(p_curr); p_curr = NULL;

  return (!p_curr) ? TRUE : FALSE;
}

BOOL NQ_node_destroy(NQ_node *n)
{
  if(n->note) Note_destroy(n->note);
  if(n->note) n->note = NULL;
  if(n) free(n);
  n = NULL;

  return (!n) ? TRUE : FALSE;
}


// eof: database.c
