// Copyright (c) 2006 Martin Knafve / hMailServer.com.  
// http://www.hmailserver.com

#include "stdafx.h"

#include "IMAPFolders.h"
#include "IMAPFolder.h"

#include "../Util/Time.h"
#include "../../IMAP/IMAPConfiguration.h"

#ifdef _DEBUG
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

namespace HM
{


   IMAPFolders::IMAPFolders(__int64 iAccountID, __int64 iParentFolderID) :
      m_iAccountID(iAccountID),
      m_iParentFolderID(iParentFolderID)
   {

   }

   IMAPFolders::IMAPFolders() :
      m_iAccountID(0)
   {

   }


   IMAPFolders::~IMAPFolders()
   {
  
   }

   void
   IMAPFolders::Refresh()
   {
      CriticalSectionScope scope(_lock);

      vecObjects.clear();

      SQLCommand command("select folderid, folderparentid, foldername, folderissubscribed, foldercurrentuid, foldercreationtime from hm_imapfolders "
                         " where folderaccountid = @FOLDERACCOUNTID order by folderid asc");

      command.AddParameter("@FOLDERACCOUNTID", m_iAccountID);
   
      shared_ptr<DALRecordset> pRS = Application::Instance()->GetDBManager()->OpenRecordset(command);
      if (!pRS)
         return;

      std::vector<std::pair<__int64, shared_ptr<IMAPFolder> > > vecIMAPFolders;

      if (!pRS->IsEOF())
      {
         __int64 iFolderID = 0;
         __int64 iParentID = 0;
         String sFolderName;
         bool bIsSubscribed = false;   
         bool bShared = false;
         unsigned int currentUID = 0;
         DateTime creationTime;

         while (!pRS->IsEOF())
         {
            iFolderID = pRS->GetLongValue("folderid");
            iParentID = pRS->GetLongValue("folderparentid");
            sFolderName = pRS->GetStringValue("foldername");
            bIsSubscribed = (pRS->GetLongValue("folderissubscribed") == 1) ? true : false;
            currentUID = (unsigned int) pRS->GetInt64Value("foldercurrentuid");
            creationTime = Time::GetDateFromSystemDate(pRS->GetStringValue("foldercreationtime"));

            // Initialize with dummy parent folder. We can't set it here since it may not
            // even be loaded from the recordset yet.
            shared_ptr<IMAPFolder> pFolder = shared_ptr<IMAPFolder>(new IMAPFolder(m_iAccountID, iParentID));
            
            pFolder->SetID(iFolderID);
            pFolder->SetFolderName(sFolderName);
            pFolder->SetIsSubscribed(bIsSubscribed);
            pFolder->SetCurrentUID(currentUID);
            pFolder->SetCreationTime(creationTime);

            vecIMAPFolders.push_back(std::make_pair(iParentID, pFolder));

            pRS->MoveNext();
         }

         // Sort theese folders into sub-folders.
         long lPanicLevel = 0;
         while (vecIMAPFolders.size() > 0)
         {
            std::vector<std::pair<__int64, shared_ptr<IMAPFolder> > >::iterator iterFolder = vecIMAPFolders.begin();

            while (iterFolder != vecIMAPFolders.end())
            {
               __int64 iFolderParentID = (*iterFolder).first;

               if (iFolderParentID == (__int64) -1)
               {
                  vecObjects.push_back((*iterFolder).second);
                  vecIMAPFolders.erase(iterFolder);
                  
                  break; 
               }
               else
               {
                  shared_ptr<IMAPFolder> pParent = GetItemByDBIDRecursive(iFolderParentID);

                  if (pParent)
                  {
                     pParent->GetSubFolders()->AddItem((*iterFolder).second); // found
                     vecIMAPFolders.erase(iterFolder);
                     break;
                  }
               }

               iterFolder++;
            }  

            lPanicLevel++;

            if (lPanicLevel > 100000)
            {
               String sMessage;
			   // NEED FOR IMPROVEMENT: User should be able to adjust max even if INI
			   // User in forum had too many folders & this limit caused big issues
			   // 
			   // Better logging details so people understand what happened until fixed/adjustable
			   sMessage.Format(_T("Unable to retrieve folder list for account. 100K+ loops trying to sort. Account: %I64d, Parent: %I64d"), m_iAccountID, m_iParentFolderID);
               
               ErrorManager::Instance()->ReportError(
                  ErrorManager::Medium, 5125, "IMAPFolders::Refresh()", sMessage);       

               return;
            }

         }

      
      }

   }

   shared_ptr<IMAPFolder> 
   IMAPFolders::GetFolderByName(const String &sName, bool bRecursive)
   { 
      CriticalSectionScope scope(_lock);

      boost_foreach(shared_ptr<IMAPFolder> pFolder, vecObjects)
      {
         if (pFolder->GetFolderName().Equals(sName, false))
            return pFolder;

         if (bRecursive)
         {
            // Visit this folder.
            shared_ptr<IMAPFolders> pSubFolders = pFolder->GetSubFolders();
            pFolder = pSubFolders->GetFolderByName(sName, bRecursive);

            if (pFolder)
               return pFolder;
         }
      }

      shared_ptr<IMAPFolder> pEmpty;
      return pEmpty;
   }


  


   shared_ptr<IMAPFolder>
   IMAPFolders::GetFolderByFullPath(const String &sPath)
   {
      CriticalSectionScope scope(_lock);

      String hierarchyDelimiter = Configuration::Instance()->GetIMAPConfiguration()->GetHierarchyDelimiter();

      std::vector<String> sVecPath = StringParser::SplitString(sPath, hierarchyDelimiter);

      return GetFolderByFullPath(sVecPath);
   }

   shared_ptr<IMAPFolder>
   IMAPFolders::GetFolderByFullPath(const std::vector<String> &vecFolders)
   {
      CriticalSectionScope scope(_lock);

      shared_ptr<IMAPFolder> pCurFolder;
      size_t lNoOfParts= vecFolders.size();
      for (unsigned int i = 0; i < lNoOfParts; i++)
      {
         if (pCurFolder)
         {
            String sFoldName = vecFolders[i];
            pCurFolder = pCurFolder->GetSubFolders()->GetFolderByName(sFoldName);
         }
         else
         {
            String sFoldName = vecFolders[i];
            pCurFolder = GetFolderByName(sFoldName);

            if (!pCurFolder)
               return pCurFolder;
         }
      }

      return pCurFolder;
      
   }

   void
   IMAPFolders::RemoveFolder(shared_ptr<IMAPFolder> pFolderToRemove)
   {
      CriticalSectionScope scope(_lock);

      vector<shared_ptr<IMAPFolder> >::iterator iterCurPos = vecObjects.begin();
      vector<shared_ptr<IMAPFolder> >::iterator iterEnd = vecObjects.end();

      __int64 lRemoveFolderID = pFolderToRemove->GetID();
      for (; iterCurPos!= iterEnd; iterCurPos++)
      {
         shared_ptr<IMAPFolder> pFolder = (*iterCurPos);

         if (pFolder->GetID() == lRemoveFolderID)
         {
            // Remove this folder fro the collection.
            vecObjects.erase(iterCurPos);
            return;
         }
      }
   }
  
   void
   IMAPFolders::CreatePath(shared_ptr<IMAPFolders> pParentContainer,
                           const std::vector<String> &vecFolderPath, 
                           bool bAutoSubscribe)
   {
      CriticalSectionScope scope(_lock);

      String hierarchyDelimiter = Configuration::Instance()->GetIMAPConfiguration()->GetHierarchyDelimiter();

      LOG_DEBUG("Creating IMAP folder " + StringParser::JoinVector(vecFolderPath, hierarchyDelimiter));

      std::vector<String> vecTempPath = vecFolderPath;

      shared_ptr<IMAPFolder> pParentFolder;

      while (vecTempPath.size() > 0)
      {
         // Get first level.
         String sTopLevel = vecTempPath[0];

         shared_ptr<IMAPFolder> pParentCheck = pParentContainer->GetFolderByName(sTopLevel, false);

         if (pParentCheck)
         {
            // This folder already exists. Create next level.
            pParentContainer = pParentCheck->GetSubFolders();
            pParentFolder = pParentCheck;
            vecTempPath = StringParser::GetAllButFirst(vecTempPath);

            continue;
         }

         __int64 iParentFolderID = -1;
         if (pParentFolder)
            iParentFolderID = pParentFolder->GetID();

         shared_ptr<IMAPFolder> pFolder = shared_ptr<IMAPFolder>(new IMAPFolder(m_iAccountID, iParentFolderID));
         pFolder->SetFolderName(sTopLevel);
         pFolder->SetIsSubscribed(bAutoSubscribe);

         PersistentIMAPFolder::SaveObject(pFolder);

         // Add the folder to the collection.
         pParentContainer->AddItem(pFolder);

         // Go down one folder.
         pParentContainer = pFolder->GetSubFolders();
         
         vecTempPath = StringParser::GetAllButFirst(vecTempPath);
         pParentFolder = pFolder;

      }
   }

   bool
   IMAPFolders::PreSaveObject(shared_ptr<IMAPFolder> pObject, XNode *node)
   {
      pObject->SetAccountID(GetAccountID());
      pObject->SetParentFolderID(m_iParentFolderID);
      return true;
   }


   shared_ptr<IMAPFolder> 
   IMAPFolders::GetItemByDBIDRecursive(__int64 lFolderID)
   {
      CriticalSectionScope scope(_lock);

      std::vector<shared_ptr<IMAPFolder> >::iterator iterCurPos = vecObjects.begin();

      boost_foreach(shared_ptr<IMAPFolder> pFolder, vecObjects)
      {
         if (pFolder->GetID() == lFolderID)
            return pFolder;

         // Visit this folder.
         shared_ptr<IMAPFolders> pSubFolders = pFolder->GetSubFolders();
         pFolder = pSubFolders->GetItemByDBIDRecursive(lFolderID);

         if (pFolder)
            return pFolder;
      }


      shared_ptr<IMAPFolder> pEmpty;
      return pEmpty;

   }

   __int64 
   IMAPFolders::GetParentID()
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Returns the ID of the IMAP folder in which these folder exists. If this is
   // a top level collection, -1 is returned.
   //---------------------------------------------------------------------------()
   {
      return m_iParentFolderID; 
   }

   __int64 
   IMAPFolders::GetAccountID()
   //---------------------------------------------------------------------------()
   // DESCRIPTION:
   // Returns the ID of the account in which these folders exist
   //---------------------------------------------------------------------------()
   {
      return m_iAccountID; 
   }

   String 
   IMAPFolders::GetCollectionName() const 
   {
      if (_GetIsPublicFolders())
         return "PublicFolders"; 
      else
         return "Folders"; 
   }

   bool 
   IMAPFolders::_GetIsPublicFolders() const
   {
      if (m_iAccountID == 0)
         return true;
      else
         return false;
   }
}
