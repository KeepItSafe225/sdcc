/*
 * Simulator of microcontrollers (cmd.cc)
 *
 * Copyright (C) 1999,99 Drotos Daniel, Talker Bt.
 * 
 * To contact author send email to drdani@mazsola.iit.uni-miskolc.hu
 *
 */

/* This file is part of microcontroller simulator: ucsim.

UCSIM is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

UCSIM is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with UCSIM; see the file COPYING.  If not, write to the Free
Software Foundation, 59 Temple Place - Suite 330, Boston, MA
02111-1307, USA. */
/*@1@*/

#include "ddconfig.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include "i_string.h"

#include "uccl.h"
#include "globals.h"

#include "dump.h"
#include "go.h"
#include "cmd_brk.h"
#include "set.h"
#include "where.h"
#include "optioncl.h"
#include "show.h"

#include "cmd.h"


/* Table is defined at the end of this file */


/*
 * HELP
 */

static bool
cmd_help(char *cmd, class cl_uc *uc, class cl_sim *sim)
{
  int i;

  i= 0;
  while (cmd_table[i].name != NULL)
    {
      if (cmd_table[i].help != NULL)
	sim->cmd->printf("%s\n", cmd_table[i].help);
      i++;
    }
  sim->cmd->printf("----\nSet of new commands:\n");
  for (i= 0; i < sim->cmdset->count; i++)
    {
      class cl_cmd *cmd= (class cl_cmd *)(sim->cmdset->at(i));
      sim->cmd->printf("%s\n", cmd->short_help);
    }
  return(DD_FALSE);
}


/*
 * GET OPTION [opt]
 */

static bool
cmd_get_option(char *cmd, class cl_uc *uc, class cl_sim *sim)
{
  char *s;
  int i;
  class cl_option *o;

  s= strtok(NULL, delimiters);
  for (i= 0; i < uc->options->count; i++)
    {
      o= (class cl_option *)(uc->options->at(i));
      if (!s ||
	  !strcmp(s, o->id))
	{
	  sim->cmd->printf("%s ", o->id);
	  o->print(sim->cmd->actual_console);
	  sim->cmd->printf(" %s\n", o->help);
	}
    }
  return(DD_FALSE);
}


/*
 * SET OPTION opt value
 */

static bool
cmd_set_option(char *cmd, class cl_uc *uc, class cl_sim *sim)
{
  char *s, *id;
  int i;
  class cl_option *o;

  if ((id= strtok(NULL, delimiters)) == NULL)
    {
      sim->cmd->printf("Name of option has not given.\n");
      return(DD_FALSE);
    }
  if ((s= strtok(NULL, delimiters)) == NULL)
    {
      sim->cmd->printf("Value has not given.\n");
      return(DD_FALSE);
    }
  for (i= 0; i < uc->options->count; i++)
    {
      o= (class cl_option *)(uc->options->at(i));
      if (!strcmp(id, o->id))
	{
	  o->set_value(s);
	  break;
	}
    }
  return(DD_FALSE);
}


/*
 * Table of commands and their names
 */

struct cmd_entry cmd_table[]=
{
  {"g" , cmd_go, DD_FALSE,
   "g [start [stop]]   Go"},

  {"stop", cmd_stop, DD_FALSE,
   "stop               Stop"},

  {"pc", cmd_pc, DD_FALSE,
   "pc [address]       Get/set content of PC"},

  {"s" , cmd_step, DD_TRUE,
   "s [step]           Step"},

  {"n" , cmd_next, DD_TRUE,
   "n [step]           Next"},

  {"bse", cmd_brk_sete, DD_FALSE,
   "bse wi|ri|wx|rx|ws|rs|rc f|d addr [hit]\n"
   "                   Set EVENT Breakpoint"},

  {"bde", cmd_brk_dele, DD_FALSE,
   "bde wi|ri|wx|rx|ws|rs|rc addr\n"
   "                   Delete EVENT Breakpoint"},
  
  {"ba", cmd_brk_delall, DD_FALSE,
   "ba                 Delete all breakpoints"},

  {"dis", cmd_disass, DD_TRUE,
   "dis [start [offset [lines]]]\n"
   "                   Disassemble code"},

  {"dp", cmd_dump_port, DD_FALSE,
   "dp                 Dump ports"},

  {"ds", cmd_dump_sfr, DD_FALSE,
   "ds [addr...]       Dump SFR"},

  {"db", cmd_dump_bit, DD_TRUE,
   "db addr...         Dump bit"},

  {"si", cmd_set_iram, DD_FALSE,
   "si addr data...    Set Internal RAM"},

  {"sx", cmd_set_xram, DD_FALSE,
   "sx addr data...    Set External RAM"},

  {"sc", cmd_set_code, DD_FALSE,
   "sc addr data...    Set code (ROM)"},
  
  {"ss", cmd_set_sfr, DD_FALSE,
   "ss addr data...    Set SFR area"},
      
  {"sb", cmd_set_bit, DD_FALSE,
   "sb addr data...    Set bit"},

  {"sp", cmd_set_port, DD_FALSE,
   "sp port data       Set port pins"},

  {"fi", cmd_fill_iram, DD_FALSE,
   "fi start stop data Fill IRAM area with `data'"},

  {"fx", cmd_fill_xram, DD_FALSE,
   "fx start stop data Fill XRAM area with `data'"},
       
  {"fs", cmd_fill_sfr, DD_FALSE,
   "fs start stop data Fill SFR area with `data'"},
	
  {"fc", cmd_fill_code, DD_FALSE,
   "fc start stop data Fill ROM area with `data'"},

  {"wi", cmd_where_iram, DD_FALSE,
   "wi,Wi string       Search for `string' in IRAM (Wi case sensitive)"},
  {"Wi", cmd_Where_iram, DD_FALSE, NULL},

  {"wx", cmd_where_xram, DD_FALSE,
   "wx,Wx string       Search for `string' in XRAM (Wx case sensitive)"},
  {"Wx", cmd_Where_xram, DD_FALSE, NULL},

  {"wc", cmd_where_code, DD_FALSE,
   "wc,Wc string       Search for `string' in ROM (Wc case sensitive)"},
  {"Wc", cmd_Where_code, DD_FALSE, NULL},
	  
  {"sopt", cmd_set_option, DD_FALSE,
   "sopt opt value     Set value of option"},

  {"gopt", cmd_get_option, DD_FALSE,
   "gopt [opt]         Get value of option(s)"},
	    
  {"show", cmd_show, DD_FALSE,
   "show c|w           Show licensing information"},
      
  {"h" , cmd_help, DD_FALSE,
   "h,?                Help about commands"},
  {"?" , cmd_help, DD_FALSE, NULL},
  
  {NULL, NULL}
};


/* End of s51.src/cmd.cc */
