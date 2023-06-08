//------------------------------------------------------------------
// sqliteConnector.cpp
//
// Author:           JuanJakobo
// Date:             18.07.2021
// Description:
//
//-------------------------------------------------------------------

#include "sqlite3.h"
#include "minifluxModel.h"
#include "hackernewsModel.h"
#include "sqliteConnector.h"
#include "log.h"

#include <string>
#include <vector>

using std::vector;
using std::string;

SqliteConnector::SqliteConnector(const string &DBpath) : _dbpath(DBpath)
{
    open();
}

vector<MfEntry> SqliteConnector::selectMfEntries()
{
    open();
    int rs;
    sqlite3_stmt *stmt = 0;
    vector<MfEntry> entries;

    rs = sqlite3_prepare_v2(
        _db.get(),
        "SELECT id, status, title, url, comments_url, content, starred, reading_time, downloaded FROM 'MfEntries';", -1,
        &stmt, 0);

    auto test = sqlite3_column_count(stmt);

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
						MfEntry temp;

						temp.id = sqlite3_column_int(stmt,0);
						temp.status = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
						temp.title = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
						temp.url =reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3));
						temp.comments_url = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 4));
						temp.content =  reinterpret_cast<const char *>(sqlite3_column_text(stmt, 5));
						temp.starred = (sqlite3_column_int(stmt, 6) == 1) ? true : false;
						temp.reading_time = sqlite3_column_int(stmt, 7);
						temp.downloaded =  static_cast<IsDownloaded>(sqlite3_column_int(stmt,8));
						entries.push_back(temp);

    }

    sqlite3_finalize(stmt);
    return entries;
}

vector<MfEntry> SqliteConnector::selectMfEntries(IsDownloaded downloaded)
{
    open();
    int rs;
    sqlite3_stmt *stmt = 0;
    vector<MfEntry> entries;

    rs = sqlite3_prepare_v2(_db.get(),
                            "SELECT id, status, title, url, comments_url, content, starred, reading_time, downloaded "
                            "FROM 'MfEntries' WHERE downloaded = ?;",
                            -1, &stmt, 0);

    rs = sqlite3_bind_int(stmt, 1, downloaded);

    auto test = sqlite3_column_count(stmt);

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
						MfEntry temp;

						temp.id = sqlite3_column_int(stmt,0);
						temp.status = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
						temp.title = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
						temp.url =reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3));
						temp.comments_url = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 4));
						temp.content =  reinterpret_cast<const char *>(sqlite3_column_text(stmt, 5));
						temp.starred = (sqlite3_column_int(stmt, 6) == 1) ? true : false;
						temp.reading_time = sqlite3_column_int(stmt, 7);
						temp.downloaded =  static_cast<IsDownloaded>(sqlite3_column_int(stmt,8));
						entries.push_back(temp);

    }

    sqlite3_finalize(stmt);
    return entries;
}

HnEntry SqliteConnector::selectHnEntry(int id)
{
    open();
    int rs;
    sqlite3_stmt *stmt = 0;
    HnEntry temp;
    temp.id = -1;

    rs = sqlite3_prepare_v2(_db.get(),
                            "SELECT id, by, time, text, parent, kids, urls, score, title, descendants, deleted, "
                            "flagged FROM 'HnItems' WHERE id = ?;",
                            -1, &stmt, 0);

    rs = sqlite3_bind_int(stmt, 1, id);

    auto test = sqlite3_column_count(stmt);

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        temp.id = sqlite3_column_int(stmt, 0);
        temp.by = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
        temp.time = sqlite3_column_int(stmt, 2);
        temp.text = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3));
        temp.parent = sqlite3_column_int(stmt, 4);
        if (!(sqlite3_column_type(stmt, 5) == SQLITE_NULL))
        {
            std::string kids = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 5));

            auto found = kids.find(",");
            while (found != std::string::npos)
            {
                auto value = kids.substr(1, found - 1);
                temp.kids.push_back(std::stoi(value));
                kids = kids.substr(found + 1);
                found = kids.find(",");
            }

            found = kids.find("]");
            if (found != std::string::npos)
            {
                auto value = kids.substr(1, found - 1);
                temp.kids.push_back(std::stoi(value));
            }
        }
        if (!(sqlite3_column_type(stmt, 6) == SQLITE_NULL))
        {

            std::string urls = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 6));

            auto found = urls.find(",");
            while (found != std::string::npos)
            {
                auto value = urls.substr(1, found - 1);
                temp.urls.push_back(value);
                urls = urls.substr(found + 1);
                found = urls.find(",");
            }

            found = urls.find("]");
            if (found != std::string::npos)
            {
                auto value = urls.substr(1, found - 1);
                temp.urls.push_back(value);
            }
        }

        temp.score = sqlite3_column_int(stmt, 7);
        temp.title = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 8));
        temp.descendants = sqlite3_column_int(stmt, 9);
        temp.deleted = sqlite3_column_int(stmt,10);
        temp.flagged = sqlite3_column_int(stmt,11);

    }

    sqlite3_finalize(stmt);
    //TODO only return if temp has been created...
    return temp;
}
//get for id?
void SqliteConnector::selectHnEntries(int parentId, vector<HnEntry> &entries)
{
    open();
    int rs;
    sqlite3_stmt *stmt = 0;

    rs = sqlite3_prepare_v2(_db.get(),
                            "SELECT id, by, time, text, parent, kids, urls, score, title, descendants,deleted,flagged "
                            "FROM 'HnItems' WHERE parent = ?;",
                            -1, &stmt, 0);

    rs = sqlite3_bind_int(stmt, 1, parentId);

    auto test = sqlite3_column_count(stmt);

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        HnEntry temp;
        temp.id = sqlite3_column_int(stmt, 0);
        temp.by = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
        temp.time = sqlite3_column_int(stmt, 2);
        temp.text = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3));
        temp.parent = sqlite3_column_int(stmt, 4);
        //test all
        if (!(sqlite3_column_type(stmt, 5) == SQLITE_NULL))
        {
            std::string kids = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 5));

            auto found = kids.find(",");
            while (found != std::string::npos)
            {
                auto value = kids.substr(1, found - 1);
                temp.kids.push_back(std::stoi(value));
                kids = kids.substr(found + 1);
                found = kids.find(",");
            }

            found = kids.find("]");
            if (found != std::string::npos)
            {
                auto value = kids.substr(1, found - 1);
                temp.kids.push_back(std::stoi(value));
            }
        }
        if (!(sqlite3_column_type(stmt, 6) == SQLITE_NULL))
        {

            std::string urls = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 6));

            auto found = urls.find(",");
            while (found != std::string::npos)
            {
                auto value = urls.substr(1, found - 1);
                temp.urls.push_back(value);
                urls = urls.substr(found + 1);
                found = urls.find(",");
            }

            found = urls.find("]");
            if (found != std::string::npos)
            {
                auto value = urls.substr(1, found - 1);
                temp.urls.push_back(value);
            }
        }

        temp.score = sqlite3_column_int(stmt, 7);
        temp.title = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 8));
        temp.descendants = sqlite3_column_int(stmt, 9);
        temp.deleted = sqlite3_column_int(stmt,10);
        temp.flagged = sqlite3_column_int(stmt,11);

        entries.push_back(temp);
    }

    sqlite3_finalize(stmt);
}

vector<HnUser> SqliteConnector::selectHnUser()
{
    //"CREATE TABLE IF NOT EXISTS HnUser (id STRING PRIMARY KEY, about TEXT, created INT, karma INT, submitted TEXT);", NULL, 0, NULL);
    return {};
}

bool SqliteConnector::updateDownloadStatusMfEntry(int entryID, IsDownloaded downloaded)
{
    open();
    int rs;
    sqlite3_stmt *stmt = 0;

    rs = sqlite3_prepare_v2(_db.get(), "UPDATE 'MfEntries' SET downloaded=? WHERE id=?", -1, &stmt, 0);
    rs = sqlite3_bind_int(stmt, 1, downloaded);
    rs = sqlite3_bind_int(stmt, 2, entryID);
    rs = sqlite3_step(stmt);

    if (rs != SQLITE_DONE)
    {
        Log::writeErrorLog(sqlite3_errmsg(_db.get()) + std::string(" (Error Code: ") + std::to_string(rs) + ")");
    }
    rs = sqlite3_clear_bindings(stmt);
    rs = sqlite3_reset(stmt);

    sqlite3_finalize(stmt);

    return true;
}

bool SqliteConnector::updateMfEntry(int entryID, bool starred, const std::string &status)
{
    open();
    int rs;
    sqlite3_stmt *stmt = 0;

    rs = sqlite3_prepare_v2(_db.get(), "UPDATE 'MfEntries' SET starred=?, status=? WHERE id=?", -1, &stmt, 0);
    rs = sqlite3_bind_int(stmt, 1, (starred) ? 1 : 0);
    rs = sqlite3_bind_text(stmt, 2, status.c_str(), status.length(), NULL);
    rs = sqlite3_bind_int(stmt, 3, entryID);
    rs = sqlite3_step(stmt);

    if (rs != SQLITE_DONE)
    {
        Log::writeErrorLog(sqlite3_errmsg(_db.get()) + std::string(" (Error Code: ") + std::to_string(rs) + ")");
    }
    rs = sqlite3_clear_bindings(stmt);
    rs = sqlite3_reset(stmt);

    sqlite3_finalize(stmt);

    return true;
}

bool SqliteConnector::deleteNotDownloadedMfEntries()
{
    open();
    int rs;
    sqlite3_stmt *stmt = 0;

    rs = sqlite3_prepare_v2(_db.get(), "DELETE FROM 'MfEntries' WHERE downloaded = 0", -1, &stmt, 0);
    rs = sqlite3_step(stmt);

    if (rs != SQLITE_DONE)
    {
        Log::writeErrorLog(sqlite3_errmsg(_db.get()) + std::string(" (Error Code: ") + std::to_string(rs) + ")");
    }
    rs = sqlite3_clear_bindings(stmt);
    rs = sqlite3_reset(stmt);

    sqlite3_finalize(stmt);

    return true;
}
bool SqliteConnector::insertMfEntries(const std::vector<MfEntry> &entries)
{
    open();
    int rs;
    sqlite3_stmt *stmt = 0;

    rs = sqlite3_prepare_v2(_db.get(),
                            "INSERT INTO 'MfEntries' (id, status, title, url, comments_url, content, starred, "
                            "reading_time, downloaded) VALUES (?,?,?,?,?,?,?,?,?);",
                            -1, &stmt, 0);
    rs = sqlite3_exec(_db.get(), "BEGIN TRANSACTION;", NULL, NULL, NULL);

    for (auto ent : entries)
    {
        rs = sqlite3_bind_int(stmt, 1, ent.id);
        rs = sqlite3_bind_text(stmt, 2, ent.status.c_str(), ent.status.length(), NULL);
        rs = sqlite3_bind_text(stmt, 3, ent.title.c_str(), ent.title.length(), NULL);
        rs = sqlite3_bind_text(stmt, 4, ent.url.c_str(), ent.url.length(), NULL);
        rs = sqlite3_bind_text(stmt, 5, ent.comments_url.c_str(), ent.comments_url.length(), NULL);
        rs = sqlite3_bind_text(stmt, 6, ent.content.c_str(), ent.content.length(), NULL);
        rs = sqlite3_bind_int(stmt, 7, (ent.starred) ? 1 : 0);
        rs = sqlite3_bind_int(stmt, 8, ent.reading_time);
				rs = sqlite3_bind_int(stmt, 9, ent.downloaded);

        rs = sqlite3_step(stmt);
        if (rs == SQLITE_CONSTRAINT)
        {
            //If item already exists it is not written
            //Log::writeInfoLog("item exists already: " + std::to_string(ent.id) + std::to_string(ent.starred) + ent.status);
            //updateMfEntry(ent.id, ent.starred, ent.status); --> database is locked, therefore only updated needed, no
        }
        else if (rs != SQLITE_DONE)
        {
            Log::writeErrorLog(sqlite3_errmsg(_db.get()) + std::string(" (Error Code: ") + std::to_string(rs) + ")");
        }
        rs = sqlite3_clear_bindings(stmt);
        rs = sqlite3_reset(stmt);
    }

    sqlite3_exec(_db.get(), "END TRANSACTION;", NULL, NULL, NULL);

    sqlite3_finalize(stmt);

    return true;
}

bool SqliteConnector::insertHnEntries(const std::vector<HnEntry> &entries)
{
    try{
    open();
    int rs;
    sqlite3_stmt *stmt = 0;
    rs = sqlite3_prepare_v2(
        _db.get(),
        "INSERT INTO 'HnItems' (id,by, time,text,parent,kids,urls,score,title,descendants,mfEntryId,deleted,flagged) "
        "VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?);",
        -1, &stmt, 0);
    rs = sqlite3_exec(_db.get(), "BEGIN TRANSACTION;", NULL, NULL, NULL);

    for (auto hn : entries)
    {
        rs = sqlite3_bind_int(stmt, 1, hn.id);
        rs = sqlite3_bind_text(stmt, 2, hn.by.c_str(), hn.by.length(), NULL);
        rs = sqlite3_bind_int(stmt, 3, hn.time);
        rs = sqlite3_bind_text(stmt, 4, hn.text.c_str(), hn.text.length(), NULL);
        rs = sqlite3_bind_int(stmt, 5, hn.parent);

        if (hn.kids.size() > 0)
        {
            string kidsString = "[";
            for (size_t i = 0; i < hn.kids.size(); i++)
            {
                if (i > 0)
                    kidsString += ", ";
                kidsString += std::to_string(hn.kids.at(i));
            }
            kidsString += "]";
            rs = sqlite3_bind_text(stmt, 6, sqlite3_mprintf(kidsString.c_str()), kidsString.length(), NULL);
        }

        if (hn.urls.size() > 0)
        {
            string urlsString = "[";
            for (size_t i = 0; i < hn.urls.size(); i++)
            {
                if (i > 0)
                    urlsString += ", ";
                urlsString += hn.urls.at(i);
            }
            urlsString += "]";
            rs = sqlite3_bind_text(stmt, 7, sqlite3_mprintf(urlsString.c_str()), urlsString.length(), NULL);
        }
        rs = sqlite3_bind_int(stmt, 8, hn.score);
        rs = sqlite3_bind_text(stmt, 9, hn.title.c_str(), hn.title.length(), NULL);
        rs = sqlite3_bind_int(stmt, 10, hn.descendants);
        rs = sqlite3_bind_int(stmt, 11, hn.mfEntryId);
        rs = sqlite3_bind_int(stmt, 12, hn.deleted);
        rs = sqlite3_bind_int(stmt, 13, hn.flagged);
        rs = sqlite3_step(stmt);

        if (rs != SQLITE_DONE)
        {
            Log::writeErrorLog("Error inserting Hn Entries " + std::string(" (Error Code: ") + std::to_string(rs) +
                               sqlite3_errmsg(_db.get()) + ")");
        }
        rs = sqlite3_clear_bindings(stmt);
        rs = sqlite3_reset(stmt);
    }

    sqlite3_exec(_db.get(), "END TRANSACTION;", NULL, NULL, NULL);

    sqlite3_finalize(stmt);

    }catch (const std::exception &e)
    {
        Log::writeErrorLog(e.what());
    }

    return true;
}

bool SqliteConnector::deleteHnEntries(int mfEntryId)
{
    open();
    int rs;
    sqlite3_stmt *stmt = 0;

    rs = sqlite3_prepare_v2(_db.get(), "DELETE FROM 'HnItems' WHERE mfEntryId=?", -1, &stmt, 0);
    rs = sqlite3_bind_int(stmt, 1, mfEntryId);
    rs = sqlite3_step(stmt);

    if (rs != SQLITE_DONE)
    {
        Log::writeErrorLog(sqlite3_errmsg(_db.get()) + std::string(" (Error Code: ") + std::to_string(rs) + ")");
    }
    rs = sqlite3_clear_bindings(stmt);
    rs = sqlite3_reset(stmt);

    sqlite3_finalize(stmt);

    return true;
}

bool SqliteConnector::open()
{
    if (_db)
    {
        return true;
    }

    sqlite3 *dbRaw = nullptr;
    auto rs{sqlite3_open(_dbpath.c_str(), &dbRaw)};
    _db.reset(dbRaw);

    if (rs)
    {
        Log::writeErrorLog("Could not open DB at " + _dbpath);
        return false;
    }
    rs = sqlite3_exec(_db.get(),
                      "CREATE TABLE IF NOT EXISTS MfEntries (id INT PRIMARY KEY, status TEXT, title TEXT, url TEXT, "
                      "comments_url TEXT, content TEXT, starred INT, reading_time INT, downloaded INT);",
                      NULL, 0, NULL);
    rs = sqlite3_exec(_db.get(),
                      "CREATE TABLE IF NOT EXISTS HnItems (id INT PRIMARY KEY, by TEXT, time INT, text TEXT, parent "
                      "INT, kids TEXT, urls TEXT, score INT, title TEXT, descendants INT, mfEntryId INT, deleted "
                      "BOOLEAN NOT NULL CHECK (deleted IN (0,1)), flagged BOOLEAN NOT NULL CHECK (flagged IN (0,1)));",
                      NULL, 0, NULL);
    rs = sqlite3_exec(_db.get(),
                      "CREATE TABLE IF NOT EXISTS HnUser (id STRING PRIMARY KEY, about TEXT, created INT, karma INT, "
                      "submitted TEXT);",
                      NULL, 0, NULL);

    return true;
}
