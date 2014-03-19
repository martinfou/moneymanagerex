/*******************************************************
Copyright (C) 2014 Gabriele-V

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
********************************************************/


#include "webapp.h"
#include "model/Model_Setting.h"
#include "model/Model_Account.h"
#include "model/Model_Payee.h"
#include "paths.h"
#include "transdialog.h"
#include "util.h"

//Expected WebAppVersion
wxString mmWebApp::getApiExpectedVersion()
{
	return "0.9.0";
}

//Internal constants
const wxString mmWebApp::getUrl()
{
    return Model_Infotable::instance().GetStringInfo("WEBAPPURL", "");
}

const wxString mmWebApp::getGuid()
{
    return Model_Infotable::instance().GetStringInfo("WEBAPPGUID", "");
}

const wxString mmWebApp::getDelimiter()
{
    return ";;";
}

const wxString mmWebApp::getServicesPage()
{
    return "services.php";
}

const wxString mmWebApp::getCheckGuidParameter()
{
	return "check_guid";
}

const wxString mmWebApp::getCheckApiVersionParameter()
{
	return "check_api_version";
}

const wxString mmWebApp::getCheckNewTransactionParameter()
{
	return "check_new_transaction";
}

const wxString mmWebApp::getImportAccountParameter()
{
    return "import_bankaccount";
}

const wxString mmWebApp::getImportPayeeParameter()
{
    return "import_payee";
}

const wxString mmWebApp::getDeletePayeeParameter()
{
	return "delete_payee";
}

const wxString mmWebApp::getDeleteOneTransactionParameter()
{
	return "delete_group";
}

const wxString mmWebApp::getWebApp_DeleteAllTransactionParameter()
{
    return "delete_all";
}

const wxString mmWebApp::getDownloadNewTransactionParameter()
{
    return "download_db";
}

const wxString mmWebApp::getMessageSucceeded()
{
    return "Operation has succeeded";
}

const wxString mmWebApp::getMessageWrongGuid()
{
    return "Wrong GUID";
}

//Public constants
const wxString mmWebApp::getDbFilePath()
{
	return mmex::getPathUser(mmex::DIRECTORY) + "WebAppNewTransaction.db";
}

/***************
** Functions  **
***************/
//Return function result
bool mmWebApp::returnResult(const int& ErrorCode, wxString& outputMessage)
{
	if (ErrorCode == 0 && outputMessage == mmWebApp::getMessageSucceeded())
	{
		return true;
	}
	else
	{
		return false;
	}
}

//Check guid
bool mmWebApp::WebApp_CheckGuid()
{
	wxString CheckGuidUrl = mmWebApp::getUrl() + "/" +
		mmWebApp::getServicesPage() + "?" +
		mmWebApp::getCheckGuidParameter() + "&"
		"guid=" + mmWebApp::getGuid()
		;

	wxString outputMessage;
	site_content(CheckGuidUrl, outputMessage);

	if (outputMessage == mmWebApp::getMessageSucceeded())
	{
		return true;
	}
	else
	{
		return false;
	}
}

//Check WebApp version
wxString mmWebApp::WebApp_getApiVersion()
{
	wxString CheckApiVersionUrl = mmWebApp::getUrl() + "/" +
		mmWebApp::getServicesPage() + "?" +
		mmWebApp::getCheckApiVersionParameter() + "&"
		"guid=" + mmWebApp::getGuid()
		;

	wxString outputMessage;
	site_content(CheckApiVersionUrl, outputMessage);

	return outputMessage;
}

//Check New Transaction
bool mmWebApp::WebApp_CheckNewTransaction()
{
	wxString CheckNewTransaction = mmWebApp::getUrl() + "/" +
		mmWebApp::getServicesPage() + "?" +
		mmWebApp::getCheckNewTransactionParameter() + "&"
		"guid=" + mmWebApp::getGuid()
		;

	wxString outputMessage;
	site_content(CheckNewTransaction, outputMessage);

	if (outputMessage == "True")
	{
		return true;
	}
	else
	{
		return false;
	}
}

// Update Account on WebApp
bool mmWebApp::WebApp_UpdateAccount()
{
    wxString AccountsList;
    const auto &Accounts = Model_Account::instance().all(Model_Account::COL_ACCOUNTNAME);
    for (const auto &Account : Accounts)
    {
        if (Model_Account::type(Account) != Model_Account::INVESTMENT)
        {
			if (AccountsList != "")
				AccountsList += mmWebApp::getDelimiter();
			AccountsList += Account.ACCOUNTNAME;
        }
    }
    
	wxString UpdateAccountUrl = mmWebApp::getUrl() + "/" +
        mmWebApp::getServicesPage() + "?" +
        mmWebApp::getImportAccountParameter() + "=" +
		AccountsList + "&"
        "guid=" + mmWebApp::getGuid()
        ;


    wxString outputMessage;
	int ErrorCode = site_content(UpdateAccountUrl, outputMessage);

	return mmWebApp::returnResult(ErrorCode, outputMessage);
}

//Update payee on WebApp
bool mmWebApp::WebApp_UpdatePayee()
{
    wxString PayeesList = "";
	wxString outputMessage = "";

	wxString UpdatePayeeUrl = mmWebApp::getUrl() + "/" +
		mmWebApp::getServicesPage() + "?" +
		"guid=" + mmWebApp::getGuid() + "&" +
		mmWebApp::getImportPayeeParameter() + "="
		;

	mmWebApp::WebApp_DeleteAllPayee();
    for (const auto &Payee : Model_Payee::instance().all(Model_Payee::COL_PAYEENAME))
    {
		if (PayeesList != "")
			PayeesList += mmWebApp::getDelimiter();
		PayeesList += Payee.PAYEENAME;

		if (PayeesList.Len() > 4000)
		{
			site_content(UpdatePayeeUrl + PayeesList, outputMessage);
			PayeesList = "";
		}
    }

	int ErrorCode = site_content(UpdatePayeeUrl + PayeesList, outputMessage);

	return mmWebApp::returnResult(ErrorCode, outputMessage);
}

//Delete all payee on WebApp
bool mmWebApp::WebApp_DeleteAllPayee()
{
	wxString DeleteAllPayeeUrl = mmWebApp::getUrl() + "/" +
		mmWebApp::getServicesPage() + "?" +
		mmWebApp::getDeletePayeeParameter() + "&"
		"guid=" + mmWebApp::getGuid()
		;

	wxString outputMessage;
	int ErrorCode = site_content(DeleteAllPayeeUrl, outputMessage);

	return mmWebApp::returnResult(ErrorCode, outputMessage);
}

//Delete all transactions on WebApp
bool mmWebApp::WebApp_DeleteAllTransaction()
{
	wxString DeleteAllTransactionUrl = mmWebApp::getUrl() + "/" +
        mmWebApp::getServicesPage() + "?" +
        mmWebApp::getWebApp_DeleteAllTransactionParameter() + "&"
        "guid=" + mmWebApp::getGuid()
        ;

    wxString outputMessage;
	int ErrorCode = site_content(DeleteAllTransactionUrl, outputMessage);

	return mmWebApp::returnResult(ErrorCode, outputMessage);
}

//Delete one transaction from WebApp
bool mmWebApp::WebApp_DeleteOneTransaction(const int& WebAppNewTransactionId)
{
	wxString DeleteOneTransactionUrl = mmWebApp::getUrl() + "/" +
		mmWebApp::getServicesPage() + "?" +
		"guid=" + mmWebApp::getGuid() + "&" +
		mmWebApp::getDeleteOneTransactionParameter() + "=" << WebAppNewTransactionId
		;

	wxString outputMessage;
	int ErrorCode = site_content(DeleteOneTransactionUrl, outputMessage);

	return mmWebApp::returnResult(ErrorCode, outputMessage);
}

//Download new transactions
bool mmWebApp::WebApp_DownloadNewTransaction()
{
	wxString DownloadNewTransactionUrl = mmWebApp::getUrl() + "/" +
		mmWebApp::getServicesPage() + "?" +
		mmWebApp::getDownloadNewTransactionParameter() + "&"
		"guid=" + mmWebApp::getGuid()
		;

	wxFileSystem fs;
	wxFileSystem::AddHandler(new wxInternetFSHandler());
	wxString outputFile = mmWebApp::getDbFilePath();

	wxFSFile *inputFile = fs.OpenFile(DownloadNewTransactionUrl);
	if (inputFile)
	{
		if (wxFileName::FileExists(outputFile))
			wxRemoveFile(outputFile);
		wxFileOutputStream output(outputFile);
		output.Write(*inputFile->GetStream());
		output.Close();
	}

	if (wxFileExists(outputFile))
	{
		return true;
	}
	else
	{
		return false;
	}
}

//Get first transaction ID
int mmWebApp::Local_getNextTransactionID()
{
	wxSQLite3Database* db = new wxSQLite3Database();
	db->Open(mmWebApp::getDbFilePath());
	wxSQLite3ResultSet MinIDResultSet = db->ExecuteQuery(wxT("SELECT ID FROM New_Transaction ORDER BY Date LIMIT 1"));
	int MinID = wxAtoi(MinIDResultSet.GetString(0));
	db->Close();
	return MinID;
}

//Insert new transaction
int mmWebApp::MMEX_InsertNewTransaction(const int& WebAppNewTransactionId)
{
	Model_Checking::Data * desktopNewTransaction;
	int desktopNewTransactionId = 0;

	wxSQLite3Database* db = new wxSQLite3Database();
	db->Open(mmWebApp::getDbFilePath());
	wxString sql = "SELECT * FROM New_Transaction WHERE ID = ?";
	wxSQLite3Statement stmt = db->PrepareStatement(sql);
	stmt.Bind(1, WebAppNewTransactionId);
	wxSQLite3ResultSet NewTrResultSet = stmt.ExecuteQuery();
	if (!NewTrResultSet.Eof())
	{
		wxString AccountName = NewTrResultSet.GetString("Account");
		const Model_Account::Data* Account = Model_Account::instance().get(AccountName);
		if (Account != NULL)
		{
			int AccountID = Account->ACCOUNTID;
			int ToAccountID = -1;
			int PayeeID = -1;
			int CategoryID = -1;
			int SubCategoryID = -1;

			//Search ToAccount
			wxString ToAccountName = NewTrResultSet.GetString("ToAccount");
			if (ToAccountName != "None")
			{
				const Model_Account::Data* ToAccount = Model_Account::instance().get(ToAccountName);
				if (ToAccount != NULL)
				{
					ToAccountID = ToAccount->ACCOUNTID;
				}
			}

			//Search Payee
			wxString PayeeName = NewTrResultSet.GetString("Payee");
			if (PayeeName != "None")
			{
				const Model_Payee::Data* Payee = Model_Payee::instance().get(PayeeName);
				if (Payee != NULL)
				{
					PayeeID = Payee->PAYEEID;
				}
				else
				{
					Model_Payee::Data* NewPayee = Model_Payee::instance().create();
					NewPayee->PAYEENAME = PayeeName;
					int NewPayeeID = Model_Payee::instance().save(NewPayee);
					PayeeID = NewPayeeID;
				}
			}

			desktopNewTransaction = Model_Checking::instance().create();
			desktopNewTransaction->TRANSDATE = NewTrResultSet.GetString("Date");
			desktopNewTransaction->STATUS = NewTrResultSet.GetString("Status");
			desktopNewTransaction->TRANSCODE = NewTrResultSet.GetString("Type");
			desktopNewTransaction->TRANSAMOUNT = NewTrResultSet.GetDouble("Amount");
			desktopNewTransaction->ACCOUNTID = AccountID;
			desktopNewTransaction->TOACCOUNTID = ToAccountID;
			desktopNewTransaction->PAYEEID = PayeeID;
			desktopNewTransaction->CATEGID = CategoryID;
			desktopNewTransaction->SUBCATEGID = SubCategoryID;
			desktopNewTransaction->TRANSACTIONNUMBER = "";
			desktopNewTransaction->NOTES = NewTrResultSet.GetString("Notes");
			desktopNewTransaction->FOLLOWUPID = -1;
			desktopNewTransaction->TOTRANSAMOUNT = NewTrResultSet.GetDouble("Amount");

			desktopNewTransactionId = Model_Checking::instance().save(desktopNewTransaction);

			if (desktopNewTransactionId > 0)
			{
				stmt.Finalize();
				db->Close();
				mmWebApp::Local_DeleteOneTransaction(WebAppNewTransactionId);
				mmWebApp::WebApp_DeleteOneTransaction(WebAppNewTransactionId);
			}
			else
			{
				stmt.Finalize();
				db->Close();
				mmWebApp::Local_DeleteOneTransaction(WebAppNewTransactionId);
			}
		}
		else
		{
			wxString msgStr = wxString() << "Account not found!" << "\n"
				<< "Please correct WebApp transaction with account '" << AccountName << "'\n";
			wxMessageBox(msgStr, "Wrong WebApp account", wxICON_ERROR);
			stmt.Finalize();
			db->Close();
			mmWebApp::Local_DeleteOneTransaction(WebAppNewTransactionId);
		}
	}
	else
	{
		stmt.Finalize();
		db->Close();
		mmWebApp::Local_DeleteOneTransaction(WebAppNewTransactionId);
	}

	return desktopNewTransactionId;
}

//Delete transaction local
bool mmWebApp::Local_DeleteOneTransaction(const int& LocalTransactionId)
{
	wxSQLite3Database* db = new wxSQLite3Database();
	db->Open(mmWebApp::getDbFilePath());
	wxString sql = "DELETE FROM New_Transaction WHERE ID = ?";
	wxSQLite3Statement stmt = db->PrepareStatement(sql);
	stmt.Bind(1, LocalTransactionId);
	int nRows = stmt.ExecuteUpdate();
	stmt.Finalize();
	db->Close();
	if (nRows == 1)
	{
		return true;
	}
	else
	{
		return false;
	}
}