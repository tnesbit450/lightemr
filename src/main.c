#include <string.h>
#include "interface.h"
#include "patient.h" 
#include "patient_add.h"
#include "database.h"

#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE 0
#endif

typedef int BOOL;

int main()
{
  ssize_t rc = 0; // return value
  size_t nbytes = 2; // for two chars max ('n' + '\x')
  BOOL EXIT = FALSE;
  char *selection = malloc(sizeof(char) * 2);
  sqlite3 *db; 
  char *query;
  char *sqlError = "SQLite3 Error!";
  // Plans at present are to only access one patient at
  // any given time. Single pointer will suffice.
  Patient *pt = NULL;

  rc = sqlite3_open("patients.db", &db);
  Database_validate(rc);
  Patient_demographics_table_create(db);

  do {
    Display_main_menu();
    rc = getline(&selection, &nbytes, stdin);
    check(rc != 0, "Error acquiring input.\n"
	  "Was your input a numeric digit?\n"
	  "The program will now exit.\n");

    // access the first character only, omiting
    // any escape characters such as \n in this
    // two char selection.
    switch(selection[0]) {
    case '1':
      Display_patient_lookup_menu();
      if(pt) Patient_destroy(pt);
      pt = Patient_lookup_mrn("2", db);
      Patient_print_info(pt);
      break;
      
    case '2':
      Display_patient_add_menu();
      if(pt) Patient_destroy(pt);
      pt = Add_patient();
      query = Create_add_user_query(pt);
      rc = sqlite3_exec(db, query, NULL, 0, &sqlError);
      break;
      
    case '3':
      // Presently only removing active
      // patient. Needs to remove from data-
      // base. No database implmented yet.
      Display_patient_remove_menu();
      if(pt) {
	Patient_destroy(pt);
	printf("Patient has been removed.\n\n");
	pt = NULL;
      } else {
	printf("No active patients.\n\n");
      }
      break;
      
    case '4':
      // empty
      Display_help_menu();
      break;
      
    case '5':
      // empty
      Display_clinical_tools_menu();
      break;
      
    case '6':
      // empty
      Display_billing_menu();
      break;

    case 'q':
    case 'Q':
    case '7':
      // Waits for exit status to be
      // TRUE then cleans up the heap
      // and exits. 
      EXIT = Display_confirm_exit();
      Evaluate_exit_signal(EXIT);
      if(EXIT){
	if(selection) free(selection);
	if(pt) Patient_destroy(pt);
	sqlite3_close(db);
	return 0;
      }
      break;
      
    default:
      Display_default_warning(selection[0]);
      break;
    }
    
    Display_confirm_continue();
    
  } while (!EXIT);
  
 error:
  return -1;
}
