#include <stdio.h>
#include <string.h>
#include "note.h"
#include "database.h"
#include "patient.h"

/*****************************
 * Similar to PQ_node, but for notes.
 ****************************/
typedef struct Note_query_node {
  int count;
  BOOL selected;
  Note *note;
  struct Note_query_node *next;
  struct Note_query_node *prev;
} NQ_node;
  

NQ_node *NQ_node_alloc(void);
Note *Note_lookup(sqlite3 *db, char *identifier, char *field);
char *Create_note_lookup_query(char *identifier, char *field);
int Note_find_callback(void *udp, int c_num, char *c_vals[], char *c_names[]);
NQ_node *NQ_list_find_tail(NQ_node *n);
BOOL NQ_node_add(NQ_node *n);
BOOL NQ_list_pop(NQ_node *n);
BOOL NQ_list_purge(NQ_node *n);
BOOL NQ_node_destroy(NQ_node *n);

int main()
{
  Note *n = Note_create("1");
  char *query = NULL;
  char *error = NULL;
  sqlite3 *db;
  int rc = 0;

  // Create a note
  strcat(n->title, "History and Physical");
  strcat(n->author, "Dr. Super Awesome");
  strcat(n->text,
	 "S: This is a 40 yr M with "
	 "DM2 here for follow up on his "
	 "insulin regimen. He has good "
	 "sugar control.\n\n"
	 "O: \n"
	 "Physical exam: \n"
	 "AAOx4, NC/AT, RRR, +BS, NT/ND\n\n"
	 "Plan:\n"
	 "# DM2: Good control\n"
	 "- continue januvia\n\0");
  // Create a note  ADD query
  query  = Create_add_note_query(n);

  // Add a note to the database
  rc = sqlite3_open("lightemr.data", &db); 
  sqlite3_exec(db, query, NULL, NULL, &error);

  // Destroy the old note
  if(n) Note_destroy(n); n = NULL;

  // Lookup the note from the database
  n = Note_lookup(db, "1", "MRN");

  // Print the results of the lookup
  fprintf(stdout,
	  "Patient MRN: %s\n"
	  "Author: %s     Title : %s\n     Time  : %s\n"
	  "Note contents\n"
	  "=============\n"
	  "%s\n",
	  n->mrn,
	  n->author,
	  n->title,
	  n->time,
	  n->text);

  // Cleanup and exit
  if(error) sqlite3_free(error); error = NULL;
  if(query) free(query); query = NULL;
  if(n) Note_destroy(n); n = NULL;
  if(db) sqlite3_close(db); db = NULL;
  
  return 0;
}

Note *Note_lookup(sqlite3 *db, char *identifier, char *field)
{
  Note *n = NULL;
  NQ_node *n_head = NQ_node_alloc();
  char *sqlQuery = Create_note_lookup_query(identifier, field);
  char *error = NULL;
  int rc;

  rc = sqlite3_exec(db, sqlQuery, Note_find_callback,  n_head, &error);
  n = n_head->note;
  if(sqlQuery) free(sqlQuery); sqlQuery = NULL;
  if(error) sqlite3_free(error); error = NULL;
  // if(n_head) NQ_list_purge(n_head);

  return n;
}

int Note_find_callback(void *udp, int c_num, char *c_vals[], char *c_names[])
{
  
  strcpy(((NQ_node*)udp)->note->mrn, c_vals[0]);
  strcpy(((NQ_node*)udp)->note->title, c_vals[1]);
  strcpy(((NQ_node*)udp)->note->author, c_vals[2]);
  strcpy(((NQ_node*)udp)->note->time, c_vals[3]);
  ((NQ_node*)udp)->note->time_sec = atoi(c_vals[4]);
  strcpy(((NQ_node*)udp)->note->replaced, c_vals[5]);
  strcpy(((NQ_node*)udp)->note->text, c_vals[6]);
  
  return 0;
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
  NQ_node *new = malloc(sizeof(NQ_node));
  new->next = NULL;
  new->prev = tail;
  new->count = tail->count + 1;
  
  return ((new) ? TRUE : FALSE);
}

BOOL NQ_list_pop(NQ_node *n)
{
  return TRUE;
}

BOOL NQ_list_purge(NQ_node *n)
{
  return TRUE;
}

BOOL NQ_node_destroy(NQ_node *n)
{
  return TRUE;
}
