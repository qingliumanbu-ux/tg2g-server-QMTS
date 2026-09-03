/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      潘陈
Version:     1.0
Date:        2021-04-28 15:08:16
Description: 历史成分数据查询
**************************************************/

#include "stdafx.h"

BM2F_ENTERACE(qmbs25_inq)
int f_qmbs25_inq(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	try
	{
		CDbCommand cmd(conn);
		CString heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim();
		CString st_sample_no = bcls_rec->Tables[0].Rows[0]["ST_SAMPLE_NO"].ToString().Trim();
		sqlstr = "SELECT * FROM HQMTS25 WHERE 1=1 ";
		if (heat_no != "")
		{
			sqlstr += " AND HEAT_NO = @heat_no ";
		}
		if (st_sample_no != "")
		{
			sqlstr += " AND st_sample_no = @st_sample_no ";
		}
		sqlstr += " ORDER BY REC_REVISE_TIME DESC";
		cmd.Parameters.Set("heat_no", heat_no);
		cmd.Parameters.Set("st_sample_no", st_sample_no);
		cmd.SetCommandText(sqlstr);
		cmd.ExecuteQuery(bcls_ret->Tables[0]);
	}
	catch (CDbException &ex)
	{
		CFormattable arguments[] = { ex.GetCode(), ex.GetMsg() };
		CMessageFormat::Format(s.msg, "Database Error,sqlcode=[{0}],sqlmsg=[{1}]", arguments, 2);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char *)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;
	}
	catch (CApplicationException &ex)
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException &ex)
	{
		strcpy(s.msg, ex.GetMsg());
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	return doFlag;
}
