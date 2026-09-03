/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   bhy
Version:    1.0
Date:     2024-08-26 9:13:56
Description: 质保书查询
**************************************************/

/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"

/***** C++ 的业务头文件部分 *****/



// service入口
BM2F_ENTERACE(qmts0r_inq)

int f_qmts0r_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString v_order_no = "";
	CString v_heat_no = "";
	CString v_decide_code = "";
	CString decide_code_1 = "";
	CString v_table_name_1 = "";
	CString v_now_row = "";


	CDbCommand cmd_inq(conn);

	try
	{
		//--------------------------------
		//获取传入参数
		if (bcls_rec->Tables[0].Columns.Contains("ORDER_NO"))
			v_order_no = bcls_rec->Tables[0].Rows[0]["ORDER_NO"].ToString().ToUpper();
		if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO"))
			v_heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().ToUpper();
		if (bcls_rec->Tables[0].Columns.Contains("DECIDE_CODE"))
			v_decide_code = bcls_rec->Tables[0].Rows[0]["DECIDE_CODE"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("DECIDE_CODE_1"))
			decide_code_1 = bcls_rec->Tables[0].Rows[0]["DECIDE_CODE_1"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("TABLE_NAME_1"))
			v_table_name_1 = bcls_rec->Tables[0].Rows[0]["TABLE_NAME_1"].ToString();
		if (bcls_rec->Tables[0].Columns.Contains("NOW_ROW"))
			v_now_row = bcls_rec->Tables[0].Rows[0]["NOW_ROW"].ToString();
		/* ***** 打印输入参数 ***** */
		Log::Info("", __FUNCTION__, "v_heat_no =[{0}]", v_heat_no);
		Log::Info("", __FUNCTION__, "v_decide_code =[{0}]", v_decide_code);
		Log::Info("", __FUNCTION__, "v_table_name_1 =[{0}]", v_table_name_1);
		Log::Info("", __FUNCTION__, "v_now_row =[{0}]", v_now_row);


		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:


			sqlstr = " SELECT * FROM " + v_table_name_1 + " WHERE 1 = 1 ";

			if (v_order_no != "")
			{
				sqlstr += " AND  ORDER_NO	= '" + v_order_no + "'";
			}
			if (v_heat_no != "")
			{
				sqlstr += " AND  HEAT_NO	= '" + v_heat_no + "'";
			}
			if (v_table_name_1 == "QMTS0RXYS2N")
			{
				if (v_now_row != "")
				{
					sqlstr += " AND  NOW_ROW	= '" + v_now_row + "'";
				}
			}
			/*if (v_decide_code != "")
			{
				if (v_decide_code == "1"){
					sqlstr += " AND  DECIDE_CODE	= '1' " ;
				}
				else
				{
					sqlstr += " AND  DECIDE_CODE	<> '1' ";
				}
				
			}*/
			if (decide_code_1 != ""){
				if (decide_code_1 == "2"){
					sqlstr += "  ";
				}
				else  if (decide_code_1 == "1")
				{
					sqlstr += " AND  DECIDE_CODE='1'  ";
				}
				else{
					sqlstr += " AND  DECIDE_CODE	<>  '1' ";
				}
			}

			break;
		}
		cmd_inq.Parameters.Set("v_decide_code", v_decide_code);
		cmd_inq.Parameters.Set("v_heat_no", v_heat_no);
		cmd_inq.Parameters.Set("v_order_no", v_order_no);
		cmd_inq.Parameters.Set("v_table_name_1", v_table_name_1);
		cmd_inq.Parameters.Set("v_now_row", v_now_row);
		cmd_inq.SetCommandText(sqlstr);
		Log::Info("", __FUNCTION__, "sqlstr =[{0}]", sqlstr);
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
