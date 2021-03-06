// swad_help.c: contextual help to enhance usability

/*
    SWAD (Shared Workspace At a Distance),
    is a web platform developed at the University of Granada (Spain),
    and used to support university teaching.

    This file is part of SWAD core.
    Copyright (C) 1999-2017 Antonio Ca�as Vargas

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
/*********************************** Headers *********************************/
/*****************************************************************************/

#include <stdlib.h>		// For exit, system, malloc, free, rand, etc.

#include "swad_action.h"
#include "swad_config.h"
#include "swad_global.h"
#include "swad_help.h"
#include "swad_user.h"

/*****************************************************************************/
/****************************** Public constants *****************************/
/*****************************************************************************/

#define WIKI "https://github.com/acanas/swad-core/wiki/"

const char *Hlp_Search   = WIKI "Search";
//const char *Hlp_Calendar = WIKI "ASSESSMENT.Calendar";
const char *Hlp_Calendar = WIKI "Calendar";

/***** SYSTEM tab *****/

const char *Hlp_SYSTEM_Countries = WIKI "SYSTEM.Countries";

const char *Hlp_SYSTEM_Pending = WIKI "SYSTEM.Pending";

const char *Hlp_SYSTEM_Old = WIKI "SYSTEM.Old";

const char *Hlp_SYSTEM_Studies      = WIKI "SYSTEM.Studies";
const char *Hlp_SYSTEM_Studies_edit = WIKI "SYSTEM.Studies#edit";

const char *Hlp_SYSTEM_Domains      = WIKI "SYSTEM.Domains";
const char *Hlp_SYSTEM_Domains_edit = WIKI "SYSTEM.Domains#edit";

const char *Hlp_SYSTEM_Banners      = WIKI "SYSTEM.Banners";
const char *Hlp_SYSTEM_Banners_edit = WIKI "SYSTEM.Banners#edit";

const char *Hlp_SYSTEM_Links      = WIKI "SYSTEM.Links";
const char *Hlp_SYSTEM_Links_edit = WIKI "SYSTEM.Links#edit";

/***** COUNTRY tab *****/

const char *Hlp_COUNTRY_Information  = WIKI "COUNTRY.Information";
const char *Hlp_COUNTRY_Institutions = WIKI "COUNTRY.Institutions";

/***** INSTITUTION tab *****/

const char *Hlp_INSTITUTION_Information = WIKI "INSTITUTION.Information";

const char *Hlp_INSTITUTION_Centres     = WIKI "INSTITUTION.Centres";

const char *Hlp_INSTITUTION_Departments      = WIKI "INSTITUTION.Departments";
const char *Hlp_INSTITUTION_Departments_edit = WIKI "INSTITUTION.Departments#edit";

const char *Hlp_INSTITUTION_Places      = WIKI "INSTITUTION.Places";
const char *Hlp_INSTITUTION_Places_edit = WIKI "INSTITUTION.Places#edit";

const char *Hlp_INSTITUTION_Holidays      = WIKI "INSTITUTION.Holidays";
const char *Hlp_INSTITUTION_Holidays_edit = WIKI "INSTITUTION.Holidays#edit";

/***** CENTRE tab *****/

const char *Hlp_CENTRE_Information = WIKI "CENTRE.Information";
const char *Hlp_CENTRE_Degrees     = WIKI "CENTRE.Degrees";

/***** DEGREE tab *****/

const char *Hlp_DEGREE_Information = WIKI "DEGREE.Information";
const char *Hlp_DEGREE_Courses     = WIKI "DEGREE.Courses";

/***** COURSE tab *****/

const char *Hlp_COURSE_Information                     = WIKI "COURSE.Information";
const char *Hlp_COURSE_Information_textual_information = WIKI "COURSE.Information#textual-information";
const char *Hlp_COURSE_Information_edit                = WIKI "COURSE.Information#edit";

const char *Hlp_COURSE_Guide      = WIKI "COURSE.Guide";
const char *Hlp_COURSE_Guide_edit = WIKI "COURSE.Guide#edit";

const char *Hlp_COURSE_Syllabus      = WIKI "COURSE.Syllabus";
const char *Hlp_COURSE_Syllabus_edit = WIKI "COURSE.Syllabus#edit";

const char *Hlp_COURSE_Timetable = WIKI "COURSE.Timetable";

const char *Hlp_COURSE_Bibliography      = WIKI "COURSE.Bibliography";
const char *Hlp_COURSE_Bibliography_edit = WIKI "COURSE.Bibliography#edit";

const char *Hlp_COURSE_FAQ      = WIKI "COURSE.FAQ";
const char *Hlp_COURSE_FAQ_edit = WIKI "COURSE.FAQ#edit";

const char *Hlp_COURSE_Links      = WIKI "COURSE.Links";
const char *Hlp_COURSE_Links_edit = WIKI "COURSE.Links#edit";

/***** ASSESSMENT tab *****/

const char *Hlp_ASSESSMENT_System      = WIKI "ASSESSMENT.System";
const char *Hlp_ASSESSMENT_System_edit = WIKI "ASSESSMENT.System#edit";

const char *Hlp_ASSESSMENT_Assignments                 = WIKI "ASSESSMENT.Assignments";
const char *Hlp_ASSESSMENT_Assignments_new_assignment  = WIKI "ASSESSMENT.Assignments#new-assignment";
const char *Hlp_ASSESSMENT_Assignments_edit_assignment = WIKI "ASSESSMENT.Assignments#edit-assignment";

const char *Hlp_ASSESSMENT_Tests              = WIKI "ASSESSMENT.Tests";
const char *Hlp_ASSESSMENT_Tests_test_results = WIKI "ASSESSMENT.Tests#test-results";

const char *Hlp_ASSESSMENT_Announcements                   = WIKI "ASSESSMENT.Announcements";
const char *Hlp_ASSESSMENT_Announcements_new_announcement  = WIKI "ASSESSMENT.Announcements#new-announcement";
const char *Hlp_ASSESSMENT_Announcements_edit_announcement = WIKI "ASSESSMENT.Announcements#edit-announcement";

/***** FILES tab *****/

const char *Hlp_FILES_Documents = WIKI "FILES.Documents";
const char *Hlp_FILES_Private   = WIKI "FILES.Private";
const char *Hlp_FILES_Shared    = WIKI "FILES.Shared";

const char *Hlp_FILES_Homework_for_students = WIKI "FILES.Homework#for-students";
const char *Hlp_FILES_Homework_for_teachers = WIKI "FILES.Homework#for-teachers";

const char *Hlp_FILES_Marks = WIKI "FILES.Marks";

/***** USERS tab *****/

const char *Hlp_USERS_Groups = WIKI "USERS.Groups";

const char *Hlp_USERS_Students                    = WIKI "USERS.Students";
const char *Hlp_USERS_Students_shared_record_card = WIKI "USERS.Students#shared-record-card";
const char *Hlp_USERS_Students_course_record_card = WIKI "USERS.Students#course-record-card";

const char *Hlp_USERS_Administration_administer_one_user       = WIKI "USERS.Administration#administer-one-user";
const char *Hlp_USERS_Administration_administer_multiple_users = WIKI "USERS.Administration#administer-multiple-users";
const char *Hlp_USERS_Administration_remove_all_students       = WIKI "USERS.Administration#remove-all-students";

const char *Hlp_USERS_Teachers                    = WIKI "USERS.Teachers";
const char *Hlp_USERS_Teachers_shared_record_card = WIKI "USERS.Teachers#shared-record-card";
const char *Hlp_USERS_Teachers_timetable          = WIKI "USERS.Teachers#timetable";

const char *Hlp_USERS_Administrators = WIKI "USERS.Administrators";

const char *Hlp_USERS_Guests = WIKI "USERS.Guests";

const char *Hlp_USERS_Duplicates_possibly_duplicate_users = WIKI "USERS.Duplicates#possibly-duplicate-users";
const char *Hlp_USERS_Duplicates_possibly_similar_users   = WIKI "USERS.Duplicates#similar-users";

const char *Hlp_USERS_Attendance                 = WIKI "USERS.Attendance";
const char *Hlp_USERS_Attendance_new_event       = WIKI "USERS.Attendance#new-event";
const char *Hlp_USERS_Attendance_edit_event      = WIKI "USERS.Attendance#edit-event";
const char *Hlp_USERS_Attendance_attendance_list = WIKI "USERS.Attendance#attendance-list";

const char *Hlp_USERS_Signup   = WIKI "USERS.Sign up";
const char *Hlp_USERS_Requests = WIKI "USERS.Requests";

const char *Hlp_USERS_Connected             = WIKI "USERS.Connected";
const char *Hlp_USERS_Connected_last_clicks = WIKI "USERS.Connected#last-clicks";

/***** SOCIAL tab *****/

const char *Hlp_SOCIAL_Timeline = WIKI "SOCIAL.Timeline";

const char *Hlp_SOCIAL_Profiles_view_public_profile = WIKI "SOCIAL.Profiles#view-public-profile";
const char *Hlp_SOCIAL_Profiles_who_to_follow       = WIKI "SOCIAL.Profiles#who-to-follow";

const char *Hlp_SOCIAL_Forums = WIKI "SOCIAL.Forums";

/***** MESSAGES tab *****/

const char *Hlp_MESSAGES_Notifications = WIKI "MESSAGES.Notifications";

const char *Hlp_MESSAGES_Announcements = WIKI "MESSAGES.Announcements";

const char *Hlp_MESSAGES_Notices = WIKI "MESSAGES.Notices";

const char *Hlp_MESSAGES_Write           = WIKI "MESSAGES.Write";
const char *Hlp_MESSAGES_Received        = WIKI "MESSAGES.Received";
const char *Hlp_MESSAGES_Received_filter = WIKI "MESSAGES.Received#filter";
const char *Hlp_MESSAGES_Sent            = WIKI "MESSAGES.Sent";
const char *Hlp_MESSAGES_Sent_filter     = WIKI "MESSAGES.Sent#filter";

const char *Hlp_MESSAGES_Email = WIKI "MESSAGES.Email";

/***** STATS tab *****/

const char *Hlp_STATS_Surveys             = WIKI "STATS.Surveys";
const char *Hlp_STATS_Surveys_new_survey  = WIKI "STATS.Surveys#new-survey";
const char *Hlp_STATS_Surveys_edit_survey = WIKI "STATS.Surveys#edit-survey";
const char *Hlp_STATS_Surveys_questions   = WIKI "STATS.Surveys#questions";

const char *Hlp_STATS_Figures                                = WIKI "STATS.Figures";
const char *Hlp_STATS_Figures_users                          = WIKI "STATS.Figures#users";
const char *Hlp_STATS_Figures_ranking                        = WIKI "STATS.Figures#ranking";
const char *Hlp_STATS_Figures_hierarchy                      = WIKI "STATS.Figures#hierarchy";
const char *Hlp_STATS_Figures_institutions                   = WIKI "STATS.Figures#institutions";
const char *Hlp_STATS_Figures_folders_and_files              = WIKI "STATS.Figures#folders-and-files";
const char *Hlp_STATS_Figures_open_educational_resources_oer = WIKI "STATS.Figures#open-educational-resources-oer";
const char *Hlp_STATS_Figures_assignments                    = WIKI "STATS.Figures#assignments";
const char *Hlp_STATS_Figures_tests                          = WIKI "STATS.Figures#tests";
const char *Hlp_STATS_Figures_public_activity                = WIKI "STATS.Figures#public-activity";
const char *Hlp_STATS_Figures_followed_followers             = WIKI "STATS.Figures#followed--followers";
const char *Hlp_STATS_Figures_forums                         = WIKI "STATS.Figures#forums";
const char *Hlp_STATS_Figures_notifications                  = WIKI "STATS.Figures#notifications";
const char *Hlp_STATS_Figures_notices                        = WIKI "STATS.Figures#notices";
const char *Hlp_STATS_Figures_messages                       = WIKI "STATS.Figures#messages";
const char *Hlp_STATS_Figures_surveys                        = WIKI "STATS.Figures#surveys";
const char *Hlp_STATS_Figures_webs_social_networks           = WIKI "STATS.Figures#webs--social-networks";
const char *Hlp_STATS_Figures_language                       = WIKI "STATS.Figures#language";
const char *Hlp_STATS_Figures_icons                          = WIKI "STATS.Figures#icons";
const char *Hlp_STATS_Figures_menu                           = WIKI "STATS.Figures#menu";
const char *Hlp_STATS_Figures_calendar                       = WIKI "STATS.Figures#calendar";
const char *Hlp_STATS_Figures_theme                          = WIKI "STATS.Figures#theme";
const char *Hlp_STATS_Figures_columns                        = WIKI "STATS.Figures#columns";
const char *Hlp_STATS_Figures_privacy                        = WIKI "STATS.Figures#privacy";

const char *Hlp_STATS_Degrees = WIKI "STATS.Degrees";

const char *Hlp_STATS_Indicators = WIKI "STATS.Indicators";

const char *Hlp_STATS_Visits_global_visits    = WIKI "STATS.Visits#global-visits";
const char *Hlp_STATS_Visits_visits_to_course = WIKI "STATS.Visits#visits-to-course";

const char *Hlp_STATS_Report = WIKI "STATS.Report";

const char *Hlp_STATS_Frequent = WIKI "STATS.Frequent";

/***** PROFILE tab *****/

const char *Hlp_PROFILE_Log_in        = WIKI "PROFILE.Log%20in";
const char *Hlp_PROFILE_Sign_up       = WIKI "PROFILE.Sign%20up";
const char *Hlp_PROFILE_Account       = WIKI "PROFILE.Account";
const char *Hlp_PROFILE_Account_email = WIKI "PROFILE.Account#email";
const char *Hlp_PROFILE_Session_role  = WIKI "PROFILE.Session#role";
const char *Hlp_PROFILE_Password      = WIKI "PROFILE.Password";

const char *Hlp_PROFILE_Courses = WIKI "PROFILE.Courses";

const char *Hlp_PROFILE_Timetable = WIKI "PROFILE.Timetable";

const char *Hlp_PROFILE_Agenda               = WIKI "PROFILE.Agenda";
const char *Hlp_PROFILE_Agenda_new_event     = WIKI "PROFILE.Agenda#new-event";
const char *Hlp_PROFILE_Agenda_edit_event    = WIKI "PROFILE.Agenda#edit-event";
const char *Hlp_PROFILE_Agenda_public_agenda = WIKI "PROFILE.Agenda#public-agenda";

const char *Hlp_PROFILE_Record      = WIKI "PROFILE.Record";
const char *Hlp_PROFILE_Institution = WIKI "PROFILE.Institution";
const char *Hlp_PROFILE_Webs        = WIKI "PROFILE.Webs";
const char *Hlp_PROFILE_Photo       = WIKI "PROFILE.Photo";

const char *Hlp_PROFILE_Preferences_language      = WIKI "PROFILE.Preferences#language";
const char *Hlp_PROFILE_Preferences_calendar      = WIKI "PROFILE.Preferences#calendar";
const char *Hlp_PROFILE_Preferences_icons         = WIKI "PROFILE.Preferences#icons";
const char *Hlp_PROFILE_Preferences_theme         = WIKI "PROFILE.Preferences#theme";
const char *Hlp_PROFILE_Preferences_menu          = WIKI "PROFILE.Preferences#menu";
const char *Hlp_PROFILE_Preferences_columns       = WIKI "PROFILE.Preferences#columns";
const char *Hlp_PROFILE_Preferences_privacy       = WIKI "PROFILE.Preferences#privacy";
const char *Hlp_PROFILE_Preferences_notifications = WIKI "PROFILE.Preferences#notifications";

const char *Hlp_PROFILE_Briefcase = WIKI "PROFILE.Briefcase";

/*****************************************************************************/
/***************************** Private constants *****************************/
/*****************************************************************************/

/*****************************************************************************/
/****************************** Internal types *******************************/
/*****************************************************************************/

/*****************************************************************************/
/************** External global variables from others modules ****************/
/*****************************************************************************/

extern struct Globals Gbl;

/*****************************************************************************/
/************************* Internal global variables *************************/
/*****************************************************************************/

/*****************************************************************************/
/***************************** Private prototypes ****************************/
/*****************************************************************************/

static void Hlp_ShowRowHelpWhatWouldYouLikeToDo (const char *Description,
                                                 Act_Action_t Action,
                                                 const char *ClassButton,
                                                 const char *TxtButton);

/*****************************************************************************/
/*************************** Show contextual help ****************************/
/*****************************************************************************/

void Hlp_ShowHelpWhatWouldYouLikeToDo (void)
  {
   extern const char *Txt_You_are_not_enrolled_in_any_course[Usr_NUM_SEXS];
   extern const char *Txt_You_can_search_for_courses_select_them_and_request_your_enrollment_in_them;
   extern const char *Txt_If_you_can_not_find_your_institution_your_centre_your_degree_or_your_courses_you_can_create_them;
   extern const char *Txt_What_would_you_like_to_do;
   extern const char *Txt_Register_students_in_the_course_X;
   extern const char *Txt_Register_students;
   extern const char *Txt_Go_to_one_of_my_courses;
   extern const char *Txt_My_courses;
   extern const char *Txt_Sign_up;
   extern const char *Txt_Remove_me_from_the_course_X;
   extern const char *Txt_Remove_me;
   extern const char *Txt_Register_me_in_X;
   extern const char *Txt_Select_or_create_another_course_in_X;
   extern const char *Txt_Select_or_create_one_course_in_X;
   extern const char *Txt_Courses;
   extern const char *Txt_Select_or_create_another_degree_in_X;
   extern const char *Txt_Select_or_create_one_degree_in_X;
   extern const char *Txt_Degrees;
   extern const char *Txt_Select_or_create_another_centre_in_X;
   extern const char *Txt_Select_or_create_one_centre_in_X;
   extern const char *Txt_Centres;
   extern const char *Txt_Select_or_create_another_institution_in_X;
   extern const char *Txt_Select_or_create_one_institution_in_X;
   extern const char *Txt_Institutions;
   extern const char *Txt_Select_another_country;
   extern const char *Txt_Select_one_country;
   extern const char *Txt_Countries;
   extern const char *Txt_Upload_my_picture;
   extern const char *Txt_Upload_photo;
   extern const char *Txt_Log_in;
   extern const char *Txt_New_on_PLATFORM_Sign_up;
   extern const char *Txt_Create_account;

   /***** Alert message *****/
   if (Gbl.Usrs.Me.Logged &&
       !Gbl.Usrs.Me.MyCrss.Num)
     {
      sprintf (Gbl.Message,"%s<br />%s<br />%s",
	       Txt_You_are_not_enrolled_in_any_course[Gbl.Usrs.Me.UsrDat.Sex],
	       Txt_You_can_search_for_courses_select_them_and_request_your_enrollment_in_them,
	       Txt_If_you_can_not_find_your_institution_your_centre_your_degree_or_your_courses_you_can_create_them);
      Lay_ShowAlert (Lay_INFO,Gbl.Message);
     }

   /***** Contextual buttons *****/
   /* Start table */
   Lay_StartRoundFrameTable (NULL,Txt_What_would_you_like_to_do,NULL,NULL,2);

   if (Gbl.Usrs.Me.Logged)		// I am logged
     {
      if (Gbl.CurrentCrs.Crs.CrsCod > 0)	// Course selected
	{
	 if (Gbl.Usrs.Me.IBelongToCurrentCrs)	// I belong to this course
	   {
	    if (Gbl.Action.Act != ActLogIn &&
		Gbl.Action.Act != ActLogInNew &&
                Gbl.Action.Act != ActLogInLan)	// I am not just logged
	      {
	       /* Request my removing from this course */
	       sprintf (Gbl.Title,Txt_Remove_me_from_the_course_X,
			Gbl.CurrentCrs.Crs.ShrtName);
	       Hlp_ShowRowHelpWhatWouldYouLikeToDo (Gbl.Title,
						    ActReqMdfOneStd,
						    "BT_REMOVE",Txt_Remove_me);
	      }
	   }
	 else					// I do not belong to this course
	   {
	    /* Request my registration in this course */
	    sprintf (Gbl.Title,Txt_Register_me_in_X,
		     Gbl.CurrentCrs.Crs.ShrtName);
	    Hlp_ShowRowHelpWhatWouldYouLikeToDo (Gbl.Title,
						 ActReqSignUp,
						 "BT_CREATE",Txt_Sign_up);
	   }
	}

      if (Gbl.Usrs.Me.MyCrss.Num)	// I am enrolled in some courses
	{
	 if (Gbl.CurrentCrs.Crs.CrsCod > 0 &&				// Course selected
	     !Gbl.CurrentCrs.Crs.NumStds &&				// Current course has no students
	     Gbl.Usrs.Me.UsrDat.RoleInCurrentCrsDB == Rol_TEACHER)	// I am a teacher in current course
	   {
	    /* Request students enrollment */
	    sprintf (Gbl.Title,Txt_Register_students_in_the_course_X,
		     Gbl.CurrentCrs.Crs.ShrtName);
	    Hlp_ShowRowHelpWhatWouldYouLikeToDo (Gbl.Title,
						 ActReqEnrSevStd,
						 "BT_CONFIRM",Txt_Register_students);
	   }

	 if (Gbl.Action.Act != ActMyCrs)	// I am not seeing the action to list my courses
	    /* Request list my courses */
	    Hlp_ShowRowHelpWhatWouldYouLikeToDo (Txt_Go_to_one_of_my_courses,
						 ActMyCrs,
						 "BT_CONFIRM",Txt_My_courses);
	}

      if (Gbl.CurrentDeg.Deg.DegCod > 0)	// Degree selected
	{
	 /* Select a course */
	 sprintf (Gbl.Title,Gbl.CurrentCrs.Crs.CrsCod > 0 ? Txt_Select_or_create_another_course_in_X :
							    Txt_Select_or_create_one_course_in_X,
		  Gbl.CurrentDeg.Deg.ShrtName);
	 Hlp_ShowRowHelpWhatWouldYouLikeToDo (Gbl.Title,
				              ActSeeCrs,
				              "BT_CONFIRM",Txt_Courses);
	}
      else if (Gbl.CurrentCtr.Ctr.CtrCod > 0)	// Centre selected
	{
	 /* Select a degree */
	 sprintf (Gbl.Title,Gbl.CurrentDeg.Deg.DegCod > 0 ? Txt_Select_or_create_another_degree_in_X :
							    Txt_Select_or_create_one_degree_in_X,
		  Gbl.CurrentCtr.Ctr.ShrtName);
	 Hlp_ShowRowHelpWhatWouldYouLikeToDo (Gbl.Title,
				              ActSeeDeg,
				              "BT_CONFIRM",Txt_Degrees);
	}
      else if (Gbl.CurrentIns.Ins.InsCod > 0)	// Institution selected
	{
	 /* Select a centre */
	 sprintf (Gbl.Title,Gbl.CurrentCtr.Ctr.CtrCod > 0 ? Txt_Select_or_create_another_centre_in_X :
							    Txt_Select_or_create_one_centre_in_X,
		  Gbl.CurrentIns.Ins.ShrtName);
	 Hlp_ShowRowHelpWhatWouldYouLikeToDo (Gbl.Title,
				              ActSeeCtr,
				              "BT_CONFIRM",Txt_Centres);
	}
      else if (Gbl.CurrentCty.Cty.CtyCod > 0)	// Country selected
	{
	 /* Select an institution */
	 sprintf (Gbl.Title,Gbl.CurrentIns.Ins.InsCod > 0 ? Txt_Select_or_create_another_institution_in_X :
							    Txt_Select_or_create_one_institution_in_X,
		  Gbl.CurrentCty.Cty.Name[Gbl.Prefs.Language]);
	 Hlp_ShowRowHelpWhatWouldYouLikeToDo (Gbl.Title,
				              ActSeeIns,
				              "BT_CONFIRM",Txt_Institutions);
	}
      else
	 /* Select a country */
	 Hlp_ShowRowHelpWhatWouldYouLikeToDo (Gbl.CurrentCty.Cty.CtyCod > 0 ? Txt_Select_another_country :
						                              Txt_Select_one_country,
				              ActSeeCty,
				              "BT_CONFIRM",Txt_Countries);

      if (!Gbl.Usrs.Me.MyPhotoExists)		// I have no photo
	 Hlp_ShowRowHelpWhatWouldYouLikeToDo (Txt_Upload_my_picture,
				              ActReqMyPho,
				              "BT_CREATE",Txt_Upload_photo);
     }
   else					// I am not logged
     {
      /* Log in */
      Hlp_ShowRowHelpWhatWouldYouLikeToDo (Txt_Log_in,
					   ActFrmLogIn,
					   "BT_CONFIRM",Txt_Log_in);

      /* Sign up */
      sprintf (Gbl.Title,Txt_New_on_PLATFORM_Sign_up,
               Cfg_PLATFORM_SHORT_NAME);
      Hlp_ShowRowHelpWhatWouldYouLikeToDo (Gbl.Title,
					   ActFrmMyAcc,
					   "BT_CREATE",Txt_Create_account);
     }

   /* End table */
   Lay_EndRoundFrameTable ();
  }

/*****************************************************************************/
/*************************** Show contextual help ****************************/
/*****************************************************************************/

static void Hlp_ShowRowHelpWhatWouldYouLikeToDo (const char *Description,
                                                 Act_Action_t Action,
                                                 const char *ClassButton,
                                                 const char *TxtButton)
  {
   /***** Description *****/
   fprintf (Gbl.F.Out,"<tr>"
		      "<td class=\"DAT RIGHT_MIDDLE\">"
		      "%s:"
		      "</td>",
            Description);

   /***** Button *****/
   fprintf (Gbl.F.Out,"<td class=\"LEFT_MIDDLE\">");
   Act_FormStart (Action);
   fprintf (Gbl.F.Out,"<button type=\"submit\" class=\"BT_SUBMIT_INLINE %s\">"
		      "%s"
		      "</button>",
	    ClassButton,TxtButton);
   Act_FormEnd ();
   fprintf (Gbl.F.Out,"</td>"
		      "</tr>");
  }
