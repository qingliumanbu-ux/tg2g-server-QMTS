/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      潘陈
Version:     1.0
Date:        2023.6.6
Description: 同时查询单记录制造标准数据和成分数据
			 避免多次查询造成相应速度过慢
**************************************************/

#include "stdafx.h"

BM2F_ENTERACE(qmtsm_elm_inq)


int f_qmtsm_elm_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CDbCommand cmd(conn);
	try
	{
		CString table_name = bcls_rec->Tables[1].Rows[0]["TABLE_NAME"];
		CString st_no = bcls_rec->Tables[0].Rows[0]["ST_NO"];
		Log::Trace("", "", "table_name = {0}", table_name);
		Log::Trace("", "", "st_no = {0}", st_no);
		sqlstr = "SELECT * FROM " + table_name + " WHERE ST_NO = @st_no";
		Log::Trace("", "", "sqlstr = {0}", sqlstr);
		cmd.SetCommandText(sqlstr);
		cmd.Parameters.Set("st_no", st_no);
		cmd.ExecuteQuery(bcls_ret->Tables[0]);

		bcls_ret->Tables.Add();
		sqlstr = "SELECT * FROM TQMTS02 WHERE IDX_NO = (SELECT ELM_STD_IDX_A FROM TQMTS0X WHERE ST_NO = @st_no)";
		cmd.SetCommandText(sqlstr);
		Log::Trace("", "", "sqlstr = {0}", sqlstr);
		cmd.Parameters.Set("st_no", st_no);
		cmd.ExecuteQuery(bcls_ret->Tables[1]);
		Log::Trace("", "", "tqmts02 count = {0}", bcls_ret->Tables[1].Rows.get_Count());
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


