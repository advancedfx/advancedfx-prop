#ifndef HLAEDIALOGLAYOUTMANAGER_H
#define HLAEDIALOGLAYOUTMANAGER_H

	/* includes */
	#include <wx/button.h>
	#include <wx/dialog.h>
	#include <wx/stattext.h>
	#include <wx/listbox.h>
	#include "Lists.h"

	/* declaration */
	class hlaeDialogLayoutManager : public wxDialog {

		private:

			DECLARE_EVENT_TABLE()

			enum {
				hlaeID_Delete = wxID_HIGHEST+1,
				hlaeID_Clone
			};

			wxListBox* m_listbox;
			hlaeLayoutList* m_layoutlist;

			void OnDelete(wxCommandEvent& evt);
			void OnClone(wxCommandEvent& evt);

		public:

			hlaeDialogLayoutManager(wxWindow* parent, wxWindowID id,
				hlaeLayoutList* layoutlist);

	};

#endif // HLAEDIALOGLAYOUTMANAGER_H