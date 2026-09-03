/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     XXX
Version:    1.0
Date:       2024-02-22
Description: 成分不合查询历史
**************************************************/
//框架头文件
#include "stdafx.h"


// service入口
BM2F_ENTERACE(qmts30lc_inq_ls)

int f_qmts30lc_inq_ls(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString sqlstr_count = "";
	CString sqlstr_temp = "";
	int		TotalRecordCount = 0;
	CString	v_area = "";
	CString	v_heat_no = "";
	CString v_mat_no = "";
	CString v_stu = "";
	CString	v_st_no = "";

	CString ch_report_time_f = "";
	CString ch_report_time_t = "";

	CString v_sg_cr = "";
	CString v_maker = "";
	CString v_c_div = "";

	//系统的分页类信息。
	CPageInfo pageInfo;

	CModel hqmts30("HQMTS30");

	CDbCommand cmd_inq(conn);

	try
	{
		try
		{//获取前台DEV控件传入的分页信息
			pageInfo.MergeFrom(bcls_rec->Tables["PageInfo"].Rows[0]);
		}
		catch (CException& ce)
		{
			pageInfo.RecordFrom = 0;
			pageInfo.PageSize = 1000;
		}


		//--------------------------------
		//获取传入参数
		hqmts30.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		if (bcls_rec->Tables[0].Columns.Contains("AREA"))
			v_area = bcls_rec->Tables[0].Rows[0]["AREA"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("REPORT_TIME_T"))
			ch_report_time_t = bcls_rec->Tables[0].Rows[0]["REPORT_TIME_T"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("REPORT_TIME_F"))
			ch_report_time_f = bcls_rec->Tables[0].Rows[0]["REPORT_TIME_F"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO"))
			v_heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("ST_NO"))
			v_st_no = bcls_rec->Tables[0].Rows[0]["ST_NO"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("MAT_NO"))
			v_mat_no = bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("STATUS_FLAG"))
			v_stu = bcls_rec->Tables[0].Rows[0]["STATUS_FLAG"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("SG_GRADE_1"))
			v_sg_cr = bcls_rec->Tables[0].Rows[0]["SG_GRADE_1"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("DECIDE_MAKER"))
			v_maker = bcls_rec->Tables[0].Rows[0]["DECIDE_MAKER"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("C_DIV"))
			v_c_div = bcls_rec->Tables[0].Rows[0]["C_DIV"].ToString();

		Log::Info("", __FUNCTION__, "AREA =[{0}]", hqmts30["AREA"].ToString());


		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:

			sqlstr_count = " SELECT COUNT(1) "
				"   FROM HQMTS30 "
				"  WHERE 1=1 "
				;

			sqlstr = " SELECT T1.REC_CREATOR, T1.MESSAGE_LIST,T1.REC_CREATE_TIME,T1.C_DIV,T1.REMARK_3,T1.REMARK_4,T1.REMARK_5,T1.NOTE,T1.REMARK_6, T1.REC_REVISOR, T1.REC_REVISE_TIME,T1.AREA, T1.REPORT_TIME,	T1.PROD_DATE, T1.HEAT_NO, T1.MAT_NO, T1.ST_NO, T1.INITIAL_STEEL,  T1.ADJUST_REASON, T1.REJUDGE_STEEL,T1.RES_PROCESS,T1.DECIDE_MAKER, T1.DECIDER, T1.CIR_COMPONENT, T1.CLASSIFY, T1.CK_REMARK, T1.JUDGE_TIME, T1.FULL_FURNACE, T1.FINAL_OPINION,T1.APPROVE_MAKER, T1.APPROVE_TIME, T1.STATUS_FLAG, T1.REASON,T1.REASON_1, T1.SG_GRADE_1, T1.ST_SAMPLE_NO, T1.STEAL_REMARK, T1.USER_NAME, T2.MAT_THEORY_WT, T2.MAT_NUM, T2.MAT_ACT_THICK, T2.MAT_ACT_WIDTH"
				"				FROM HQMTS30 T1"
				"				LEFT JOIN"
				"				(SELECT HEAT_NO, sum(SLAB_WT) AS MAT_THEORY_WT, count(*) AS MAT_NUM, max(SLAB_THICK) AS MAT_ACT_THICK, max(SLAB_WIDTH) AS MAT_ACT_WIDTH FROM tmmsm33 GROUP BY HEAT_NO) T2"
				"				ON T1.HEAT_NO = T2.HEAT_NO "
				"   WHERE  MAT_NO =' '  ";

			if (v_area != "")
			{
				sqlstr_temp += " AND T1.AREA	= '" + v_area + "'";
			}
			if (v_c_div != "")
			{
				sqlstr_temp += " AND T1.C_DIV	= '" + v_c_div + "'";
			}
			if (ch_report_time_t.Trim() != "")
			{
				sqlstr_temp += " AND T1.REPORT_TIME			>= '" + ch_report_time_t + "'";
			}
			if (ch_report_time_f.Trim() != "")
			{
				sqlstr_temp += " AND T1.REPORT_TIME			<= '" + ch_report_time_f + "'";
			}
			if (v_heat_no != "")
			{
				sqlstr_temp += " AND T1.HEAT_NO	 LIKE '%" + v_heat_no + "%'";
			}
			if (v_st_no != "")
			{
				sqlstr_temp += " AND T1.ST_NO	LIKE '%" + v_st_no + "%'";
			}
			if (v_mat_no.Trim() != "")
			{
				sqlstr_temp += " AND T1.MAT_NO	LIKE '%" + v_mat_no + "%'";
			}
			if (v_stu.Trim() != "")
			{
				sqlstr_temp += " AND 	T1.STATUS_FLAG= '" + v_stu + "'";
			}
			if (v_sg_cr.Trim() != "")
			{
				sqlstr_temp += " AND 	T1.SG_GRADE_1 LIKE '%" + v_sg_cr + "%'";
			}
			if (v_maker.Trim() != "")
			{
				sqlstr_temp += " AND 	T1.DECIDE_MAKER LIKE '%" + v_maker + "%'";
			}


			sqlstr_count = sqlstr_count + sqlstr_temp;
			sqlstr_temp += " ORDER BY  T1.HEAT_NO";
			sqlstr = sqlstr + sqlstr_temp;
			break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
		cmd_inq.Close();

	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
	}
	catch (CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}


	return doFlag;

}
