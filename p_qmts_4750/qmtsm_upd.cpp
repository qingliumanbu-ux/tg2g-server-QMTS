/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      潘陈
Version:     1.0
Date:        2023-06-17 15:08:16
Description: 制造标准更新，并新增历史表
**************************************************/

#include "stdafx.h"

BM2F_ENTERACE(qmtsm_upd)
int f_qmtsm_upd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	try
	{
		CString table_name = bcls_rec->Tables[1].Rows[0]["TABLE_NAME"];
		CModel tqmts("T" + table_name);
		CModel tqmts_old("T" + table_name);
		tqmts.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		tqmts_old["ST_NO"] = tqmts["ST_NO"];
		tqmts_old["FACTORY_DIV"] = tqmts["FACTORY_DIV"];
		tqmts_old.Query("ST_NO,FACTORY_DIV");
		CModel hqmts("H" + table_name);
		tqmts["DU_MAKER"] = s.userid;
		tqmts["REC_CREATE_TIME"] = datetime;
		tqmts["REC_REVISE_TIME"] = datetime;
		tqmts["DU_TIME"] = datetime;
		tqmts["DU_FLAG"] = "U";
		tqmts["VERSION"] = tqmts_old["VERSION"].ToDecimal() + 1;
		tqmts.Update("*", "ST_NO,FACTORY_DIV");
		hqmts.CopyFrom(tqmts_old);
		hqmts["DU_TIME"] = datetime;
		hqmts["DU_FLAG"] = "U";
		hqmts["DU_MAKER"] = s.userid;
		hqmts.Insert();
	}
	catch (CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode(), ex.GetMsg() };
		CMessageFormat::Format(s.msg, "Database Error,sqlcode=[{0}],sqlmsg=[{1}]", arguments, 2);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;
	}
	catch (CApplicationException& ex)
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strcpy(s.msg, ex.GetMsg());
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	return doFlag;
}


