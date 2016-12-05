// swad_agenda.c: user's agenda (personal organizer)

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2016 Antonio Ca�as Vargas

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General 3 License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/*****************************************************************************/
/********************************* Headers ***********************************/
/*****************************************************************************/

#include <linux/limits.h>	// For PATH_MAX
#include <linux/stddef.h>	// For NULL
#include <stdlib.h>		// For calloc
#include <string.h>		// For string functions

#include "swad_agenda.h"
#include "swad_database.h"
#include "swad_global.h"
#include "swad_group.h"
#include "swad_notification.h"
#include "swad_pagination.h"
#include "swad_parameter.h"
#include "swad_photo.h"
#include "swad_QR.h"
#include "swad_string.h"

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/******************************* Private types *******************************/
/*****************************************************************************/

#define Agd_NUM_AGENDA_TYPES 2
typedef enum
  {
   Agd_USR_AGENDA,
   Agd_MY_AGENDA,
  } Agd_AgendaType_t;

/*****************************************************************************/
/***************************** Private variables *****************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Agd_ShowEvents (Agd_AgendaType_t AgendaType);

static void Agd_PutIconToViewEditMyAgenda (void);
static void Agd_PutIconsListEvents (void);
static void Agd_PutIconToCreateNewEvent (void);
static void Agd_PutIconToShowQR (void);
static void Agd_PutButtonToCreateNewEvent (void);
static void Agd_PutParamsToCreateNewEvent (void);
static void Agd_ShowFormToSelWhichEvents (Act_Action_t Action);
static void Agd_GetParamWhichEvents (void);
static void Agd_ShowOneEvent (Agd_AgendaType_t AgendaType,long AgdCod);
static void Agd_WriteEventAuthor (struct AgendaEvent *AgdEvent);
static void Agd_GetParamEventOrderType (void);

static void Agd_PutFormsToRemEditOneEvent (struct AgendaEvent *AgdEvent);
static void Agd_PutParams (void);
static void Agd_GetListEvents (Agd_AgendaType_t AgendaType);
static void Agd_GetDataOfEventByCod (struct AgendaEvent *AgdEvent);
static void Agd_GetDataOfEvent (struct AgendaEvent *AgdEvent,const char *Query);
static void Agd_GetEventTxtFromDB (struct AgendaEvent *AgdEvent,char *Txt);
static void Agd_PutParamAgdCod (long AgdCod);
static bool Agd_CheckIfSimilarEventExists (const char *Field,const char *Value,long AgdCod);
static void Agd_CreateEvent (struct AgendaEvent *AgdEvent,const char *Txt);
static void Agd_UpdateEvent (struct AgendaEvent *AgdEvent,const char *Txt);

/*****************************************************************************/
/********** Put form to log in and then show another user's agenda ***********/
/*****************************************************************************/

void Agd_PutFormLogInToShowUsrAgenda (void)
  {
   /***** Form to log in *****/
   Usr_WriteFormLogin (ActLogInUsrAgd,Agd_PutParamAgd);
  }

void Agd_PutParamAgd (void)
  {
   char Nickname[Nck_MAX_BYTES_NICKNAME_WITH_ARROBA+1];

   sprintf (Nickname,"@%s",Gbl.Usrs.Other.UsrDat.Nickname);
   Par_PutHiddenParamString ("agd",Nickname);
  }

/*****************************************************************************/
/***************** Show another user's agenda after log in *******************/
/*****************************************************************************/

void Agd_ShowUsrAgendaAfterLogIn (void)
  {
   extern const unsigned Txt_Current_CGI_SWAD_Language;
   extern const char *Txt_User_not_found_or_you_do_not_have_permission_;
   extern const char *Txt_Switching_to_LANGUAGE[1+Txt_NUM_LANGUAGES];

   if (Gbl.Usrs.Me.Logged)
     {
      if (Gbl.Usrs.Me.UsrDat.Prefs.Language == Txt_Current_CGI_SWAD_Language)
        {
	 /***** Get user *****/
	 /* If nickname is correct, user code is already got from nickname */
	 if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&Gbl.Usrs.Other.UsrDat))        // Existing user
	    /***** Show all the visible events in the user's agenda *****/
	    Agd_ShowEvents (Agd_USR_AGENDA);
	 else
	    Lay_ShowAlert (Lay_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
       }
      else
	 /* The current language is not my preferred language
	    ==> change automatically to my language */
         Lay_ShowAlert (Lay_INFO,Txt_Switching_to_LANGUAGE[Gbl.Usrs.Me.UsrDat.Prefs.Language]);
     }
  }

/*****************************************************************************/
/************************ Show another user's agenda *************************/
/*****************************************************************************/

void Agd_ShowUsrAgenda (void)
  {
   extern const char *Txt_User_not_found_or_you_do_not_have_permission_;

   /***** Get user *****/
   if (Usr_GetParamOtherUsrCodEncryptedAndGetUsrData ())
      /***** Show all the visible events in the user's agenda *****/
      Agd_ShowEvents (Agd_USR_AGENDA);
   else
      Lay_ShowAlert (Lay_WARNING,Txt_User_not_found_or_you_do_not_have_permission_);
  }

/*****************************************************************************/
/******************************* Show my agenda ******************************/
/*****************************************************************************/

void Agd_ShowMyAgenda (void)
  {
   /***** Get whether to show all events or only my events ******/
   if (Gbl.Usrs.Me.AvailableRoles & (1 << Rol_TEACHER))	// I am a teacher in some courses
      Agd_GetParamWhichEvents ();

   /***** Show all my events *****/
   Agd_ShowEvents (Agd_MY_AGENDA);
  }

/*****************************************************************************/
/*************************** Show events in agenda ***************************/
/*****************************************************************************/

static void Agd_ShowEvents (Agd_AgendaType_t AgendaType)
  {
   extern const char *Hlp_PROFILE_Agenda;
   extern const char *Txt_Public_agenda_USER;
   extern const char *Txt_My_agenda;
   extern const char *Txt_ASG_ATT_OR_SVY_HELP_ORDER[2];
   extern const char *Txt_ASG_ATT_OR_SVY_ORDER[2];
   extern const char *Txt_Event;
   extern const char *Txt_Location;
   extern const char *Txt_No_events;
   bool ItsMe;
   Agd_Order_t Order;
   struct Pagination Pagination;
   unsigned NumEvent;
   Pag_WhatPaginate_t WhatPaginate[Agd_NUM_AGENDA_TYPES] =
     {
      Pag_USR_AGENDA,
      Pag_MY_AGENDA,
     };

   /***** Get parameters *****/
   Agd_GetParamEventOrderType ();
   Pag_GetParamPagNum (WhatPaginate[AgendaType]);

   /***** Get list of events *****/
   Agd_GetListEvents (AgendaType);

   /***** Compute variables related to pagination *****/
   Pagination.NumItems = Gbl.Agenda.Num;
   Pagination.CurrentPage = (int) Gbl.Pag.CurrentPage;
   Pag_CalculatePagination (&Pagination);
   Gbl.Pag.CurrentPage = (unsigned) Pagination.CurrentPage;

   /***** Write links to pages *****/
   if (Pagination.MoreThanOnePage)
      Pag_WriteLinksToPagesCentered (WhatPaginate[AgendaType],0,&Pagination);

   switch (AgendaType)
     {
      case Agd_USR_AGENDA:
         /***** Start frame *****/
	 sprintf (Gbl.Title,Txt_Public_agenda_USER,Gbl.Usrs.Other.UsrDat.FullName);
	 ItsMe = (Gbl.Usrs.Me.UsrDat.UsrCod == Gbl.Usrs.Other.UsrDat.UsrCod);
	 Lay_StartRoundFrame ("100%",Gbl.Title,
			      ItsMe ? Agd_PutIconToViewEditMyAgenda :
				      NULL,
			      Hlp_PROFILE_Agenda);	// TODO: Change
	 break;
      case Agd_MY_AGENDA:
         /***** Start frame *****/
	 switch (Gbl.Agenda.WhichEvents)
	   {
	    case Agd_ALL_EVENTS:
	       Lay_StartRoundFrame ("100%",Txt_My_agenda,
				    Agd_PutIconsListEvents,
				    Hlp_PROFILE_Agenda);
	       break;
	    case Agd_ONLY_PUBLIC_EVENTS:
	       sprintf (Gbl.Title,Txt_Public_agenda_USER,Gbl.Usrs.Me.UsrDat.FullName);
	       Lay_StartRoundFrame ("100%",Gbl.Title,
				    Agd_PutIconsListEvents,
				    Hlp_PROFILE_Agenda);
	       break;
	   }

	 /***** Put form to choice whether to show
                all events or only public events *****/
	 Act_FormStart (ActSeeMyAgd);
	 Agd_ShowFormToSelWhichEvents (ActSeeMyAgd);
	 Act_FormEnd ();
	 break;
     }

   if (Gbl.Agenda.Num)
     {
      /***** Table head *****/
      fprintf (Gbl.F.Out,"<table class=\"FRAME_TBL_MARGIN CELLS_PAD_2\">"
                         "<tr>");
      for (Order = Agd_ORDER_BY_START_DATE;
	   Order <= Agd_ORDER_BY_END_DATE;
	   Order++)
	{
	 fprintf (Gbl.F.Out,"<th class=\"LEFT_MIDDLE\">");
	 switch (AgendaType)
	   {
	    case Agd_USR_AGENDA:
	       Act_FormStart (ActSeeUsrAgd);
	       Usr_PutParamOtherUsrCodEncrypted ();
	       break;
	    case Agd_MY_AGENDA:
	       Act_FormStart (ActSeeMyAgd);
	       break;
	   }
	 Pag_PutHiddenParamPagNum (Gbl.Pag.CurrentPage);
	 Par_PutHiddenParamUnsigned ("Order",(unsigned) Order);
	 Act_LinkFormSubmit (Txt_ASG_ATT_OR_SVY_HELP_ORDER[Order],"TIT_TBL",NULL);
	 if (Order == Gbl.Agenda.SelectedOrderType)
	    fprintf (Gbl.F.Out,"<u>");
	 fprintf (Gbl.F.Out,"%s",Txt_ASG_ATT_OR_SVY_ORDER[Order]);
	 if (Order == Gbl.Agenda.SelectedOrderType)
	    fprintf (Gbl.F.Out,"</u>");
	 fprintf (Gbl.F.Out,"</a>");
	 Act_FormEnd ();
	 fprintf (Gbl.F.Out,"</th>");
	}
      fprintf (Gbl.F.Out,"<th class=\"LEFT_MIDDLE\">"
			 "%s"
			 "</th>"
			 "<th class=\"LEFT_MIDDLE\">"
			 "%s"
			 "</th>"
			 "</tr>",
	       Txt_Event,
	       Txt_Location);

      /***** Write all the events *****/
      for (NumEvent = Pagination.FirstItemVisible;
	   NumEvent <= Pagination.LastItemVisible;
	   NumEvent++)
	 Agd_ShowOneEvent (AgendaType,Gbl.Agenda.LstAgdCods[NumEvent - 1]);

      /***** End table *****/
      fprintf (Gbl.F.Out,"</table>");
     }
   else	// No events created
      Lay_ShowAlert (Lay_INFO,Txt_No_events);

   /***** Button to create a new event *****/
   if (AgendaType == Agd_MY_AGENDA)
      Agd_PutButtonToCreateNewEvent ();

   /***** End frame *****/
   Lay_EndRoundFrame ();

   /***** Write again links to pages *****/
   if (Pagination.MoreThanOnePage)
      Pag_WriteLinksToPagesCentered (WhatPaginate[AgendaType],0,&Pagination);

   /***** Free list of events *****/
   Agd_FreeListEvents ();
  }

/*****************************************************************************/
/*************** Put contextual icon to view/edit my agenda ******************/
/*****************************************************************************/

static void Agd_PutIconToViewEditMyAgenda (void)
  {
   extern const char *Txt_Edit;

   /***** Put icon to view/edit my agenda *****/
   Lay_PutContextualLink (ActSeeMyAgd,NULL,
                          "edit64x64.png",
                          Txt_Edit,NULL,
                          NULL);
  }

/*****************************************************************************/
/***************** Put contextual icons in list of events ********************/
/*****************************************************************************/

static void Agd_PutIconsListEvents (void)
  {
   /***** Put icon to create a new event *****/
   Agd_PutIconToCreateNewEvent ();

   /***** Put icon to show QR code *****/
   Agd_PutIconToShowQR ();
  }

/*****************************************************************************/
/********************** Put icon to create a new event ***********************/
/*****************************************************************************/

static void Agd_PutIconToCreateNewEvent (void)
  {
   extern const char *Txt_New_event;

   /***** Put form to create a new event *****/
   Lay_PutContextualLink (ActFrmNewEvtMyAgd,Agd_PutParamsToCreateNewEvent,
                          "plus64x64.png",
                          Txt_New_event,NULL,
                          NULL);
  }

/*****************************************************************************/
/********************** Put icon to create a new event ***********************/
/*****************************************************************************/

static void Agd_PutIconToShowQR (void)
  {
   char URL[Cns_MAX_BYTES_URL+1];
   extern const char *Txt_STR_LANG_ID[1+Txt_NUM_LANGUAGES];

   sprintf (URL,"%s/%s?agd=@%s",
            Cfg_URL_SWAD_CGI,
            Txt_STR_LANG_ID[Gbl.Prefs.Language],
            Gbl.Usrs.Me.UsrDat.Nickname);
   Gbl.QR.Str = URL;
   QR_PutLinkToPrintQRCode (false);
  }

/*****************************************************************************/
/********************* Put button to create a new event **********************/
/*****************************************************************************/

static void Agd_PutButtonToCreateNewEvent (void)
  {
   extern const char *Txt_New_event;

   Act_FormStart (ActFrmNewEvtMyAgd);
   Agd_PutParamsToCreateNewEvent ();
   Lay_PutConfirmButton (Txt_New_event);
   Act_FormEnd ();
  }

/*****************************************************************************/
/******************** Put parameters to create a new event *******************/
/*****************************************************************************/

static void Agd_PutParamsToCreateNewEvent (void)
  {
   Agd_PutHiddenParamEventsOrderType ();
   Pag_PutHiddenParamPagNum (Gbl.Pag.CurrentPage);
  }

/*****************************************************************************/
/*** Show form to choice whether to show all events or only public events ****/
/*****************************************************************************/

static void Agd_ShowFormToSelWhichEvents (Act_Action_t Action)
  {
   extern const char *Txt_Show_WHICH_events[2];
   Agd_WhichEvents_t WhichEvents;

   fprintf (Gbl.F.Out,"<div style=\"display:table-cell; padding:0 20px;\">");
   for (WhichEvents = Agd_ALL_EVENTS;
	WhichEvents <= Agd_ONLY_PUBLIC_EVENTS;
	WhichEvents++)
     {
      fprintf (Gbl.F.Out,"<div class=\"%s\" style=\"display:table-cell;\">",
	       WhichEvents == Gbl.Agenda.WhichEvents ? "PREF_ON" :
					               "PREF_OFF");
      Act_FormStart (Action);
      Par_PutHiddenParamUnsigned ("WhichEvents",(unsigned) WhichEvents);
      fprintf (Gbl.F.Out,"<input type=\"image\" src=\"%s/%s\""
			 " alt=\"%s\" title=\"%s\" class=\"ICO25x25\""
			 " style=\"margin:0 auto;\" />",
	       Gbl.Prefs.IconsURL,
	       WhichEvents == Agd_ONLY_PUBLIC_EVENTS ? "unlock64x64.png" :
		                                       "lockunlock64x64.png",
	       Txt_Show_WHICH_events[WhichEvents],
	       Txt_Show_WHICH_events[WhichEvents]);
      Act_FormEnd ();
      fprintf (Gbl.F.Out,"</div>");
     }
   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/************* Get whether to show all events or only my events **************/
/*****************************************************************************/

static void Agd_GetParamWhichEvents (void)
  {
   char UnsignedStr[10+1];
   unsigned UnsignedNum;

   /***** Get which events (all events or my events) *****/
   Par_GetParToText ("WhichEvents",UnsignedStr,1);
   if (UnsignedStr[0])
     {
      if (sscanf (UnsignedStr,"%u",&UnsignedNum) != 1)
	 Lay_ShowErrorAndExit ("Which events to show is missing.");
      if (UnsignedNum >= 2)
	 Lay_ShowErrorAndExit ("Wrong parameter with which events to show.");
      Gbl.Agenda.WhichEvents = (Agd_WhichEvents_t) UnsignedNum;
     }
  }

/*****************************************************************************/
/******************************* Show one event ******************************/
/*****************************************************************************/

static void Agd_ShowOneEvent (Agd_AgendaType_t AgendaType,long AgdCod)
  {
   extern const char *Dat_TimeStatusClassVisible[Dat_NUM_TIME_STATUS];
   extern const char *Dat_TimeStatusClassHidden[Dat_NUM_TIME_STATUS];
   extern const char *Txt_Today;
   static unsigned UniqueId = 0;
   struct AgendaEvent AgdEvent;
   char Txt[Cns_MAX_BYTES_TEXT+1];

   /***** Get data of this event *****/
   AgdEvent.AgdCod = AgdCod;
   switch (AgendaType)
     {
      case Agd_USR_AGENDA:
	 AgdEvent.UsrCod = Gbl.Usrs.Other.UsrDat.UsrCod;
         break;
      case Agd_MY_AGENDA:
	 AgdEvent.UsrCod = Gbl.Usrs.Me.UsrDat.UsrCod;
         break;
     }
   Agd_GetDataOfEventByCod (&AgdEvent);

   /***** Write first row of data of this event *****/
   /* Start date/time */
   UniqueId++;
   fprintf (Gbl.F.Out,"<tr>"
	              "<td id=\"agd_date_start_%u\" class=\"%s LEFT_TOP COLOR%u\">"
                      "<script type=\"text/javascript\">"
                      "writeLocalDateHMSFromUTC('agd_date_start_%u',%ld,'<br />','%s');"
                      "</script>"
	              "</td>",
	    UniqueId,
            AgdEvent.Hidden ? Dat_TimeStatusClassHidden[AgdEvent.TimeStatus] :
        	              Dat_TimeStatusClassVisible[AgdEvent.TimeStatus],
            Gbl.RowEvenOdd,
            UniqueId,AgdEvent.TimeUTC[Agd_START_TIME],Txt_Today);

   /* End date/time */
   UniqueId++;
   fprintf (Gbl.F.Out,"<td id=\"agd_date_end_%u\" class=\"%s LEFT_TOP COLOR%u\">"
                      "<script type=\"text/javascript\">"
                      "writeLocalDateHMSFromUTC('agd_date_end_%u',%ld,'<br />','%s');"
                      "</script>"
	              "</td>",
	    UniqueId,
            AgdEvent.Hidden ? Dat_TimeStatusClassHidden[AgdEvent.TimeStatus] :
        	              Dat_TimeStatusClassVisible[AgdEvent.TimeStatus],
            Gbl.RowEvenOdd,
            UniqueId,AgdEvent.TimeUTC[Agd_END_TIME],Txt_Today);

   /* Event */
   fprintf (Gbl.F.Out,"<td class=\"LEFT_TOP COLOR%u\">"
                      "<div class=\"%s\">%s</div>"
                      "</td>",
            Gbl.RowEvenOdd,
            AgdEvent.Hidden ? "ASG_TITLE_LIGHT" :
        	              "ASG_TITLE",
            AgdEvent.Event);

   /* Event */
   fprintf (Gbl.F.Out,"<td class=\"LEFT_TOP COLOR%u\">"
                      "<div class=\"%s\">%s</div>"
                      "</td>"
	              "</tr>",
            Gbl.RowEvenOdd,
            AgdEvent.Hidden ? "ASG_TITLE_LIGHT" :
        	              "ASG_TITLE",
            AgdEvent.Location);

   /***** Write second row of data of this event *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td colspan=\"2\" class=\"LEFT_TOP COLOR%u\">",
            Gbl.RowEvenOdd);

   /* Author of the event */
   Agd_WriteEventAuthor (&AgdEvent);

   /* Forms to remove/edit this event */
   if (AgendaType == Agd_MY_AGENDA)
      Agd_PutFormsToRemEditOneEvent (&AgdEvent);

   fprintf (Gbl.F.Out,"</td>");

   /* Text of the event */
   Agd_GetEventTxtFromDB (&AgdEvent,Txt);
   Str_ChangeFormat (Str_FROM_HTML,Str_TO_RIGOROUS_HTML,
                     Txt,Cns_MAX_BYTES_TEXT,false);	// Convert from HTML to recpectful HTML
   Str_InsertLinks (Txt,Cns_MAX_BYTES_TEXT,60);	// Insert links
   fprintf (Gbl.F.Out,"<td colspan=\"2\" class=\"LEFT_TOP COLOR%u\">",
            Gbl.RowEvenOdd);

   fprintf (Gbl.F.Out,"<p class=\"%s\">"
                      "%s"
                      "</p>"
                      "</td>"
                      "</tr>",
            AgdEvent.Hidden ? "DAT_LIGHT" :
        	              "DAT",
            Txt);

   Gbl.RowEvenOdd = 1 - Gbl.RowEvenOdd;
  }

/*****************************************************************************/
/*********************** Write the author of an event ************************/
/*****************************************************************************/

static void Agd_WriteEventAuthor (struct AgendaEvent *AgdEvent)
  {
   bool ShowPhoto = false;
   char PhotoURL[PATH_MAX+1];
   char FirstName[Usr_MAX_BYTES_NAME+1];
   char Surnames[2*(Usr_MAX_BYTES_NAME+1)];
   struct UsrData UsrDat;

   /***** Initialize structure with user's data *****/
   Usr_UsrDataConstructor (&UsrDat);

   /***** Get data of author *****/
   UsrDat.UsrCod = AgdEvent->UsrCod;
   if (Usr_ChkUsrCodAndGetAllUsrDataFromUsrCod (&UsrDat))
      ShowPhoto = Pho_ShowUsrPhotoIsAllowed (&UsrDat,PhotoURL);

   /***** Show photo *****/
   Pho_ShowUsrPhoto (&UsrDat,ShowPhoto ? PhotoURL :
                	                 NULL,
	             "PHOTO15x20",Pho_ZOOM,false);

   /***** Write name *****/
   strcpy (FirstName,UsrDat.FirstName);
   strcpy (Surnames,UsrDat.Surname1);
   if (UsrDat.Surname2[0])
     {
      strcat (Surnames," ");
      strcat (Surnames,UsrDat.Surname2);
     }
   Str_LimitLengthHTMLStr (FirstName,9);
   Str_LimitLengthHTMLStr (Surnames,9);
   fprintf (Gbl.F.Out,"<span class=\"%s\">%s %s</span>",
            AgdEvent->Hidden ? "MSG_AUT_LIGHT" :
        	               "MSG_AUT",
            FirstName,Surnames);

   /***** Free memory used for user's data *****/
   Usr_UsrDataDestructor (&UsrDat);
  }

/*****************************************************************************/
/********* Get parameter with the type or order in list of events ************/
/*****************************************************************************/

static void Agd_GetParamEventOrderType (void)
  {
   char UnsignedStr[10+1];
   unsigned UnsignedNum;

   Par_GetParToText ("Order",UnsignedStr,10);
   if (sscanf (UnsignedStr,"%u",&UnsignedNum) == 1)
      Gbl.Agenda.SelectedOrderType = (Agd_Order_t) UnsignedNum;
   else
      Gbl.Agenda.SelectedOrderType = Agd_DEFAULT_ORDER_TYPE;
  }

/*****************************************************************************/
/****** Put a hidden parameter with the type of order in list of events ******/
/*****************************************************************************/

void Agd_PutHiddenParamEventsOrderType (void)
  {
   Par_PutHiddenParamUnsigned ("Order",(unsigned) Gbl.Agenda.SelectedOrderType);
  }

/*****************************************************************************/
/******************* Put a link (form) to edit one event *********************/
/*****************************************************************************/

static void Agd_PutFormsToRemEditOneEvent (struct AgendaEvent *AgdEvent)
  {
   extern const char *Txt_Remove;
   extern const char *Txt_Show;
   extern const char *Txt_Hide;
   extern const char *Txt_Event_private_click_to_make_it_visible_to_the_users_of_your_courses;
   extern const char *Txt_Event_visible_to_the_users_of_your_courses_click_to_make_it_private;
   extern const char *Txt_Edit;

   fprintf (Gbl.F.Out,"<div>");

   Gbl.Agenda.AgdCodToEdit = AgdEvent->AgdCod;	// Used as parameter in contextual links

   /***** Put form to remove event *****/
   Lay_PutContextualLink (ActReqRemEvtMyAgd,Agd_PutParams,
                          "remove-on64x64.png",
                          Txt_Remove,NULL,
                          NULL);

   /***** Put form to hide/show event *****/
   if (AgdEvent->Hidden)
      Lay_PutContextualLink (ActShoEvtMyAgd,Agd_PutParams,
                             "eye-slash-on64x64.png",
			     Txt_Show,NULL,
                             NULL);
   else
      Lay_PutContextualLink (ActHidEvtMyAgd,Agd_PutParams,
                             "eye-on64x64.png",
			     Txt_Hide,NULL,
                             NULL);

   /***** Put form to edit event *****/
   Lay_PutContextualLink (ActEdiOneEvtMyAgd,Agd_PutParams,
                          "edit64x64.png",
                          Txt_Edit,NULL,
                          NULL);

   /***** Put form to make event public/private *****/
   if (Gbl.Usrs.Me.AvailableRoles & (1 << Rol_TEACHER))	// I am a teacher in some courses
     {
      if (AgdEvent->Public)
	 Lay_PutContextualLink (ActPrvEvtMyAgd,Agd_PutParams,
				"unlock-on64x64.png",
				Txt_Event_visible_to_the_users_of_your_courses_click_to_make_it_private,NULL,
				NULL);
      else
	 Lay_PutContextualLink (ActPubEvtMyAgd,Agd_PutParams,
				"lock-on64x64.png",
				Txt_Event_private_click_to_make_it_visible_to_the_users_of_your_courses,NULL,
				NULL);
     }

   fprintf (Gbl.F.Out,"</div>");
  }

/*****************************************************************************/
/********************** Params used to edit an event *************************/
/*****************************************************************************/

static void Agd_PutParams (void)
  {
   Agd_PutParamAgdCod (Gbl.Agenda.AgdCodToEdit);
   Agd_PutHiddenParamEventsOrderType ();
   Pag_PutHiddenParamPagNum (Gbl.Pag.CurrentPage);
  }

/*****************************************************************************/
/************************* Get list of agenda events *************************/
/*****************************************************************************/

static void Agd_GetListEvents (Agd_AgendaType_t AgendaType)
  {
   char OrderBySubQuery[256];
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;
   unsigned NumEvent;

   if (Gbl.Agenda.LstIsRead)
      Agd_FreeListEvents ();

   /***** Get list of events from database *****/
   switch (Gbl.Agenda.SelectedOrderType)
     {
      case Agd_ORDER_BY_START_DATE:
         sprintf (OrderBySubQuery,"StartTime DESC,"
                                  "EndTime DESC,"
                                  "Event DESC,"
                                  "Location DESC");
         break;
      case Agd_ORDER_BY_END_DATE:
         sprintf (OrderBySubQuery,"EndTime DESC,"
                                  "StartTime DESC,"
                                  "Event DESC,"
                                  "Location DESC");
         break;
     }
   switch (AgendaType)
     {
      case Agd_USR_AGENDA:
	 sprintf (Query,"SELECT AgdCod FROM agendas"
			" WHERE UsrCod='%ld' AND Public='Y' AND Hidden='N'"
			" AND EndTime>NOW()"	// Only present and future events
			" ORDER BY %s",
		  Gbl.Usrs.Other.UsrDat.UsrCod,OrderBySubQuery);
         break;
      case Agd_MY_AGENDA:
	 switch (Gbl.Agenda.WhichEvents)
	   {
	    case Agd_ALL_EVENTS:
	       sprintf (Query,"SELECT AgdCod FROM agendas"
			      " WHERE UsrCod='%ld'"
			      " ORDER BY %s",
			Gbl.Usrs.Me.UsrDat.UsrCod,OrderBySubQuery);
	       break;
	    case Agd_ONLY_PUBLIC_EVENTS:
	       sprintf (Query,"SELECT AgdCod FROM agendas"
			      " WHERE UsrCod='%ld' AND Public='Y'"
			      " ORDER BY %s",
			Gbl.Usrs.Me.UsrDat.UsrCod,OrderBySubQuery);
	       break;
	   }
         break;
     }
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get agenda events");

   if (NumRows) // Events found...
     {
      Gbl.Agenda.Num = (unsigned) NumRows;

      /***** Create list of events *****/
      if ((Gbl.Agenda.LstAgdCods = (long *) calloc (NumRows,sizeof (long))) == NULL)
          Lay_ShowErrorAndExit ("Not enough memory to store list of agenda events.");

      /***** Get the events codes *****/
      for (NumEvent = 0;
	   NumEvent < Gbl.Agenda.Num;
	   NumEvent++)
        {
         /* Get next event code */
         row = mysql_fetch_row (mysql_res);
         if ((Gbl.Agenda.LstAgdCods[NumEvent] = Str_ConvertStrCodToLongCod (row[0])) < 0)
            Lay_ShowErrorAndExit ("Error: wrong event code.");
        }
     }
   else
      Gbl.Agenda.Num = 0;

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   Gbl.Agenda.LstIsRead = true;
  }

/*****************************************************************************/
/*********************** Get event data using its code ***********************/
/*****************************************************************************/

static void Agd_GetDataOfEventByCod (struct AgendaEvent *AgdEvent)
  {
   char Query[512];

   /***** Build query *****/
   sprintf (Query,"SELECT AgdCod,UsrCod,Public,Hidden,"
                  "UNIX_TIMESTAMP(StartTime),"
                  "UNIX_TIMESTAMP(EndTime),"
                  "NOW()>EndTime,"	// Past event?
                  "NOW()<StartTime,"	// Future event?
                  "Event,Location"
                  " FROM agendas"
                  " WHERE AgdCod='%ld' AND UsrCod='%ld'",
            AgdEvent->AgdCod,AgdEvent->UsrCod);

   /***** Get data of event *****/
   Agd_GetDataOfEvent (AgdEvent,Query);
  }

/*****************************************************************************/
/****************************** Get event data *******************************/
/*****************************************************************************/

static void Agd_GetDataOfEvent (struct AgendaEvent *AgdEvent,const char *Query)
  {
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;

   /***** Clear all event data *****/
   AgdEvent->AgdCod = -1L;
   AgdEvent->UsrCod = -1L;
   AgdEvent->Public = false;
   AgdEvent->Hidden = false;
   AgdEvent->TimeUTC[Agd_START_TIME] =
   AgdEvent->TimeUTC[Agd_END_TIME  ] = (time_t) 0;
   AgdEvent->TimeStatus = Dat_FUTURE;
   AgdEvent->Event[0]    = '\0';
   AgdEvent->Location[0] = '\0';

   /***** Get data of event from database *****/
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get agenda event data");

   if (NumRows) // Event found...
     {
      /* Get row:
      row[0] AgdCod
      row[1] UsrCod
      row[2] Public
      row[3] Hidden
      row[4] UNIX_TIMESTAMP(StartTime)
      row[5] UNIX_TIMESTAMP(EndTime)
      row[6] NOW()>EndTime	// Past event?
      row[7] NOW()<StartTime	// Future event?
      row[8] Event
      row[9] Location
      */
      row = mysql_fetch_row (mysql_res);

      /* Get code of the event (row[0]) */
      AgdEvent->AgdCod = Str_ConvertStrCodToLongCod (row[0]);

      /* Get author of the event (row[1]) */
      AgdEvent->UsrCod = Str_ConvertStrCodToLongCod (row[1]);

      /* Get whether the event is public or not (row[2]) */
      AgdEvent->Public = (row[2][0] == 'Y');

      /* Get whether the event is hidden or not (row[3]) */
      AgdEvent->Hidden = (row[3][0] == 'Y');

      /* Get start date (row[4] holds the start UTC time) */
      AgdEvent->TimeUTC[Agd_START_TIME] = Dat_GetUNIXTimeFromStr (row[4]);

      /* Get end date   (row[5] holds the end   UTC time) */
      AgdEvent->TimeUTC[Agd_END_TIME  ] = Dat_GetUNIXTimeFromStr (row[5]);

      /* Get whether the event is past, present or futur (row(6), row[7]) */
      AgdEvent->TimeStatus = ((row[6][0] == '1') ? Dat_PAST :
	                     ((row[7][0] == '1') ? Dat_FUTURE :
	                	                   Dat_PRESENT));

      /* Get the event (row[8]) */
      strcpy (AgdEvent->Event,row[8]);

      /* Get the event (row[9]) */
      strcpy (AgdEvent->Location,row[9]);
     }

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);
  }

/*****************************************************************************/
/*************************** Free list of events *****************************/
/*****************************************************************************/

void Agd_FreeListEvents (void)
  {
   if (Gbl.Agenda.LstIsRead && Gbl.Agenda.LstAgdCods)
     {
      /***** Free memory used by the list of events *****/
      free ((void *) Gbl.Agenda.LstAgdCods);
      Gbl.Agenda.LstAgdCods = NULL;
      Gbl.Agenda.Num = 0;
      Gbl.Agenda.LstIsRead = false;
     }
  }

/*****************************************************************************/
/*********************** Get event text from database ************************/
/*****************************************************************************/

static void Agd_GetEventTxtFromDB (struct AgendaEvent *AgdEvent,char *Txt)
  {
   char Query[512];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned long NumRows;

   /***** Get text of event from database *****/
   sprintf (Query,"SELECT Txt FROM agendas"
	          " WHERE AgdCod='%ld' AND UsrCod='%ld'",
            AgdEvent->AgdCod,AgdEvent->UsrCod);
   NumRows = DB_QuerySELECT (Query,&mysql_res,"can not get event text");

   /***** The result of the query must have one row or none *****/
   if (NumRows == 1)
     {
      /* Get info text */
      row = mysql_fetch_row (mysql_res);
      strcpy (Txt,row[0]);
     }
   else
      Txt[0] = '\0';

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   if (NumRows > 1)
      Lay_ShowErrorAndExit ("Error when getting event text.");
  }

/*****************************************************************************/
/******************* Write parameter with code of event **********************/
/*****************************************************************************/

static void Agd_PutParamAgdCod (long AgdCod)
  {
   Par_PutHiddenParamLong ("AgdCod",AgdCod);
  }

/*****************************************************************************/
/******************** Get parameter with code of event ***********************/
/*****************************************************************************/

long Agd_GetParamAgdCod (void)
  {
   char LongStr[1+10+1];

   /***** Get parameter with code of event *****/
   Par_GetParToText ("AgdCod",LongStr,1+10);
   return Str_ConvertStrCodToLongCod (LongStr);
  }

/*****************************************************************************/
/************** Ask for confirmation of removing of an event *****************/
/*****************************************************************************/

void Agd_AskRemEvent (void)
  {
   extern const char *Txt_Do_you_really_want_to_remove_the_event_X;
   extern const char *Txt_Remove_event;
   struct AgendaEvent AgdEvent;

   /***** Get parameters *****/
   Agd_GetParamEventOrderType ();
   Pag_GetParamPagNum (Pag_MY_AGENDA);

   /***** Get event code *****/
   if ((AgdEvent.AgdCod = Agd_GetParamAgdCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of event is missing.");

   /***** Get data of the event from database *****/
   AgdEvent.UsrCod = Gbl.Usrs.Me.UsrDat.UsrCod;
   Agd_GetDataOfEventByCod (&AgdEvent);

   /***** Button of confirmation of removing *****/
   Act_FormStart (ActRemEvtMyAgd);
   Agd_PutParamAgdCod (AgdEvent.AgdCod);
   Agd_PutHiddenParamEventsOrderType ();
   Pag_PutHiddenParamPagNum (Gbl.Pag.CurrentPage);

   /***** Ask for confirmation of removing *****/
   sprintf (Gbl.Message,Txt_Do_you_really_want_to_remove_the_event_X,
            AgdEvent.Event);
   Lay_ShowAlert (Lay_WARNING,Gbl.Message);
   Lay_PutRemoveButton (Txt_Remove_event);
   Act_FormEnd ();

   /***** Show events again *****/
   Agd_ShowMyAgenda ();
  }

/*****************************************************************************/
/****************************** Remove an event ******************************/
/*****************************************************************************/

void Agd_RemoveEvent (void)
  {
   extern const char *Txt_Event_X_removed;
   char Query[512];
   struct AgendaEvent AgdEvent;

   /***** Get event code *****/
   if ((AgdEvent.AgdCod = Agd_GetParamAgdCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of event is missing.");

   /***** Get data of the event from database *****/
   AgdEvent.UsrCod = Gbl.Usrs.Me.UsrDat.UsrCod;
   Agd_GetDataOfEventByCod (&AgdEvent);	// Inside this function, the course is checked to be the current one

   /***** Remove event *****/
   sprintf (Query,"DELETE FROM agendas"
                  " WHERE AgdCod='%ld' AND UsrCod='%ld'",
            AgdEvent.AgdCod,Gbl.Usrs.Me.UsrDat.UsrCod);
   DB_QueryDELETE (Query,"can not remove event");

   /***** Write message to show the change made *****/
   sprintf (Gbl.Message,Txt_Event_X_removed,AgdEvent.Event);
   Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);

   /***** Show events again *****/
   Agd_ShowMyAgenda ();
  }

/*****************************************************************************/
/********************************* Hide event ********************************/
/*****************************************************************************/

void Agd_HideEvent (void)
  {
   extern const char *Txt_Event_X_is_now_hidden;
   char Query[512];
   struct AgendaEvent AgdEvent;

   /***** Get event code *****/
   if ((AgdEvent.AgdCod = Agd_GetParamAgdCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of event is missing.");

   /***** Get data of the event from database *****/
   AgdEvent.UsrCod = Gbl.Usrs.Me.UsrDat.UsrCod;
   Agd_GetDataOfEventByCod (&AgdEvent);

   /***** Set event private *****/
   sprintf (Query,"UPDATE agendas SET Hidden='Y'"
                  " WHERE AgdCod='%ld' AND UsrCod='%ld'",
            AgdEvent.AgdCod,Gbl.Usrs.Me.UsrDat.UsrCod);
   DB_QueryUPDATE (Query,"can not hide event");

   /***** Write message to show the change made *****/
   sprintf (Gbl.Message,Txt_Event_X_is_now_hidden,AgdEvent.Event);
   Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);

   /***** Show events again *****/
   Agd_ShowMyAgenda ();
  }

/*****************************************************************************/
/******************************** Show event *********************************/
/*****************************************************************************/

void Agd_ShowEvent (void)
  {
   extern const char *Txt_Event_X_is_now_visible;
   char Query[256];
   struct AgendaEvent AgdEvent;

   /***** Get event code *****/
   if ((AgdEvent.AgdCod = Agd_GetParamAgdCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of event is missing.");

   /***** Get data of the event from database *****/
   AgdEvent.UsrCod = Gbl.Usrs.Me.UsrDat.UsrCod;
   Agd_GetDataOfEventByCod (&AgdEvent);

   /***** Set event public *****/
   sprintf (Query,"UPDATE agendas SET Hidden='N'"
                  " WHERE AgdCod='%ld' AND UsrCod='%ld'",
            AgdEvent.AgdCod,Gbl.Usrs.Me.UsrDat.UsrCod);
   DB_QueryUPDATE (Query,"can not show event");

   /***** Write message to show the change made *****/
   sprintf (Gbl.Message,Txt_Event_X_is_now_visible,
            AgdEvent.Event);
   Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);

   /***** Show events again *****/
   Agd_ShowMyAgenda ();
  }

/*****************************************************************************/
/****************************** Make event private ***************************/
/*****************************************************************************/

void Agd_MakeEventPrivate (void)
  {
   extern const char *Txt_Event_X_is_now_private;
   char Query[512];
   struct AgendaEvent AgdEvent;

   /***** Get event code *****/
   if ((AgdEvent.AgdCod = Agd_GetParamAgdCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of event is missing.");

   /***** Get data of the event from database *****/
   AgdEvent.UsrCod = Gbl.Usrs.Me.UsrDat.UsrCod;
   Agd_GetDataOfEventByCod (&AgdEvent);

   /***** Make event private *****/
   sprintf (Query,"UPDATE agendas SET Public='N'"
                  " WHERE AgdCod='%ld' AND UsrCod='%ld'",
            AgdEvent.AgdCod,Gbl.Usrs.Me.UsrDat.UsrCod);
   DB_QueryUPDATE (Query,"can not make event private");

   /***** Write message to show the change made *****/
   sprintf (Gbl.Message,Txt_Event_X_is_now_private,AgdEvent.Event);
   Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);

   /***** Show events again *****/
   Agd_ShowMyAgenda ();
  }

/*****************************************************************************/
/******** Make event public (make it visible to users of my courses) *********/
/*****************************************************************************/

void Agd_MakeEventPublic (void)
  {
   extern const char *Txt_Event_X_is_now_visible_to_users_of_your_courses;
   char Query[256];
   struct AgendaEvent AgdEvent;

   /***** Get event code *****/
   if ((AgdEvent.AgdCod = Agd_GetParamAgdCod ()) == -1L)
      Lay_ShowErrorAndExit ("Code of event is missing.");

   /***** Get data of the event from database *****/
   AgdEvent.UsrCod = Gbl.Usrs.Me.UsrDat.UsrCod;
   Agd_GetDataOfEventByCod (&AgdEvent);

   /***** Make event public *****/
   sprintf (Query,"UPDATE agendas SET Public='Y'"
                  " WHERE AgdCod='%ld' AND UsrCod='%ld'",
            AgdEvent.AgdCod,Gbl.Usrs.Me.UsrDat.UsrCod);
   DB_QueryUPDATE (Query,"can not make event public");

   /***** Write message to show the change made *****/
   sprintf (Gbl.Message,Txt_Event_X_is_now_visible_to_users_of_your_courses,
            AgdEvent.Event);
   Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);

   /***** Show events again *****/
   Agd_ShowMyAgenda ();
  }

/*****************************************************************************/
/*********** Check if the title or the folder of an event exists *************/
/*****************************************************************************/

static bool Agd_CheckIfSimilarEventExists (const char *Field,const char *Value,long AgdCod)
  {
   char Query[512];

   /***** Get number of events with a field value from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM agendas"
	          " WHERE UsrCod='%ld' AND %s='%s' AND AgdCod<>'%ld'",
            Gbl.Usrs.Me.UsrDat.UsrCod,Field,Value,AgdCod);
   return (DB_QueryCOUNT (Query,"can not get similar events") != 0);
  }

/*****************************************************************************/
/******************** Put a form to create a new event ***********************/
/*****************************************************************************/

void Agd_RequestCreatOrEditEvent (void)
  {
   extern const char *Hlp_PROFILE_Agenda_new_event;
   extern const char *Hlp_PROFILE_Agenda_edit_event;
   extern const char *The_ClassForm[The_NUM_THEMES];
   extern const char *Txt_New_event;
   extern const char *Txt_Edit_event;
   extern const char *Txt_Location;
   extern const char *Txt_Event;
   extern const char *Txt_Description;
   extern const char *Txt_Create_event;
   extern const char *Txt_Save;
   struct AgendaEvent AgdEvent;
   bool ItsANewEvent;
   char Txt[Cns_MAX_BYTES_TEXT+1];

   /***** Get parameters *****/
   Agd_GetParamEventOrderType ();
   Pag_GetParamPagNum (Pag_MY_AGENDA);

   /***** Get the code of the event *****/
   ItsANewEvent = ((AgdEvent.AgdCod = Agd_GetParamAgdCod ()) == -1L);

   /***** Get from the database the data of the event *****/
   if (ItsANewEvent)
     {
      /* Initialize to empty event */
      AgdEvent.AgdCod = -1L;
      AgdEvent.UsrCod = Gbl.Usrs.Me.UsrDat.UsrCod;
      AgdEvent.TimeUTC[Agd_START_TIME] = Gbl.StartExecutionTimeUTC;
      AgdEvent.TimeUTC[Agd_END_TIME  ] = Gbl.StartExecutionTimeUTC + (2 * 60 * 60);	// +2 hours
      AgdEvent.TimeStatus = Dat_FUTURE;
      AgdEvent.Event[0]    = '\0';
      AgdEvent.Location[0] = '\0';
     }
   else
     {
      AgdEvent.UsrCod = Gbl.Usrs.Me.UsrDat.UsrCod;

      /* Get data of the event from database */
      Agd_GetDataOfEventByCod (&AgdEvent);

      /* Get text of the event from database */
      Agd_GetEventTxtFromDB (&AgdEvent,Txt);
     }

   /***** Start form *****/
   if (ItsANewEvent)
      Act_FormStart (ActNewEvtMyAgd);
   else
     {
      Act_FormStart (ActChgEvtMyAgd);
      Agd_PutParamAgdCod (AgdEvent.AgdCod);
     }
   Agd_PutHiddenParamEventsOrderType ();
   Pag_PutHiddenParamPagNum (Gbl.Pag.CurrentPage);

   /***** Table start *****/
   Lay_StartRoundFrameTable (NULL,
                             ItsANewEvent ? Txt_New_event :
                                            Txt_Edit_event,
                             NULL,
                             ItsANewEvent ? Hlp_PROFILE_Agenda_new_event :
                        	            Hlp_PROFILE_Agenda_edit_event,
                             2);

   /***** Event *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"%s RIGHT_MIDDLE\">"
	              "%s:"
	              "</td>"
                      "<td class=\"LEFT_MIDDLE\">"
                      "<input type=\"text\" name=\"Event\""
                      " size=\"45\" maxlength=\"%u\" value=\"%s\""
                      " required=\"required\" />"
                      "</td>"
                      "</tr>",
            The_ClassForm[Gbl.Prefs.Theme],
            Txt_Event,
            Agd_MAX_LENGTH_EVENT,AgdEvent.Event);

   /***** Location *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"%s RIGHT_MIDDLE\">"
	              "%s:"
	              "</td>"
                      "<td class=\"LEFT_MIDDLE\">"
                      "<input type=\"text\" name=\"Location\""
                      " size=\"45\" maxlength=\"%u\" value=\"%s\""
                      " required=\"required\" />"
                      "</td>"
                      "</tr>",
            The_ClassForm[Gbl.Prefs.Theme],
            Txt_Location,
            Agd_MAX_LENGTH_LOCATION,AgdEvent.Location);

   /***** Start and end dates *****/
   Dat_PutFormStartEndClientLocalDateTimes (AgdEvent.TimeUTC,
                                            Dat_FORM_SECONDS_OFF);

   /***** Text *****/
   fprintf (Gbl.F.Out,"<tr>"
	              "<td class=\"%s RIGHT_TOP\">"
	              "%s:"
	              "</td>"
                      "<td class=\"LEFT_TOP\">"
                      "<textarea name=\"Txt\" cols=\"60\" rows=\"10\">",
            The_ClassForm[Gbl.Prefs.Theme],
            Txt_Description);
   if (!ItsANewEvent)
      fprintf (Gbl.F.Out,"%s",Txt);
   fprintf (Gbl.F.Out,"</textarea>"
                      "</td>"
                      "</tr>");

   /***** New event *****/
   if (ItsANewEvent)
      Lay_EndRoundFrameTableWithButton (Lay_CREATE_BUTTON,Txt_Create_event);
   else
      Lay_EndRoundFrameTableWithButton (Lay_CONFIRM_BUTTON,Txt_Save);
   Act_FormEnd ();

   /***** Show current events, if any *****/
   Agd_ShowEvents (Agd_MY_AGENDA);
  }

/*****************************************************************************/
/********************* Receive form to create a new event ********************/
/*****************************************************************************/

void Agd_RecFormEvent (void)
  {
   extern const char *Txt_Already_existed_an_event_with_the_title_X;
   extern const char *Txt_You_must_specify_the_title_of_the_event;
   extern const char *Txt_Created_new_event_X;
   extern const char *Txt_The_event_has_been_modified;
   struct AgendaEvent OldAgdEvent;
   struct AgendaEvent NewAgdEvent;
   bool ItsANewEvent;
   bool NewEventIsCorrect = true;
   char Txt[Cns_MAX_BYTES_TEXT+1];

   /***** Get the code of the event *****/
   ItsANewEvent = ((NewAgdEvent.AgdCod = Agd_GetParamAgdCod ()) == -1L);

   if (!ItsANewEvent)
     {
      /* Get data of the old (current) event from database */
      OldAgdEvent.AgdCod = NewAgdEvent.AgdCod;
      OldAgdEvent.UsrCod = Gbl.Usrs.Me.UsrDat.UsrCod;
      Agd_GetDataOfEventByCod (&OldAgdEvent);
     }

   /***** Get start/end date-times *****/
   NewAgdEvent.TimeUTC[Agd_START_TIME] = Dat_GetTimeUTCFromForm ("StartTimeUTC");
   NewAgdEvent.TimeUTC[Agd_END_TIME  ] = Dat_GetTimeUTCFromForm ("EndTimeUTC"  );

   /***** Get event *****/
   Par_GetParToText ("Location",NewAgdEvent.Location,Agd_MAX_LENGTH_LOCATION);

   /***** Get event *****/
   Par_GetParToText ("Event",NewAgdEvent.Event,Agd_MAX_LENGTH_EVENT);

   /***** Get text *****/
   Par_GetParToHTML ("Txt",Txt,Cns_MAX_BYTES_TEXT);	// Store in HTML format (not rigorous)

   /***** Adjust dates *****/
   if (NewAgdEvent.TimeUTC[Agd_START_TIME] == 0)
      NewAgdEvent.TimeUTC[Agd_START_TIME] = Gbl.StartExecutionTimeUTC;
   if (NewAgdEvent.TimeUTC[Agd_END_TIME] == 0)
      NewAgdEvent.TimeUTC[Agd_END_TIME] = NewAgdEvent.TimeUTC[Agd_START_TIME] + 2*60*60;	// +2 hours

   /***** Check if event is correct *****/
   if (!NewAgdEvent.Location[0])	// If there is no event
     {
      NewEventIsCorrect = false;
      Lay_ShowAlert (Lay_WARNING,Txt_You_must_specify_the_title_of_the_event);
     }

   /***** Check if event is correct *****/
   if (NewAgdEvent.Event[0])	// If there's event
     {
      /* If title of event was in database... */
      if (Agd_CheckIfSimilarEventExists ("Event",NewAgdEvent.Event,NewAgdEvent.AgdCod))
        {
         NewEventIsCorrect = false;
         sprintf (Gbl.Message,Txt_Already_existed_an_event_with_the_title_X,
                  NewAgdEvent.Event);
         Lay_ShowAlert (Lay_WARNING,Gbl.Message);
        }
     }
   else	// If there is no event
     {
      NewEventIsCorrect = false;
      Lay_ShowAlert (Lay_WARNING,Txt_You_must_specify_the_title_of_the_event);
     }

   /***** Create a new event or update an existing one *****/
   if (NewEventIsCorrect)
     {
      if (ItsANewEvent)
	{
         Agd_CreateEvent (&NewAgdEvent,Txt);	// Add new event to database

	 /***** Write success message *****/
	 sprintf (Gbl.Message,Txt_Created_new_event_X,NewAgdEvent.Event);
	 Lay_ShowAlert (Lay_SUCCESS,Gbl.Message);
	}
      else
        {
	 Agd_UpdateEvent (&NewAgdEvent,Txt);

	 /***** Write success message *****/
	 Lay_ShowAlert (Lay_SUCCESS,Txt_The_event_has_been_modified);
        }

      /* Free memory for list of selected groups */
      Grp_FreeListCodSelectedGrps ();

      /***** Show events again *****/
      Agd_ShowMyAgenda ();
     }
   else
      // TODO: The form should be filled with partial data, now is always empty
      Agd_RequestCreatOrEditEvent ();
  }

/*****************************************************************************/
/************************** Create a new event *******************************/
/*****************************************************************************/

static void Agd_CreateEvent (struct AgendaEvent *AgdEvent,const char *Txt)
  {
   char Query[1024+Cns_MAX_BYTES_TEXT];

   /***** Create a new event *****/
   sprintf (Query,"INSERT INTO agendas"
	          " (UsrCod,StartTime,EndTime,Event,Location,Txt)"
                  " VALUES"
                  " ('%ld',FROM_UNIXTIME('%ld'),FROM_UNIXTIME('%ld'),"
                  "'%s','%s','%s')",
            Gbl.Usrs.Me.UsrDat.UsrCod,
            AgdEvent->TimeUTC[Agd_START_TIME],
            AgdEvent->TimeUTC[Agd_END_TIME  ],
            AgdEvent->Event,
            AgdEvent->Location,
            Txt);
   AgdEvent->AgdCod = DB_QueryINSERTandReturnCode (Query,"can not create new event");
  }

/*****************************************************************************/
/************************ Update an existing event ***************************/
/*****************************************************************************/

static void Agd_UpdateEvent (struct AgendaEvent *AgdEvent,const char *Txt)
  {
   char Query[1024+Cns_MAX_BYTES_TEXT];

   /***** Update the data of the event *****/
   sprintf (Query,"UPDATE agendas SET "
	          "StartTime=FROM_UNIXTIME('%ld'),"
	          "EndTime=FROM_UNIXTIME('%ld'),"
                  "Event='%s',Location='%s',Txt='%s'"
                  " WHERE AgdCod='%ld' AND UsrCod='%ld'",
            AgdEvent->TimeUTC[Agd_START_TIME],
            AgdEvent->TimeUTC[Agd_END_TIME  ],
            AgdEvent->Event,AgdEvent->Location,Txt,
            AgdEvent->AgdCod,Gbl.Usrs.Me.UsrDat.UsrCod);
   DB_QueryUPDATE (Query,"can not update event");
  }

/*****************************************************************************/
/********************** Remove all the events of a user **********************/
/*****************************************************************************/

void Agd_RemoveUsrEvents (long UsrCod)
  {
   char Query[128];

   /***** Remove events *****/
   sprintf (Query,"DELETE FROM agendas WHERE UsrCod='%ld'",UsrCod);
   DB_QueryDELETE (Query,"can not remove all the events of a user");
  }

/*****************************************************************************/
/********************* Get number of events from a user **********************/
/*****************************************************************************/

unsigned Agd_GetNumEventsFromUsr (long UsrCod)
  {
   char Query[128];

   /***** Get number of events in a course from database *****/
   sprintf (Query,"SELECT COUNT(*) FROM agendas WHERE UsrCod='%ld'",
            UsrCod);
   return (unsigned) DB_QueryCOUNT (Query,"can not get number of events from user");
  }

/*****************************************************************************/
/********************** Get number of users with events **********************/
/*****************************************************************************/
// Returns the number of users with events in a given scope

unsigned Agd_GetNumUsrsWithEvents (Sco_Scope_t Scope)
  {
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumUsrs;

   /***** Get number of courses with events from database *****/
   switch (Scope)
     {
      case Sco_SCOPE_SYS:
         sprintf (Query,"SELECT COUNT(DISTINCT UsrCod)"
                        " FROM agendas"
                        " WHERE UsrCod>'0'");
         break;
       case Sco_SCOPE_CTY:
         sprintf (Query,"SELECT COUNT(DISTINCT agendas.UsrCod)"
                        " FROM institutions,centres,degrees,courses,crs_usr,agendas"
                        " WHERE institutions.CtyCod='%ld'"
                        " AND institutions.InsCod=centres.InsCod"
                        " AND centres.CtrCod=degrees.CtrCod"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.Status=0"
                        " AND courses.CrsCod=crs_usr.CrsCod"
                        " AND crs_usr.UsrCod=agendas.UsrCod",
                  Gbl.CurrentCty.Cty.CtyCod);
         break;
       case Sco_SCOPE_INS:
         sprintf (Query,"SELECT COUNT(DISTINCT agendas.UsrCod)"
                        " FROM centres,degrees,courses,crs_usr,agendas"
                        " WHERE centres.InsCod='%ld'"
                        " AND centres.CtrCod=degrees.CtrCod"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.Status=0"
                        " AND courses.CrsCod=crs_usr.CrsCod"
                        " AND crs_usr.UsrCod=agendas.UsrCod",
                  Gbl.CurrentIns.Ins.InsCod);
         break;
      case Sco_SCOPE_CTR:
         sprintf (Query,"SELECT COUNT(DISTINCT agendas.UsrCod)"
                        " FROM degrees,courses,crs_usr,agendas"
                        " WHERE degrees.CtrCod='%ld'"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.Status=0"
                        " AND courses.CrsCod=crs_usr.CrsCod"
                        " AND crs_usr.UsrCod=agendas.UsrCod",
                  Gbl.CurrentCtr.Ctr.CtrCod);
         break;
      case Sco_SCOPE_DEG:
         sprintf (Query,"SELECT COUNT(DISTINCT agendas.UsrCod)"
                        " FROM courses,crs_usr,agendas"
                        " WHERE courses.DegCod='%ld'"
                        " AND courses.Status=0"
                        " AND courses.CrsCod=crs_usr.CrsCod"
                        " AND crs_usr.UsrCod=agendas.UsrCod",
                  Gbl.CurrentDeg.Deg.DegCod);
         break;
      case Sco_SCOPE_CRS:
         sprintf (Query,"SELECT COUNT(DISTINCT agendas.UsrCod)"
                        " FROM crs_usr,agendas"
                        " WHERE crs_usr.CrsCod='%ld'"
                        " AND crs_usr.UsrCod=agendas.UsrCod",
                  Gbl.CurrentCrs.Crs.CrsCod);
         break;
      default:
	 Lay_ShowErrorAndExit ("Wrong scope.");
	 break;
     }
   DB_QuerySELECT (Query,&mysql_res,"can not get number of users with events");

   /***** Get number of users *****/
   row = mysql_fetch_row (mysql_res);
   if (sscanf (row[0],"%u",&NumUsrs) != 1)
      Lay_ShowErrorAndExit ("Error when getting number of users with events.");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NumUsrs;
  }

/*****************************************************************************/
/*************************** Get number of events ****************************/
/*****************************************************************************/
// Returns the number of events in a given scope

unsigned Agd_GetNumEvents (Sco_Scope_t Scope)
  {
   char Query[1024];
   MYSQL_RES *mysql_res;
   MYSQL_ROW row;
   unsigned NumEvents;

   /***** Get number of events from database *****/
   switch (Scope)
     {
      case Sco_SCOPE_SYS:
         sprintf (Query,"SELECT COUNT(*)"
                        " FROM agendas"
                        " WHERE UsrCod>'0'");
         break;
      case Sco_SCOPE_CTY:
         sprintf (Query,"SELECT COUNT(*)"
                        " FROM institutions,centres,degrees,courses,crs_usr,agendas"
                        " WHERE institutions.CtyCod='%ld'"
                        " AND institutions.InsCod=centres.InsCod"
                        " AND centres.CtrCod=degrees.CtrCod"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.CrsCod=crs_usr.CrsCod"
                        " AND crs_usr.UsrCod=agendas.UsrCod",
                  Gbl.CurrentCty.Cty.CtyCod);
         break;
      case Sco_SCOPE_INS:
         sprintf (Query,"SELECT COUNT(*)"
                        " FROM centres,degrees,courses,crs_usr,agendas"
                        " WHERE centres.InsCod='%ld'"
                        " AND centres.CtrCod=degrees.CtrCod"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.CrsCod=crs_usr.CrsCod"
                        " AND crs_usr.UsrCod=agendas.UsrCod",
                  Gbl.CurrentIns.Ins.InsCod);
         break;
      case Sco_SCOPE_CTR:
         sprintf (Query,"SELECT COUNT(*)"
                        " FROM degrees,courses,crs_usr,agendas"
                        " WHERE degrees.CtrCod='%ld'"
                        " AND degrees.DegCod=courses.DegCod"
                        " AND courses.CrsCod=crs_usr.CrsCod"
                        " AND crs_usr.UsrCod=agendas.UsrCod",
                  Gbl.CurrentCtr.Ctr.CtrCod);
         break;
      case Sco_SCOPE_DEG:
         sprintf (Query,"SELECT COUNT(*)"
                        " FROM courses,crs_usr,agendas"
                        " WHERE courses.DegCod='%ld'"
                        " AND courses.CrsCod=crs_usr.CrsCod"
                        " AND crs_usr.UsrCod=agendas.UsrCod",
                  Gbl.CurrentDeg.Deg.DegCod);
         break;
      case Sco_SCOPE_CRS:
         sprintf (Query,"SELECT COUNT(*)"
                        " FROM crs_usr,agendas"
                        " WHERE crs_usr.CrsCod='%ld'"
                        " AND crs_usr.UsrCod=agendas.UsrCod",
                  Gbl.CurrentCrs.Crs.CrsCod);
         break;
      default:
	 Lay_ShowErrorAndExit ("Wrong scope.");
	 break;
     }
   DB_QuerySELECT (Query,&mysql_res,"can not get number of events");

   /***** Get number of events *****/
   row = mysql_fetch_row (mysql_res);
   if (sscanf (row[0],"%u",&NumEvents) != 1)
      Lay_ShowErrorAndExit ("Error when getting number of events.");

   /***** Free structure that stores the query result *****/
   DB_FreeMySQLResult (&mysql_res);

   return NumEvents;
  }
