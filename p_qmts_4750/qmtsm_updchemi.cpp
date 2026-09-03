/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      潘陈
Version:     1.0
Date:        2023-06-17 15:08:16
Description: 化学成分修改，并新增历史表
**************************************************/

#include "stdafx.h"

BM2F_ENTERACE(qmtsm_updchemi)
int f_qmtsm_updchemi(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	try
	{
		CModel tqmts02("TQMTS02");
		CModel tqmts02_old("TQMTS02");
		CModel hqmts02("HQMTS02");
		for (size_t i = 0; i < bcls_rec->Tables["QMTS_CHEMI_MODIFY"].Rows.get_Count(); i++)
		{
			tqmts02.MergeFrom(bcls_rec->Tables["QMTS_CHEMI_MODIFY"].Rows[i]);
			tqmts02_old.MergeFrom(bcls_rec->Tables["QMTS_CHEMI_MODIFY"].Rows[i]);
			tqmts02_old.Query("ST_NO,FACTORY_DIV,ELM_CODE,WHOLE_BACKLOG_CODE");
			hqmts02.CopyFrom(tqmts02_old);
			tqmts02["DU_MAKER"] = s.userid;
			tqmts02["REC_CREATE_TIME"] = datetime;
			tqmts02["REC_REVISE_TIME"] = datetime;
			tqmts02["DU_TIME"] = datetime;
			tqmts02["DU_FLAG"] = "U";
			tqmts02["VERSION"] = tqmts02_old["VERSION"].ToDecimal() + 1;
			tqmts02.Update("*", "ST_NO,FACTORY_DIV,ELM_CODE,WHOLE_BACKLOG_CODE");
			hqmts02["DU_TIME"] = datetime;
			hqmts02["DU_FLAG"] = "U";
			hqmts02["DU_MAKER"] = s.userid;
			hqmts02.Insert();
		}
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


