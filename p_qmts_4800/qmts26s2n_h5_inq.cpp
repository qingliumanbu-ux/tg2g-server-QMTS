/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      潘陈
Version:     1.0
Date:        2023-12-12 14:48:51 ag翻新
Description: 渣样成分实际查询
**************************************************/

#include "stdafx.h"
BM2F_ENTERACE(qmts26s2n_h5_inq)

int f_qmts26s2n_h5_inq(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn)
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
		/*拼接查询sql语句*/
		CString sql = " WITH qmts26 AS("
			"		SELECT"
			"		*"
			"		FROM"
			"		("
			"		SELECT"
			"		T1.ST_SAMPLE_NO,"
			"		T1.HEAT_NO,"
			"		T1.ELM_ACT,"
			"		T1.ELM_CODE"
			"		FROM"
			"		TQMTS26 T1"
			"		)"
			"		PIVOT("
			"		SUM(ELM_ACT)"
			"		FOR elm_code IN('001' AS ELM_ACT_001,"
			"		'002' AS ELM_ACT_002,"
			"		'003' AS ELM_ACT_003,"
			"		'004' AS ELM_ACT_004,"
			"		'005' AS ELM_ACT_005,"
			"		'006' AS ELM_ACT_006,"
			"		'007' AS ELM_ACT_007,"
			"		'008' AS ELM_ACT_008,"
			"		'009' AS ELM_ACT_009,"
			"		'010' AS ELM_ACT_010,"
			"		'011' AS ELM_ACT_011,"
			"		'012' AS ELM_ACT_012,"
			"		'013' AS ELM_ACT_013,"
			"		'014' AS ELM_ACT_014,"
			"		'015' AS ELM_ACT_015,"
			"		'017' AS ELM_ACT_017,"
			"		'018' AS ELM_ACT_018"
			"		)"
			"		)"
			"		)"
			"		SELECT"
			"		TQMTS23.FIN_ST_NO,"
			"		TQMTS23.ST_NO,"
			"		TQMTS24.ANALYSE_TIME,"
			"		TQMTS24.PONO,"
			"		TQMTS24.HEAT_NO,"
			"		TQMTS24.REP_ELM_SEL_FLAG,"
			"		TQMTS24.SM_PLAN_NO,"
			"		TQMTS24.ST_NO,"
			"		TQMTS24.JUDGE_CODE,"
			"		TQMTS24.ST_SAMPLE_DIV,"
			"		TQMTS24.GAS_TYPE_DIV,"
			"		TQMTS24.ST_SAMPLE_SEQ,"
			"		TQMTS24.PREC_ST_NO,"
			"		TQMTS24.COMPANY_CODE,"
			"		TQMTS24.COMPANY_NAME,"
			"		qmts26.*"
			"		FROM"
			"		TQMTS24"
			"		LEFT JOIN qmts26"
			"		ON"
			"		TQMTS24.ST_SAMPLE_NO = qmts26.ST_SAMPLE_NO"
			"		AND TQMTS24.HEAT_NO = qmts26.HEAT_NO"
			"		LEFT JOIN TQMTS23"
			"		ON"
			"		TQMTS24.HEAT_NO = TQMTS23.HEAT_NO ";

		CString sql_where = " WHERE TQMTS24.ST_SAMPLE_DIV = 'S'";
		CString sql_order_by = " ORDER BY TQMTS24.ANALYSE_TIME DESC";

		/*拼接查询sql条数语句*/
		CString sql_count = "SELECT COUNT(*) FROM ( ";

		if (bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim() != "")
		{
			sql_where += " AND TQMTS24.HEAT_NO  LIKE '" + bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString() + "%'";
			Log::Trace("", "", "HEAT_NO={0}", bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim());
		}
		if (bcls_rec->Tables[0].Rows[0]["ST_SAMPLE_NO"].ToString().Trim() != "")
		{
			sql_where += " AND TQMTS24.ST_SAMPLE_NO LIKE '" + bcls_rec->Tables[0].Rows[0]["ST_SAMPLE_NO"].ToString() + "%'";
			Log::Trace("", "", "ST_SAMPLE_NO={0}", bcls_rec->Tables[0].Rows[0]["ST_SAMPLE_NO"].ToString().Trim());
		}
		//2026.04.03 查询条件增加分析时间
		if (bcls_rec->Tables[0].Rows[0]["ANALYSE_TIME_FROM"].ToString().Trim() != "")
		{
			sql_where += " AND TQMTS24.ANALYSE_TIME >='" + bcls_rec->Tables[0].Rows[0]["ANALYSE_TIME_FROM"].ToString() + "' ";
			Log::Trace("", "", "ANALYSE_TIME_FROM={0}", bcls_rec->Tables[0].Rows[0]["ANALYSE_TIME_FROM"].ToString().Trim());
		}
		if (bcls_rec->Tables[0].Rows[0]["ANALYSE_TIME_TO"].ToString().Trim() != "")
		{
			sql_where += " AND TQMTS24.ANALYSE_TIME <='" + bcls_rec->Tables[0].Rows[0]["ANALYSE_TIME_TO"].ToString() + "' ";
			Log::Trace("", "", "ANALYSE_TIME_TO={0}", bcls_rec->Tables[0].Rows[0]["ANALYSE_TIME_TO"].ToString().Trim());
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
		sqlstr = sql + sql_where + sql_order_by;
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
