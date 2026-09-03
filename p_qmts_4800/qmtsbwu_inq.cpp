/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      wsl
Version:     1.0
Date:        2024.1.5
Description: ÍÆËÍ±¦ÎäÁÄÌì²éÑ¯
**************************************************/

#include "stdafx.h"

BM2F_ENTERACE(qmtsbwu_inq)


int f_qmtsbwu_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CDbCommand cmd(conn);
	try
	{
		CString code = bcls_rec->Tables[0].Rows[0]["CODE"].ToString().Trim();
		CString q_rec_create_time_from = bcls_rec->Tables[0].Rows[0]["REC_CREATE_TIME_FO"].ToString().Trim();
		CString q_rec_create_time_to = bcls_rec->Tables[0].Rows[0]["REC_CREATE_TIME_TO"].ToString().Trim();


		Log::Trace("", "", "code = {0}", code);
		Log::Trace("", "", "q_rec_create_time_from = {0}", q_rec_create_time_from);
		Log::Trace("", "", "q_rec_create_time_to = {0}", q_rec_create_time_to);

		sqlstr = "SELECT * FROM  TQMTSBWU WHERE 1=1 ";
		if (code != "")
		{
			sqlstr += " AND  CODE = @Code";
		}
		if (q_rec_create_time_from != "")
		{
			sqlstr += " AND REC_CREATE_TIME >= @Q_rec_create_time_from || '000000' ";
		}
		if (q_rec_create_time_to != "")
		{
			sqlstr += " AND REC_CREATE_TIME <= @Q_rec_create_time_to ";
		}
		sqlstr += "  ORDER BY REC_CREATE_TIME ASC ";


		Log::Trace("", "", "sqlstr = {0}", sqlstr);
		cmd.SetCommandText(sqlstr);
		cmd.Parameters.Set("Code", code);
		cmd.Parameters.Set("Q_rec_create_time_from", q_rec_create_time_from);
		cmd.Parameters.Set("Q_rec_create_time_to", q_rec_create_time_to);
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


