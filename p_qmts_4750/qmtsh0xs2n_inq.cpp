/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      wsl
Version:     1.0
Date:        2024.1.5
Description: ¹¤ÒÕ¿¨²éÑ¯
**************************************************/

#include "stdafx.h"

BM2F_ENTERACE(qmtsh0xs2n_inq)


int f_qmtsh0xs2n_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CDbCommand cmd(conn);
	CString ST_NO = "";
	CString tableName = "";
	try
	{
		tableName = bcls_rec->Tables["PAGEINFO"].Rows[0]["tableName"].ToString().Trim();
		Log::Trace("", "", "qmtsh5_inq IN:---tableName = [{0}]", tableName);
		ST_NO = bcls_rec->Tables[0].Rows[0]["ST_NO"].ToString().Trim();
		Log::Trace("", "", "qmtsh5_inq IN:---ST_NO = [{0}]", ST_NO);


		sqlstr = "SELECT * FROM "+ tableName + " WHERE 1=1 ";
		if (ST_NO != "")
		{
			sqlstr += " AND  ST_NO = @ST_NO";
		}

		Log::Trace("", "", "sqlstr = {0}", sqlstr);
		cmd.SetCommandText(sqlstr);
		cmd.Parameters.Set("ST_NO", ST_NO);
		cmd.ExecuteQuery(bcls_ret->Tables[0]);

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


