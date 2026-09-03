/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      wsl
Version:     1.0
Date:        2024-9-2 10:48:51
Description: 二炼钢铸坯分级判定结果查询
**************************************************/

#include "stdafx.h"
BM2F_ENTERACE(qmts23t802_inq)

int f_qmts23t802_inq(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString sqlstr_count = " ";
	CDecimal cd_count = 0;
	int	record_count_per_page = 0; /* 每页记录数 */
	int	current_page_no = 0; /* 需查询的页号,从0开始计数 */
	int	start_row = 0; /* 将要压入outBlock的起始行 */
	CDbCommand cmd(conn);
	try
	{
		if (bcls_rec->Tables.Contains("PAGEINFO"))
		{
			if (bcls_rec->Tables["PAGEINFO"].Columns.Contains("PAGE_NUM") && bcls_rec->Tables["PAGEINFO"].Columns.Contains("PAGE_SIZE"))
			{
				current_page_no = bcls_rec->Tables["PAGEINFO"].Rows[0]["PAGE_NUM"].ToDecimal().ToInt32() + 1;
				record_count_per_page = bcls_rec->Tables["PAGEINFO"].Rows[0]["PAGE_SIZE"];
			}
			else {
				record_count_per_page = bcls_rec->Tables[0].Rows[0]["RECORD_COUNT_PER_PAGE"];
				current_page_no = bcls_rec->Tables[0].Rows[0]["CURRENT_PAGE_NO"];
			}
		}
		else {
			record_count_per_page = bcls_rec->Tables[0].Rows[0]["RECORD_COUNT_PER_PAGE"];
			current_page_no = bcls_rec->Tables[0].Rows[0]["CURRENT_PAGE_NO"];
		}

		/*sql语句*/
		CString sql = " SELECT * FROM  TQMTST802";

		CString sql_where = " WHERE  1=1";

		/*拼接查询sql条数语句*/
		CString sql_count = "SELECT COUNT(*) FROM ( ";

		if (bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim() != "")
		{
			sql_where += " AND HEAT_NO  LIKE '" + bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString() + "%'";
			Log::Trace("", "", "HEAT_NO={0}", bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim());
		}
		if (bcls_rec->Tables[0].Rows[0]["SLAB_NO"].ToString().Trim() != "")
		{
			sql_where += " AND SLAB_NO LIKE '" + bcls_rec->Tables[0].Rows[0]["SLAB_NO"].ToString() + "%'";
			Log::Trace("", "", "SLAB_NO={0}", bcls_rec->Tables[0].Rows[0]["SLAB_NO"].ToString().Trim());
		}




		/*连接sql语句*/
		sqlstr = sql_count + sql + sql_where + ")";
		cmd.SetCommandText(sqlstr);
		cd_count = cmd.ExecuteScalar();
		start_row = record_count_per_page * (current_page_no - 1);
		if (start_row > cd_count.ToDouble())
		{
			start_row = 0;
		}

		/*完成拼接查询sql*/
		sqlstr = sql + sql_where;
		Log::Trace("", "", "条件查询sql[{0}],", sqlstr);
		cmd.SetCommandText(sqlstr);
		cmd.ExecuteQuery(bcls_ret->Tables[0], start_row, record_count_per_page);
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
