/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      wsl
Version:     1.0
Date:        2024.3.20
Description: 代表成分查询
**************************************************/

#include "stdafx.h"

BM2F_ENTERACE(qmtsb0s2n_inq)


int f_qmtsb0s2n_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr_count = " ";
	CString sqlstr = " ";
	CDecimal cd_count = 0;
	int	record_count_per_page = 0; /* 每页记录数 */
	int	current_page_no = 0; /* 需查询的页号,从0开始计数 */
	int	start_row = 0; /* 将要压入outBlock的起始行 */
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CDbCommand cmd(conn);
	try
	{
		CString heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim();
		CString st_sample_no = bcls_rec->Tables[0].Rows[0]["ST_SAMPLE_NO"].ToString().Trim();


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

		Log::Trace("", "", "heat_no = {0}", heat_no);
		Log::Trace("", "", "st_sample_no = {0}", st_sample_no);
		Log::Trace("", "", "record_count_per_page = {0}", record_count_per_page);
		Log::Trace("", "", "current_page_no = {0}", current_page_no);

		sqlstr_count = " SELECT COUNT(1) "
			"   FROM TQMTSB0 "
			"  WHERE 1=1 ";


		sqlstr = "SELECT * FROM TQMTSB0  WHERE 1=1 ";
		if (heat_no != "")
		{
			sqlstr += " AND HEAT_NO  = @heat_no";
			sqlstr_count += " AND HEAT_NO  = '" + heat_no + "'";
		}
		if (st_sample_no != "")
		{
			sqlstr += " AND ST_SAMPLE_NO  = @st_sample_no";
			sqlstr_count += " AND ST_SAMPLE_NO  = '" + st_sample_no + "'";
		}
		Log::Trace("", "", "sqlstr = {0}", sqlstr);
		cmd.SetCommandText(sqlstr_count);
		cd_count = cmd.ExecuteScalar();
		start_row = record_count_per_page * (current_page_no - 1);
		if (start_row > cd_count.ToDouble())
		{
			start_row = 0;
		}

		cmd.SetCommandText(sqlstr);
		cmd.Parameters.Set("heat_no", heat_no);
		cmd.Parameters.Set("st_sample_no", st_sample_no);
		cmd.ExecuteQuery(bcls_ret->Tables[0], start_row, record_count_per_page);


		//返回分页信息
		bcls_ret->Tables.Add("PAGEINFO");	//增加块
		bcls_ret->Tables["PAGEINFO"].Columns.Add(DT_DECIMAL, "TOTAL_RECORD");						//总记录数
		bcls_ret->Tables["PAGEINFO"].Rows.Add();
		bcls_ret->Tables["PAGEINFO"].Rows[0][0] = cd_count.ToInt32();
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


