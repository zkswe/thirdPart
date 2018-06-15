/*
 * Database.cpp
 *
 *  Created on: 2018年5月27日
 *      Author: Kwolve
 */

#include "Database.h"
#include "utils/Log.h"
#include "uart/ProtocolData.h"
#include "uart/ProtocolParser.h"



void Database::recodeResult(std::string name,std::string num,std::string age,std::string score) {
	std::string sqltestr ="";
	sqltestr += "insert into recode(name,num,age,score) values('"+name+"','"+
			num+"','"+
			age+"','"+
			score+"');";

	char* errmsg;

	LOGD("sqlite exec :%s",sqltestr.c_str());
	if(pDB != NULL){
		int ret = sqlite3_exec(pDB,sqltestr.c_str(),0,0, &errmsg);
		if(ret != SQLITE_OK){
			LOGD("sqlite exec err:%s",errmsg);
		}
	}

}

std::vector<S_INFOS> Database::getRecodes() {
	const char* cmd = "select * from recode";
	char** result;
	int nRow;
	int nCol;
	char* errmsg;
	dbs.clear();
	dbs.empty();
	int ret = sqlite3_get_table(pDB, cmd, &result, &nRow, &nCol, &errmsg);
	if(ret != SQLITE_OK){
		LOGD("select * form recode ret:%s",errmsg);
		return dbs;
	}
	std::string strOut;
   int nIndex = nCol;
   S_INFOS info;
   for(int i=0;i<nRow;i++)
   {
	   info.name = result[nIndex];
	   ++nIndex;
	   info.num = result[nIndex];
	   ++nIndex;
	   info.age = result[nIndex];
	   ++nIndex;
	   info.score = result[nIndex];
	   ++nIndex;
	   dbs.push_back(info);
   }
   LOGD("read size %d",nRow);
   LOGD("read size %d",dbs.size());

	sqlite3_free_table(result);
	return dbs;
}

Database::Database(std::string path) {
	// TODO 自动生成的构造函数存根
	int ret = sqlite3_open(path.c_str(), &pDB);
	if(ret != SQLITE_OK){
		LOGD("open sqlite ret:%d",ret);
		pDB = NULL;
		return;
	}
	const char* cmd = "CREATE TABLE IF NOT EXISTS recode(name,num,age,score);";
	char* errmsg;
	sqlite3_exec(pDB,cmd,0,0, &errmsg);
	if(ret != SQLITE_OK){
		LOGD("create table ret:%s",errmsg);
	}
}

Database::~Database() {
	// TODO 自动生成的析构函数存根
	sqlite3_free(pDB);
}

void Database::clear() {
	const char* cmd = "DELETE FROM recode";
	char* errmsg;
	int ret = sqlite3_exec(pDB,cmd,0,0, &errmsg);
	if(ret != SQLITE_OK){
		LOGD("create DELETE ret:%s",errmsg);
	}
}

void Database::saveas(std::string path) {
}
