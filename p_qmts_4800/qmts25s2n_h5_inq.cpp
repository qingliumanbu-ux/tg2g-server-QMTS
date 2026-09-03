/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      wsl
Version:     1.0
Date:        2023-12-12 14:48:51 ag翻新
Description: 工序成分实际查询
**************************************************/

#include "stdafx.h"
BM2F_ENTERACE(qmts25s2n_h5_inq)

int f_qmts25s2n_h5_inq(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString sqlstr_count = " ";
	CDecimal cd_count = 0;
	CDbCommand cmd(conn);
	try
	{
		/*拼接查询sql语句*/
		CString sql = "SELECT * FROM TQMTS24 ";

		CString sql_where = " WHERE 1=1 ";
		CString sql_order_by = " ORDER BY ANALYSE_TIME DESC";

		/*拼接查询sql条数语句*/
		CString sql_count = "SELECT COUNT(*) FROM ( ";

		if (bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim() != "")
		{
			sql_where += " AND HEAT_NO  LIKE '" + bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString() + "%'";
			Log::Trace("", "", "HEAT_NO={0}", bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim());
		}
		if (bcls_rec->Tables[0].Rows[0]["ST_SAMPLE_NO"].ToString().Trim() != "")
		{
			sql_where += " AND ST_SAMPLE_NO LIKE '" + bcls_rec->Tables[0].Rows[0]["ST_SAMPLE_NO"].ToString() + "%'";
			Log::Trace("", "", "ST_SAMPLE_NO={0}", bcls_rec->Tables[0].Rows[0]["ST_SAMPLE_NO"].ToString().Trim());
		}
		if (bcls_rec->Tables[0].Rows[0]["SAMPLE_TAKEN_TIME_TO"].ToString().Trim() != "")
		{
			sql_where += " AND SAMPLE_TAKEN_TIME >= '" + bcls_rec->Tables[0].Rows[0]["SAMPLE_TAKEN_TIME_TO"].ToString().Trim().Substring(0, 8) + "'";
			Log::Trace("", "", "SAMPLE_TAKEN_TIME_TO={0}", bcls_rec->Tables[0].Rows[0]["SAMPLE_TAKEN_TIME_TO"].ToString().Trim());
		}
		if (bcls_rec->Tables[0].Rows[0]["SAMPLE_TAKEN_TIME_FROM"].ToString().Trim() != "")
		{
			sql_where += " AND SAMPLE_TAKEN_TIME <= '" + bcls_rec->Tables[0].Rows[0]["SAMPLE_TAKEN_TIME_FROM"].ToString().Trim().Substring(0, 8) + "'";
			Log::Trace("", "", "SAMPLE_TAKEN_TIME_FROM={0}", bcls_rec->Tables[0].Rows[0]["SAMPLE_TAKEN_TIME_FROM"].ToString().Trim());
		}
		if (bcls_rec->Tables[0].Rows[0]["ST_SAMPLE_DIV"].ToString().Trim() != "")
		{
			sql_where += " AND ST_SAMPLE_DIV = '" + bcls_rec->Tables[0].Rows[0]["ST_SAMPLE_DIV"].ToString() + "'";
			Log::Trace("", "", "ST_SAMPLE_DIV={0}", bcls_rec->Tables[0].Rows[0]["ST_SAMPLE_DIV"].ToString().Trim());
		}
		if (bcls_rec->Tables[0].Rows[0]["DEV_CODE"].ToString().Trim() != "")
		{
			sql_where += " AND DEV_CODE = '" + bcls_rec->Tables[0].Rows[0]["DEV_CODE"].ToString() + "'";
			Log::Trace("", "", "DEV_CODE={0}", bcls_rec->Tables[0].Rows[0]["DEV_CODE"].ToString().Trim());
		}



		/*连接sql语句*/
		sqlstr = sql_count + sql + sql_where + ")";
		cmd.SetCommandText(sqlstr);
		cd_count = cmd.ExecuteScalar();

		/*完成拼接查询sql*/
		sqlstr = sql + sql_where + sql_order_by;
		Log::Trace("", "", "条件查询sql[{0}],", sqlstr);
		cmd.SetCommandText(sqlstr);
		cmd.ExecuteQuery(bcls_ret->Tables[0]);
		cmd.Close();

		//返回分页信息
		bcls_ret->Tables.Add("PAGEINFO");	//增加块
		bcls_ret->Tables["PAGEINFO"].Columns.Add(DT_DECIMAL, "TOTAL_RECORD");						//总记录数
		bcls_ret->Tables["PAGEINFO"].Rows.Add();
		bcls_ret->Tables["PAGEINFO"].Rows[0][0] = cd_count.ToInt32();
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
