/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      潘陈
Version:     1.0
Date:        2023-06-03 19:54:28
Description: 函数模板
**************************************************/

#include "stdafx.h"

BM2_FUNCTION_EXPORT


int f_qmtsm_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	try
	{
		for (size_t i = 0; i < bcls_rec->Tables[1].Rows.get_Count(); i++)
		{
			sqlstr = " ";
			int	record_count_per_page = 0;		/* 每页记录数 */
			int	current_page_no = 0;			/* 需查询的页号,从0开始计数 */
			int	start_row = 0;					/* 将要压入outBlock的起始行 */
			/* 获取传入的表名 */
			CString sql_tableName = (CString)bcls_rec->Tables[1].Rows[i]["table_name"].ToString();
			Log::Trace("", "", "sql_tableName[{0}],", sql_tableName);
			/* 获取传入的列名 */
			CString sql_colName = (CString)bcls_rec->Tables[1].Rows[i]["colName"].ToString().Trim();
			CString sql_orderBy = (CString)bcls_rec->Tables[1].Rows[i]["ORDER_BY"].ToString().Trim();
			/* 每页记录数 */
			record_count_per_page = bcls_rec->Tables[1].Rows[i]["PAGE_SIZE"];
			/* 需查询的页号 */
			current_page_no = bcls_rec->Tables[1].Rows[i]["PAGE_NUM"];
			CDbCommand cmd(conn);
			/*拼接查询sql语句*/
			CString sql = "SELECT * FROM " + sql_tableName;
			CString sql_where = " WHERE 1=1";
			/*拼接查询sql条数语句*/
			CString sql_count = "SELECT COUNT(*) FROM " + sql_tableName;
			/*获取查询条件传入列数*/
			int count_row = bcls_rec->Tables[0].Rows.get_Count();
			Log::Trace("", "", "count_row[{0}]",count_row);

			for (int i = 0; i < count_row; i++)
			{
				Log::Trace("", "", "bcls_rec->Tables[0].Rows[0][i].ToString().Trim()= {0},OP_VALUE = {1}", bcls_rec->Tables[0].Rows[i]["ITEM_CODE"].ToString().Trim(), bcls_rec->Tables[0].Rows[i]["OP_VALUE"].ToString().Trim());
				/*如果查询条件的值为空则跳出*/
				if (bcls_rec->Tables[0].Rows[i]["OP_VALUE"].ToString().Trim().IsEmpty())
				{
					continue;
				}
				if (bcls_rec->Tables[0].Rows[i]["ITEM_CODE"].ToString().ToUpper() == "TABLE_TYPE")
				{
					continue;
				}
				sql_where += " AND " + bcls_rec->Tables[0].Rows[i]["ITEM_CODE"].ToString() + " LIKE @" + bcls_rec->Tables[0].Rows[i]["ITEM_CODE"].ToString() + "||'%'";
				cmd.Parameters.Set(bcls_rec->Tables[0].Rows[i]["ITEM_CODE"].ToString(), bcls_rec->Tables[0].Rows[i]["OP_VALUE"].ToString().Trim());
			}
			Log::Trace("", "", "sql_where=[{0}]", sql_where);

			/*连接sql语句*/
			sqlstr = sql_count + sql_where;
			cmd.SetCommandText(sqlstr);
			/*获取条数*/
			CDecimal rc = cmd.ExecuteScalar();
			/*把值压入RC中，传出前台*/
			bcls_ret->ExtendedProperties.Add("RC" + i, rc.ToString());
			if (sql_orderBy.Trim() != "")
			{
				sql_orderBy = " ORDER BY " + sql_orderBy;
			}
			/*完成拼接查询sql*/
			sqlstr = sql + sql_where + sql_orderBy;
			Log::Trace("", "", "条件查询sql[{0}],", sqlstr);
			cmd.SetCommandText(sqlstr);
			start_row = record_count_per_page * (current_page_no - 1);
			if (start_row > rc.ToDouble())
			{
				start_row = 0;
			}
			Log::Trace("", __FUNCTION__, "current_page_no		= [{0}]", current_page_no);
			Log::Trace("", __FUNCTION__, "record_count_per_page = [{0}]", record_count_per_page);
			Log::Trace("", __FUNCTION__, "start_row			    = [{0}]", start_row);
			int count = cmd.ExecuteQuery(bcls_ret->Tables[0], start_row, record_count_per_page);
			bcls_ret->Tables[0].set_TableName(sql_tableName);
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


