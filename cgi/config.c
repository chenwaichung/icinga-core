/***********************************************************************
 *
 * CONFIG.C - Nagios Configuration CGI (View Only)
 *
 * Copyright (c) 1999-2002 Ethan Galstad (nagios@nagios.org)
 * Last Modified: 11-10-2002
 *
 * This CGI program will display various configuration information.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 ***********************************************************************/

#include "../common/config.h"
#include "../common/locations.h"
#include "../common/common.h"
#include "../common/objects.h"

#include "getcgi.h"
#include "cgiutils.h"
#include "auth.h"

extern char   main_config_file[MAX_FILENAME_LENGTH];
extern char   url_html_path[MAX_FILENAME_LENGTH];
extern char   url_docs_path[MAX_FILENAME_LENGTH];
extern char   url_images_path[MAX_FILENAME_LENGTH];
extern char   url_stylesheets_path[MAX_FILENAME_LENGTH];

extern hostgroup *hostgroup_list;
extern contactgroup *contactgroup_list;
extern command *command_list;
extern timeperiod *timeperiod_list;
extern contact *contact_list;
extern hostgroupescalation *hostgroupescalation_list;
extern servicedependency *servicedependency_list;
extern serviceescalation *serviceescalation_list;
extern hostdependency *hostdependency_list;
extern hostescalation *hostescalation_list;


#define DISPLAY_NONE                     0
#define DISPLAY_HOSTS                    1
#define DISPLAY_HOSTGROUPS               2
#define DISPLAY_CONTACTS                 3
#define DISPLAY_CONTACTGROUPS            4
#define DISPLAY_SERVICES                 5
#define DISPLAY_TIMEPERIODS              6
#define DISPLAY_COMMANDS                 7
#define DISPLAY_HOSTGROUPESCALATIONS     8
#define DISPLAY_SERVICEDEPENDENCIES      9
#define DISPLAY_SERVICEESCALATIONS       10
#define DISPLAY_HOSTDEPENDENCIES         11
#define DISPLAY_HOSTESCALATIONS          12

void document_header(int);
void document_footer(void);
int process_cgivars(void);

void display_options(void);

void display_hosts(void);
void display_hostgroups(void);
void display_contacts(void);
void display_contactgroups(void);
void display_services(void);
void display_timeperiods(void);
void display_commands(void);
void display_hostgroupescalations(void);
void display_servicedependencies(void);
void display_serviceescalations(void);
void display_hostdependencies(void);
void display_hostescalations(void);

void unauthorized_message(void);


authdata current_authdata;

int display_type=DISPLAY_NONE;

int embedded=FALSE;


int main(void){
	int result=OK;

	/* get the arguments passed in the URL */
	process_cgivars();

	/* reset internal variables */
	reset_cgi_vars();

	/* read the CGI configuration file */
	result=read_cgi_config_file(DEFAULT_CGI_CONFIG_FILE);
	if(result==ERROR){
		document_header(FALSE);
		cgi_config_file_error(DEFAULT_CGI_CONFIG_FILE);
		document_footer();
		return ERROR;
	        }

	/* read the main configuration file */
	result=read_main_config_file(main_config_file);
	if(result==ERROR){
		document_header(FALSE);
		main_config_file_error(main_config_file);
		document_footer();
		return ERROR;
	        }

	/* read all object configuration data */
	result=read_all_object_configuration_data(main_config_file,READ_ALL_OBJECT_DATA);
	if(result==ERROR){
		document_header(FALSE);
		object_data_error();
		document_footer();
		return ERROR;
                }

	document_header(TRUE);

	/* get authentication information */
	get_authentication_information(&current_authdata);


	/* begin top table */
	printf("<table border=0 width=100%%>\n");
	printf("<tr>\n");

	/* left column of the first row */
	printf("<td align=left valign=top width=50%%>\n");
	display_info_table("Configuration",FALSE,&current_authdata);
	printf("</td>\n");

	/* right hand column of top row */
	printf("<td align=right valign=bottom width=50%%>\n");

	if(display_type!=DISPLAY_NONE){

		printf("<form method=\"get\" action=\"%s\">\n",CONFIG_CGI);
		printf("<table border=0>\n");

		printf("<tr><td align=left class='reportSelectSubTitle'>Object Type:</td></tr>\n");
		printf("<tr><td align=left class='reportSelectItem'>");
		printf("<select name='type'>\n");
		printf("<option value='hosts' %s>Hosts\n",(display_type==DISPLAY_HOSTS)?"SELECTED":"");
		printf("<option value='hostdependencies' %s>Host Dependencies\n",(display_type==DISPLAY_HOSTDEPENDENCIES)?"SELECTED":"");
		printf("<option value='hostescalations' %s>Host Escalations\n",(display_type==DISPLAY_HOSTESCALATIONS)?"SELECTED":"");
		printf("<option value='hostgroups' %s>Host Groups\n",(display_type==DISPLAY_HOSTGROUPS)?"SELECTED":"");
		printf("<option value='hostgroupescalations' %s>Host Group Escalations\n",(display_type==DISPLAY_HOSTGROUPESCALATIONS)?"SELECTED":"");
		printf("<option value='services' %s>Services\n",(display_type==DISPLAY_SERVICES)?"SELECTED":"");
		printf("<option value='servicedependencies' %s>Service Dependencies\n",(display_type==DISPLAY_SERVICEDEPENDENCIES)?"SELECTED":"");
		printf("<option value='serviceescalations' %s>Service Escalations\n",(display_type==DISPLAY_SERVICEESCALATIONS)?"SELECTED":"");
		printf("<option value='contacts' %s>Contacts\n",(display_type==DISPLAY_CONTACTS)?"SELECTED":"");
		printf("<option value='contactgroups' %s>Contact Groups\n",(display_type==DISPLAY_CONTACTGROUPS)?"SELECTED":"");
		printf("<option value='timeperiods' %s>Timeperiods\n",(display_type==DISPLAY_TIMEPERIODS)?"SELECTED":"");
		printf("<option value='commands' %s>Commands\n",(display_type==DISPLAY_COMMANDS)?"SELECTED":"");
		printf("</select>\n");
		printf("</td></tr>\n");

		printf("<tr><td class='reportSelectItem'><input type='submit' value='Update'></td></tr>\n");
		printf("</table>\n");
		printf("</form>\n");
	        }

	/* display context-sensitive help */
	switch(display_type){
	case DISPLAY_HOSTS:
		display_context_help(CONTEXTHELP_CONFIG_HOSTS);
		break;
	case DISPLAY_HOSTGROUPS:
		display_context_help(CONTEXTHELP_CONFIG_HOSTGROUPS);
		break;
	case DISPLAY_CONTACTS:
		display_context_help(CONTEXTHELP_CONFIG_CONTACTS);
		break;
	case DISPLAY_CONTACTGROUPS:
		display_context_help(CONTEXTHELP_CONFIG_CONTACTGROUPS);
		break;
	case DISPLAY_SERVICES:
		display_context_help(CONTEXTHELP_CONFIG_SERVICES);
		break;
	case DISPLAY_TIMEPERIODS:
		display_context_help(CONTEXTHELP_CONFIG_TIMEPERIODS);
		break;
	case DISPLAY_COMMANDS:
		display_context_help(CONTEXTHELP_CONFIG_COMMANDS);
		break;
	case DISPLAY_HOSTGROUPESCALATIONS:
		display_context_help(CONTEXTHELP_CONFIG_HOSTGROUPESCALATIONS);
		break;
	case DISPLAY_SERVICEDEPENDENCIES:
		display_context_help(CONTEXTHELP_CONFIG_SERVICEDEPENDENCIES);
		break;
	case DISPLAY_SERVICEESCALATIONS:
		display_context_help(CONTEXTHELP_CONFIG_HOSTESCALATIONS);
		break;
	case DISPLAY_HOSTDEPENDENCIES:
		display_context_help(CONTEXTHELP_CONFIG_HOSTDEPENDENCIES);
		break;
	case DISPLAY_HOSTESCALATIONS:
		display_context_help(CONTEXTHELP_CONFIG_HOSTESCALATIONS);
		break;
	default:
		display_context_help(CONTEXTHELP_CONFIG_MENU);
		break;
	        }

	printf("</td>\n");

	/* end of top table */
	printf("</tr>\n");
	printf("</table>\n");


	switch(display_type){
	case DISPLAY_HOSTS:
		display_hosts();
		break;
	case DISPLAY_HOSTGROUPS:
		display_hostgroups();
		break;
	case DISPLAY_CONTACTS:
		display_contacts();
		break;
	case DISPLAY_CONTACTGROUPS:
		display_contactgroups();
		break;
	case DISPLAY_SERVICES:
		display_services();
		break;
	case DISPLAY_TIMEPERIODS:
		display_timeperiods();
		break;
	case DISPLAY_COMMANDS:
		display_commands();
		break;
	case DISPLAY_HOSTGROUPESCALATIONS:
		display_hostgroupescalations();
		break;
	case DISPLAY_SERVICEDEPENDENCIES:
		display_servicedependencies();
		break;
	case DISPLAY_SERVICEESCALATIONS:
		display_serviceescalations();
		break;
	case DISPLAY_HOSTDEPENDENCIES:
		display_hostdependencies();
		break;
	case DISPLAY_HOSTESCALATIONS:
		display_hostescalations();
		break;
	default:
		display_options();
		break;
	        }

	document_footer();

	return OK;
        }




void document_header(int use_stylesheet){
	char date_time[MAX_DATETIME_LENGTH];
	time_t t;

	if(embedded==TRUE)
		return;

	time(&t);
	get_time_string(&t,date_time,sizeof(date_time),HTTP_DATE_TIME);

	printf("Cache-Control: no-store\n");
	printf("Pragma: no-cache\n");
	printf("Last-Modified: %s\n",date_time);
	printf("Expires: %s\n",date_time);
	printf("Content-type: text/html\r\n\r\n");

	printf("<html>\n");
	printf("<head>\n");
	printf("<META HTTP-EQUIV='Pragma' CONTENT='no-cache'>\n");
	printf("<title>\n");
	printf("Configuration\n");
	printf("</title>\n");

	if(use_stylesheet==TRUE)
		printf("<LINK REL='stylesheet' TYPE='text/css' HREF='%s%s'>\n",url_stylesheets_path,CONFIG_CSS);

	printf("</head>\n");

	printf("<body CLASS='config'>\n");

	/* include user SSI header */
	include_ssi_files(CONFIG_CGI,SSI_HEADER);

	return;
        }


void document_footer(void){

	if(embedded==TRUE)
		return;

	/* include user SSI footer */
	include_ssi_files(CONFIG_CGI,SSI_FOOTER);

	printf("</body>\n");
	printf("</html>\n");

	return;
        }


int process_cgivars(void){
	char **variables;
	int error=FALSE;
	int x;

	variables=getcgivars();

	for(x=0;variables[x]!=NULL;x++){

		/* do some basic length checking on the variable identifier to prevent buffer overflows */
		if(strlen(variables[x])>=MAX_INPUT_BUFFER-1){
			x++;
			continue;
		        }

		/* we found the configuration type argument */
		else if(!strcmp(variables[x],"type")){
			x++;
			if(variables[x]==NULL){
				error=TRUE;
				break;
			        }

			/* what information should we display? */
			if(!strcmp(variables[x],"hosts"))
				display_type=DISPLAY_HOSTS;
			else if(!strcmp(variables[x],"hostgroups"))
				display_type=DISPLAY_HOSTGROUPS;
			else if(!strcmp(variables[x],"contacts"))
				display_type=DISPLAY_CONTACTS;
			else if(!strcmp(variables[x],"contactgroups"))
				display_type=DISPLAY_CONTACTGROUPS;
			else if(!strcmp(variables[x],"services"))
				display_type=DISPLAY_SERVICES;
			else if(!strcmp(variables[x],"timeperiods"))
				display_type=DISPLAY_TIMEPERIODS;
			else if(!strcmp(variables[x],"commands"))
				display_type=DISPLAY_COMMANDS;
			else if(!strcmp(variables[x],"hostgroupescalations"))
				display_type=DISPLAY_HOSTGROUPESCALATIONS;
			else if(!strcmp(variables[x],"servicedependencies"))
				display_type=DISPLAY_SERVICEDEPENDENCIES;
			else if(!strcmp(variables[x],"serviceescalations"))
				display_type=DISPLAY_SERVICEESCALATIONS;
			else if(!strcmp(variables[x],"hostdependencies"))
				display_type=DISPLAY_HOSTDEPENDENCIES;
			else if(!strcmp(variables[x],"hostescalations"))
				display_type=DISPLAY_HOSTESCALATIONS;

			/* we found the embed option */
			else if(!strcmp(variables[x],"embedded"))
				embedded=TRUE;
		        }

		/* we recieved an invalid argument */
		else
			error=TRUE;
	
	        }

	/* free memory allocated to the CGI variables */
	free_cgivars(variables);

	return error;
        }



void display_hosts(void){
	host *temp_host;
	hostsmember *temp_hostsmember;
	int options=0;
	int odd=0;
	char time_string[16];
	char *bg_class="";

	/* see if user is authorized to view host information... */
	if(is_authorized_for_configuration_information(&current_authdata)==FALSE){
		unauthorized_message();
		return;
	        }

	printf("<P><DIV ALIGN=CENTER CLASS='dataTitle'>Hosts</DIV></P>\n");

	printf("<P><DIV ALIGN=CENTER>\n");
	printf("<TABLE BORDER=0 CLASS='data'>\n");

	printf("<TR>\n");
	printf("<TH CLASS='data'>Host Name</TH>");
	printf("<TH CLASS='data'>Alias/Description</TH>");
	printf("<TH CLASS='data'>Address</TH>");
	printf("<TH CLASS='data'>Parent Hosts</TH>");
	printf("<TH CLASS='data'>Notification Interval</TH>");
	printf("<TH CLASS='data'>Notification Options</TH>");
	printf("<TH CLASS='data'>Notification Period</TH>");
	printf("<TH CLASS='data'>Max. Check Attempts</TH>");
	printf("<TH CLASS='data'>Host Check Command</TH>");
	printf("<TH CLASS='data'>Enable Checks</TH>");
	printf("<TH CLASS='data'>Event Handler</TH>");
	printf("<TH CLASS='data'>Enable Event Handler</TH>");
	printf("<TH CLASS='data'>Stalking Options</TH>\n");
	printf("<TH CLASS='data'>Enable Flap Detection</TH>");
	printf("<TH CLASS='data'>Low Flap Threshold</TH>");
	printf("<TH CLASS='data'>High Flap Threshold</TH>");
	printf("<TH CLASS='data'>Process Performance Data</TH>");
	printf("<TH CLASS='data'>Enable Failure Prediction</TH>");
	printf("<TH CLASS='data'>Failure Prediction Options</TH>");
	printf("<TH CLASS='data'>Retention Options</TH>");
	printf("</TR>\n");

	/* check all the hosts... */
	move_first_host();
	while(temp_host = get_next_host()) {
		if(odd){
			odd=0;
			bg_class="dataOdd";
		        }
		else{
			odd=1;
			bg_class="dataEven";
		        }

		printf("<TR CLASS='%s'>\n",bg_class);

		printf("<TD CLASS='%s'><a name='%s'>%s</a></TD>\n",bg_class,url_encode(temp_host->name),temp_host->name);
		printf("<TD CLASS='%s'>%s</TD>\n",bg_class,temp_host->alias);
		printf("<TD CLASS='%s'>%s</TD>\n",bg_class,temp_host->address);

		printf("<TD CLASS='%s'>",bg_class);
		for(temp_hostsmember=temp_host->parent_hosts;temp_hostsmember!=NULL;temp_hostsmember=temp_hostsmember->next){

			if(temp_hostsmember!=temp_host->parent_hosts)
				printf(", ");

			printf("<a href='%s?type=hosts#%s'>%s</a>\n",CONFIG_CGI,url_encode(temp_hostsmember->host_name),temp_hostsmember->host_name);
		        }
		if(temp_host->parent_hosts==NULL)
			printf("&nbsp;");
		printf("</TD>\n");

		get_interval_time_string(temp_host->notification_interval,time_string,sizeof(time_string));
		printf("<TD CLASS='%s'>%s</TD>\n",bg_class,(temp_host->notification_interval==0)?"<i>No Re-notification</I>":time_string);

		printf("<TD CLASS='%s'>",bg_class);
		options=0;
		if(temp_host->notify_on_down==TRUE){
			options=1;
			printf("Down");
		        }
		if(temp_host->notify_on_unreachable==TRUE){
			printf("%sUnreachable",(options)?", ":"");
			options=1;
		        }
		if(temp_host->notify_on_recovery==TRUE){
			printf("%sRecovery",(options)?", ":"");
			options=1;
		        }
		if(options==0)
			printf("None");
		printf("</TD>\n");

		printf("<TD CLASS='%s'>",bg_class);
		if(temp_host->notification_period==NULL)
			printf("&nbsp;");
		else
			printf("<a href='%s?type=timeperiods#%s'>%s</a>",CONFIG_CGI,url_encode(temp_host->notification_period),temp_host->notification_period);
		printf("</TD>\n");

		printf("<TD CLASS='%s'>%d</TD>\n",bg_class,temp_host->max_attempts);

		printf("<TD CLASS='%s'>",bg_class);
		if(temp_host->host_check_command==NULL)
			printf("&nbsp;");
		else
		printf("<a href='%s?type=commands#%s'>%s</a></TD>\n",CONFIG_CGI,url_encode(temp_host->host_check_command),temp_host->host_check_command);
		printf("</TD>\n");

		printf("<TD CLASS='%s'>",bg_class);
		printf("%s\n",(temp_host->event_handler_enabled==TRUE)?"Yes":"No");
		printf("</TD>\n");

		printf("<TD CLASS='%s'>",bg_class);
		if(temp_host->event_handler==NULL)
			printf("&nbsp");
		else
			printf("<a href='%s?type=commands#%s'>%s</a></TD>\n",CONFIG_CGI,url_encode(temp_host->event_handler),temp_host->event_handler);
		printf("</TD>\n");

		printf("<TD CLASS='%s'>",bg_class);
		printf("%s\n",(temp_host->event_handler_enabled==TRUE)?"Yes":"No");
		printf("</TD>\n");

		printf("<TD CLASS='%s'>",bg_class);
		options=0;
		if(temp_host->stalk_on_up==TRUE){
			options=1;
			printf("Up");
		        }
		if(temp_host->stalk_on_down==TRUE){
			printf("%sDown",(options)?", ":"");
			options=1;
		        }
		if(temp_host->stalk_on_unreachable==TRUE){
			printf("%sUnreachable",(options)?", ":"");
			options=1;
		        }
		if(options==0)
			printf("None");
		printf("</TD>\n");

		printf("<TD CLASS='%s'>",bg_class);
		printf("%s\n",(temp_host->flap_detection_enabled==TRUE)?"Yes":"No");
		printf("</TD>\n");

		printf("<TD CLASS='%s'>",bg_class);
		if(temp_host->low_flap_threshold==0.0)
			printf("Program-wide value\n");
		else
			printf("%3.1f%%\n",temp_host->low_flap_threshold);
		printf("</TD>\n");

		printf("<TD CLASS='%s'>",bg_class);
		if(temp_host->high_flap_threshold==0.0)
			printf("Program-wide value\n");
		else
			printf("%3.1f%%\n",temp_host->high_flap_threshold);
		printf("</TD>\n");

		printf("<TD CLASS='%s'>",bg_class);
		printf("%s\n",(temp_host->process_performance_data==TRUE)?"Yes":"No");
		printf("</TD>\n");

		printf("<TD CLASS='%s'>",bg_class);
		printf("%s\n",(temp_host->failure_prediction_enabled==TRUE)?"Yes":"No");
		printf("</TD>\n");

		printf("<TD CLASS='%s'>%s</TD>\n",bg_class,(temp_host->failure_prediction_options==NULL)?"&nbsp;":temp_host->failure_prediction_options);

		printf("<TD CLASS='%s'>",bg_class);
		options=0;
		if(temp_host->retain_status_information==TRUE){
			options=1;
			printf("Status Information");
		        }
		if(temp_host->retain_nonstatus_information==TRUE){
			printf("%sNon-Status Information",(options==1)?", ":"");
			options=1;
		        }
		if(options==0)
			printf("None");
		printf("</TD>\n");

		printf("</TR>\n");
	        }

	printf("</TABLE>\n");
	printf("</DIV>\n");
	printf("</P>\n");

	return;
        }



void display_hostgroups(void){
	hostgroup *temp_hostgroup;
	contactgroupsmember *temp_contactgroupsmember;
	hostgroupmember *temp_hostgroupmember;
	int odd=0;
	char *bg_class="";

	/* see if user is authorized to view hostgroup information... */
	if(is_authorized_for_configuration_information(&current_authdata)==FALSE){
		unauthorized_message();
		return;
	        }

	printf("<P><DIV ALIGN=CENTER CLASS='dataTitle'>Host Groups</DIV></P>\n");

	printf("<P>\n");
	printf("<DIV ALIGN=CENTER>\n");

	printf("<TABLE BORDER=0 CLASS='data'>\n");
	printf("<TR>\n");
	printf("<TH CLASS='data'>Group Name</TH>");
	printf("<TH CLASS='data'>Description</TH>");
	printf("<TH CLASS='data'>Default Contact Groups</TH>");
	printf("<TH CLASS='data'>Host Members</TH>");
	printf("</TR>\n");

	/* check all the hostgroups... */
	for(temp_hostgroup=hostgroup_list;temp_hostgroup!=NULL;temp_hostgroup=temp_hostgroup->next){

		if(odd){
			odd=0;
			bg_class="dataOdd";
		        }
		else{
			odd=1;
			bg_class="dataEven";
		        }

		printf("<TR CLASS='%s'>\n",bg_class);

		printf("<TD CLASS='%s'>%s</TD>",bg_class,temp_hostgroup->group_name);

		printf("<TD CLASS='%s'>%s</TD>\n",bg_class,temp_hostgroup->alias);
		printf("<TD CLASS='%s'>",bg_class);

		/* find all the contact groups for this hostgroup... */
		for(temp_contactgroupsmember=temp_hostgroup->contact_groups;temp_contactgroupsmember!=NULL;temp_contactgroupsmember=temp_contactgroupsmember->next){

			if(temp_contactgroupsmember!=temp_hostgroup->contact_groups)
				printf(", ");

			printf("<A HREF='%s?type=contactgroups#%s'>%s</A>\n",CONFIG_CGI,url_encode(temp_contactgroupsmember->group_name),temp_contactgroupsmember->group_name);
		        }
		printf("</TD>\n");

		printf("<TD CLASS='%s'>",bg_class);

		/* find all the hosts that are members of this hostgroup... */
		for(temp_hostgroupmember=temp_hostgroup->members;temp_hostgroupmember!=NULL;temp_hostgroupmember=temp_hostgroupmember->next){

			if(temp_hostgroupmember!=temp_hostgroup->members)
				printf(", ");
			printf("<A HREF='%s?type=hosts#%s'>%s</A>\n",CONFIG_CGI,url_encode(temp_hostgroupmember->host_name),temp_hostgroupmember->host_name);
		        }
		printf("</TD>\n");

		printf("</TR>\n");
	        }

	printf("</TABLE>\n");
	printf("</DIV>\n");
	printf("</P>\n");

	return;
        }



void display_contacts(void){
	contact *temp_contact;
	commandsmember *temp_commandsmember;
	int odd=0;
	int options;
	int found;
	char *bg_class="";

	/* see if user is authorized to view contact information... */
	if(is_authorized_for_configuration_information(&current_authdata)==FALSE){
		unauthorized_message();
		return;
	        }

	/* read in contact definitions... */
	read_all_object_configuration_data(main_config_file,READ_CONTACTS);

	printf("<P><DIV ALIGN=CENTER CLASS='dataTitle'>Contacts</DIV></P>\n");

	printf("<P>\n");
	printf("<DIV ALIGN=CENTER>\n");

	printf("<TABLE CLASS='data'>\n");
    
	printf("<TR>\n");
	printf("<TH CLASS='data'>Contact Name</TH>");
	printf("<TH CLASS='data'>Alias</TH>");
	printf("<TH CLASS='data'>Email Address</TH>");
	printf("<TH CLASS='data'>Pager Address/Number</TH>");
	printf("<TH CLASS='data'>Service Notification Options</TH>");
	printf("<TH CLASS='data'>Host Notification Options</TH>");
	printf("<TH CLASS='data'>Service Notification Period</TH>");
	printf("<TH CLASS='data'>Host Notification Period</TH>");
	printf("<TH CLASS='data'>Service Notification Commands</TH>");
	printf("<TH CLASS='data'>Host Notification Commands</TH>");
	printf("</TR>\n");
	
	/* check all contacts... */
	for(temp_contact=contact_list;temp_contact!=NULL;temp_contact=temp_contact->next){

		if(odd){
			odd=0;
			bg_class="dataOdd";
		        }
		else{
			odd=1;
			bg_class="dataEven";
		        }

		printf("<TR CLASS='%s'>\n",bg_class);

		printf("<TD CLASS='%s'><A NAME='%s'>%s</a></TD>\n",bg_class,url_encode(temp_contact->name),temp_contact->name);
		printf("<TD CLASS='%s'>%s</TD>\n",bg_class,temp_contact->alias);
		printf("<TD CLASS='%s'><A HREF='mailto:%s'>%s</A></TD>\n",bg_class,(temp_contact->email==NULL)?"&nbsp;":temp_contact->email,(temp_contact->email==NULL)?"&nbsp;":temp_contact->email);
		printf("<TD CLASS='%s'>%s</TD>\n",bg_class,(temp_contact->pager==NULL)?"&nbsp;":temp_contact->pager);

		printf("<TD CLASS='%s'>",bg_class);
		options=0;
		if(temp_contact->notify_on_service_unknown==TRUE){
			options=1;
			printf("Unknown");
		        }
		if(temp_contact->notify_on_service_warning==TRUE){
			printf("%sWarning",(options)?", ":"");
			options=1;
		        }
		if(temp_contact->notify_on_service_critical==TRUE){
			printf("%sCritical",(options)?", ":"");
			options=1;
		        }
		if(temp_contact->notify_on_service_recovery==TRUE){
			printf("%sRecovery",(options)?", ":"");
			options=1;
		        }
		if(!options)
			printf("None");
		printf("</TD>\n");

		printf("<TD CLASS='%s'>",bg_class);
		options=0;
		if(temp_contact->notify_on_host_down==TRUE){
			options=1;
			printf("Down");
		        }
		if(temp_contact->notify_on_host_unreachable==TRUE){
			printf("%sUnreachable",(options)?", ":"");
			options=1;
		        }
		if(temp_contact->notify_on_host_recovery==TRUE){
			printf("%sRecovery",(options)?", ":"");
			options=1;
		        }
		if(!options)
			printf("None");
		printf("</TD>\n");

		printf("<TD CLASS='%s'>\n",bg_class);
		if(temp_contact->service_notification_period==NULL)
			printf("&nbsp;");
		else
			printf("<A HREF='%s?type=timeperiods#%s'>%s</A>",CONFIG_CGI,url_encode(temp_contact->service_notification_period),temp_contact->service_notification_period);
		printf("</TD>\n");

		printf("<TD CLASS='%s'>\n",bg_class);
		if(temp_contact->host_notification_period==NULL)
			printf("&nbsp;");
		else
			printf("<A HREF='%s?type=timeperiods#%s'>%s</A>",CONFIG_CGI,url_encode(temp_contact->host_notification_period),temp_contact->host_notification_period);
		printf("</TD>\n");

		printf("<TD CLASS='%s'>",bg_class);
		found=FALSE;
		for(temp_commandsmember=temp_contact->service_notification_commands;temp_commandsmember!=NULL;temp_commandsmember=temp_commandsmember->next){

			if(temp_commandsmember!=temp_contact->service_notification_commands)
				printf(", ");

			printf("<A HREF='%s?type=commands#%s'>%s</A>",CONFIG_CGI,url_encode(temp_commandsmember->command),temp_commandsmember->command);

			found=TRUE;
		        }
		if(found==FALSE)
			printf("None");
		printf("</TD>\n");

		printf("<TD CLASS='%s'>",bg_class);
		found=FALSE;
		for(temp_commandsmember=temp_contact->host_notification_commands;temp_commandsmember!=NULL;temp_commandsmember=temp_commandsmember->next){

			if(temp_commandsmember!=temp_contact->host_notification_commands)
				printf(", ");

			printf("<A HREF='%s?type=commands#%s'>%s</A>",CONFIG_CGI,url_encode(temp_commandsmember->command),temp_commandsmember->command);

			found=TRUE;
		        }
		if(found==FALSE)
			printf("None");
		printf("</TD>\n");

		printf("</TR>\n");
	        }

	printf("</TABLE>\n");
	printf("</DIV>\n");
	printf("</P>\n");

	return;
        }



void display_contactgroups(void){
	contactgroup *temp_contactgroup;
	contactgroupmember *temp_contactgroupmember;
	int odd=0;
	char *bg_class="";

	/* see if user is authorized to view contactgroup information... */
	if(is_authorized_for_configuration_information(&current_authdata)==FALSE){
		unauthorized_message();
		return;
	        }


	printf("<P><DIV ALIGN=CENTER CLASS='dataTitle'>Contact Groups</DIV></P>\n");

	printf("<P>\n");
	printf("<DIV ALIGN=CENTER>\n");

	printf("<TABLE BORDER=0 CELLSPACING=3 CELLPADDING=0>\n");

	printf("<TR>\n");
	printf("<TH CLASS='data'>Group Name</TH>\n");
	printf("<TH CLASS='data'>Description</TH>\n");
	printf("<TH CLASS='data'>Contact Members</TH>\n");
	printf("</TR>\n");

	/* check all the contact groups... */
	for(temp_contactgroup=contactgroup_list;temp_contactgroup!=NULL;temp_contactgroup=temp_contactgroup->next){

		if(odd){
			odd=0;
			bg_class="dataOdd";
		        }
		else{
			odd=1;
			bg_class="dataEven";
		        }

		printf("<TR CLASS='%s'>\n",bg_class);

		printf("<TD CLASS='%s'><A NAME='%s'></A>%s</TD>\n",bg_class,url_encode(temp_contactgroup->group_name),temp_contactgroup->group_name);
		printf("<TD CLASS='%s'>%s</TD>\n",bg_class,temp_contactgroup->alias);

		/* find all the contact who are members of this contact group... */
		printf("<TD CLASS='%s'>",bg_class);
		for(temp_contactgroupmember=temp_contactgroup->members;temp_contactgroupmember!=NULL;temp_contactgroupmember=temp_contactgroupmember->next){
			
			if(temp_contactgroupmember!=temp_contactgroup->members)
				printf(", ");

			printf("<A HREF='%s?type=contacts#%s'>%s</A>\n",CONFIG_CGI,url_encode(temp_contactgroupmember->contact_name),temp_contactgroupmember->contact_name);
		        }
		printf("</TD>\n");

		printf("</TR>\n");
	        }

	printf("</TABLE>\n");
	printf("</DIV>\n");
	printf("</P>\n");

	return;
        }



void display_services(void){
	service *temp_service;
	contactgroupsmember *temp_contactgroupsmember;
	char command_line[MAX_INPUT_BUFFER];
	char *command_name="";
	int options;
	int odd=0;
	char time_string[16];
	char *bg_class;


	/* see if user is authorized to view service information... */
	if(is_authorized_for_configuration_information(&current_authdata)==FALSE){
		unauthorized_message();
		return;
	        }

	/* read in service definitions... */
	read_all_object_configuration_data(main_config_file,READ_SERVICES);

	printf("<P><DIV ALIGN=CENTER CLASS='dataTitle'>Services</DIV></P>\n");

	printf("<P>\n");
	printf("<DIV ALIGN=CENTER>\n");

	printf("<TABLE BORDER=0 CLASS='data'>\n");
	printf("<TR>\n");
	printf("<TH CLASS='data' COLSPAN=2>Service</TH>");
	printf("</TR>\n");
	printf("<TR>\n");
	printf("<TH CLASS='data'>Host</TH>\n");
	printf("<TH CLASS='data'>Description</TH>\n");
	printf("<TH CLASS='data'>Max. Check Attempts</TH>\n");
	printf("<TH CLASS='data'>Normal Check Interval</TH>\n");
	printf("<TH CLASS='data'>Retry Check Interal</TH>\n");
	printf("<TH CLASS='data'>Check Command</TH>\n");
	printf("<TH CLASS='data'>Check Period</TH>\n");
	printf("<TH CLASS='data'>Parallelize</TH>\n");
	printf("<TH CLASS='data'>Volatile</TH>\n");
	printf("<TH CLASS='data'>Obsess Over</TH>\n");
	printf("<TH CLASS='data'>Enable Active Checks</TH>\n");
	printf("<TH CLASS='data'>Enable Passive Checks</TH>\n");
	printf("<TH CLASS='data'>Check Freshness</TH>\n");
	printf("<TH CLASS='data'>Freshness Threshold</TH>\n");
	printf("<TH CLASS='data'>Default Contact Groups</TH>\n");
	printf("<TH CLASS='data'>Enable Notifications</TH>\n");
	printf("<TH CLASS='data'>Notification Interval</TH>\n");
	printf("<TH CLASS='data'>Notification Options</TH>\n");
	printf("<TH CLASS='data'>Notification Period</TH>\n");
	printf("<TH CLASS='data'>Event Handler</TH>");
	printf("<TH CLASS='data'>Enable Event Handler</TH>");
	printf("<TH CLASS='data'>Stalking Options</TH>\n");
	printf("<TH CLASS='data'>Enable Flap Detection</TH>");
	printf("<TH CLASS='data'>Low Flap Threshold</TH>");
	printf("<TH CLASS='data'>High Flap Threshold</TH>");
	printf("<TH CLASS='data'>Process Performance Data</TH>");
	printf("<TH CLASS='data'>Enable Failure Prediction</TH>");
	printf("<TH CLASS='data'>Failure Prediction Options</TH>");
	printf("<TH CLASS='data'>Retention Options</TH>");
	printf("</TR>\n");

	/* check all the services... */
	move_first_service();
	while(temp_service=get_next_service()) {
		if(odd){
			odd=0;
			bg_class="dataOdd";
		        }
		else{
			odd=1;
			bg_class="dataEven";
		        }

		printf("<TR CLASS='%s'>\n",bg_class);

		printf("<TD CLASS='%s'><A NAME='%s;",bg_class,url_encode(temp_service->host_name));
		printf("%s'></A>",url_encode(temp_service->description));
		printf("<A HREF='%s?type=hosts#%s'>%s</A></TD>\n",CONFIG_CGI,url_encode(temp_service->host_name),temp_service->host_name);
		
		printf("<TD CLASS='%s'>%s</TD>\n",bg_class,temp_service->description);

		printf("<TD CLASS='%s'>%d</TD>\n",bg_class,temp_service->max_attempts);

		get_interval_time_string(temp_service->check_interval,time_string,sizeof(time_string));
		printf("<TD CLASS='%s'>%s</TD>\n",bg_class,time_string);
		get_interval_time_string(temp_service->retry_interval,time_string,sizeof(time_string));
		printf("<TD CLASS='%s'>%s</TD>\n",bg_class,time_string);

		strncpy(command_line,temp_service->service_check_command,sizeof(command_line));
		command_line[sizeof(command_line)-1]='\x0';
		command_name=strtok(command_line,"!");
		printf("<TD CLASS='%s'><A HREF='%s?type=commands#%s'>%s</A></TD>\n",bg_class,CONFIG_CGI,url_encode(command_name),temp_service->service_check_command);

		printf("<TD CLASS='%s'>",bg_class);
		if(temp_service->check_period==NULL)
			printf("&nbsp;");
		else
			printf("<A HREF='%s?type=timeperiods#%s'>%s</A>",CONFIG_CGI,url_encode(temp_service->check_period),temp_service->check_period);
		printf("</TD>\n");

		printf("<TD CLASS='%s'>%s</TD>\n",bg_class,(temp_service->parallelize==TRUE)?"Yes":"No");

		printf("<TD CLASS='%s'>%s</TD>\n",bg_class,(temp_service->is_volatile==TRUE)?"Yes":"No");

		printf("<TD CLASS='%s'>%s</TD>\n",bg_class,(temp_service->obsess_over_service==TRUE)?"Yes":"No");

		printf("<TD CLASS='%s'>%s</TD>\n",bg_class,(temp_service->checks_enabled==TRUE)?"Yes":"No");

		printf("<TD CLASS='%s'>%s</TD>\n",bg_class,(temp_service->accept_passive_service_checks==TRUE)?"Yes":"No");

		printf("<TD CLASS='%s'>%s</TD>\n",bg_class,(temp_service->check_freshness==TRUE)?"Yes":"No");

		printf("<TD CLASS='%s'>",bg_class);
		if(temp_service->freshness_threshold==0)
			printf("Auto-determined value\n");
		else
			printf("%d seconds\n",temp_service->freshness_threshold);
		printf("</TD>\n");

		printf("<TD CLASS='%s'>",bg_class);
		for(temp_contactgroupsmember=temp_service->contact_groups;temp_contactgroupsmember!=NULL;temp_contactgroupsmember=temp_contactgroupsmember->next){

			if(temp_contactgroupsmember!=temp_service->contact_groups)
				printf(", ");

		        printf("<A HREF='%s?type=contactgroups#%s'>%s</A>",CONFIG_CGI,url_encode(temp_contactgroupsmember->group_name),temp_contactgroupsmember->group_name);
		        }
		if(temp_service->contact_groups==NULL)
			printf("&nbsp;");
		printf("</TD>\n");

		printf("<TD CLASS='%s'>",bg_class);
		printf("%s\n",(temp_service->notifications_enabled==TRUE)?"Yes":"No");
		printf("</TD>\n");

		get_interval_time_string(temp_service->notification_interval,time_string,sizeof(time_string));
		printf("<TD CLASS='%s'>%s</TD>\n",bg_class,(temp_service->notification_interval==0)?"<i>No Re-notification</i>":time_string);

		printf("<TD CLASS='%s'>",bg_class);
		options=0;
		if(temp_service->notify_on_unknown==TRUE){
			options=1;
			printf("Unknown");
		        }
		if(temp_service->notify_on_warning==TRUE){
			printf("%sWarning",(options)?", ":"");
			options=1;
		        }
		if(temp_service->notify_on_critical==TRUE){
			printf("%sCritical",(options)?", ":"");
			options=1;
		        }
		if(temp_service->notify_on_recovery==TRUE){
			printf("%sRecovery",(options)?", ":"");
			options=1;
		        }
		if(!options)
			printf("None");
		printf("</TD>\n");

		printf("<TD CLASS='%s'>",bg_class);
		if(temp_service->notification_period==NULL)
			printf("&nbsp;");
		else
			printf("<A HREF='%s?type=timeperiods#%s'>%s</A>",CONFIG_CGI,url_encode(temp_service->notification_period),temp_service->notification_period);
		printf("</TD>\n");

		printf("<TD CLASS='%s'>",bg_class);
		if(temp_service->event_handler==NULL)
			printf("&nbsp;");
		else
			printf("<A HREF='%s?type=commands#%s'>%s</A>",CONFIG_CGI,url_encode(temp_service->event_handler),temp_service->event_handler);
		printf("</TD>\n");

		printf("<TD CLASS='%s'>",bg_class);
		printf("%s\n",(temp_service->event_handler_enabled==TRUE)?"Yes":"No");
		printf("</TD>\n");

		printf("<TD CLASS='%s'>",bg_class);
		options=0;
		if(temp_service->stalk_on_ok==TRUE){
			options=1;
			printf("Ok");
		        }
		if(temp_service->stalk_on_warning==TRUE){
			printf("%sWarning",(options)?", ":"");
			options=1;
		        }
		if(temp_service->stalk_on_unknown==TRUE){
			printf("%sUnknown",(options)?", ":"");
			options=1;
		        }
		if(temp_service->stalk_on_critical==TRUE){
			printf("%sCritical",(options)?", ":"");
			options=1;
		        }
		if(options==0)
			printf("None");
		printf("</TD>\n");

		printf("<TD CLASS='%s'>",bg_class);
		printf("%s\n",(temp_service->flap_detection_enabled==TRUE)?"Yes":"No");
		printf("</TD>\n");

		printf("<TD CLASS='%s'>",bg_class);
		if(temp_service->low_flap_threshold==0.0)
			printf("Program-wide value\n");
		else
			printf("%3.1f%%\n",temp_service->low_flap_threshold);
		printf("</TD>\n");

		printf("<TD CLASS='%s'>",bg_class);
		if(temp_service->high_flap_threshold==0.0)
			printf("Program-wide value\n");
		else
			printf("%3.1f%%\n",temp_service->high_flap_threshold);
		printf("</TD>\n");

		printf("<TD CLASS='%s'>",bg_class);
		printf("%s\n",(temp_service->process_performance_data==TRUE)?"Yes":"No");
		printf("</TD>\n");

		printf("<TD CLASS='%s'>",bg_class);
		printf("%s\n",(temp_service->failure_prediction_enabled==TRUE)?"Yes":"No");
		printf("</TD>\n");

		printf("<TD CLASS='%s'>%s</TD>\n",bg_class,(temp_service->failure_prediction_options==NULL)?"&nbsp;":temp_service->failure_prediction_options);

		printf("<TD CLASS='%s'>",bg_class);
		options=0;
		if(temp_service->retain_status_information==TRUE){
			options=1;
			printf("Status Information");
		        }
		if(temp_service->retain_nonstatus_information==TRUE){
			printf("%sNon-Status Information",(options==1)?", ":"");
			options=1;
		        }
		if(options==0)
			printf("None");
		printf("</TD>\n");

		printf("</TR>\n");
	        }

	printf("</TABLE>\n");
	printf("</DIV>\n");
	printf("</P>\n");

	return;
        }



void display_timeperiods(void){
	timerange *temp_timerange;
	timeperiod *temp_timeperiod;
	int odd=0;
	int day=0;
	char *bg_class="";
	char timestring[10];
	int hours=0;
	int minutes=0;
	int seconds=0;

	/* see if user is authorized to view time period information... */
	if(is_authorized_for_configuration_information(&current_authdata)==FALSE){
		unauthorized_message();
		return;
	        }

	/* read in time period definitions... */
	read_all_object_configuration_data(main_config_file,READ_TIMEPERIODS);

	printf("<P><DIV ALIGN=CENTER CLASS='dataTitle'>Time Periods</DIV></P>\n");

	printf("<P>\n");
	printf("<DIV ALIGN=CENTER>\n");

	printf("<TABLE BORDER=0 CLASS='data'>\n");
	printf("<TR>\n");
	printf("<TH CLASS='data'>Name</TH>\n");
	printf("<TH CLASS='data'>Alias/Description</TH>\n");
	printf("<TH CLASS='data'>Sunday Time Ranges</TH>\n");
	printf("<TH CLASS='data'>Monday Time Ranges</TH>\n");
	printf("<TH CLASS='data'>Tuesday Time Ranges</TH>\n");
	printf("<TH CLASS='data'>Wednesday Time Ranges</TH>\n");
	printf("<TH CLASS='data'>Thursday Time Ranges</TH>\n");
	printf("<TH CLASS='data'>Friday Time Ranges</TH>\n");
	printf("<TH CLASS='data'>Saturday Time Ranges</TH>\n");
	printf("</TR>\n");

	/* check all the time periods... */
	for(temp_timeperiod=timeperiod_list;temp_timeperiod!=NULL;temp_timeperiod=temp_timeperiod->next){

		if(odd){
			odd=0;
			bg_class="dataOdd";
		        }
		else{
			odd=1;
			bg_class="dataEven";
		        }

		printf("<TR CLASS='%s'>\n",bg_class);

		printf("<TD CLASS='%s'><A NAME='%s'>%s</A></TD>\n",bg_class,url_encode(temp_timeperiod->name),temp_timeperiod->name);
		printf("<TD CLASS='%s'>%s</TD>\n",bg_class,temp_timeperiod->alias);

		for(day=0;day<7;day++){

			printf("<TD CLASS='%s'>",bg_class);

			for(temp_timerange=temp_timeperiod->days[day];temp_timerange!=NULL;temp_timerange=temp_timerange->next){

				if(temp_timerange!=temp_timeperiod->days[day])
					printf(", ");

				hours=temp_timerange->range_start/3600;
				minutes=(temp_timerange->range_start-(hours*3600))/60;
				seconds=temp_timerange->range_start-(hours*3600)-(minutes*60);
				snprintf(timestring,sizeof(timestring)-1,"%02d:%02d:%02d",hours,minutes,seconds);
				timestring[sizeof(timestring)-1]='\x0';
				printf("%s - ",timestring);

				hours=temp_timerange->range_end/3600;
				minutes=(temp_timerange->range_end-(hours*3600))/60;
				seconds=temp_timerange->range_end-(hours*3600)-(minutes*60);
				snprintf(timestring,sizeof(timestring)-1,"%02d:%02d:%02d",hours,minutes,seconds);
				timestring[sizeof(timestring)-1]='\x0';
				printf("%s",timestring);
			        }
			if(temp_timeperiod->days[day]==NULL)
				printf("&nbsp;");

			printf("</TD>\n");
		        }

		printf("</TR>\n");
	        }

	printf("</TABLE>\n");
	printf("</DIV>\n");
	printf("</P>\n");

	return;
        }



void display_commands(void){
	command *temp_command;
	int odd=0;
	char *bg_class="";

	/* see if user is authorized to view command information... */
	if(is_authorized_for_configuration_information(&current_authdata)==FALSE){
		unauthorized_message();
		return;
	        }

	/* read in command definitions... */
	read_all_object_configuration_data(main_config_file,READ_COMMANDS);

	printf("<P><DIV ALIGN=CENTER CLASS='dataTitle'>Commands</DIV></P>\n");

	printf("<P><DIV ALIGN=CENTER>\n");
	printf("<TABLE BORDER=0 CLASS='data'>\n");
	printf("<TR><TH CLASS='data'>Command Name</TH><TH CLASS='data'>Command Line</TH></TR>\n");

	/* check all commands */
	for(temp_command=command_list;temp_command!=NULL;temp_command=temp_command->next){

		if(odd){
			odd=0;
			bg_class="dataEven";
		        }
		else{
			odd=1;
			bg_class="dataOdd";
		        }

		printf("<TR CLASS='%s'>\n",bg_class);

		printf("<TD CLASS='%s'><A NAME='%s'></A>%s</TD>\n",bg_class,url_encode(temp_command->name),temp_command->name);
		printf("<TD CLASS='%s'>%s</TD>\n",bg_class,temp_command->command_line);

		printf("</TR>\n");
	        }

	printf("</TABLE>\n");
	printf("</DIV></P>\n");

	return;
        }



void display_hostgroupescalations(void){
	hostgroupescalation *temp_hge;
	contactgroupsmember *temp_contactgroupsmember;
	int odd=0;
	char *bg_class="";

	/* see if user is authorized to view hostgroup information... */
	if(is_authorized_for_configuration_information(&current_authdata)==FALSE){
		unauthorized_message();
		return;
	        }

	printf("<P><DIV ALIGN=CENTER CLASS='dataTitle'>Hostgroup Escalations</DIV></P>\n");

	printf("<P>\n");
	printf("<DIV ALIGN=CENTER>\n");

	printf("<TABLE BORDER=0 CLASS='data'>\n");
	printf("<TR>\n");
	printf("<TH CLASS='data'>Hostgroup</TH>");
	printf("<TH CLASS='data'>Contact Groups</TH>");
	printf("<TH CLASS='data'>First Notification</TH>");
	printf("<TH CLASS='data'>Last Notification</TH>");
	printf("<TH CLASS='data'>Notification Interval</TH>");
	printf("</TR>\n");

	/* check all the hostgroup escalations... */
	for(temp_hge=hostgroupescalation_list;temp_hge!=NULL;temp_hge=temp_hge->next){

		if(odd){
			odd=0;
			bg_class="dataOdd";
		        }
		else{
			odd=1;
			bg_class="dataEven";
		        }

		printf("<TR CLASS='%s'>\n",bg_class);

		printf("<TD CLASS='%s'><A HREF='%s?type=hostgroups#%s'>%s</A></TD>",bg_class,CONFIG_CGI,url_encode(temp_hge->group_name),temp_hge->group_name);

		printf("<TD CLASS='%s'>",bg_class);
		for(temp_contactgroupsmember=temp_hge->contact_groups;temp_contactgroupsmember!=NULL;temp_contactgroupsmember=temp_contactgroupsmember->next){

			if(temp_contactgroupsmember!=temp_hge->contact_groups)
				printf(", ");

			printf("<A HREF='%s?type=contactgroups#%s'>%s</A>\n",CONFIG_CGI,url_encode(temp_contactgroupsmember->group_name),temp_contactgroupsmember->group_name);
		        }
		printf("</TD>\n");

		printf("<TD CLASS='%s'>%d</TD>",bg_class,temp_hge->first_notification);

		printf("<TD CLASS='%s'>",bg_class);
		if(temp_hge->last_notification==0)
			printf("Infinity");
		else
			printf("%d",temp_hge->last_notification);
		printf("</TD>\n");

		printf("<TD CLASS='%s'>",bg_class);
		if(temp_hge->notification_interval==0)
			printf("Notify Only Once (No Re-notification)");
		else
			printf("%d",temp_hge->notification_interval);
		printf("</TD>\n");


		printf("</TR>\n");
	        }

	printf("</TABLE>\n");
	printf("</DIV>\n");
	printf("</P>\n");

	return;
        }



void display_servicedependencies(void){
	servicedependency *temp_sd;
	int odd=0;
	int options;
	char *bg_class="";

	/* see if user is authorized to view hostgroup information... */
	if(is_authorized_for_configuration_information(&current_authdata)==FALSE){
		unauthorized_message();
		return;
	        }

	/* read in command definitions... */
	read_all_object_configuration_data(main_config_file,READ_SERVICEDEPENDENCIES);

	printf("<P><DIV ALIGN=CENTER CLASS='dataTitle'>Service Dependencies</DIV></P>\n");

	printf("<P>\n");
	printf("<DIV ALIGN=CENTER>\n");

	printf("<TABLE BORDER=0 CLASS='data'>\n");
	printf("<TR>\n");
	printf("<TH CLASS='data' COLSPAN=2>Dependent Service</TH>");
	printf("<TH CLASS='data' COLSPAN=2>Master Service</TH>");
	printf("</TR>\n");
	printf("<TR>\n");
	printf("<TH CLASS='data'>Host</TH>");
	printf("<TH CLASS='data'>Service</TH>");
	printf("<TH CLASS='data'>Host</TH>");
	printf("<TH CLASS='data'>Service</TH>");
	printf("<TH CLASS='data'>Dependency Type</TH>");
	printf("<TH CLASS='data'>Dependency Failure Options</TH>");
	printf("</TR>\n");

	/* check all the service dependencies... */
	for(temp_sd=servicedependency_list;temp_sd!=NULL;temp_sd=temp_sd->next){

		if(odd){
			odd=0;
			bg_class="dataOdd";
		        }
		else{
			odd=1;
			bg_class="dataEven";
		        }

		printf("<TR CLASS='%s'>\n",bg_class);

		printf("<TD CLASS='%s'><A HREF='%s?type=hosts#%s'>%s</A></TD>",bg_class,CONFIG_CGI,url_encode(temp_sd->dependent_host_name),temp_sd->dependent_host_name);

		printf("<TD CLASS='%s'><A HREF='%s?type=services#%s;",bg_class,CONFIG_CGI,url_encode(temp_sd->dependent_host_name));
		printf("%s'>%s</A></TD>\n",url_encode(temp_sd->dependent_service_description),temp_sd->dependent_service_description);

		printf("<TD CLASS='%s'><A HREF='%s?type=hosts#%s'>%s</A></TD>",bg_class,CONFIG_CGI,url_encode(temp_sd->host_name),temp_sd->host_name);

		printf("<TD CLASS='%s'><A HREF='%s?type=services#%s;",bg_class,CONFIG_CGI,url_encode(temp_sd->host_name));
		printf("%s'>%s</A></TD>\n",url_encode(temp_sd->service_description),temp_sd->service_description);

		printf("<TD CLASS='%s'>%s</TD>",bg_class,(temp_sd->dependency_type==NOTIFICATION_DEPENDENCY)?"Notification":"Check Execution");

		printf("<TD CLASS='%s'>",bg_class);
		options=FALSE;
		if(temp_sd->fail_on_ok==TRUE){
			printf("Ok");
			options=TRUE;
		        }
		if(temp_sd->fail_on_warning==TRUE){
			printf("%sWarning",(options==TRUE)?", ":"");
			options=TRUE;
		        }
		if(temp_sd->fail_on_unknown==TRUE){
			printf("%sUnknown",(options==TRUE)?", ":"");
			options=TRUE;
		        }
		if(temp_sd->fail_on_critical==TRUE){
			printf("%sCritical",(options==TRUE)?", ":"");
			options=TRUE;
		        }
		printf("</TD>\n");

		printf("</TR>\n");
	        }

	printf("</TABLE>\n");
	printf("</DIV>\n");
	printf("</P>\n");

	return;
        }



void display_serviceescalations(void){
	serviceescalation *temp_se;
	contactgroupsmember *temp_contactgroupsmember;
	int odd=0;
	char *bg_class="";

	/* see if user is authorized to view hostgroup information... */
	if(is_authorized_for_configuration_information(&current_authdata)==FALSE){
		unauthorized_message();
		return;
	        }

	/* read in command definitions... */
	read_all_object_configuration_data(main_config_file,READ_SERVICEESCALATIONS);

	printf("<P><DIV ALIGN=CENTER CLASS='dataTitle'>Service Escalations</DIV></P>\n");

	printf("<P>\n");
	printf("<DIV ALIGN=CENTER>\n");

	printf("<TABLE BORDER=0 CLASS='data'>\n");
	printf("<TR>\n");
	printf("<TH CLASS='data' COLSPAN=2>Service</TH>");
	printf("</TR>\n");
	printf("<TR>\n");
	printf("<TH CLASS='data'>Host</TH>");
	printf("<TH CLASS='data'>Description</TH>");
	printf("<TH CLASS='data'>Contact Groups</TH>");
	printf("<TH CLASS='data'>First Notification</TH>");
	printf("<TH CLASS='data'>Last Notification</TH>");
	printf("<TH CLASS='data'>Notification Interval</TH>");
	printf("</TR>\n");

	/* check all the service escalations... */
	for(temp_se=serviceescalation_list;temp_se!=NULL;temp_se=temp_se->next){

		if(odd){
			odd=0;
			bg_class="dataOdd";
		        }
		else{
			odd=1;
			bg_class="dataEven";
		        }

		printf("<TR CLASS='%s'>\n",bg_class);

		printf("<TD CLASS='%s'><A HREF='%s?type=hosts#%s'>%s</A></TD>",bg_class,CONFIG_CGI,url_encode(temp_se->host_name),temp_se->host_name);

		printf("<TD CLASS='%s'><A HREF='%s?type=services#%s;",bg_class,CONFIG_CGI,url_encode(temp_se->host_name));
		printf("%s'>%s</A></TD>\n",url_encode(temp_se->description),temp_se->description);

		printf("<TD CLASS='%s'>",bg_class);
		for(temp_contactgroupsmember=temp_se->contact_groups;temp_contactgroupsmember!=NULL;temp_contactgroupsmember=temp_contactgroupsmember->next){

			if(temp_contactgroupsmember!=temp_se->contact_groups)
				printf(", ");

			printf("<A HREF='%s?type=contactgroups#%s'>%s</A>\n",CONFIG_CGI,url_encode(temp_contactgroupsmember->group_name),temp_contactgroupsmember->group_name);
		        }
		printf("</TD>\n");

		printf("<TD CLASS='%s'>%d</TD>",bg_class,temp_se->first_notification);

		printf("<TD CLASS='%s'>",bg_class);
		if(temp_se->last_notification==0)
			printf("Infinity");
		else
			printf("%d",temp_se->last_notification);
		printf("</TD>\n");

		printf("<TD CLASS='%s'>",bg_class);
		if(temp_se->notification_interval==0)
			printf("Notify Only Once (No Re-notification)");
		else
			printf("%d",temp_se->notification_interval);
		printf("</TD>\n");


		printf("</TR>\n");
	        }

	printf("</TABLE>\n");
	printf("</DIV>\n");
	printf("</P>\n");

	return;
        }



void display_hostdependencies(void){
	hostdependency *temp_hd;
	int odd=0;
	int options;
	char *bg_class="";

	/* see if user is authorized to view hostdependency information... */
	if(is_authorized_for_configuration_information(&current_authdata)==FALSE){
		unauthorized_message();
		return;
	        }

	/* read in command definitions... */
	read_all_object_configuration_data(main_config_file,READ_HOSTDEPENDENCIES);

	printf("<P><DIV ALIGN=CENTER CLASS='dataTitle'>Host Dependencies</DIV></P>\n");

	printf("<P>\n");
	printf("<DIV ALIGN=CENTER>\n");

	printf("<TABLE BORDER=0 CLASS='data'>\n");
	printf("<TR>\n");
	printf("<TH CLASS='data'>Dependent Host</TH>");
	printf("<TH CLASS='data'>Master Host</TH>");
	printf("<TH CLASS='data'>Dependency Type</TH>");
	printf("<TH CLASS='data'>Dependency Failure Options</TH>");
	printf("</TR>\n");

	/* check all the host dependencies... */
	for(temp_hd=hostdependency_list;temp_hd!=NULL;temp_hd=temp_hd->next){

		if(odd){
			odd=0;
			bg_class="dataOdd";
		        }
		else{
			odd=1;
			bg_class="dataEven";
		        }

		printf("<TR CLASS='%s'>\n",bg_class);

		printf("<TD CLASS='%s'><A HREF='%s?type=hosts#%s'>%s</A></TD>",bg_class,CONFIG_CGI,url_encode(temp_hd->dependent_host_name),temp_hd->dependent_host_name);

		printf("<TD CLASS='%s'><A HREF='%s?type=hosts#%s'>%s</A></TD>",bg_class,CONFIG_CGI,url_encode(temp_hd->host_name),temp_hd->host_name);

		printf("<TD CLASS='%s'>%s</TD>",bg_class,(temp_hd->dependency_type==NOTIFICATION_DEPENDENCY)?"Notification":"Check Execution");

		printf("<TD CLASS='%s'>",bg_class);
		options=FALSE;
		if(temp_hd->fail_on_up==TRUE){
			printf("Up");
			options=TRUE;
		        }
		if(temp_hd->fail_on_down==TRUE){
			printf("%sDown",(options==TRUE)?", ":"");
			options=TRUE;
		        }
		if(temp_hd->fail_on_unreachable==TRUE){
			printf("%sUnreachable",(options==TRUE)?", ":"");
			options=TRUE;
		        }
		printf("</TD>\n");

		printf("</TR>\n");
	        }

	printf("</TABLE>\n");
	printf("</DIV>\n");
	printf("</P>\n");

	return;
        }



void display_hostescalations(void){
	hostescalation *temp_he;
	contactgroupsmember *temp_contactgroupsmember;
	int odd=0;
	char *bg_class="";

	/* see if user is authorized to view hostgroup information... */
	if(is_authorized_for_configuration_information(&current_authdata)==FALSE){
		unauthorized_message();
		return;
	        }

	/* read in command definitions... */
	read_all_object_configuration_data(main_config_file,READ_HOSTESCALATIONS);

	printf("<P><DIV ALIGN=CENTER CLASS='dataTitle'>Host Escalations</DIV></P>\n");

	printf("<P>\n");
	printf("<DIV ALIGN=CENTER>\n");

	printf("<TABLE BORDER=0 CLASS='data'>\n");
	printf("<TR>\n");
	printf("<TH CLASS='data'>Host</TH>");
	printf("<TH CLASS='data'>Contact Groups</TH>");
	printf("<TH CLASS='data'>First Notification</TH>");
	printf("<TH CLASS='data'>Last Notification</TH>");
	printf("<TH CLASS='data'>Notification Interval</TH>");
	printf("</TR>\n");

	/* check all the host escalations... */
	for(temp_he=hostescalation_list;temp_he!=NULL;temp_he=temp_he->next){

		if(odd){
			odd=0;
			bg_class="dataOdd";
		        }
		else{
			odd=1;
			bg_class="dataEven";
		        }

		printf("<TR CLASS='%s'>\n",bg_class);

		printf("<TD CLASS='%s'><A HREF='%s?type=hosts#%s'>%s</A></TD>",bg_class,CONFIG_CGI,url_encode(temp_he->host_name),temp_he->host_name);

		printf("<TD CLASS='%s'>",bg_class);
		for(temp_contactgroupsmember=temp_he->contact_groups;temp_contactgroupsmember!=NULL;temp_contactgroupsmember=temp_contactgroupsmember->next){

			if(temp_contactgroupsmember!=temp_he->contact_groups)
				printf(", ");

			printf("<A HREF='%s?type=contactgroups#%s'>%s</A>\n",CONFIG_CGI,url_encode(temp_contactgroupsmember->group_name),temp_contactgroupsmember->group_name);
		        }
		printf("</TD>\n");

		printf("<TD CLASS='%s'>%d</TD>",bg_class,temp_he->first_notification);

		printf("<TD CLASS='%s'>",bg_class);
		if(temp_he->last_notification==0)
			printf("Infinity");
		else
			printf("%d",temp_he->last_notification);
		printf("</TD>\n");

		printf("<TD CLASS='%s'>",bg_class);
		if(temp_he->notification_interval==0)
			printf("Notify Only Once (No Re-notification)");
		else
			printf("%d",temp_he->notification_interval);
		printf("</TD>\n");


		printf("</TR>\n");
	        }

	printf("</TABLE>\n");
	printf("</DIV>\n");
	printf("</P>\n");

	return;
        }



void unauthorized_message(void){

	printf("<P><DIV CLASS='errorMessage'>It appears as though you do not have permission to view the configuration information you requested...</DIV></P>\n");
	printf("<P><DIV CLASS='errorDescription'>If you believe this is an error, check the HTTP server authentication requirements for accessing this CGI<br>");
	printf("and check the authorization options in your CGI configuration file.</DIV></P>\n");

	return;
	}




void display_options(void){

	printf("<br><br>\n");

	printf("<div align=center class='reportSelectTitle'>Select Type of Config Data You Wish To View</div>\n");

	printf("<br><br>\n");

        printf("<form method=\"get\" action=\"%s\">\n",CONFIG_CGI);

	printf("<div align=center>\n");
	printf("<table border=0>\n");

	printf("<tr><td align=left class='reportSelectSubTitle'>Object Type:</td></tr>\n");
	printf("<tr><td align=left class='reportSelectItem'>");
	printf("<select name='type'>\n");
	printf("<option value='hosts' %s>Hosts\n",(display_type==DISPLAY_HOSTS)?"SELECTED":"");
	printf("<option value='hostdependencies' %s>Host Dependencies\n",(display_type==DISPLAY_HOSTDEPENDENCIES)?"SELECTED":"");
	printf("<option value='hostescalations' %s>Host Escalations\n",(display_type==DISPLAY_HOSTESCALATIONS)?"SELECTED":"");
	printf("<option value='hostgroups' %s>Host Groups\n",(display_type==DISPLAY_HOSTGROUPS)?"SELECTED":"");
	printf("<option value='hostgroupescalations' %s>Host Group Escalations\n",(display_type==DISPLAY_HOSTGROUPESCALATIONS)?"SELECTED":"");
	printf("<option value='services' %s>Services\n",(display_type==DISPLAY_SERVICES)?"SELECTED":"");
	printf("<option value='servicedependencies' %s>Service Dependencies\n",(display_type==DISPLAY_SERVICEDEPENDENCIES)?"SELECTED":"");
	printf("<option value='serviceescalations' %s>Service Escalations\n",(display_type==DISPLAY_SERVICEESCALATIONS)?"SELECTED":"");
	printf("<option value='contacts' %s>Contacts\n",(display_type==DISPLAY_CONTACTS)?"SELECTED":"");
	printf("<option value='contactgroups' %s>Contact Groups\n",(display_type==DISPLAY_CONTACTGROUPS)?"SELECTED":"");
	printf("<option value='timeperiods' %s>Timeperiods\n",(display_type==DISPLAY_TIMEPERIODS)?"SELECTED":"");
	printf("<option value='commands' %s>Commands\n",(display_type==DISPLAY_COMMANDS)?"SELECTED":"");
	printf("</select>\n");
	printf("</td></tr>\n");

	printf("<tr><td class='reportSelectItem'><input type='submit' value='Continue'></td></tr>\n");
	printf("</table>\n");
	printf("</div>\n");

	printf("</form>\n");

	return;
        }
