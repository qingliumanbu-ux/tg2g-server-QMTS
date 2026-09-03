/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      潘陈
Version:     1.0
Date:        2021-04-28 15:08:16
Description: 查询TMMSM01信息
**************************************************/

#include "stdafx.h"

BM2F_ENTERACE(qmbs_mmsm01_inq)
int f_qmbs_mmsm01_inq(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString q_heat_no = "";
	CString q_mat_no = "";
	CString q_time_to = "";
	CString q_time_from = "";
	CDecimal cd_count = 0;
	CString q_code = "";
	CString sqlstr1 = " ";

	int	record_count_per_page = 0; /* 每页记录数 */
	int	current_page_no = 0; /* 需查询的页号,从0开始计数 */
	int	start_row = 0; /* 将要压入outBlock的起始行 */
	CDbCommand cmd(conn);
	CDbCommand cmd_inq1(conn);
	try
	{

		/*获得传入参数*/
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

		q_heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim();
		q_mat_no = bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString().Trim();
		q_code = bcls_rec->Tables[0].Rows[0]["UNIT_CODE"].ToString().Trim();
		q_time_to = bcls_rec->Tables[0].Rows[0]["SLAB_CUT_TIME_TO"].ToString().Trim();
		q_time_from = bcls_rec->Tables[0].Rows[0]["SLAB_CUT_TIME_FROM"].ToString().Trim();

		/*拼接查询sql条数语句*/
		CString sql_count = "SELECT COUNT(*) FROM ( ";

		sqlstr = "SELECT * FROM TMMSM01 WHERE 1=1 ";
		if (q_heat_no != "")
		{
			sqlstr += " AND HEAT_NO = '" + q_heat_no +"'";
		}
		if (q_mat_no != "")
		{
			sqlstr += " AND MAT_NO = '" + q_mat_no + "'";
		}
		if (q_code != "")
		{
			sqlstr += " AND UNIT_CODE = '" + q_code + "'";
		}
		if (q_time_to != "")
		{
			sqlstr += " AND SLAB_CUT_TIME <= '" + q_time_to + "'";
		}
		if (q_time_from != "")
		{
			sqlstr += " AND SLAB_CUT_TIME >= '" + q_time_from + "'";
		}
		CString sqlstr2 = " ORDER BY MAT_NO ASC ";

		/*连接sql语句*/
		sqlstr1 = sql_count + sqlstr + ")";
		cmd_inq1.SetCommandText(sqlstr1);
		cd_count = cmd_inq1.ExecuteScalar();
		start_row = record_count_per_page * (current_page_no - 1);
		if (start_row > cd_count.ToDouble())
		{
			start_row = 0;
		}

		/*完成拼接查询sql*/
		Log::Trace("", "", "条件查询sql[{0}],", sqlstr);
		CString sqlstr3 = sqlstr + sqlstr2;
		cmd.SetCommandText(sqlstr3);
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
