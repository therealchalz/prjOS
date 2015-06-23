/*------------------------------------------------------------------
 * cli.c -- command line interpreter
 *
 * Copyright (c) 2009 by B. Berry
 * All rights resevered. 
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *------------------------------------------------------------------
 * This file modified by Charles Hache <chalz@member.fsf.org> for the
 * prjOS project.
 */

#include <inttypes.h> 
#include <prjOS/include/cli/baseCli.h>
#include <prjOS/include/cli/serialUI.h>
#include <prjOS/include/bwio.h>
#include <prjOS/include/syscall.h>

/** 
 * cli -- command line interpreter
 *  
 * DESCRIPTION
 *    command line interpreter (cli)  provides a directory
 *    hierarchy of commands similar in concept to the linux
 *    directory structure.  There exists the concept of a root
 *    directory and subdirectory.  Subdirectories can be created
 *    under root or under other subdirectories.  Commands can
 *    be created under root or any subdirectory.
 *
 *    When the user enters a command, the matching command 
 *    structure is located from the registered commands
 *    and the associated call-back function is invoked to 
 *    execute the command.
 * 
 *    When the user enters a directory name to change directories,
 *    the cli will change to that directory and display the
 *    contents.  This new directory becomes the present
 *    working directory (pwd).
 *
 *    User call backs are invoked with the (argc, *argv[]) parameters.
 *    argv[0] is the command string while argv[1,2,...,ARGC_MAX] are
 *    command specific parameters.  
 *
 *    There are several built-in commands for directory navigation:
 *    h    help  
 *    ~    moves to the top directory, root
 *    ..   moves the directory up one level
 *    ld   lists both commands and directories
 *    !    (bang) global command execution  
 *    ls   global list of commands 
 *
 *    Optional built in directories 
 *      cli_config_dir - Commands to manage configuration.  
 *      cli_show_dir - Commands to show internals: configuration, data, etc.  
 *      cli_clear_dir - Commands to clear counters, runtime data, etc. 
 *      cli_opertion_dir - Commands to manage the runtime operation. 
 *      cli_debug_dir - Commands to facilitate debugging. 
 * 
 * PUBLIC FUNCTIONS  
 *     cli_mkcmd       - add a command to a directory
 *     cli_mkdir       - add a subdirectory to directory
 *     cli_engine      - command processing engine  
 *     cli_init        - initialization
 * 
 * 
 */

/*
 * #define CLI_DEBUG  ( 1 )
 */
//#define CLI_DEBUG  ( 1 )


#ifdef CLI_DEBUG  
#define DEBUG_PRINTF(myargs...)   cliprintf(myargs)
#else
#define DEBUG_PRINTF(myargs...)  ((void)0)
#endif


/*
 * The maximum number of entries displayed on a single line
 * is ENTRIES_PER_LINE. If there are more than ENTRIES_PER_LINE,
 * a newline is injected to avoid line wrap.  Useful for small
 * screens. 
 */
#define ENTRIES_PER_LINE   ( 3 )

/*
 * navigation commands
 */
#define CLI_H             "h"       /* h => help */
#define CLI_HELP          "help"    /* help */
#define CLI_QUESTION      "?"       /* help */
#define CLI_UP_ONE        ".."      /* move up one directory level */
#define CLI_ROOT_DIR      "~"       /* move to root directory  */
#define CLI_LIST_DIR      "ld"      /* list the current directory  */
#define CLI_GLOBAL_BANG   "!"       /* global execution */
#define CLI_GLOBAL_LIST   "ls"      /* global command list   */

static void cliprintf(cli_internal_data_t* data, const char *format, ...) {
	char out[128];
	register uint32_t *varg = (uint32_t *)(&format);
	bwsprintf2(out, format, varg);
	prjPutStr(out, data->serialDriverTid);
}

static void cliredraw(cli_internal_data_t* data) {
	serialUIMessage message;
	message.messageType = SERIAL_UI_MESSAGE_TYPE_REDRAW;
	char dummy;
	prjSend(data->serialUITid, (uint8_t*)&message, sizeof(message),
					&dummy, 1);
}

static void
cli_help (cli_internal_data_t* data)
{
	cliprintf(data, "\r\n");
	cliprintf(data, "  Built-in commands \r\n");
	cliprintf(data, "     h   help \r\n");
	cliprintf(data, "     ~   Returns to the top or root command directory \r\n");
	cliprintf(data, "     ..  Moves the from the current directory up one directory \r\n");
	cliprintf(data, "     ld  current directory listing  \r\n");
	cliprintf(data, "     !   (bang) global command seek and execute \r\n");
    cliprintf(data, "     ls  Global list of commands \r\n");
    cliprintf(data, "  All commands followed by <enter> \r\n");

    cliprintf(data, "\r\n");
    return;
}



static char
str_exact (char *p1, char *p2, uint16_t n)
{
    while (*p1) {
       if (*p1 == *p2) {
           p1++;
           p2++;
       } else {
           return 0;
       }
    }

    if (strlen((const char*)p1) == strlen((const char *)p2)) {
        return 1;
    } else {
        return 0;
    }
}

/*
 * NAME 
 *    cli_bind_entry
 * 
 * SYNOPSIS
 *    #include "cli.h" 
 *    static RC_CLI_t
 *    cli_bind_entry(char *name,
 *            cli_funcvar_t fv,
 *            cli_record_t *dir_rec,
 *            cli_record_t *cmd_rec)
 *
 * DESCRIPTION
 *    This is an internal function to add a command/directory 
 *    into the specified chain.  To add an entry into the 
 *    root directory, specify NULL for the directory pointer.
 *
 * INPUT PARAMETERS
 *    name           the name of the user command.
 *
 *    fv             a pointer to the target function to be invoked
 *                   when the command is entered.  If NULL then
 *                   the name is considered to be a directory.
 *
 *    dir_rec        a pointer to the directory record
 *
 *    cmd_rec        a pointer to the command record to be added
 *
 * RETURN VALUE
 *    RC_CLI_OK
 *    error otherwise
 * 
 */
static RC_CLI_t
cli_bind_entry (cli_internal_data_t* data,
			char *name,
             cli_funcvar_t fv,
             cli_record_t *dir_rec,
             cli_record_t *cmd_rec)
{
    cli_record_t  *p2list;
    cli_record_t  *p2parent_directory;

    if (name == 0) {
        return (RC_CLI_NULL_NAME);
    }

    if (cmd_rec == 0) {
        return (RC_CLI_NULL_CMD);
    }

    if (dir_rec == 0) {
        p2list = &data->cli_root_element;
    } else {
        p2list = dir_rec;
    }

    p2parent_directory = p2list;


    /*
     * can not re-use a record
     */
    if (cmd_rec == p2list) {
        return (RC_CLI_RECORD_CORRUPT);
    }

    /*
     * check if the directory is empty to add the
     * first entry
     */
    if (p2list->link2subdir == 0) {
        p2list->link2subdir = cmd_rec;

        DEBUG_PRINTF(data, "\"%s\" is the First entry in directory [%s]\r\n",
                  name, p2list->name );

    } else {
        /*
         * Directory is not empty, so walk the list to
         * check for duplicates and append the entry.
         */

        p2list = p2list->link2subdir;

        while (p2list->next_entry != 0)  {
            /*
             * can not re-use a record
             */
            if (cmd_rec == p2list) {
                return (RC_CLI_RECORD_CORRUPT);
            }

            /*
             * do not allow duplicate names within the same subdirectory
             */
            if (str_exact(p2list->name, name, CLI_ENTRY_LEN)) {
                return (RC_CLI_DUPLICATE);
            }

            p2list = p2list->next_entry;
        }

        /*
         * Now we can append the new entry.
         */
        p2list->next_entry = cmd_rec;

        DEBUG_PRINTF(data, "Appended \"%s\" in directory [%s]\r\n",
                name, dir_rec->name);
      }

    /*
     * setup the new entry record
     */
    strcpy(cmd_rec->name, (const char*)name);
    cmd_rec->name[CLI_ENTRY_LEN-1] = '\0';
    cmd_rec->parent_dir   = p2parent_directory;
    cmd_rec->next_entry   = 0;
    cmd_rec->link2subdir  = NULL;
    cmd_rec->fv           = fv;

    if (cmd_rec->fv) {
        cmd_rec->cmd_list     = data->list;
        data->list                  = cmd_rec;
    }
    
    return (RC_CLI_OK);
}


/** 
 * NAME
 *    cli_mkcmd
 * 
 * SYNOPSIS
 *    #include "cli.h"
 *    RC_CLI_t
 *    cli_mkcmd(char  *name,
 *          cli_funcvar_t fv,
 *          cli_record_t *dir_rec,
 *          cli_record_t *cmd_rec)
 *
 * DESCRIPTION
 *    Add the new command into the specified directory.
 *    To add a command into the root directory,
 *    specify NULL for the directory pointer.
 *
 * INPUT PARAMETERS
 *    name            pointer to the command name.
 *
 *    fv              pointer to the target function to be invoked
 *                    when the command is entered.
 *
 *    dir_rec         pointer to the directory record where the
 *                    new command is to be added.  Use NULL for root.
 *
 *    cmd_rec         pointer to the new command record.
 *
 * RETURN VALUE
 *    RC_CLI_OK
 *    error otherwise
 *
 * EXAMPLES  
 *    The examples below add three commands to my_directory.
 *
 *      rcode = cli_mkcmd ( "command_one",   funct_one,
 *                            &my_directory_record, &cmd_one_record );
 *      rcode = cli_mkcmd ( "command_two",   funct_two,
 *                            &my_directory_record, &cmd_two_record );
 *      rcode = cli_mkcmd ( "command_three", funct_three,
 *                            &my_directory_record, &cmd_three_record );
 * 
 */
RC_CLI_t
cli_mkcmd (cli_internal_data_t* data,
		char *name,
       cli_funcvar_t fv,
       cli_record_t *dir_rec,
       cli_record_t *cmd_rec)
{
    RC_CLI_t rc;

    if (fv == 0) {
        return (RC_CLI_NULL_FV);
    }

    /*
     * bind into ROOT if directory not specified
     */
    if (dir_rec == 0) {
        dir_rec = &data->cli_root_element;
    }

    /*
     * you must bind only to a directory
     */
    if (dir_rec->fv != 0) {
       return (RC_CLI_DIR_INVALID);
    }

    rc = cli_bind_entry(data, name, fv, dir_rec, cmd_rec);
    return (rc);
}


/** 
 * NAME 
 *     cli_mkdir
 * 
 * SYNOPSIS
 *    #include "cli.h"
 *    RC_CLI_t
 *    cli_mkdir(char *name, cli_record_t *dir_rec,
 *              cli_record_t *sub_dir)
 *
 * DESCRIPTION
 *    Add a new directory into the specified directory.
 *    To add a new directory into the ROOT directory,
 *    specify NULL for the directory pointer.
 *
 * INPUT PARAMETERS
 *    name          pointer to the direcory name.
 *
 *    dir_rec         pointer to the directory record where the
 *                    new subdir is to be added.  Use NULL for root.
 *
 *    cmd_rec         pointer to the new directory record.
 *
 * RETURN VALUE
 *    RC_CLI_OK
 *    error otherwise
 *
 * EXAMPLES
 *    The example below adds a_directory (subdirectory) to the 
 *    root directory.  This is a visible entry.
 *
 *       rc = cli_mkdir( "a_directory", NULL,
 *                       &a_directory_record);
 *
 *
 *    The example below adds a_sub_directory (subdirectory) to the
 *    previously created directory 'a_directory_record'.
 *
 *       rc = cli_mkdir( "a_sub_directory", &a_directory_record,
 *                       &a_subdir_record);
 * 
 */
RC_CLI_t
cli_mkdir (cli_internal_data_t* data,
		char *name, cli_record_t *dir_rec,
        cli_record_t *sub_dir)
{
    RC_CLI_t rc;

    /*
     * bind into ROOT if the directory is not specified
     */
    if (dir_rec == 0) {
        dir_rec = &data->cli_root_element;
    }

    /*
     * must bind only to an existing directory
     */
    if (dir_rec->fv != 0) {
        return (RC_CLI_DIR_INVALID);
    }

    rc = cli_bind_entry(data, name, 0, dir_rec, sub_dir);
    return (rc);
}


/*
 * NAME
 *    global_command_list
 * 
 * SYNOPSIS
 *    #include "cli.h"
 *    static void
 *    global_command_list(void)
 *
 * DESCRIPTION
 *    Displays the global list of commands. 
 *
 * INPUT PARAMETERS
 *    none 
 *
 * OUTPUT PARAMETERS
 *    none 
 *
 * RETURN VALUE
 *    void
 * 
 */
static void
global_command_list (cli_internal_data_t* data)
{
    uint16_t count;
    cli_record_t *cmd_rec;
    
    count = 0;
    
    /*
     * global list of command 
     */
    cmd_rec = data->list;
    while (cmd_rec != 0) {
        if (count){ cliprintf(data, ", "); }
        
        cliprintf(data, "%s", cmd_rec->name);
        count++;
        
        /*
         * prevent line wrap
         */
        if (count >= ENTRIES_PER_LINE) {
        	cliprintf(data, "\r\n");
            count = 0;
        }
        cmd_rec = cmd_rec->cmd_list;
    }
    
    if (count != 0)
    	cliprintf(data, "\r\n");

    return;
}


/*
 * NAME
 *    list_directory
 * 
 * SYNOPSIS
 *    #include "cli.h"
 *    static void
 *    list_directory(void)
 *
 * DESCRIPTION
 *    Display the contents of the directory, both commands and
 *    subdirectories. Directories are enclosed in brackets, [..].
 *
 * INPUT PARAMETERS
 *    none 
 *
 * RETURN VALUE
 *    void
 * 
 */
static void
list_directory (cli_internal_data_t* data)
{
   uint16_t count;
   cli_record_t *cmd_tmp;

   count = 0;

    /*
     * first list all directoies
     * list directory with brackets: [ directory ]
     */
    cmd_tmp = data->current_directory->link2subdir;
    while (cmd_tmp != 0) {

        /* fv == NULL means it is a directory */
        if (cmd_tmp->fv == NULL) {
            if (count){ cliprintf(data, ", "); }

            cliprintf(data, "[%s]", cmd_tmp->name);
            count++;

            /*
             * prevent line wrap
             */
            if (count >= ENTRIES_PER_LINE) {
            	cliprintf(data, "\r\n");
                count = 0;
            }
        }
        cmd_tmp = cmd_tmp->next_entry;
    }

    /*
     * list commands
     */
    cmd_tmp = data->current_directory->link2subdir;
    while (cmd_tmp != NULL) {

        if (cmd_tmp->fv != NULL) {
            if (count){ cliprintf(data, ", "); }

            cliprintf(data, "%s", cmd_tmp->name);
            count++;

            /*
             * prevent line wrap
             */
            if (count >= ENTRIES_PER_LINE) {
            	cliprintf(data, "\r\n");
                count = 0;
            }
        }
        cmd_tmp = cmd_tmp->next_entry;
    }

    if (count != 0)
    	cliprintf(data, "\r\n");
    return;
}


/*
 * NAME 
 *    cli_find_cmd
 * 
 * SYNOPSIS
 *    #include "cli.h" 
 *    static cli_record_t
 *    *cli_find_cmd(char  *name)
 *
 * DESCRIPTION
 *     An internal function to scan the current directory for the
 *     specified entry.
 *
 * INPUT PARAMETERS
 *     name       pointer to the string to look for (null terminated).
 *
 * RETURN VALUE
 *     NULL       string NOT found
 *     ptr        Pointer to entry record if a match was found
 *
 */
static cli_record_t
*cli_find_cmd (cli_internal_data_t* data, char  *name)
{
    cli_record_t  *cmd_rec;

    //DEBUG_PRINTF("\nAttempting to Find \"%s\" in directory [%s]\n",
    //        name, current_directory->name );

    /*
     * check for global command search  
     */
    if (*name == '!') {
        DEBUG_PRINTF(data, "\nGlobal command search! \r\n");

        name++;
        cmd_rec = data->list;
        while (cmd_rec != 0) {
            DEBUG_PRINTF(data, "\nglobal comparing \"%s\" with entry \"%s\"\r\n",
                      name, cmd_rec->name);

            if (str_exact(cmd_rec->name, name, CLI_ENTRY_LEN)) {
                DEBUG_PRINTF(data, "\nGlobal match found for -%s- \r\n", name);
                return (cmd_rec);
            }
            cmd_rec = cmd_rec->cmd_list;
        }

        cliprintf(data, "\n*Unknown entry \"%s\"\r\n", name);
        return (0);
    }

    /*
     * now compare to existing command and directory entries
     */
    cmd_rec = data->current_directory->link2subdir;
    while (cmd_rec != NULL) {
        DEBUG_PRINTF(data, "\ncomparing \"%s\" with entry \"%s\"\r\n",
                      name, cmd_rec->name);

        if (str_exact(cmd_rec->name, name, CLI_ENTRY_LEN)) { 
            DEBUG_PRINTF(data, "\nMatch found for -%s- \r\n", name);
            return (cmd_rec);
        }
        cmd_rec = cmd_rec->next_entry;
    }

    /*
     * check to see if the user wants to go back one directory
     */
    if (str_exact(name, CLI_UP_ONE, 2)) { 
        DEBUG_PRINTF(data, "\nGo up one dir level \r\n");
        data->current_directory = data->current_directory->parent_dir;
        return (0);
    }

    /*
     * check to see if user wants to go to the root directory
     */
    if (str_exact(name, CLI_ROOT_DIR, 1)) { 
        DEBUG_PRINTF(data, "\nGo to root \r\n");
        data->current_directory = &data->cli_root_element;
        return (0);
    }

    /*
     * check to see if user wants to list entries
     */
    if (str_exact(name, CLI_LIST_DIR, 2)) { 
        DEBUG_PRINTF(data, "\nList directory \r\n");
        list_directory(data);
        return (0);
    }

    if (str_exact(name, CLI_GLOBAL_LIST, 2)) { 
        DEBUG_PRINTF(data, "\nGlobal List \r\n");
        global_command_list(data);
        return (0);
    }

    if (str_exact(name, CLI_H, 1) ||  
        str_exact(name, CLI_HELP, 4) ||  
        str_exact(name, CLI_QUESTION, 1)) {
        cli_help(data);
        return (NULL);
    }

    /* did not find a match */
    cliprintf(data, "\n*Unknown entry \"%s\"\r\n", name);
    return (0);
}


/** 
 * NAME 
 *    cli_engine 
 * 
 * SYNOPSIS
 *    #include "cli.h"
 *    void
 *    cli_engine(char *input_string)
 *
 * DESCRIPTION
 *    The CLI engine.
 *
 * INPUT PARAMETERS
 *    input_string - The string as entered by the user.  The cli
 *                   processor will use the first token to match a
 *                   valid directory or command entry.  If the
 *                   entry matches with a directory, the current
 *                   directory is changed to that directory.
 *
 *                   If the entry is a command, the remainder of the
 *                   string is passed to the user function.  The user
 *                   function can then continue parsing token.
 *
 * RETURN VALUE
 *    None 
 *
 * :
 *    For example, a simple CLI process.
 *     for(;;) {
 *         gets(input_string);
 *         cli_engine(input_string);
 *     }
 * 
 */
void
cli_engine (cli_internal_data_t* data, char *input_string)
{
    cli_record_t *next_level;
    uint32_t i;
    char *p2str;

    uint32_t argc;
    char *argv[ARGC_MAX];

    if ((input_string == 0) || (*input_string == '\0')) {
        DEBUG_PRINTF(data, "\r\nEmpty string entered\r\n");
        cliredraw(data);
        //cliprintf(data, "\n%s >", cli_prompt);
        //fflush(stdout);
        return;
    }

    for (i=0; i<ARGC_MAX; i++) {
       argv[i] = 0;
    }

    /*
     * Tokenize the user input into std C syntax
     * argv[0] - dir or command name
     * argv[1] - first parm
     *  ....
     */
    p2str = input_string;
    char* lasts;
    argv[0] = strtok_r(p2str, ", \t\n\r", &lasts);

    if (argv[0] == '\0') {
        DEBUG_PRINTF(data, "\nEmpty keyword\r\n");
        cliredraw(data);
        //cliprintf(data, "\n%s >", cli_prompt);
        //fflush(stdout);
        return;
    } 

    DEBUG_PRINTF(data, "\n0 input token \"%s\", 0x%x\r\n", argv[0], argv[0]);

    /*
     * now pick off parameters.
     */
    argc = 1;

    for (i=1; i<ARGC_MAX; i++) {

        argv[i] = strtok_r(NULL, " \t\n", &lasts);
        if (argv[i] == NULL) {
            break;
        } else {
            argc++;

            DEBUG_PRINTF(data, "\n%u input token \"%s\", 0x%x\r\n", i, argv[i], argv[i]);
        }
    }

    /*
     * Search for the entry in current list
     */
    next_level = cli_find_cmd(data, argv[0]);
    if (next_level) {
        /*
         * Found a matching entry.
         * If there is a function associated with this entry,
         * then invoke it, else it is a directory.
         */
        if (next_level->fv != NULL) {
            /*
             * call users registered function
             */
            (next_level->fv)(argc, &argv[0]);

        } else {
            /*
             * change to the subdirectory
             */
            data->current_directory = next_level;
        }
    } else {
        /* Did not find a match */
    }

    cliredraw(data);
    //cliprintf("\n%s >", cli_prompt);
    //fflush(stdout);
    return;
}



/** 
 * NAME
 *    cli_set_prompt
 *
 * SYNOPSIS
 *    #include "cli.h"
 *    void
 *    cli_set_prompt(char *prompt)
 *
 * DESCRIPTION
 *    This function is used to set a prompt string.
 *
 * INPUT PARAMETERS
 *    prompt - pointer to the string prompt
 *
 * RETURN VALUE
 *    none
 * 
 */
void
cli_set_prompt (cli_internal_data_t* data, char *prompt)
{
    if (prompt) {
        strcpy(data->cli_prompt, (const char*)prompt);
        data->cli_prompt[CLI_PROMPT_LEN-1] = '\0';
    } 

    return;
}



/** 
 * NAME 
 *    cli_init
 * 
 * SYNOPSIS
 *    #include "cli.h"
 *    RC_CLI_t
 *    cli_init (cli_init_data_t *init_data)
 *
 * DESCRIPTION
 *    This function initializes the CLI facility. All the 
 *    parameters listed in init_data must be provided.
 *
 * INPUT PARAMETERS
 *    init_data    pointer to initialization information
 *                 prefix - string to be displayed at the
 *                          root directory
 *
 * RETURN VALUE
 *    RC_CLI_OK
 *    error otherwise
 * 
 */
RC_CLI_t
cli_init (cli_init_data_t *init_data, cli_internal_data_t* data)
{
    RC_CLI_t rc;

    data->serialDriverTid = init_data->serialDriverTid;
    data->serialUITid = init_data->serialUITid;

    strcpy(data->cli_prompt, "prompt");
    data->cli_prompt[CLI_PROMPT_LEN-1] = '\0';

    /*
     * initialize the ROOT directory
     */
    strcpy(data->cli_root_element.name, (const char*)init_data->prefix);

    data->cli_root_element.name[CLI_ENTRY_LEN-1] = '\0';
    data->cli_root_element.fv           = 0;
    data->cli_root_element.parent_dir   = &data->cli_root_element;
    data->cli_root_element.next_entry   = 0;
    data->cli_root_element.link2subdir  = 0;

    /* mark end of the command list */
    data->list = 0;

    data->current_directory = &data->cli_root_element;
    return (RC_CLI_OK);
}

