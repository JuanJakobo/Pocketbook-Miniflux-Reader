//------------------------------------------------------------------
// sqliteConnector.h
//
// Author:           JuanJakobo
// Date:             18.07.2021
// Description:
//
//-------------------------------------------------------------------

#ifndef SQLITECONNECTOR
#define SQLITECONNECTOR

#include "sqlite3.h"
#include "minifluxModel.h"
#include "hackernewsModel.h"

#include <string>
#include <vector>

class SqliteConnector
{
public:

    /**
     * Creates a new sqlConnector object containing the path to the DB
     *
     */
    SqliteConnector(const std::string &DBpath);

    ~SqliteConnector();

    std::vector<MfEntry> selectMfEntries();

    std::vector<MfEntry> selectMfEntries(IsDownloaded downloaded);

    bool updateMfEntry(int entryID, bool starred, const std::string &status);

    bool updateDownloadStatusMfEntry(int entryID, IsDownloaded downloaded);

    bool deleteNotDownloadedMfEntries();

    bool insertMfEntries(const std::vector<MfEntry> &entries);

    std::vector<HnEntry> selectHnEntries(int parentId);

    HnEntry selectHnEntry(int id);

    bool insertHnEntries(const std::vector<HnEntry> &entries);

    bool deleteHnEntries(int mfEntryId);

    std::vector<HnUser> selectHnUser();

private:
    std::string _dbpath;
    sqlite3 *_db;

    bool open();
};

#endif
