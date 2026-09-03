/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     XXX
Version:    1.0
Date:       2024-02-22
Description: 板坯待判处置查询历史
**************************************************/
//框架头文件
#include "stdafx.h"


// service入口
BM2F_ENTERACE(qmts30_inq_ls)

int f_qmts30_inq_ls(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
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
				"   FROM hqmts30 "
				"  WHERE 1=1 "
				;
			sqlstr = "SELECT * FROM hqmts30 WHERE MAT_NO <> ' ' AND FULL_FURNACE='否'";

			if (hqmts30["AREA"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND AREA	= @v_area";
			}
			if (v_c_div.Trim() != "")
			{
				sqlstr_temp += " AND C_DIV	= '" + v_c_div + "'";
			}
			if (ch_report_time_t.Trim() != "")
			{
				sqlstr_temp += " AND REPORT_TIME			>= @ch_report_time_t";
			}
			if (ch_report_time_f.Trim() != "")
			{
				sqlstr_temp += " AND REPORT_TIME			<= @ch_report_time_f";
			}
			if (hqmts30["ST_NO"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND ST_NO	 LIKE '%" + v_st_no + "%'";
			}
			if (hqmts30["INITIAL_STEEL"].ToString().Trim() != "")
			{
				sqlstr_temp += " AND INITIAL_STEEL	= @v_initial_steel";
			}
			if (v_mat_no.Trim() != "")
			{
				sqlstr_temp += " AND MAT_NO	LIKE '%" + v_mat_no + "%'";
			}
			if (v_stu.Trim() != "")
			{
				sqlstr_temp += " AND 	STATUS_FLAG= @v_stu";
			}


			sqlstr_count = sqlstr_count + sqlstr_temp;
			sqlstr_temp += " ORDER BY  MAT_NO";
			sqlstr = sqlstr + sqlstr_temp;
			break;
		}


		cmd_inq.Parameters.Set("v_area", v_area);
		cmd_inq.Parameters.Set("ch_report_time_t", ch_report_time_t);
		cmd_inq.Parameters.Set("ch_report_time_f", ch_report_time_f);
		cmd_inq.Parameters.Set("v_stu", v_stu);



		cmd_inq.SetCommandText(sqlstr_count);
		TotalRecordCount = cmd_inq.ExecuteScalar().ToInt32();
		//分页获取
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0], pageInfo.RecordFrom, pageInfo.PageSize);
		cmd_inq.Close();

		//返回分页总数量信息
		bcls_ret->Tables.Add("PageInfo");
		bcls_ret->Tables["PageInfo"].Columns.Add(DT_DECIMAL, "TotalRecordCount");
		bcls_ret->Tables["PageInfo"].Rows.Add();
		bcls_ret->Tables["PageInfo"].Rows[0]["TotalRecordCount"] = TotalRecordCount;

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
